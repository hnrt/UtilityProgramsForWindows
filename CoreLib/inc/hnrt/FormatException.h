#pragma once

#include <Windows.h>
#include "hnrt/Exception.h"

namespace hnrt
{
    class FormatException
        : public Exception
    {
    public:

        FormatException(const FormatException& src);
        virtual ~FormatException() = default;
        FormatException& operator =(const FormatException& src);
        UINT get_Offset() const;

        __declspec(property(get = get_Offset)) UINT Offset;

    protected:

        FormatException(SIZE_T uOffset);

        SSIZE_T m_Offset;
    };

    inline UINT FormatException::get_Offset() const
    {
        return static_cast<UINT>(m_Offset);
    }
}
