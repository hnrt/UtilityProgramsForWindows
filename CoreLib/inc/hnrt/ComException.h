#pragma once


#include <Windows.h>
#include "hnrt/Exception.h"


namespace hnrt
{
    class ComException
        : public Exception
    {
    public:

        ComException();
        ComException(HRESULT hr, PCWSTR pszFormat, ...);
        ComException(const ComException& src);
        virtual ~ComException() = default;
        ComException& operator =(const ComException& src);
        HRESULT get_Result() const;
        void set_Result(HRESULT value);

        __declspec(property(get = get_Result, put = set_Result)) HRESULT Result;

    protected:

        HRESULT m_hr;
    };

    inline HRESULT ComException::get_Result() const
    {
        return m_hr;
    }

    inline void ComException::set_Result(HRESULT value)
    {
        m_hr = value;
    }
}
