#include "pch.h"
#include "Agent.h"
#include "KeyboardMouseConstants.h"
#include "hnrt/Debug.h"
#include "hnrt/RefPtr.h"
#include <atlcomcli.h>


using namespace hnrt;


HRESULT STDMETHODCALLTYPE Agent::OnChange(
    /* [in] */ __RPC__in REFGUID rguid)
{
    DBGFNC(L"KeyboardMouse%d::Agent::OnChange", BITNESS);
    if (IsEqualGUID(rguid, GUID_COMPARTMENT_KEYBOARD_OPENCLOSE))
    {
        HRESULT hr = GetCompartmentLong(m_pTfCompartmentKeyboardOpenClose, m_pAgentBlock->KeyboardOpenClose);
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
        return S_OK;
    }
    else if (IsEqualGUID(rguid, GUID_COMPARTMENT_KEYBOARD_INPUTMODE_CONVERSION))
    {
        HRESULT hr = GetCompartmentLong(m_pTfCompartmentInputModeConversion, m_pAgentBlock->InputModeConversion);
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
        return S_OK;
    }
    else
    {
        return E_UNEXPECTED;
    }
}


HRESULT Agent::GetCompartment(const GUID& rguid, RefPtr<ITfCompartment>& pCompartment)
{
    HRESULT hr;

    hr = m_pTfCompartmentMgr->GetCompartment(rguid, &pCompartment);
    if (hr != S_OK)
    {
        return hr;
    }

    RefPtr<ITfSource> pSource;
    hr = pCompartment->QueryInterface(IID_ITfSource, reinterpret_cast<void**>(&pSource));
    if (hr != S_OK)
    {
        pCompartment = RefPtr<ITfCompartment>();
        return hr;
    }

    DWORD dwCookie = TF_INVALID_COOKIE;
    hr = pSource->AdviseSink(IID_ITfCompartmentEventSink, static_cast<ITfCompartmentEventSink*>(this), &dwCookie);
    if (hr == S_OK)
    {
        m_CompartmentEventSinkCookieMap.insert(std::pair<RefPtr<ITfCompartment>, DWORD>(pCompartment, dwCookie));
    }
    else
    {
        pCompartment = RefPtr<ITfCompartment>();
    }

    return hr;
}


HRESULT Agent::GetCompartmentLong(RefPtr<ITfCompartment>& pCompartment, LONG& value)
{
    CComVariant varValue;
    HRESULT hr = pCompartment->GetValue(&varValue);
    if (hr == S_OK)
    {
        if (varValue.vt == VT_I4)
        {
            value = varValue.lVal;
        }
        else
        {
            hr = E_UNEXPECTED;
        }
    }
    return hr;
}


HRESULT Agent::SetCompartmentLong(RefPtr<ITfCompartment>& pCompartment, LONG value)
{
    CComVariant varValue(value);
    HRESULT hr = pCompartment->SetValue(m_TfClientId, &varValue);
    return hr;
}


void Agent::UnadviseAllCompartmentEventSinks()
{
    for (std::map<RefPtr<ITfCompartment>, DWORD>::const_iterator iter = m_CompartmentEventSinkCookieMap.begin(); iter != m_CompartmentEventSinkCookieMap.end(); iter++)
    {
        RefPtr<ITfCompartment> pCompartment = iter->first;
        DWORD dwCookie = iter->second;
        RefPtr<ITfSource> pSource;
        HRESULT hr = pCompartment->QueryInterface(IID_ITfSource, reinterpret_cast<void**>(&pSource));
        if (hr == S_OK)
        {
            pSource->UnadviseSink(dwCookie);
        }
    }
    m_CompartmentEventSinkCookieMap.clear();
}
