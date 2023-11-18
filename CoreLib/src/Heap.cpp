#include "pch.h"
#include <stdexcept>
#include "hnrt/Heap.h"
#include "hnrt/Debug.h"
#include "hnrt/Exception.h"


void* hnrt::Malloc(size_t nbytes)
{
    void* ptr = malloc(nbytes);
    if (!ptr)
    {
        throw std::bad_alloc();
    }
    return ptr;
}


void* hnrt::Realloc(void* ptr, size_t nbytes)
{
#pragma warning(push)
#pragma warning(disable:6308)
    // warning C6308:
    // realloc may return null pointer:
    // assigning a null pointer to <variable>, which is passed as an argument to realloc, will cause the original memory block to be leaked
    ptr = realloc(ptr, nbytes);
#pragma warning(pop)
    if (!ptr)
    {
        throw std::bad_alloc();
    }
    return ptr;
}


void* hnrt::Calloc(size_t count, size_t nbytes)
{
    void* ptr = calloc(count, nbytes);
    if (!ptr)
    {
        throw std::bad_alloc();
    }
    return ptr;
}


PSTR hnrt::Clone(PCSTR psz)
{
    PSTR psz2 = _strdup(psz);
    if (!psz2)
    {
        throw std::bad_alloc();
    }
    return psz2;
}


PSTR hnrt::Clone(PCSTR psz, size_t cb)
{
    PSTR psz2 = reinterpret_cast<PSTR>(Malloc(cb + 1));
    size_t len = strnlen(psz, cb);
    memcpy_s(psz2, len, psz, len);
    memset(psz2 + len, '\0', cb + 1 - len);
    return psz2;
}


PSTR* hnrt::Clone(PSTR* ppsz, size_t count, size_t size)
{
    if (size < count)
    {
        size = count;
    }
    if (size)
    {
        size_t* ptr = reinterpret_cast<size_t*>(Malloc(sizeof(size_t) + size * sizeof(PSTR)));
        ptr[0] = size;
        PSTR* ppsz2 = reinterpret_cast<PSTR*>(&ptr[1]);
        for (size_t index = 0; index < count; index++)
        {
            ppsz2[index] = Clone(ppsz[index]);
        }
        if (count < size)
        {
            memset(&ppsz2[count], 0, (size - count) * sizeof(ppsz2[0]));
        }
        return ppsz2;
    }
    else
    {
        return nullptr;
    }
}


PSTR* hnrt::Resize(PSTR* ppsz, size_t size)
{
    if (ppsz)
    {
        size_t* ptr = &reinterpret_cast<size_t*>(ppsz)[-1];
        size_t size0 = ptr[0];
        if (size0 < size)
        {
            ptr = reinterpret_cast<size_t*>(Realloc(ptr, sizeof(size_t) + size * sizeof(ppsz[0])));
            ptr[0] = size;
            ppsz = reinterpret_cast<PSTR*>(&ptr[1]);
            memset(&ppsz[size0], 0, (size - size0) * sizeof(ppsz[0]));
        }
        else if (size < size0)
        {
            for (size_t index = size; index < size0; index++)
            {
                free(ppsz[index]);
            }
            memset(&ppsz[size], 0, (size0 - size) * sizeof(ppsz[0]));
            if (size > 0)
            {
                ptr = reinterpret_cast<size_t*>(Realloc(ptr, sizeof(size_t) + size * sizeof(ppsz[0])));
                ptr[0] = size;
                ppsz = reinterpret_cast<PSTR*>(&ptr[1]);
            }
            else
            {
                free(ptr);
                ppsz = nullptr;
            }
        }
    }
    else if (size)
    {
        size_t* ptr = reinterpret_cast<size_t*>(Malloc(sizeof(size_t) + size * sizeof(ppsz[0])));
        ptr[0] = size;
        ppsz = reinterpret_cast<PSTR*>(&ptr[1]);
        memset(&ppsz[0], 0, size * sizeof(ppsz[0]));
    }
    return ppsz;
}


void hnrt::Free(PSTR* ppsz)
{
    if (ppsz)
    {
        size_t* ptr = &reinterpret_cast<size_t*>(ppsz)[-1];
        size_t size = ptr[0];
        for (size_t index = 0; index < size; index++)
        {
            free(ppsz[index]);
        }
        memset(&ppsz[0], 0, size * sizeof(ppsz[0]));
        free(ptr);
    }
}


DWORD hnrt::ArraySize(PSTR* ppsz)
{
    return ppsz ? static_cast<DWORD>(reinterpret_cast<size_t*>(ppsz)[-1]) : 0;
}


