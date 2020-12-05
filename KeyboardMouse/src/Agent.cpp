#include "pch.h"
#include "Agent.h"
#include "KeyboardMouseConstants.h"
#include "hnrt/Debug.h"
#include "hnrt/WindowsPlatform.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/String.h"
#include "hnrt/WhileInScope.h"


using namespace hnrt;


UINT Agent::GetCommandMessage()
{
    return RegisterWindowMessageW(L"WM_AGENT_COMMAND");
}


void Agent::MapSharedBlock(SharedMemory<AgentBlock>& sm, DWORD dw)
{
    DWORD dwError = sm.Map(String::Format(L"Local\\AGENT_F3ACC1F6-5624-4FA9-9742-20F9B24BAF54_%08ld", dw));
    if (dwError)
    {
        throw Win32Exception(dwError, L"Failed to map the agent block.");
    }
}


Agent::Agent()
    : ComLibrary(COINIT_APARTMENTTHREADED)
    , m_ulRefCnt(1)
    , m_WM_AGENT_COMMAND(GetCommandMessage())
    , m_pAgentBlock()
    , m_bInitialized(false)
    , m_bOnCall(false)
    , m_pTfThreadMgr()
    , m_TfClientId(TF_CLIENTID_NULL)
    , m_pTfCompartmentMgr()
    , m_pTfCompartmentKeyboardOpenClose()
    , m_pTfCompartmentInputModeConversion()
    , m_CompartmentEventSinkCookieMap()
    , m_pInputProcessorProfiles()
    , m_dwLanguageProfileNotifySinkCookie(TF_INVALID_COOKIE)
{
    DBGFNC(L"KeyboardMouse%d::Agent::ctor", BITNESS);
    MapSharedBlock(m_pAgentBlock, GetCurrentThreadId());
}


Agent::~Agent()
{
    DBGFNC(L"KeyboardMouse%d::Agent::dtor", BITNESS);
    m_pAgentBlock.Unmap();
}


