#include "pch.h"
#include <wchar.h>
#include "hnrt/ResourceString.h"
#include "hnrt/Heap.h"


using namespace hnrt;


HINSTANCE ResourceString::m_hInstance = NULL;


ResourceString::ResourceString(UINT uId)
    : m_psz(nullptr)
{
    LPWSTR psz = nullptr;
    size_t len = LoadStringW(m_hInstance, uId, reinterpret_cast<LPWSTR>(&psz), 0);
    if (len)
    {
        m_psz = Allocate<WCHAR>(len + 1);
        wmemcpy_s(m_psz, len + 1, psz, len);
        m_psz[len] = L'\0';
    }
}


ResourceString::~ResourceString()
{
    free(m_psz);
}
