#pragma once


#include <Windows.h>
#include <bcrypt.h>


namespace hnrt
{
    class BCryptAlgHandle
    {
    public:

        BCryptAlgHandle(BCRYPT_ALG_HANDLE h = nullptr);
        BCryptAlgHandle(const BCryptAlgHandle&) = delete;
        ~BCryptAlgHandle();
        void operator =(const BCryptAlgHandle&) = delete;
        operator BCRYPT_ALG_HANDLE() const;
        BCRYPT_ALG_HANDLE* operator &();

    private:

        BCRYPT_ALG_HANDLE m_h;
    };

    inline BCryptAlgHandle::BCryptAlgHandle(BCRYPT_ALG_HANDLE h)
        : m_h(h)
    {
    }

    inline BCryptAlgHandle::~BCryptAlgHandle()
    {
        if (m_h)
        {
            BCryptCloseAlgorithmProvider(m_h, 0);
        }
    }

    inline BCryptAlgHandle::operator BCRYPT_ALG_HANDLE() const
    {
        return m_h;
    }

    inline BCRYPT_ALG_HANDLE* BCryptAlgHandle::operator &()
    {
        if (m_h)
        {
            BCryptCloseAlgorithmProvider(m_h, 0);
            m_h = nullptr;
        }
        return &m_h;
    }
}
