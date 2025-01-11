#pragma once

#include "hnrt/String.h"
#include "hnrt/LineBreak.h"

namespace hnrt
{
    class ByteString
    {
    public:

        ByteString(size_t = 0);
        ByteString(const void*, size_t);
        ByteString(const ByteString&);
        ~ByteString();
        ByteString& operator =(const ByteString&);
        ByteString& operator +=(const ByteString&);
        ByteString operator +(const ByteString&);
        bool operator ==(const ByteString&) const;
        bool operator !=(const ByteString&) const;
        bool operator <(const ByteString&) const;
        bool operator <=(const ByteString&) const;
        bool operator >(const ByteString&) const;
        bool operator >=(const ByteString&) const;
        operator const void*() const;
        operator void*();
        operator const char*() const;
        operator char*();
        operator const unsigned char*() const;
        operator unsigned char*();
        const unsigned char& operator [](INT_PTR) const;
        unsigned char& operator [](INT_PTR);
        ByteString& Resize(size_t);
        ByteString& Fill(int);
        ByteString Clone() const;
        ByteString Pkcs5Padding(DWORD dwBlockLength) const;
        ByteString& RemovePkcs5Padding(DWORD dwBlockLength);
        String ToString(UINT = CP_UTF8) const;
        void* get_Ptr() const;
        size_t get_Len() const;

        __declspec(property(get = get_Ptr)) void* Ptr;
        __declspec(property(get = get_Len)) size_t Len;

    private:

        void* m_ptr;

    public:

        static int Compare(const ByteString&, const ByteString&);
        static ByteString FromHex(PCWSTR);
        static ByteString FromHex(PCSTR);
        static ByteString FromString(const String&, UINT, LineBreak = LineBreak::DONOTCARE);
    };

    inline bool ByteString::operator ==(const ByteString& other) const
    {
        return Compare(*this, other) == 0;
    }

    inline bool ByteString::operator !=(const ByteString& other) const
    {
        return Compare(*this, other) != 0;
    }

    inline bool ByteString::operator <(const ByteString& other) const
    {
        return Compare(*this, other) < 0;
    }

    inline bool ByteString::operator <=(const ByteString& other) const
    {
        return Compare(*this, other) <= 0;
    }

    inline bool ByteString::operator >(const ByteString& other) const
    {
        return Compare(*this, other) > 0;
    }

    inline bool ByteString::operator >=(const ByteString& other) const
    {
        return Compare(*this, other) >= 0;
    }

    inline ByteString::operator const void*() const
    {
        return m_ptr;
    }

    inline ByteString::operator void*()
    {
        return m_ptr;
    }

    inline ByteString::operator const char*() const
    {
        return reinterpret_cast<const char*>(m_ptr);
    }

    inline ByteString::operator char*()
    {
        return reinterpret_cast<char*>(m_ptr);
    }

    inline ByteString::operator const unsigned char*() const
    {
        return reinterpret_cast<const unsigned char*>(m_ptr);
    }

    inline ByteString::operator unsigned char* ()
    {
        return reinterpret_cast<unsigned char*>(m_ptr);
    }

    inline void* ByteString::get_Ptr() const
    { 
        return m_ptr;
    }
}
