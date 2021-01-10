#include "pch.h"
#include "hnrt/SecretBuffer.h"
#include "hnrt/Debug.h"


using namespace hnrt;


SecretBuffer::SecretBuffer(size_t len)
    : Buffer<unsigned char>(len)
{
}


SecretBuffer::~SecretBuffer()
{
    if (m_ptr)
    {
        memset(m_ptr, 0, m_len);
    }
}


void SecretBuffer::Resize(size_t len)
{
    if (m_ptr)
    {
        memset(m_ptr, 0, m_len);
    }
    Buffer::Resize(len);
}
