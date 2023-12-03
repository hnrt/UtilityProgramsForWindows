#include "pch.h"
#include "hnrt/SecretBuffer.h"
#include "hnrt/Debug.h"


using namespace hnrt;


SecretBuffer::SecretBuffer(size_t size)
    : RefObj()
    , Buffer<unsigned char>(size)
{
}


SecretBuffer::~SecretBuffer()
{
    if (m_ptr)
    {
        memset(m_ptr, 0, m_len);
    }
}


void SecretBuffer::Resize(size_t size)
{
    if (m_ptr)
    {
        memset(m_ptr, 0, m_len);
    }
    Buffer::Resize(size);
}
