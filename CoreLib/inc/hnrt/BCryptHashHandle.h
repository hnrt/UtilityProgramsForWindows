#pragma once

#include "hnrt/BCryptHandle.h"

namespace hnrt
{
    class BCryptAlgHandle;

    class BCryptHashHandle
        : protected BCryptHandle
    {
    public:

        BCryptHashHandle();
        BCryptHashHandle(const BCryptHashHandle&) = delete;
        virtual ~BCryptHashHandle();
        void operator =(const BCryptHashHandle&) = delete;
        operator BCRYPT_HASH_HANDLE() const;
        void Open(const BCryptAlgHandle&);
        void Close();
        void Feed(void*, size_t);
        void Finalize(void*, size_t);

    private:

        PUCHAR m_pObject;
    };

    inline BCryptHashHandle::BCryptHashHandle()
        : BCryptHandle()
        , m_pObject(nullptr)
    {
    }

    inline BCryptHashHandle::~BCryptHashHandle()
    {
        if (m_h || m_pObject)
        {
            Close();
        }
    }

    inline BCryptHashHandle::operator BCRYPT_HASH_HANDLE() const
    {
        return m_h;
    }
}
