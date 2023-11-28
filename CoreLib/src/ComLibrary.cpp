#include "pch.h"
#include "hnrt/ComLibrary.h"
#include "hnrt/Debug.h"
#include "hnrt/StringBuffer.h"


using namespace hnrt;


class CoInitFlags
{
public:

    CoInitFlags(DWORD dwCoInit)
        : m_buf(64)
    {
        if ((dwCoInit & COINIT_APARTMENTTHREADED)) m_buf += L"|APARTMENTTHREADED";
        else m_buf += L"|MULTITHREADED";
        if ((dwCoInit & COINIT_DISABLE_OLE1DDE)) m_buf += L"|DISABLE_OLE1DDE";
        if ((dwCoInit & COINIT_SPEED_OVER_MEMORY)) m_buf += L"|SPEED_OVER_MEMORY";
    }
    CoInitFlags(const CoInitFlags&) = delete;
    ~CoInitFlags() = default;
    void operator =(const CoInitFlags&) = delete;
    PCWSTR get_Text() const
    {
        return m_buf.Ptr + 1;
    }
    __declspec(property(get = get_Text)) PCWSTR Text;

private:

    StringBuffer m_buf;
};


ComLibrary::ComLibrary(DWORD dwCoInit)
    : m_hrInit(S_FALSE)
{
    DBGFNC(L"ComLibrary::ctor");
    m_hrInit = CoInitializeEx(NULL, dwCoInit);
    DBGPUT(L"CoInitializeEx(%s): %s", CoInitFlags(dwCoInit).Text, ComLibrary::ToString(m_hrInit).Ptr);
}


ComLibrary::~ComLibrary()
{
    DBGFNC(L"ComLibrary::dtor");
    if (m_hrInit == S_OK || m_hrInit == S_FALSE)
    {
        CoUninitialize();
        DBGPUT(L"CoUninitialize");
    }
}


String ComLibrary::ToString(HRESULT hr)
{
    switch (hr)
    {
#define CASE(x) case x: return String(L#x)
        CASE(S_OK);
        CASE(S_FALSE);
        CASE(RPC_E_CHANGED_MODE);
        CASE(E_UNEXPECTED);
        CASE(E_NOTIMPL);
        CASE(E_OUTOFMEMORY);
        CASE(E_INVALIDARG);
        CASE(E_NOINTERFACE);
        CASE(E_POINTER);
        CASE(E_HANDLE);
        CASE(E_ABORT);
        CASE(E_FAIL);
        CASE(E_ACCESSDENIED);
        CASE(E_PENDING);
        CASE(E_BOUNDS);
        CASE(E_CHANGED_STATE);
        CASE(E_ILLEGAL_STATE_CHANGE);
        CASE(E_ILLEGAL_METHOD_CALL);
#undef  CASE
    default: break;
    }
    return String(PRINTF, L"%08X", static_cast<int>(hr));
}
