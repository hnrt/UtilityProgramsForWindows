#include "pch.h"
#include "hnrt/DoubleNullTerminatedStringArray.h"
#include "hnrt/Heap.h"
#include "hnrt/Exception.h"


using namespace hnrt;


DoubleNullTerminatedStringArray::DoubleNullTerminatedStringArray()
    : m_pszz(nullptr)
    , m_count(0)
{
}


DoubleNullTerminatedStringArray::DoubleNullTerminatedStringArray(const DoubleNullTerminatedStringArray& src)
    : m_pszz(nullptr)
    , m_count(0)
{
    operator =(src);
}


DoubleNullTerminatedStringArray::~DoubleNullTerminatedStringArray()
{
    free(m_pszz);
}


DoubleNullTerminatedStringArray& DoubleNullTerminatedStringArray::operator =(const DoubleNullTerminatedStringArray& src)
{
    if (src.m_pszz)
    {
        size_t count = 0;
        wchar_t* pCur = src.m_pszz;
        while (true)
        {
            count++;
            pCur += wcslen(pCur) + 1;
            if (!*pCur)
            {
                break;
            }
        }
        pCur++;
        m_pszz = Allocate(m_pszz, pCur - src.m_pszz);
        size_t cb = (pCur - src.m_pszz) * sizeof(wchar_t);
        memcpy_s(m_pszz, cb, src.m_pszz, cb);
        m_count = count;
    }
    else if (m_pszz)
    {
        Deallocate(m_pszz);
        m_count = 0;
    }

    return *this;
}


PCWSTR DoubleNullTerminatedStringArray::operator [](ULONG index)
{
    if (index >= m_count)
    {
        throw Exception(L"DoubleNullTerminatedStringArray::operator []: Index out of range.");
    }

    wchar_t* pCur = m_pszz;

    unsigned long skip = index;
    while (skip--)
    {
        pCur += wcslen(pCur) + 1;
        if (!*pCur)
        {
            throw Exception(L"DoubleNullTerminatedStringArray::operator []: Unexpected end of data.");
        }
    }

    return pCur;
}


DoubleNullTerminatedStringArray& DoubleNullTerminatedStringArray::Clear()
{
    if (m_pszz)
    {
        Deallocate(m_pszz);
        m_count = 0;
    }

    return *this;
}


DoubleNullTerminatedStringArray& DoubleNullTerminatedStringArray::Add(PCWSTR psz)
{
    if (!psz || !*psz)
    {
        throw Exception(L"DoubleNullTerminatedStringArray::Add: Null or empty string not allowed.");
    }

    size_t count = 0;
    size_t cch1;
    if (m_pszz)
    {
        wchar_t* pCur = m_pszz;
        while (true)
        {
            count++;
            pCur += wcslen(pCur) + 1;
            if (!*pCur)
            {
                break;
            }
        }
        cch1 = pCur - m_pszz;
    }
    else
    {
        cch1 = 0;
    }

    count++;
    size_t cch2 = wcslen(psz) + 1;

    wchar_t* pszz = Allocate(m_pszz, cch1 + cch2 + 1);
    size_t cb = (cch2 + 1) * sizeof(wchar_t);
    memcpy_s(pszz + cch1, (cch2 + 1) * sizeof(wchar_t), psz, cch2 * sizeof(wchar_t));
    pszz[cch1 + cch2] = L'\0';

    m_pszz = pszz;
    m_count = count;

    return *this;
}
