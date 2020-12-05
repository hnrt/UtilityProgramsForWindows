#include "pch.h"
#include "hnrt/SecretBuffer.h"
#include "hnrt/Debug.h"


using namespace hnrt;


SecretBuffer::SecretBuffer(size_t len)
    : Buffer<unsigned char>(len)
{
    DBGFNC(L"SecretBuffer::ctor(%zu)", len);
}


SecretBuffer::~SecretBuffer()
{
    DBGFNC(L"SecretBuffer::dtor");
    if (m_ptr)
    {
        memset(m_ptr, 0, m_len);
    }
}


void SecretBuffer::Resize(size_t len)
{
    DBGFNC(L"SecretBuffer::Resize(%zu)", len);
    if (m_ptr)
    {
        memset(m_ptr, 0, m_len);
    }
    Buffer::Resize(len);
}
