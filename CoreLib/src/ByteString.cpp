#include "pch.h"
#include "hnrt/ByteString.h"
#include "hnrt/Heap.h"


using namespace hnrt;


ByteString::~ByteString()
{
    free(m_ptr);
}


ByteString::ByteString(size_t len)
    : m_ptr(len ? Malloc(len) : nullptr)
    , m_len(len)
{
}


ByteString::ByteString(const ByteString& src)
    : m_ptr(nullptr)
    , m_len(0)
{
    operator = (src);
}


void ByteString::operator =(const ByteString& src)
{
    Resize(src.m_len);
    memcpy_s(m_ptr, m_len, src.m_ptr, src.m_len);
}


void ByteString::Resize(size_t len)
{
    if (len)
    {
        m_ptr = Realloc(m_ptr, len);
        m_len = len;
    }
    else
    {
        Deallocate(m_ptr);
        m_len = 0;
    }
}


void* ByteString::Detach()
{
    m_len = 0;
    return ::Detach(m_ptr);
}
