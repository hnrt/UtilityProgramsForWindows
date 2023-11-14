#pragma once


#include <Windows.h>
#include <malloc.h>


namespace hnrt
{
    void* Malloc(size_t nbytes);
    void* Realloc(void* ptr, size_t nbytes);
    void* Calloc(size_t count, size_t nbytes);

    template<typename T>
    T* Allocate(size_t count)
    {
        return reinterpret_cast<T*>(Malloc(count * sizeof(T)));
    }

    template<typename T>
    T* ZeroAllocate(size_t count)
    {
        return reinterpret_cast<T*>(Calloc(count, sizeof(T)));
    }

    template<typename T>
    T* Allocate(T* ptr, size_t count)
    {
        return reinterpret_cast<T*>(Realloc(ptr, count * sizeof(T)));
    }

    template<typename T>
    T* Detach(T*& ptr)
    {
        return reinterpret_cast<T*>(InterlockedExchangePointer(reinterpret_cast<void**>(&ptr), nullptr));
    }

    template<typename T>
    void Deallocate(T*& ptr)
    {
        free(Detach<T>(ptr));
    }

    PSTR Clone(PCSTR psz);
    PSTR Clone(PCSTR psz, size_t cb);
    PSTR* Clone(PSTR* ppsz, size_t count, size_t size = 0);
    PSTR* Resize(PSTR* ppsz, size_t size);
    void Free(PSTR* ppsz);
    DWORD ArraySize(PSTR* ppsz);
    PSTR Format(PCSTR pszFormat, ...);
    PSTR VaFormat(PCSTR pszFormat, va_list argList);
    PSTR Concat(PCSTR psz1, PCSTR psz2);
    PSTR Concat(PCSTR psz1, PCSTR psz2, PCSTR psz3);
    PSTR Concat(PCSTR psz1, PCSTR psz2, PCSTR psz3, PCSTR psz4);
    PSTR Concat(PCSTR psz1, PCSTR psz2, PCSTR psz3, PCSTR psz4, PCSTR psz5);

    PWSTR Clone(PCWSTR psz);
    PWSTR Clone(PCWSTR psz, size_t cch);
    PWSTR* Clone(PWSTR* ppsz, size_t count, size_t size = 0);
    PWSTR* Resize(PWSTR* ppsz, size_t size);
    void Free(PWSTR* ppsz);
    DWORD ArraySize(PWSTR* ppsz);
    PWSTR Format(PCWSTR pszFormat, ...);
    PWSTR VaFormat(PCWSTR pszFormat, va_list argList);
    PWSTR Concat(PCWSTR psz1, PCWSTR psz2);
    PWSTR Concat(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3);
    PWSTR Concat(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, PCWSTR psz4);
    PWSTR Concat(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, PCWSTR psz4, PCWSTR psz5);
}