void Agent::Initialize()
{
    if (m_bInitialized)
    {
        return;
    }

    DBGFNC(L"KeyboardMouse%d::Agent::Initialize", BITNESS);

    m_bInitialized = true;

    m_pAgentBlock->dwFlags |= AGENT_FLAG_ENABLED;

    HRESULT hr;

    hr = CoCreateInstance(CLSID_TF_InputProcessorProfiles, NULL, CLSCTX_INPROC_SERVER, IID_ITfInputProcessorProfiles, reinterpret_cast<void**>(&m_pInputProcessorProfiles));
    if (hr == S_OK)
    {
        hr = m_pInputProcessorProfiles->GetCurrentLanguage(&m_pAgentBlock->Language);
        if (hr == S_OK)
        {
            DBGPUT(L"LanguageId=%04x", m_pAgentBlock->Language);
            m_pAgentBlock->dwFlags |= KEYBOARD_FLAG_LANGUAGE;
        }
        else
        {
            Debug::Put(L"TfInputProcessorProfiles::GetCurrentLanguage failed: %08lx", hr);
            m_pAgentBlock->dwFlags &= ~KEYBOARD_FLAG_LANGUAGE;
        }
        RefPtr<ITfSource> pSource;
        hr = m_pInputProcessorProfiles->QueryInterface(IID_ITfSource, reinterpret_cast<void**>(&pSource));
        if (hr == S_OK)
        {
            hr = pSource->AdviseSink(IID_ITfLanguageProfileNotifySink, static_cast<ITfLanguageProfileNotifySink*>(this), &m_dwLanguageProfileNotifySinkCookie);
            if (hr != S_OK)
            {
                Debug::Put(L"TfSource::AdviseSink(ITfLanguageProfileNotifySink) failed: %08lx", hr);
            }
        }
        else
        {
            Debug::Put(L"TfInputProcessorProfiles::QueryInterface(TfSource) failed: %08lx", hr);
        }
    }
    else
    {
        Debug::Put(L"CoCreateInstance(TF_InputProcessorProfiles) failed: %08lx", hr);
    }

    hr = CoCreateInstance(CLSID_TF_ThreadMgr, NULL, CLSCTX_INPROC_SERVER, IID_ITfThreadMgr, reinterpret_cast<void**>(&m_pTfThreadMgr));
    if (hr == S_OK)
    {
        hr = m_pTfThreadMgr->Activate(&m_TfClientId);
        if (hr == S_OK)
        {
            hr = m_pTfThreadMgr->QueryInterface(IID_ITfCompartmentMgr, reinterpret_cast<void**>(&m_pTfCompartmentMgr));
            if (hr == S_OK)
            {
                hr = GetCompartment(GUID_COMPARTMENT_KEYBOARD_OPENCLOSE, m_pTfCompartmentKeyboardOpenClose);
                if (hr == S_OK)
                {
                    hr = GetCompartmentLong(m_pTfCompartmentKeyboardOpenClose, m_pAgentBlock->KeyboardOpenClose);
                    if (hr == S_OK)
                    {
                        DBGPUT(L"KEYBOARD_OPENCLOSE=%08lx", m_pAgentBlock->KeyboardOpenClose);
                        m_pAgentBlock->dwFlags |= KEYBOARD_FLAG_OPENCLOSE;
                    }
                    else
                    {
                        Debug::Put(L"KEYBOARD_OPENCLOSE::GetValue failed: %08lx", hr);
                        m_pAgentBlock->dwFlags &= ~KEYBOARD_FLAG_OPENCLOSE;
                    }
                }
                else
                {
                    Debug::Put(L"KEYBOARD_OPENCLOSE::GetCompartment failed: %08lx", hr);
                }
                hr = GetCompartment(GUID_COMPARTMENT_KEYBOARD_INPUTMODE_CONVERSION, m_pTfCompartmentInputModeConversion);
                if (hr == S_OK)
                {
                    hr = GetCompartmentLong(m_pTfCompartmentInputModeConversion, m_pAgentBlock->InputModeConversion);
                    if (hr == S_OK)
                    {
                        DBGPUT(L"INPUTMODE_CONVERSION=%08lx", m_pAgentBlock->InputModeConversion);
                        m_pAgentBlock->dwFlags |= KEYBOARD_FLAG_INPUTMODE;
                    }
                    else
                    {
                        Debug::Put(L"INPUTMODE_CONVERSION::GetValue failed: %08lx", hr);
                        m_pAgentBlock->dwFlags &= ~KEYBOARD_FLAG_INPUTMODE;
                    }
                }
                else
                {
                    Debug::Put(L"INPUTMODE_CONVERSION::GetCompartment failed: %08lx", hr);
                }
            }
            else
            {
                Debug::Put(L"TfThreadMgr::QueryInterface(TfCompartmentMgr) failed: %08lx", hr);
            }
        }
        else
        {
            Debug::Put(L"TfThreadMgr::Activate failed: %08lx", hr);
        }
    }
    else
    {
        Debug::Put(L"CoCreateInstance(TF_ThreadMgr) failed: %08lx", hr);
    }
}


void Agent::Uninitialize()
{
    if (!m_bInitialized)
    {
        return;
    }

    DBGFNC(L"KeyboardMouse%d::Agent::Uninitialize", BITNESS);

    m_bInitialized = false;

    m_pAgentBlock->dwFlags &= ~AGENT_FLAG_ENABLED;

    HRESULT hr;

    if (m_pInputProcessorProfiles)
    {
        RefPtr<ITfSource> pSource;
        hr = m_pInputProcessorProfiles->QueryInterface(IID_ITfSource, reinterpret_cast<void**>(&pSource));
        if (hr == S_OK)
        {
            hr = pSource->UnadviseSink(m_dwLanguageProfileNotifySinkCookie);
            if (hr != S_OK)
            {
                Debug::Put(L"TfSource::UnadviseSink(LanguageProfileNotifySink) failed: %08lx", hr);
            }
        }
        m_pInputProcessorProfiles = nullptr;
    }

    UnadviseAllCompartmentEventSinks();

    m_pTfCompartmentInputModeConversion = nullptr;

    m_pTfCompartmentKeyboardOpenClose = nullptr;

    m_pTfCompartmentMgr = nullptr;

    if (m_pTfThreadMgr)
    {
        if (m_TfClientId != TF_CLIENTID_NULL)
        {
            m_pTfThreadMgr->Deactivate();
            m_TfClientId = TF_CLIENTID_NULL;
        }
        m_pTfThreadMgr = nullptr;
    }
}


