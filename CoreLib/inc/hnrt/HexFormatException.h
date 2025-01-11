#pragma once

#include <Windows.h>
#include "hnrt/Exception.h"

namespace hnrt
{
    class HexFormatException
        : public Exception
    {
    public:

        HexFormatException();
        HexFormatException(SIZE_T uOffset, PCWSTR pszFormat, ...);
        HexFormatException(const HexFormatException& src);
        virtual ~HexFormatException() = default;
        HexFormatException& operator =(const HexFormatException& src);
        UINT get_Offset() const;

        __declspec(property(get = get_Offset)) UINT Offset;

    protected:

        SSIZE_T m_Offset;
    };

    inline UINT HexFormatException::get_Offset() const
    {
        return static_cast<UINT>(m_Offset);
    }
}
