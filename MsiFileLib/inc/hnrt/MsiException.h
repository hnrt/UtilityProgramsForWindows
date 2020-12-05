#pragma once


#include "hnrt/Exception.h"


namespace hnrt
{
    class MsiException
        : public Exception
    {
    public:

        MsiException(UINT error);
        MsiException(UINT error, PCWSTR pszFormat, ...);
        MsiException(PCWSTR pszFormat, ...);
        MsiException(const MsiException&);
        virtual ~MsiException() = default;
        MsiException& operator =(const MsiException&);
        UINT get_Error() const;

        __declspec(property(get = get_Error)) UINT Error;

    protected:

        UINT m_error;
    };

    inline UINT MsiException::get_Error() const
    {
        return m_error;
    }
}
