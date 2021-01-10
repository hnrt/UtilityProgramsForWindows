#pragma once


#include <Windows.h>
#include <bcrypt.h>


namespace hnrt
{
    class BCryptKeyHandle
    {
    public:

        BCryptKeyHandle(BCRYPT_KEY_HANDLE h = nullptr);
        BCryptKeyHandle(const BCryptKeyHandle&) = delete;
        ~BCryptKeyHandle();
        void operator =(const BCryptKeyHandle&) = delete;
        operator BCRYPT_KEY_HANDLE() const;
        BCRYPT_KEY_HANDLE* operator &();

    private:

        BCRYPT_KEY_HANDLE m_h;
    };

    inline BCryptKeyHandle::BCryptKeyHandle(BCRYPT_KEY_HANDLE h)
        : m_h(h)
    {
    }

    inline BCryptKeyHandle::~BCryptKeyHandle()
    {
        if (m_h)
        {
            BCryptDestroyKey(m_h);
        }
    }

    inline BCryptKeyHandle::operator BCRYPT_KEY_HANDLE() const
    {
        return m_h;
    }

    inline BCRYPT_KEY_HANDLE* BCryptKeyHandle::operator &()
    {
        if (m_h)
        {
            BCryptDestroyKey(m_h);
            m_h = nullptr;
        }
        return &m_h;
    }
}
