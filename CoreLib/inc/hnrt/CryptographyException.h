#pragma once


#include "hnrt/Exception.h"


namespace hnrt
{
    class CryptographyException
        : public Exception
    {
    public:

        CryptographyException(LONG, PCWSTR, ...);
        CryptographyException(const CryptographyException&);
        virtual ~CryptographyException() = default;
        CryptographyException& operator =(const CryptographyException&);
        LONG get_Status() const;

        __declspec(property(get = get_Status)) LONG Status;

    protected:

        LONG m_Status;
    };

    inline LONG CryptographyException::get_Status() const
    {
        return m_Status;
    }
}
