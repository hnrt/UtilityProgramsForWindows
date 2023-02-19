#include "pch.h"
#include <stdexcept>
#include "hnrt/Heap.h"
#include "hnrt/Debug.h"


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


PWSTR hnrt::Clone(PCWSTR psz)
{
    PWSTR psz2 = _wcsdup(psz);
    if (!psz2)
    {
        throw std::bad_alloc();
    }
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
