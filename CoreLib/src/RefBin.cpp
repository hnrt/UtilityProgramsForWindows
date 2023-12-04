#include "pch.h"
#include "hnrt/RefBin.h"
#include "hnrt/Heap.h"
#include "hnrt/Debug.h"


using namespace hnrt;


void* RefBin::operator new(size_t size, size_t cb)
{
    size_t required = (size_t)(((RefBin*)0)->m_buf) + cb * sizeof(char);
    return Malloc(required > size ? required : size);
}


void RefBin::operator delete(void* ptr)
{
    free(ptr);
}


void RefBin::operator delete(void* ptr, size_t)
{
    free(ptr);
}


void* RefBin::Get(RefBin* ptr)
{
    return &ptr->m_buf[0];
}


RefBin& RefBin::Get(void* ptr)
{
    static const size_t offset = (size_t)(((RefBin*)0)->m_buf);
    return *(RefBin*)((PBYTE)ptr - offset);
}


void* RefBin::Create(size_t len)
{
    return Get(len ? new(len) RefBin(len) : nullptr);
}


void* RefBin::Create(const void* ptr, size_t len)
{
    return Get(len ? new(len) RefBin(ptr, len) : nullptr);
}


RefBin::RefBin(size_t len)
    : RefObj()
    , m_len(len)
    , m_buf()
{
    memset(m_buf, 0, len);
}


RefBin::RefBin(const void* ptr, size_t len)
    : RefObj()
    , m_len(len)
    , m_buf()
{
    if (ptr)
    {
        memcpy_s(&m_buf[0], m_len, ptr, len);
    }
    else
    {
        memset(&m_buf[0], 0, m_len);
    }
}


RefBin::~RefBin()
{
    memset(&m_buf[0], 0, m_len);
}
