#pragma once


#include <Windows.h>
#include <malloc.h>


namespace hnrt
{
    void* Malloc(size_t nbytes);
    void* Realloc(void* ptr, size_t nbytes);
    void* Calloc(size_t count, size_t nbytes);

    PSTR Clone(PCSTR psz);
    PSTR* Clone(PSTR* ppsz, size_t count, size_t size = 0);
    PSTR* Resize(PSTR* ppsz, size_t size);
    void Free(PSTR* ppsz);
    DWORD ArraySize(PSTR* ppsz);

    PWSTR Clone(PCWSTR psz);
    PWSTR* Clone(PWSTR* ppsz, size_t count, size_t size = 0);
    PWSTR* Resize(PWSTR* ppsz, size_t size);
    void Free(PWSTR* ppsz);
    DWORD ArraySize(PWSTR* ppsz);

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
}
