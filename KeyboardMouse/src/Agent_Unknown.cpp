#include "pch.h"
#include "Agent.h"
#include "hnrt/Debug.h"


using namespace hnrt;


HRESULT STDMETHODCALLTYPE Agent::QueryInterface(
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject)
{
    if (!ppvObject)
    {
        return E_POINTER;
    }
    else if (IsEqualIID(riid, IID_IUnknown))
    {
        *ppvObject = (ITfCompartmentEventSink*)this;
        m_ulRefCnt++;
        return S_OK;
    }
    else if (IsEqualIID(riid, IID_ITfCompartmentEventSink))
    {
        *ppvObject = (ITfCompartmentEventSink*)this;
        m_ulRefCnt++;
        return S_OK;
    }
    else if (IsEqualIID(riid, IID_ITfLanguageProfileNotifySink))
    {
        *ppvObject = (ITfLanguageProfileNotifySink*)this;
        m_ulRefCnt++;
        return S_OK;
    }
    else
    {
        return E_NOINTERFACE;
    }
}


ULONG STDMETHODCALLTYPE Agent::AddRef(void)
{
    ++m_ulRefCnt;
    DBGPUT(L"Agent::AddRef: return=%lu", m_ulRefCnt);
    return m_ulRefCnt;
}


ULONG STDMETHODCALLTYPE Agent::Release(void)
{
    if (--m_ulRefCnt == 0)
    {
        DBGPUT(L"Agent::Release: return=0");
        delete this;
        return 0;
    }
    DBGPUT(L"Agent::Release: return=%ld", m_ulRefCnt);
    return m_ulRefCnt;
}