PSTR hnrt::Format(PCSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    PSTR psz = VaFormat(pszFormat, argList);
    va_end(argList);
    return psz;
}


PSTR hnrt::VaFormat(PCSTR pszFormat, va_list argList)
{
    va_list argList2;
    va_copy(argList2, argList);
    LONG_PTR cb = static_cast<LONG_PTR>(_vscprintf(pszFormat, argList));
    va_end(argList2);
    if (cb < 0)
    {
        throw Exception(L"VaFormat(PCSTR) failed.");
    }
    PSTR psz = reinterpret_cast<PSTR>(Malloc(++cb));
    _vsnprintf_s(psz, cb, _TRUNCATE, pszFormat, argList);
    return psz;
}


PSTR hnrt::Concat(PCSTR psz1, PCSTR psz2)
{
    size_t cb1 = strlen(psz1);
    size_t cb2 = strlen(psz2);
    PSTR psz = reinterpret_cast<PSTR>(Malloc(cb1 + cb2 + 1));
    PSTR p = psz;
    memcpy_s(p, cb1, psz1, cb1); p += cb1;
    memcpy_s(p, cb2, psz2, cb2); p += cb2;
    *p = '\0';
    return psz;
}


PSTR hnrt::Concat(PCSTR psz1, PCSTR psz2, PCSTR psz3)
{
    size_t cb1 = strlen(psz1);
    size_t cb2 = strlen(psz2);
    size_t cb3 = strlen(psz3);
    PSTR psz = reinterpret_cast<PSTR>(Malloc(cb1 + cb2 + cb3 + 1));
    PSTR p = psz;
    memcpy_s(p, cb1, psz1, cb1); p += cb1;
    memcpy_s(p, cb2, psz2, cb2); p += cb2;
    memcpy_s(p, cb3, psz3, cb3); p += cb3;
    *p = '\0';
    return psz;
}


PSTR hnrt::Concat(PCSTR psz1, PCSTR psz2, PCSTR psz3, PCSTR psz4)
{
    size_t cb1 = strlen(psz1);
    size_t cb2 = strlen(psz2);
    size_t cb3 = strlen(psz3);
    size_t cb4 = strlen(psz4);
    PSTR psz = reinterpret_cast<PSTR>(Malloc(cb1 + cb2 + cb3 + cb4 + 1));
    PSTR p = psz;
    memcpy_s(p, cb1, psz1, cb1); p += cb1;
    memcpy_s(p, cb2, psz2, cb2); p += cb2;
    memcpy_s(p, cb3, psz3, cb3); p += cb3;
    memcpy_s(p, cb4, psz4, cb4); p += cb4;
    *p = '\0';
    return psz;
}


PSTR hnrt::Concat(PCSTR psz1, PCSTR psz2, PCSTR psz3, PCSTR psz4, PCSTR psz5)
{
    size_t cb1 = strlen(psz1);
    size_t cb2 = strlen(psz2);
    size_t cb3 = strlen(psz3);
    size_t cb4 = strlen(psz4);
    size_t cb5 = strlen(psz5);
    PSTR psz = reinterpret_cast<PSTR>(Malloc(cb1 + cb2 + cb3 + cb4 + cb5 + 1));
    PSTR p = psz;
    memcpy_s(p, cb1, psz1, cb1); p += cb1;
    memcpy_s(p, cb2, psz2, cb2); p += cb2;
    memcpy_s(p, cb3, psz3, cb3); p += cb3;
    memcpy_s(p, cb4, psz4, cb4); p += cb4;
    memcpy_s(p, cb5, psz5, cb5); p += cb5;
    *p = '\0';
    return psz;
}


PSTR hnrt::ToAcp(PCWSTR psz)
{
    return ToAcp(CP_ACP, psz);
}


PSTR hnrt::ToAcp(PCWSTR psz, size_t cch)
{
    return ToAcp(CP_ACP, psz, cch);
}


PSTR hnrt::ToAcp(UINT cp, PCWSTR psz)
{
    int cb = WideCharToMultiByte(cp, 0, psz, -1, NULL, 0, NULL, NULL);
    if (!cb)
    {
        throw Exception(L"ToAcp(%u) failed.", cp);
    }
    CHAR* pm = Allocate<CHAR>(cb);
    WideCharToMultiByte(cp, 0, psz, -1, pm, cb, NULL, NULL);
    return pm;
}


