#include "pch.h"
#include "hnrt/RefMbs.h"
#include "hnrt/Heap.h"
#include "hnrt/StringUtils.h"
#include "hnrt/Exception.h"
#undef _DEBUG
#include "hnrt/Debug.h"


using namespace hnrt;


void* RefMbs::operator new(size_t size, size_t cb)
{
    size_t required = (size_t)(((RefMbs*)0)->m_buf) + (cb + 1) * sizeof(CHAR);
    return Malloc(required > size ? required : size);
}


void RefMbs::operator delete(void* ptr)
{
    free(ptr);
}


void RefMbs::operator delete(void* ptr, size_t)
{
    free(ptr);
}


PCSTR RefMbs::Get(RefMbs* ptr)
{
    return &ptr->m_buf[0];
}


RefMbs& RefMbs::Get(PCSTR psz)
{
    static const size_t offset = (size_t)(((RefMbs*)0)->m_buf);
    return *(RefMbs*)((PBYTE)psz - offset);
}


PCSTR RefMbs::Create(PCSTR psz, INT_PTR cb)
{
    if (psz && cb)
    {
        cb = cb < 0 ? strlen(psz) : strnlen(psz, cb);
        return Get(new (cb) RefMbs(psz, cb));
    }
    else
    {
        return nullptr;
    }
}


PCSTR RefMbs::Create(PCSTR pszFormat, va_list argList)
{
    va_list argList2;
    va_copy(argList2, argList);
    INT_PTR cb = _vscprintf(pszFormat, argList2);
    va_end(argList2);
    if (cb < 0)
    {
        throw Exception(L"RefMbs::Create(PCSTR,va_list) failed.");
    }
    return Get(new (cb) RefMbs(pszFormat, argList, cb));
}


PCSTR RefMbs::Create(PCSTR psz1, PCSTR psz2, INT_PTR cb)
{
    if (psz1 && psz2 && cb)
    {
        size_t cb1 = strlen(psz1);
        size_t cb2 = cb < 0 ? strlen(psz2) : strnlen(psz2, cb);
        return Get(new (cb1 + cb2) RefMbs(psz1, cb1, psz2, cb2));
    }
    else
    {
        return nullptr;
    }
}


PCSTR RefMbs::Create(PCSTR psz1, PCSTR pszFormat, va_list argList)
{
    INT_PTR cb1 = strlen(psz1);
    va_list argList2;
    va_copy(argList2, argList);
    INT_PTR cb2 = _vscprintf(pszFormat, argList2);
    va_end(argList2);
    if (cb2 < 0)
    {
        throw Exception(L"RefMbs::Create(PCSTR,PCSTR,va_list) failed.");
    }
    return Get(new (cb1 + cb2) RefMbs(psz1, cb1, pszFormat, argList, cb2));
}


PCSTR RefMbs::Create(UINT cp, PCWSTR psz, INT_PTR cch)
{
    if (psz && cch)
    {
        cch = cch < 0 ? wcslen(psz) : wcsnlen(psz, cch);
        int cb = WideCharToMultiByte(cp, WC_ERR_INVALID_CHARS, psz, static_cast<int>(cch), NULL, 0, NULL, NULL);
        return Get(new (cb) RefMbs(cp, psz, static_cast<int>(cch), cb));
    }
    else
    {
        return nullptr;
    }
}


PCSTR RefMbs::Create(StringOptions option, PCSTR psz)
{
    switch (option)
    {
    case UPPERCASE:
    case LOWERCASE:
    {
        size_t cb = strlen(psz);
        return Get(new (cb) RefMbs(option, psz, cb));
    }
    case TRIM:
    case TRIM_HEAD:
    case TRIM_TAIL:
    {
        int start = 0;
        int end = 0;
        StringUtils::TrimScan(option, psz, start, end);
        int cb = end - start;
        return Get(new (cb) RefMbs(psz + start, cb));
    }
    default:
        throw Exception(L"RefMbs::Create(StringOptions,PCSTR) failed.");
    }
}


PCSTR RefMbs::Create(StringOptions option, PCSTR psz, va_list argList)
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
        size_t cb = StringUtils::VaConcatSize(option, psz, argList);
        return Get(new (cb) RefMbs(option, psz, argList, cb));
    }
    default:
        throw Exception(L"RefMbs::Create(StringOptions,PCSTR,va_list) failed.");
    }
}


