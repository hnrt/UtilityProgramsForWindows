#include "pch.h"
#include <stack>
#include <string>
#include <locale.h>
#include "hnrt/Debug.h"
#include "hnrt/Buffer.h"


using namespace hnrt;


////////////////////////////////////////////////////////////////////////////
////
////  F U N C T I O N   N A M E   S T A C K
////
////////////////////////////////////////////////////////////////////////////


typedef std::stack<std::wstring> FuncNameStack;


static DWORD s_dwTlsIndex = TlsAlloc();


static const wchar_t* GetFuncName()
{
    FuncNameStack* pStack = reinterpret_cast<FuncNameStack*>(TlsGetValue(s_dwTlsIndex));
    if (pStack && pStack->size())
    {
        return pStack->top().c_str();
    }
    else
    {
        return L"";
    }
}


static void PushFuncName(const wchar_t* pszFuncName)
{
    FuncNameStack* pStack = reinterpret_cast<FuncNameStack*>(TlsGetValue(s_dwTlsIndex));
    if (!pStack)
    {
        pStack = new FuncNameStack();
        TlsSetValue(s_dwTlsIndex, pStack);
    }
    pStack->push(std::wstring(pszFuncName));
}


static void PopFuncName()
{
    FuncNameStack* pStack = reinterpret_cast<FuncNameStack*>(TlsGetValue(s_dwTlsIndex));
    if (!pStack)
    {
        OutputDebugStringW(L"Debug::PopFuncName: Null stack.");
        return;
    }
    if (!pStack->size())
    {
        OutputDebugStringW(L"Debug::PopFuncName: Empty stack.");
        return;
    }
    pStack->pop();
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
    static const wchar_t szLeaderFormat[] = L"%s: ";
    const wchar_t* pszFuncName = GetFuncName();
    int cch1 = *pszFuncName ? _scwprintf(szLeaderFormat, pszFuncName) : 0;
    if (cch1 < 0)
    {
        OutputDebugStringW(L"Debug::VaPut: _scwprintf failed.\n");
        return;
    }
    va_list argList2;
    va_copy(argList2, argList);
    int cch2 = _vscwprintf(pszFormat, argList2);
    va_end(argList2);
    if (cch2 < 0)
    {
        OutputDebugStringW(L"Debug::VaPut: _vscwprintf failed.\n");
        return;
    }
    int cch = cch1 + cch2;
    if (cch > 0)
    {
        Buffer<wchar_t> buf(cch + 2);
        if (cch1 > 0)
        {
            _snwprintf_s(buf, buf.Len, _TRUNCATE, szLeaderFormat, pszFuncName);
        }
        if (cch2 > 0)
        {
            _vsnwprintf_s(buf + cch1, buf.Len - cch1, _TRUNCATE, pszFormat, argList);
        }
        if (buf[cch - 1] == L'\n')
        {
            buf[cch + 0] = L'\0';
        }
        else
        {
            buf[cch + 0] = L'\n';
            buf[cch + 1] = L'\0';
        }
        OutputDebugStringW(buf);
    }
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


static _locale_t CreateLocale(INT codepage)
{
    char szSpec[16] = { 0 };
    _snprintf_s(szSpec, _TRUNCATE, codepage == CP_ACP ? ".ACP" : ".%d", codepage);
    return _create_locale(LC_ALL, szSpec);
}


struct LocaleHelper
{
    _locale_t locale;

    LocaleHelper(INT codepage)
        : locale(CreateLocale(codepage))
    {
    }

    ~LocaleHelper()
    {
        _free_locale(locale);
    }
};


void Debug::VaPut(INT cp, PCSTR pszFormat, va_list argList)
{
    static const char szLeaderFormat[] = "%S: ";
    LocaleHelper h(cp);
    const wchar_t* pszFuncName = GetFuncName();
    int cb1 = *pszFuncName ? _scprintf(szLeaderFormat, pszFuncName) : 0;
    if (cb1 < 0)
    {
        OutputDebugStringW(L"Debug::VaPut: _scprintf failed.\n");
        return;
    }
    va_list argList2;
    va_copy(argList2, argList);
    int cb2 = _vscprintf_l(pszFormat, h.locale, argList2);
    va_end(argList2);
    if (cb2 < 0)
    {
        OutputDebugStringW(L"Debug::VaPut: _vscprintf_l failed.\n");
        return;
    }
    int cb = cb1 + cb2;
    if (cb > 0)
    {
        Buffer<char> buf(cb + 2);
        if (cb1 > 0)
        {
            _snprintf_s(buf, buf.Len, _TRUNCATE, szLeaderFormat, pszFuncName);
        }
        if (cb2 > 0)
        {
            _vsnprintf_s_l(buf + cb1, buf.Len - cb1, _TRUNCATE, pszFormat, h.locale, argList);
        }
        if (buf[cb - 1] == '\n')
        {
            buf[cb + 0] = '\0';
            cb++;
        }
        else
        {
            buf[cb + 0] = '\n';
            buf[cb + 1] = '\0';
            cb += 2;
        }
        int cch = MultiByteToWideChar(cp, MB_PRECOMPOSED, buf, cb, nullptr, 0);
        if (cch < 0)
        {
            OutputDebugStringW(L"Debug::VaPut: MultiByteToWideChar failed.\n");
            return;
        }
        Buffer<wchar_t> buf2(cch);
        MultiByteToWideChar(cp, MB_PRECOMPOSED, buf, cb, buf2, cch);
        OutputDebugStringW(buf2);
    }
}


////////////////////////////////////////////////////////////////////////////
////
////  D E B U G F U N C
////
////////////////////////////////////////////////////////////////////////////


DebugFunc::DebugFunc(PCWSTR pszFormat, ...)
{
    static const wchar_t szUnknownFunction[] = L"?";
    static const wchar_t szThreadIdFormat[] = L"%lu/";
    DWORD dwThreadId = GetCurrentThreadId();
    int cch1 = _scwprintf(szThreadIdFormat, dwThreadId);
    if (cch1 < 0)
    {
        OutputDebugStringW(L"DebugFunc::ctor: _scwprintf failed.\n");
        PushFuncName(szUnknownFunction);
        Debug::Put(L"Started.");
        return;
    }
    va_list argList2;
    va_start(argList2, pszFormat);
    int cch2 = _vscwprintf(pszFormat, argList2);
    va_end(argList2);
    if (cch2 < 0)
    {
        OutputDebugStringW(L"DebugFunc::ctor: _vscwprintf failed.\n");
        PushFuncName(szUnknownFunction);
        Debug::Put(L"Started.");
        return;
    }
    Buffer<wchar_t> buf(cch1 + cch2 + 1);
    _snwprintf_s(buf, buf.Len, _TRUNCATE, szThreadIdFormat, dwThreadId);
    va_list argList;
    va_start(argList, pszFormat);
    _vsnwprintf_s(buf + cch1, buf.Len - cch1, _TRUNCATE, pszFormat, argList);
    va_end(argList);
    PushFuncName(buf);
    Debug::Put(L"Started.");
}


DebugFunc::~DebugFunc()
{
    Debug::Put(L"Ended.");
    PopFuncName();
}
