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


char* hnrt::Clone(const char* psz)
{
    char* psz2 = _strdup(psz);
    if (!psz2)
    {
        throw std::bad_alloc();
    }
    return psz2;
}


wchar_t* hnrt::Clone(PCWSTR psz)
{
    wchar_t* psz2 = _wcsdup(psz);
    if (!psz2)
    {
        throw std::bad_alloc();
    }
    return psz2;
}
