#pragma once


#include <stddef.h>


namespace hnrt
{
    class ByteString
    {
    public:

        ~ByteString();
        ByteString(size_t len = 0);
        ByteString(const ByteString&);
        void operator =(const ByteString&);
        void Resize(size_t len);
        void* Detach();
        void* get_Ptr() { return m_ptr; }
        size_t get_Len() const { return m_len; }
        operator const void* () const { return m_ptr; }
        operator void* () { return m_ptr; }
        operator const char* () const { return reinterpret_cast<const char*>(m_ptr); }
        operator char* () { return reinterpret_cast<char*>(m_ptr); }
        operator const unsigned char* () const { return reinterpret_cast<const unsigned char*>(m_ptr); }
        operator unsigned char* () { return reinterpret_cast<unsigned char*>(m_ptr); }

        __declspec(property(get = get_Ptr)) void* Ptr;
        __declspec(property(get = get_Len)) size_t Len;

    private:

        void* m_ptr;
        size_t m_len;
    };
}
