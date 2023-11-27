#include "pch.h"
#include "hnrt/RefStr.h"
#include "hnrt/Heap.h"
#include "hnrt/StringUtils.h"
#include "hnrt/Exception.h"
#include "hnrt/Debug.h"


using namespace hnrt;


#pragma warning(disable:4291)


void* RefStr::operator new(size_t size, size_t cch)
{
	return Malloc(size + cch * sizeof(WCHAR));
}


void RefStr::operator delete(void* ptr)
{
	free(ptr);
}


PCWSTR RefStr::Get(RefStr* ptr)
{
    return &ptr->m_buf[0];
}


RefStr& RefStr::Get(PCWSTR psz)
{
    static const size_t offset = (size_t)(((RefStr*)0)->m_buf);
    return *(RefStr*)((PBYTE)psz - offset);
}


PCWSTR RefStr::Create(PCWSTR psz, INT_PTR cch)
{
    cch = cch > 0 ? wcsnlen(psz, cch) : cch < 0 ? wcslen(psz) : 0;
	return Get(new (cch) RefStr(psz, cch));
}


PCWSTR RefStr::Create(PCWSTR pszFormat, va_list argList)
{
    va_list argList2;
    va_copy(argList2, argList);
    INT_PTR cch = _vscwprintf(pszFormat, argList2);
    va_end(argList2);
    if (cch < 0)
    {
        throw Exception(L"RefStr::Create(PCWSTR,va_list) failed.");
    }
    return Get(new (cch) RefStr(pszFormat, argList, cch));
}


PCWSTR RefStr::Create(PCWSTR psz1, PCWSTR psz2, INT_PTR cch)
{
    size_t cch1 = wcslen(psz1);
    size_t cch2 = cch > 0 ? wcsnlen(psz2, cch) : cch < 0 ? wcslen(psz2) : 0;
    return Get(new (cch1 + cch2) RefStr(psz1, cch1, psz2, cch2));
}


PCWSTR RefStr::Create(PCWSTR psz1, PCWSTR pszFormat, va_list argList)
{
    INT_PTR cch1 = wcslen(psz1);
    va_list argList2;
    va_copy(argList2, argList);
    INT_PTR cch2 = _vscwprintf(pszFormat, argList2);
    va_end(argList2);
    if (cch2 < 0)
    {
        throw Exception(L"RefStr::Create(PCWSTR,PCWSTR,va_list) failed.");
    }
    return Get(new (cch1 + cch2) RefStr(psz1, cch1, pszFormat, argList, cch2));
}


PCWSTR RefStr::Create(UINT cp, PCSTR psz, INT_PTR cb)
{
    if (cb < 0)
    {
        cb = strlen(psz);
    }
    int cch = MultiByteToWideChar(cp, MB_PRECOMPOSED, psz, static_cast<int>(cb), NULL, 0);
    return Get(new (cch) RefStr(cp, psz, static_cast<int>(cb), cch));
}


PCWSTR RefStr::Create(StringOptions option, ...)
{
    RefStr* ptr;
    va_list argList;
    va_start(argList, option);
    switch (option)
    {
    case CONCAT:
    case CONCAT2:
    case CONCAT3:
    case CONCAT4:
    case CONCAT5:
    case CONCAT6:
    case CONCAT7:
    case CONCAT8:
    case CONCAT9:
    {
        va_list argList2;
        va_copy(argList2, argList);
        PCWSTR psz = va_arg(argList2, PCWSTR);
        size_t cch = StringUtils::VaConcatSize(option, psz, va_arg(argList2, va_list));
        va_end(argList2);
        ptr = new (cch) RefStr(option, argList, cch);
        break;
    }
    case UPPERCASE:
    case LOWERCASE:
    {
        va_list argList2;
        va_copy(argList2, argList);
        size_t cch = wcslen(va_arg(argList2, PCWSTR));
        va_end(argList2);
        ptr = new (cch) RefStr(option, argList, cch);
        break;
    }
    default:
        throw Exception(L"RefStr::Create(StringOptions) failed.");
    }
    va_end(argList);
    return Get(ptr);
}


RefStr::RefStr(PCWSTR psz, size_t cch)
    : RefObj()
    , m_len(cch)
    , m_buf()
{
    wmemcpy_s(m_buf, m_len, psz, m_len);
    m_buf[m_len] = L'\0';
}


RefStr::RefStr(PCWSTR pszFormat, va_list argList, size_t cch)
    : RefObj()
    , m_len(cch)
    , m_buf()
{
    if (_vsnwprintf_s(&m_buf[0], cch + 1, _TRUNCATE, pszFormat, argList) != static_cast<int>(cch))
    {
        throw Exception(L"RefStr::ctor(PCWSTR,va_list,size_t) failed.");
    }
}


RefStr::RefStr(PCWSTR psz1, size_t cch1, PCWSTR psz2, size_t cch2)
    : RefObj()
    , m_len(cch1 + cch2)
    , m_buf()
{
    wmemcpy_s(&m_buf[0], cch1, psz1, cch1);
    wmemcpy_s(&m_buf[cch1], cch2, psz2, cch2);
    m_buf[m_len] = L'\0';
}


RefStr::RefStr(PCWSTR psz1, size_t cch1, PCWSTR pszFormat, va_list argList, size_t cch2)
    : RefObj()
    , m_len(cch1 + cch2)
    , m_buf()
{
    wmemcpy_s(&m_buf[0], cch1, psz1, cch1);
    if (_vsnwprintf_s(&m_buf[cch1], cch2 + 1, _TRUNCATE, pszFormat, argList) != static_cast<int>(cch2))
    {
        throw Exception(L"RefStr::ctor(PCWSTR,size_t,PCWSTR,va_list,size_t) failed.");
    }
}


RefStr::RefStr(UINT cp, PCSTR psz, int cb, int cch)
    : RefObj()
    , m_len(cch)
    , m_buf()
{
    if (MultiByteToWideChar(cp, MB_PRECOMPOSED, psz, cb, m_buf, cch) != cch)
    {
        throw Exception(L"RefStr::ctor(UINT,PCSTR,int,int) failed.");
    }
    m_buf[m_len] = L'\0';
}


RefStr::RefStr(StringOptions option, va_list argList, size_t cch)
    : RefObj()
    , m_len(cch)
    , m_buf()
{
    switch (option)
    {
    case CONCAT:
    case CONCAT2:
    case CONCAT3:
    case CONCAT4:
    case CONCAT5:
    case CONCAT6:
    case CONCAT7:
    case CONCAT8:
    case CONCAT9:
    {
        PCWSTR psz = va_arg(argList, PCWSTR);
        va_list argList2 = va_arg(argList, va_list);
        StringUtils::VaConcat(option, psz, argList2, &m_buf[0]);
        break;
    }
    case UPPERCASE:
    {
        wmemcpy_s(&m_buf[0], m_len, va_arg(argList, PCWSTR), m_len);
        m_buf[m_len] = L'\0';
        StringUtils::Uppercase(&m_buf[0]);
        break;
    }
    case LOWERCASE:
    {
        wmemcpy_s(&m_buf[0], m_len, va_arg(argList, PCWSTR), m_len);
        m_buf[m_len] = L'\0';
        StringUtils::Lowercase(&m_buf[0]);
        break;
    }
    default:
        throw Exception(L"RefStr::Create(StringOptions) failed.");
    }
}


RefStr::~RefStr()
{
    DBGPUT(L"RefStr::dtor@%p: %s", this, m_buf);
}


void RefStr::ZeroFill()
{
    wmemset(&m_buf[0], L'\0', m_len);
    m_len = 0;
}
