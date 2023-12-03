#include "pch.h"
#include <stack>
#include <locale.h>
#include "hnrt/Debug.h"
#include "hnrt/Exception.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/Buffer.h"


using namespace hnrt;


////////////////////////////////////////////////////////////////////////////
////
////  F U N C T I O N   N A M E   S T A C K
////
////////////////////////////////////////////////////////////////////////////


typedef std::stack<PWSTR> FuncNameStack;


static DWORD s_dwTlsIndex = TlsAlloc();


static PCWSTR GetFuncName()
{
    FuncNameStack* pStack = reinterpret_cast<FuncNameStack*>(TlsGetValue(s_dwTlsIndex));
    if (pStack && pStack->size())
    {
        return pStack->top();
    }
    else
    {
        return nullptr;
    }
}


static void PushFuncName(PWSTR pszFuncName)
{
    FuncNameStack* pStack = reinterpret_cast<FuncNameStack*>(TlsGetValue(s_dwTlsIndex));
    if (!pStack)
    {
        pStack = new FuncNameStack();
        TlsSetValue(s_dwTlsIndex, pStack);
    }
    pStack->push(pszFuncName);
}


static PWSTR PopFuncName()
{
    FuncNameStack* pStack = reinterpret_cast<FuncNameStack*>(TlsGetValue(s_dwTlsIndex));
    if (!pStack)
    {
        throw Exception(L"Debug::PopFuncName: Null stack.");
    }
    if (!pStack->size())
    {
        throw Exception(L"Debug::PopFuncName: Empty stack.");
    }
    PWSTR pszFuncName = pStack->top();
    pStack->pop();
    return pszFuncName;
}


////////////////////////////////////////////////////////////////////////////
////
////  D E B U G
////
////////////////////////////////////////////////////////////////////////////


void Debug::Put(PCWSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    VaPut(pszFormat, argList);
    va_end(argList);
}


void Debug::VaPut(PCWSTR pszFormat, va_list argList)
{
    StringBuffer buf(128);
    PCWSTR pszFuncName = GetFuncName();
    if (pszFuncName)
    {
        buf.AppendFormat(L"%s: ", pszFuncName);
    }
    buf.VaAppendFormat(pszFormat, argList);
    if (buf.Len && buf[buf.Len - 1] != L'\n')
    {
        buf.AppendFormat(L"\n");
    }
    OutputDebugStringW(buf);
}


void Debug::Put(PCSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    VaPut((UINT)CP_ACP, pszFormat, argList);
    va_end(argList);
}


void Debug::VaPut(PCSTR pszFormat, va_list argList)
{
    VaPut((UINT)CP_ACP, pszFormat, argList);
}


void Debug::Put(UINT cp, PCSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    VaPut(cp, pszFormat, argList);
    va_end(argList);
}


void Debug::VaPut(INT cp, PCSTR pszFormat, va_list argList)
{
    static _locale_t locale = _create_locale(LC_ALL, "C");
    if (!*pszFormat)
    {
        return;
    }
    int cb = _vscprintf_l(pszFormat, locale, argList);
    if (cb < 1)
    {
        OutputDebugStringW(L"Debug::VaPut: _vscprintf failed.\n");
        return;
    }
    int size1 = cb + 1;
    Buffer<CHAR> buf1(size1);
    _vsnprintf_s_l(buf1, size1, _TRUNCATE, pszFormat, locale, argList);
    int cch = MultiByteToWideChar(cp, MB_PRECOMPOSED, buf1, cb, NULL, 0);
    if (cch < 1)
    {
        if (cp == CP_ACP)
        {
            cch = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, buf1, cb, NULL, 0);
        }
        if (cch < 1)
        {
            StringBuffer buf(128);
            buf.Append(L"Debug::VaPut: MultiByteToWideChar failed:");
            for (PCHAR pCur = buf1; *pCur; pCur++)
            {
                buf.AppendFormat(L" %02X", *pCur & 0xFF);
            }
            buf.Append(L"\n");
            OutputDebugStringW(buf);
            return;
        }
    }
    int size2 = cch + 1;
    Buffer<WCHAR> buf2(size2);
    MultiByteToWideChar(cp, MB_PRECOMPOSED, buf1, cb, buf2, cch);
    buf2[cch] = L'\0';
    StringBuffer buf(128);
    PCWSTR pszFuncName = GetFuncName();
    if (pszFuncName)
    {
        buf.AppendFormat(L"%s: ", pszFuncName);
    }
    buf.Append(buf2);
    if (buf.Len && buf[buf.Len - 1] != L'\n')
    {
        buf.AppendFormat(L"\n");
    }
    OutputDebugStringW(buf);
}


////////////////////////////////////////////////////////////////////////////
////
////  D E B U G F U N C
////
////////////////////////////////////////////////////////////////////////////


DebugFunc::DebugFunc(PCWSTR pszFormat, ...)
{
    StringBuffer buf(64);
    buf.AppendFormat(L"%lu/", GetCurrentThreadId());
    va_list argList;
    va_start(argList, pszFormat);
    buf.VaAppendFormat(pszFormat, argList);
    va_end(argList);
    PushFuncName(buf.Detach());
    Debug::Put(L"Started.");
}


DebugFunc::~DebugFunc()
{
    Debug::Put(L"Ended.");
    free(PopFuncName());
}
