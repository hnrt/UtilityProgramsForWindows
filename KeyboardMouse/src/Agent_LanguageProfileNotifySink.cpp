#include "pch.h"
#include "Agent.h"
#include "KeyboardMouseConstants.h"
#include "hnrt/Debug.h"


using namespace hnrt;


HRESULT STDMETHODCALLTYPE Agent::OnLanguageChange(
    /* [in] */ LANGID langid,
    /* [out] */ __RPC__out BOOL* pfAccept)
{
    DBGFNC(L"KeyboardMouse%d::Agent::OnLanguageChange", BITNESS);
    if (m_pAgentBlock->PreferredLanguage && m_pAgentBlock->PreferredLanguage != langid)
    {
        DBGPUT(L"langid=%04x accept=FALSE", langid);
        *pfAccept = FALSE;
        return S_OK;
    }
    DBGPUT(L"langid=%04x accept=TRUE", langid);
    *pfAccept = TRUE;
    return S_OK;
}


HRESULT STDMETHODCALLTYPE Agent::OnLanguageChanged(void)
{
    DBGFNC(L"KeyboardMouse%d::Agent::OnLanguageChanged", BITNESS);
    HRESULT hr = m_pInputProcessorProfiles->GetCurrentLanguage(&m_pAgentBlock->Language);
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
    return S_OK;
}
