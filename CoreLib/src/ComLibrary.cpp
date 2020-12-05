#include "pch.h"
#include "hnrt/ComLibrary.h"
#include "hnrt/Debug.h"
#include "hnrt/StringBuffer.h"


using namespace hnrt;


struct CoInitFlags
{
    wchar_t Text[64];
    CoInitFlags(DWORD dwCoInit)
    {
        StringBuffer sb(64);
        if ((dwCoInit & COINIT_APARTMENTTHREADED)) sb.AppendFormat(L"|APARTMENTTHREADED");
        else sb.AppendFormat(L"|MULTITHREADED");
        if ((dwCoInit & COINIT_DISABLE_OLE1DDE)) sb.AppendFormat(L"|DISABLE_OLE1DDE");
        if ((dwCoInit & COINIT_SPEED_OVER_MEMORY)) sb.AppendFormat(L"|SPEED_OVER_MEMORY");
        wcscpy_s(Text, sb.Ptr + 1);
    }
    CoInitFlags(const CoInitFlags&) = delete;
    ~CoInitFlags() = default;
    void operator =(const CoInitFlags&) = delete;
};


ComLibrary::ComLibrary(DWORD dwCoInit)
    : m_hrInit(S_FALSE)
{
    DBGFNC(L"ComLibrary::ctor");
    m_hrInit = CoInitializeEx(NULL, dwCoInit);
    DBGPUT(L"CoInitializeEx(%s): %s", CoInitFlags(dwCoInit).Text, ComLibrary::ToString(m_hrInit));
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


PCWSTR ComLibrary::ToString(HRESULT hr)
{
    switch (hr)
    {
#define CASE(x) case x: return L#x
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
    static unsigned long next = 0;
    static wchar_t buf[16][10];
    unsigned long index = _InterlockedIncrement(&next) % 16;
    _snwprintf_s(buf[index], _TRUNCATE, L"%08X", static_cast<int>(hr));
    return buf[index];
}
