#include "pch.h"
#include "hnrt/RefBin.h"
#include "hnrt/Heap.h"
#include "hnrt/Debug.h"


using namespace hnrt;


RefBin::RefBin(size_t len)
    : RefObj()
    , m_Len(len)
{
    memset(Get(*this), 0, len);
}


RefBin::RefBin(const void* ptr, size_t len)
    : RefObj()
    , m_Len(len)
{
    memcpy_s(Get(*this), len, ptr, len);
}


RefBin::~RefBin()
{
    memset(Get(*this), 0, m_Len);
    m_Len = 0;
}


void* RefBin::operator new(size_t size, size_t cb)
{
    size_t required = sizeof(RefBin) + cb;
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
