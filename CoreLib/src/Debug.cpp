#include "pch.h"
#include <stack>
#include "hnrt/Debug.h"
#include "hnrt/Exception.h"
#include "hnrt/StringBuffer.h"


using namespace hnrt;


////////////////////////////////////////////////////////////////////////////
////
////  F U N C T I O N   N A M E   S T A C K
////
////////////////////////////////////////////////////////////////////////////


typedef std::stack<wchar_t*> FuncNameStack;


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


static void PushFuncName(wchar_t* pszFuncName)
{
    FuncNameStack* pStack = reinterpret_cast<FuncNameStack*>(TlsGetValue(s_dwTlsIndex));
    if (!pStack)
    {
        pStack = new FuncNameStack();
        TlsSetValue(s_dwTlsIndex, pStack);
    }
    pStack->push(pszFuncName);
}


static wchar_t* PopFuncName()
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
    wchar_t* pszFuncName = pStack->top();
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
