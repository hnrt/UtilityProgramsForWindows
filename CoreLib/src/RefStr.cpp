#include "pch.h"
#include "hnrt/RefStr.h"
#include "hnrt/Heap.h"
#include "hnrt/StringUtils.h"
#include "hnrt/Exception.h"
#include "hnrt/Debug.h"


#pragma warning(disable:4291)


using namespace hnrt;


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
    if (psz && cch)
    {
        cch = cch < 0 ? wcslen(psz) : wcsnlen(psz, cch);
        return Get(new (cch) RefStr(psz, cch));
    }
    else
    {
        return nullptr;
    }
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
    if (psz1 && psz2 && cch)
    {
        size_t cch1 = wcslen(psz1);
        size_t cch2 = cch < 0 ? wcslen(psz2) : wcsnlen(psz2, cch);
        return Get(new (cch1 + cch2) RefStr(psz1, cch1, psz2, cch2));
    }
    else
    {
        return nullptr;
    }
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
    if (psz && cb)
    {
        cb = cb < 0 ? strlen(psz) : strnlen(psz, cb);
        int cch = MultiByteToWideChar(cp, MB_PRECOMPOSED, psz, static_cast<int>(cb), NULL, 0);
        return Get(new (cch) RefStr(cp, psz, static_cast<int>(cb), cch));
    }
    else
    {
        return nullptr;
    }
}


PCWSTR RefStr::Create(StringOptions option, PCWSTR psz)
{
    switch (option)
    {
    case UPPERCASE:
    case LOWERCASE:
    {
        size_t cch = wcslen(psz);
        return Get(new (cch) RefStr(option, psz, cch));
    }
    case TRIM:
    case TRIM_HEAD:
    case TRIM_TAIL:
    {
        int start = 0;
        int end = 0;
        StringUtils::TrimScan(option, psz, start, end);
        int cch = end - start;
        return Get(new (cch) RefStr(psz + start, cch));
    }
    default:
        throw Exception(L"RefStr::Create(StringOptions,PCWSTR) failed.");
    }
}


PCWSTR RefStr::Create(StringOptions option, PCWSTR psz, va_list argList)
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
        size_t cch = StringUtils::VaConcatSize(option, psz, argList);
        return Get(new (cch) RefStr(option, psz, argList, cch));
    }
    default:
        throw Exception(L"RefStr::Create(StringOptions,PCWSTR,va_list) failed.");
    }
}


PCWSTR RefStr::Create(size_t cch, WCHAR fill)
{
    return Get(new (cch) RefStr(cch, fill));
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


RefStr::RefStr(StringOptions option, PCWSTR psz, size_t cch)
    : RefObj()
    , m_len(cch)
    , m_buf()
{
    switch (option)
    {
    case UPPERCASE:
    {
        wmemcpy_s(&m_buf[0], m_len, psz, m_len);
        m_buf[m_len] = L'\0';
        StringUtils::Uppercase(&m_buf[0], m_len + 1);
        break;
    }
    case LOWERCASE:
    {
        wmemcpy_s(&m_buf[0], m_len, psz, m_len);
        m_buf[m_len] = L'\0';
        StringUtils::Lowercase(&m_buf[0], m_len + 1);
        break;
    }
    default:
        throw Exception(L"RefStr::Create(StringOptions,PCWSTR,size_t) failed.");
    }
}


RefStr::RefStr(StringOptions option, PCWSTR psz, va_list argList, size_t cch)
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
        StringUtils::VaConcat(option, psz, argList, &m_buf[0]);
        break;
    }
    default:
        throw Exception(L"RefStr::Create(StringOptions,PCWSTR,va_list,size_t) failed.");
    }
}


RefStr::RefStr(size_t cch, WCHAR fill)
    : RefObj()
    , m_len(cch)
    , m_buf()
{
    wmemset(&m_buf[0], fill, m_len);
    m_buf[m_len] = L'\0';
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