bool Agent::OnCall(
    _In_ HWND   hwnd,
    _In_ UINT   uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam)
{
    if (!m_bOnCall)
    {
        WhileInScope<bool> wip(m_bOnCall, true, false);
        if (uMsg == m_WM_AGENT_COMMAND)
        {
            switch (wParam)
            {
            case AGENT_INITIALIZE:
            {
                DBGFNC(L"KeyboardMouse%d::AGENT_INITIALIZE", BITNESS);
                Initialize();
                break;
            }
            case AGENT_UNINITIALIZE:
            {
                DBGFNC(L"KeyboardMouse%d::AGENT_UNINITIALIZE", BITNESS);
                Uninitialize();
                return false;
            }
            case AGENT_GET_LANGUAGE:
            {
                DBGFNC(L"KeyboardMouse%d::AGENT_GET_LANGUAGE", BITNESS);
                GetLanguage();
                break;
            }
            case AGENT_SET_LANGUAGE:
            {
                DBGFNC(L"KeyboardMouse%d::AGENT_SET_LANGUAGE", BITNESS);
                SetLanguage(static_cast<LANGID>(lParam));
                break;
            }
            case AGENT_GET_OPENCLOSE:
            {
                DBGFNC(L"KeyboardMouse%d::AGENT_GET_OPENCLOSE", BITNESS);
                GetOpenClose();
                break;
            }
            case AGENT_SET_OPENCLOSE:
            {
                DBGFNC(L"KeyboardMouse%d::AGENT_SET_OPENCLOSE", BITNESS);
                SetOpenClose(static_cast<LONG>(lParam));
                break;
            }
            case AGENT_GET_INPUTMODE:
            {
                DBGFNC(L"KeyboardMouse%d::AGENT_GET_INPUTMODE", BITNESS);
                GetInputMode();
                break;
            }
            case AGENT_SET_INPUTMODE:
            {
                DBGFNC(L"KeyboardMouse%d::AGENT_SET_INPUTMODE", BITNESS);
                SetInputMode(static_cast<LONG>(lParam));
                break;
            }
            default:
                break;
            }
        }
        else if (uMsg == WM_DESTROY)
        {
            if (hwnd == reinterpret_cast<HWND>(static_cast<DWORD_PTR>(m_pAgentBlock->dwHwnd)))
            {
                DBGFNC(L"KeyboardMouse%d::WM_DESTROY", BITNESS);
                if (m_bInitialized)
                {
                    Uninitialize();
                }
                return false;
            }
        }
    }
    return true;
}


HRESULT Agent::GetLanguage()
{
    m_pAgentBlock->dwFlags &= ~KEYBOARD_FLAG_LANGUAGE;
    HRESULT hr;
    if (m_pInputProcessorProfiles)
    {
        hr = m_pInputProcessorProfiles->GetCurrentLanguage(&m_pAgentBlock->Language);
        if (hr == S_OK)
        {
            DBGPUT(L"Language=%04x", m_pAgentBlock->Language);
            m_pAgentBlock->dwFlags |= KEYBOARD_FLAG_LANGUAGE;
        }
        else
        {
            Debug::Put(L"TfInputProcessorProfiles::GetCurrentLanguage failed: %08lx", hr);
        }
    }
    else
    {
        hr = E_FAIL;
    }
    return hr;
}


