#pragma once


#include <Windows.h>
#include <bcrypt.h>


namespace hnrt
{
    class BCryptHashHandle
    {
    public:

        BCryptHashHandle(BCRYPT_HASH_HANDLE h = nullptr);
        BCryptHashHandle(const BCryptHashHandle&) = delete;
        ~BCryptHashHandle();
        void operator =(const BCryptHashHandle&) = delete;
        operator BCRYPT_HASH_HANDLE() const;
        BCRYPT_HASH_HANDLE* operator &();
        void Close();
        PUCHAR AllocateObject(ULONG length);
        PUCHAR get_Object();
        ULONG get_ObjectLength() const;

        __declspec(property(get = get_Object)) PUCHAR Object;
        __declspec(property(get = get_ObjectLength)) ULONG ObjectLength;

    private:

        BCRYPT_HASH_HANDLE m_h;
        PUCHAR m_pObject;
        ULONG m_cbObjectLength;
    };

    inline BCryptHashHandle::BCryptHashHandle(BCRYPT_HASH_HANDLE h)
        : m_h(h)
        , m_pObject(nullptr)
        , m_cbObjectLength(0UL)
    {
    }

    inline BCryptHashHandle::~BCryptHashHandle()
    {
        Close();
        AllocateObject(0UL);
    }

    inline BCryptHashHandle::operator BCRYPT_HASH_HANDLE() const
    {
        return m_h;
    }

    inline BCRYPT_HASH_HANDLE* BCryptHashHandle::operator &()
    {
        Close();
        return &m_h;
    }

    inline void BCryptHashHandle::Close()
    {
        if (m_h)
        {
            BCryptDestroyHash(m_h);
            m_h = nullptr;
        }
    }

    inline PUCHAR BCryptHashHandle::AllocateObject(ULONG length)
    {
        m_cbObjectLength = length;
        delete[] m_pObject;
        m_pObject = length > 0 ? new UCHAR[length] : nullptr;
        return m_pObject;
    }

    inline PUCHAR BCryptHashHandle::get_Object()
    {
        return m_pObject;
    }

    inline ULONG BCryptHashHandle::get_ObjectLength() const
    {
        return m_cbObjectLength;
    }
}
