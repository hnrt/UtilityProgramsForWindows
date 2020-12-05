#pragma once


#include <malloc.h>


namespace hnrt
{
    void* Malloc(size_t nbytes);
    void* Realloc(void* ptr, size_t nbytes);
    void* Calloc(size_t count, size_t nbytes);

    char* Clone(const char* psz);
    wchar_t* Clone(PCWSTR psz);

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