PCSTR RefMbs::Create(size_t cb, CHAR fill)
{
    return Get(new (cb) RefMbs(cb, fill));
}


RefMbs::RefMbs(PCSTR psz, size_t cb)
    : RefObj()
    , m_cp(CP_ACP)
    , m_len(cb)
    , m_buf()
{
    memcpy_s(m_buf, m_len, psz, m_len);
    m_buf[m_len] = '\0';
}


RefMbs::RefMbs(PCSTR pszFormat, va_list argList, size_t cb)
    : RefObj()
    , m_cp(CP_ACP)
    , m_len(cb)
    , m_buf()
{
    if (_vsnprintf_s(&m_buf[0], cb + 1, _TRUNCATE, pszFormat, argList) != static_cast<int>(cb))
    {
        throw Exception(L"RefMbs::ctor(PCSTR,va_list,size_t) failed.");
    }
}


RefMbs::RefMbs(PCSTR psz1, size_t cb1, PCSTR psz2, size_t cb2)
    : RefObj()
    , m_cp(CP_ACP)
    , m_len(cb1 + cb2)
    , m_buf()
{
    memcpy_s(&m_buf[0], cb1, psz1, cb1);
    memcpy_s(&m_buf[cb1], cb2, psz2, cb2);
    m_buf[m_len] = '\0';
}


RefMbs::RefMbs(PCSTR psz1, size_t cb1, PCSTR pszFormat, va_list argList, size_t cb2)
    : RefObj()
    , m_cp(CP_ACP)
    , m_len(cb1 + cb2)
    , m_buf()
{
    memcpy_s(&m_buf[0], cb1, psz1, cb1);
    if (_vsnprintf_s(&m_buf[cb1], cb2 + 1, _TRUNCATE, pszFormat, argList) != static_cast<int>(cb2))
    {
        throw Exception(L"RefMbs::ctor(PCSTR,size_t,PCSTR,va_list,size_t) failed.");
    }
}


RefMbs::RefMbs(UINT cp, PCWSTR psz, int cch, int cb)
    : RefObj()
    , m_cp(cp)
    , m_len(cb)
    , m_buf()
{
    if (WideCharToMultiByte(cp, WC_ERR_INVALID_CHARS, psz, cch, m_buf, cb, NULL, NULL) != cb)
    {
        throw Exception(L"RefMbs::ctor(UINT,PCWSTR,int,int) failed.");
    }
    m_buf[m_len] = '\0';
}


RefMbs::RefMbs(StringOptions option, PCSTR psz, size_t cb)
    : RefObj()
    , m_cp(CP_ACP)
    , m_len(cb)
    , m_buf()
{
    switch (option)
    {
    case UPPERCASE:
    {
        memcpy_s(&m_buf[0], m_len, psz, m_len);
        m_buf[m_len] = '\0';
        StringUtils::Uppercase(&m_buf[0], m_len + 1);
        break;
    }
    case LOWERCASE:
    {
        memcpy_s(&m_buf[0], m_len, psz, m_len);
        m_buf[m_len] = '\0';
        StringUtils::Lowercase(&m_buf[0], m_len + 1);
        break;
    }
    default:
        throw Exception(L"RefMbs::Create(StringOptions,PCSTR,size_t) failed.");
    }
}


RefMbs::RefMbs(StringOptions option, PCSTR psz, va_list argList, size_t cb)
    : RefObj()
    , m_cp(CP_ACP)
    , m_len(cb)
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
        throw Exception(L"RefMbs::Create(StringOptions,PCSTR,va_list,size_t) failed.");
    }
}


RefMbs::RefMbs(size_t cb, CHAR fill)
    : RefObj()
    , m_cp(CP_ACP)
    , m_len(cb)
    , m_buf()
{
    memset(&m_buf[0], fill, m_len);
    m_buf[m_len] = '\0';
}


RefMbs::~RefMbs()
{
    DBGPUT(m_cp, "RefMbs::dtor@%p: %s", this, m_buf);
}


void RefMbs::SetLength(INT_PTR cb)
{
    m_len = cb < 0 ? strlen(&m_buf[0]) : strnlen(&m_buf[0], cb);
}


void RefMbs::ZeroFill()
{
    memset(&m_buf[0], '\0', m_len);
    m_len = 0;
}


void RefMbs::SetCodePage(UINT uCodePage)
{
    m_cp = uCodePage;
}