PSTR hnrt::ToAcp(UINT cp, PCWSTR psz, size_t cch)
{
    int cb = WideCharToMultiByte(cp, 0, psz, static_cast<int>(cch), NULL, 0, NULL, NULL);
    if (!cb)
    {
        throw Exception(L"ToAcp(%u) failed.", cp);
    }
    CHAR* pm = Allocate<CHAR>(static_cast<size_t>(cb) + 1);
    WideCharToMultiByte(cp, 0, psz, static_cast<int>(cch), pm, cb, NULL, NULL);
    pm[cb] = '\0';
    return pm;
}


PWSTR hnrt::Clone(PCWSTR psz)
{
    PWSTR psz2 = _wcsdup(psz);
    if (!psz2)
    {
        throw std::bad_alloc();
    }
    return psz2;
}


PWSTR hnrt::Clone(PCWSTR psz, size_t cch)
{
    PWSTR psz2 = reinterpret_cast<PWSTR>(Malloc((cch + 1) * sizeof(WCHAR)));
    size_t len = wcsnlen(psz, cch);
    wmemcpy_s(psz2, len, psz, len);
    wmemset(psz2 + len, L'\0', cch + 1 - len);
    return psz2;
}


PWSTR* hnrt::Clone(PWSTR* ppsz, size_t count, size_t size)
{
    if (size < count)
    {
        size = count;
    }
    if (size)
    {
        size_t* ptr = reinterpret_cast<size_t*>(Malloc(sizeof(size_t) + size * sizeof(PWSTR)));
        ptr[0] = size;
        PWSTR* ppsz2 = reinterpret_cast<PWSTR*>(&ptr[1]);
        for (size_t index = 0; index < count; index++)
        {
            ppsz2[index] = Clone(ppsz[index]);
        }
        if (count < size)
        {
            memset(&ppsz2[count], 0, (size - count) * sizeof(ppsz2[0]));
        }
        return ppsz2;
    }
    else
    {
        return nullptr;
    }
}


PWSTR* hnrt::Resize(PWSTR* ppsz, size_t size)
{
    if (ppsz)
    {
        size_t* ptr = &reinterpret_cast<size_t*>(ppsz)[-1];
        size_t size0 = ptr[0];
        if (size0 < size)
        {
            ptr = reinterpret_cast<size_t*>(Realloc(ptr, sizeof(size_t) + size * sizeof(ppsz[0])));
            ptr[0] = size;
            ppsz = reinterpret_cast<PWSTR*>(&ptr[1]);
            memset(&ppsz[size0], 0, (size - size0) * sizeof(ppsz[0]));
        }
        else if (size < size0)
        {
            for (size_t index = size; index < size0; index++)
            {
                free(ppsz[index]);
            }
            memset(&ppsz[size], 0, (size0 - size) * sizeof(ppsz[0]));
            if (size > 0)
            {
                ptr = reinterpret_cast<size_t*>(Realloc(ptr, sizeof(size_t) + size * sizeof(ppsz[0])));
                ptr[0] = size;
                ppsz = reinterpret_cast<PWSTR*>(&ptr[1]);
            }
            else
            {
                free(ptr);
                ppsz = nullptr;
            }
        }
    }
    else if (size)
    {
        size_t* ptr = reinterpret_cast<size_t*>(Malloc(sizeof(size_t) + size * sizeof(ppsz[0])));
        ptr[0] = size;
        ppsz = reinterpret_cast<PWSTR*>(&ptr[1]);
        memset(&ppsz[0], 0, size * sizeof(ppsz[0]));
    }
    return ppsz;
}


void hnrt::Free(PWSTR* ppsz)
{
    if (ppsz)
    {
        size_t* ptr = &reinterpret_cast<size_t*>(ppsz)[-1];
        size_t size = ptr[0];
        for (size_t index = 0; index < size; index++)
        {
            free(ppsz[index]);
        }
        memset(&ppsz[0], 0, size * sizeof(ppsz[0]));
        free(ptr);
    }
}


DWORD hnrt::ArraySize(PWSTR* ppsz)
{
    return ppsz ? static_cast<DWORD>(reinterpret_cast<size_t*>(ppsz)[-1]) : 0;
}


PWSTR hnrt::Format(PCWSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    PWSTR psz = VaFormat(pszFormat, argList);
    va_end(argList);
    return psz;
}


PWSTR hnrt::VaFormat(PCWSTR pszFormat, va_list argList)
{
    va_list argList2;
    va_copy(argList2, argList);
    LONG_PTR cch = static_cast<LONG_PTR>(_vscwprintf(pszFormat, argList2));
    va_end(argList2);
    if (cch < 0)
    {
        throw Exception(L"VaFormat(PCWSTR) failed.");
    }
    PWSTR psz = reinterpret_cast<PWSTR>(Malloc(++cch * sizeof(WCHAR)));
    _vsnwprintf_s(psz, cch, _TRUNCATE, pszFormat, argList);
    return psz;
}