HRESULT Agent::SetLanguage(LANGID requested)
{
    m_pAgentBlock->dwFlags &= ~KEYBOARD_FLAG_LANGUAGE;
    HRESULT hr;
    if (m_pInputProcessorProfiles)
    {
        LANGID* pLangIds = NULL;
        ULONG ulLangCount = 0;
        hr = m_pInputProcessorProfiles->GetLanguageList(&pLangIds, &ulLangCount);
        if (hr == S_OK)
        {
            DBGPUT(L"TfInputProcessorProfiles::GetLanguageList: count=%lu", ulLangCount);
            if (pLangIds)
            {
                LANGID candidate = 0;
                for (ULONG index = 0; index < ulLangCount; index++)
                {
                    DBGPUT(L"LANGID=%04x", pLangIds[index]);
                    if (pLangIds[index] == requested)
                    {
                        candidate = pLangIds[index];
                        break;
                    }
                    else if (candidate)
                    {
                        continue;
                    }
                    else if (PRIMARYLANGID(pLangIds[index]) == PRIMARYLANGID(requested))
                    {
                        candidate = pLangIds[index];
                    }
                }
                if (candidate)
                {
                    hr = m_pInputProcessorProfiles->ChangeCurrentLanguage(candidate);
                    if (hr == S_OK)
                    {
                        DBGPUT(L"InputProcessorProfiles::ChangeCurrentLanguage(%04x)", candidate);
                    }
                    else
                    {
                        Debug::Put(L"InputProcessorProfiles::ChangeCurrentLanguage(%04x) failed. error=%08lx", candidate, hr);
                    }
                }
                CoTaskMemFree(pLangIds);
            }
        }
        else
        {
            Debug::Put(L"TfInputProcessorProfiles::GetLanguageList failed. error=%08lx", hr);
        }
    }
    else
    {
        hr = E_FAIL;
    }
    return hr;
}


HRESULT Agent::GetOpenClose()
{
    m_pAgentBlock->dwFlags &= ~KEYBOARD_FLAG_OPENCLOSE;
    HRESULT hr;
    if (m_pTfCompartmentKeyboardOpenClose)
    {
        hr = GetCompartmentLong(m_pTfCompartmentKeyboardOpenClose, m_pAgentBlock->KeyboardOpenClose);
        if (hr == S_OK)
        {
            DBGPUT(L"KEYBOARD_OPENCLOSE=%08lx", m_pAgentBlock->KeyboardOpenClose);
            m_pAgentBlock->dwFlags |= KEYBOARD_FLAG_OPENCLOSE;
        }
        else
        {
            Debug::Put(L"KEYBOARD_OPENCLOSE::GetValue failed: %08lx", hr);
        }
    }
    else
    {
        hr = E_FAIL;
    }
    return hr;
}


HRESULT Agent::SetOpenClose(LONG value)
{
    m_pAgentBlock->dwFlags &= ~KEYBOARD_FLAG_OPENCLOSE;
    HRESULT hr;
    if (m_pTfCompartmentKeyboardOpenClose)
    {
        hr = SetCompartmentLong(m_pTfCompartmentKeyboardOpenClose, value);
    }
    else
    {
        hr = E_FAIL;
    }
    return hr;
}


HRESULT Agent::GetInputMode()
{
    m_pAgentBlock->dwFlags &= ~KEYBOARD_FLAG_INPUTMODE;
    HRESULT hr;
    if (m_pTfCompartmentInputModeConversion)
    {
        hr = GetCompartmentLong(m_pTfCompartmentInputModeConversion, m_pAgentBlock->InputModeConversion);
        if (hr == S_OK)
        {
            DBGPUT(L"INPUTMODE_CONVERSION=%08lx", m_pAgentBlock->InputModeConversion);
            m_pAgentBlock->dwFlags |= KEYBOARD_FLAG_INPUTMODE;
        }
        else
        {
            Debug::Put(L"INPUTMODE_CONVERSION::GetValue failed: %08lx", hr);
        }
    }
    else
    {
        hr = E_FAIL;
    }
    return hr;
}


HRESULT Agent::SetInputMode(LONG value)
{
    m_pAgentBlock->dwFlags &= ~KEYBOARD_FLAG_INPUTMODE;
    HRESULT hr;
    if (m_pTfCompartmentInputModeConversion)
    {
        hr = SetCompartmentLong(m_pTfCompartmentInputModeConversion, value);
    }
    else
    {
        hr = E_FAIL;
    }
    return hr;
}
