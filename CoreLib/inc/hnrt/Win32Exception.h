#pragma once


#include "hnrt/Exception.h"


namespace hnrt
{
    class Win32Exception
        : public Exception
    {
    public:

        Win32Exception(unsigned long error = 0);
        Win32Exception(unsigned long error, PCWSTR pszFormat, ...);
        Win32Exception(const Win32Exception& src);
        virtual ~Win32Exception() = default;
        Win32Exception& operator =(const Win32Exception& src);
        unsigned long get_Error() const;
        void set_Error(unsigned long value);

        __declspec(property(get = get_Error, put = set_Error)) unsigned long Error;

    protected:

        unsigned long m_error;
    };

    inline unsigned long Win32Exception::get_Error() const
    {
        return m_error;
    }

    inline void Win32Exception::set_Error(unsigned long value)
    {
        m_error = value;
    }
}