PWSTR hnrt::Concat(PCWSTR psz1, PCWSTR psz2)
{
    size_t cch1 = wcslen(psz1);
    size_t cch2 = wcslen(psz2);
    PWSTR psz = reinterpret_cast<PWSTR>(Malloc(cch1 + cch2 + 1));
    PWSTR p = psz;
    wmemcpy_s(p, cch1, psz1, cch1); p += cch1;
    wmemcpy_s(p, cch2, psz2, cch2); p += cch2;
    *p = L'\0';
    return psz;
}


PWSTR hnrt::Concat(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3)
{
    size_t cch1 = wcslen(psz1);
    size_t cch2 = wcslen(psz2);
    size_t cch3 = wcslen(psz3);
    PWSTR psz = reinterpret_cast<PWSTR>(Malloc(cch1 + cch2 + cch3 + 1));
    PWSTR p = psz;
    wmemcpy_s(p, cch1, psz1, cch1); p += cch1;
    wmemcpy_s(p, cch2, psz2, cch2); p += cch2;
    wmemcpy_s(p, cch3, psz3, cch3); p += cch3;
    *p = L'\0';
    return psz;
}


PWSTR hnrt::Concat(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, PCWSTR psz4)
{
    size_t cch1 = wcslen(psz1);
    size_t cch2 = wcslen(psz2);
    size_t cch3 = wcslen(psz3);
    size_t cch4 = wcslen(psz4);
    PWSTR psz = reinterpret_cast<PWSTR>(Malloc(cch1 + cch2 + cch3 + cch4 + 1));
    PWSTR p = psz;
    wmemcpy_s(p, cch1, psz1, cch1); p += cch1;
    wmemcpy_s(p, cch2, psz2, cch2); p += cch2;
    wmemcpy_s(p, cch3, psz3, cch3); p += cch3;
    wmemcpy_s(p, cch4, psz4, cch4); p += cch4;
    *p = L'\0';
    return psz;
}


PWSTR hnrt::Concat(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, PCWSTR psz4, PCWSTR psz5)
{
    size_t cch1 = wcslen(psz1);
    size_t cch2 = wcslen(psz2);
    size_t cch3 = wcslen(psz3);
    size_t cch4 = wcslen(psz4);
    size_t cch5 = wcslen(psz5);
    PWSTR psz = reinterpret_cast<PWSTR>(Malloc(cch1 + cch2 + cch3 + cch4 + cch5 + 1));
    PWSTR p = psz;
    wmemcpy_s(p, cch1, psz1, cch1); p += cch1;
    wmemcpy_s(p, cch2, psz2, cch2); p += cch2;
    wmemcpy_s(p, cch3, psz3, cch3); p += cch3;
    wmemcpy_s(p, cch4, psz4, cch4); p += cch4;
    wmemcpy_s(p, cch5, psz5, cch5); p += cch5;
    *p = L'\0';
    return psz;
}


PWSTR hnrt::ToUcs(PCSTR psz)
{
    return ToUcs(CP_ACP, psz);
}


PWSTR hnrt::ToUcs(PCSTR psz, size_t cb)
{
    return ToUcs(CP_ACP, psz, cb);
}


PWSTR hnrt::ToUcs(UINT cp, PCSTR psz)
{
    int cch = MultiByteToWideChar(cp, MB_PRECOMPOSED, psz, -1, NULL, 0);
    if (!cch)
    {
        throw Exception(L"ToUcs(%u) failed.", cp);
    }
    WCHAR* pw = Allocate<WCHAR>(cch);
    MultiByteToWideChar(cp, MB_PRECOMPOSED, psz, -1, pw, cch);
    return pw;
}


PWSTR hnrt::ToUcs(UINT cp, PCSTR psz, size_t cb)
{
    int cch = MultiByteToWideChar(cp, MB_PRECOMPOSED, psz, static_cast<int>(cb), NULL, 0);
    if (!cch)
    {
        throw Exception(L"ToUcs(%u) failed.", cp);
    }
    WCHAR* pw = Allocate<WCHAR>(static_cast<size_t>(cch) + 1);
    MultiByteToWideChar(cp, MB_PRECOMPOSED, psz, static_cast<int>(cb), pw, cch);
    pw[cch] = L'\0';
    return pw;
}
