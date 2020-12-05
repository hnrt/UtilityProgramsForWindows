#pragma once


#include "hnrt/Exception.h"


namespace hnrt
{
    enum class CabinetError
    {
        None,
        CabinetNotFound,
        IncorrectFormat,
        UnknownVersion,
        Corrupt,
        OutOfMemory,
        UnknownCompressionType,
        DecompressionFailure,
        TargetFileFailure,
        ReserveSizesMismatch,
        WrongCabinet,
        AbortedByUser,
        UnexpectedEndOfFile,
        UnknownError,
    };

    class CabinetException
        : public Exception
    {
    public:

        CabinetException(int type, int code);
        CabinetException(const CabinetException&);
        virtual ~CabinetException() = default;
        CabinetException& operator =(const CabinetException&);
        CabinetError get_Type() const { return m_type; }
        UINT get_Code() const { return m_code; }

        __declspec(property(get = get_Type)) CabinetError Type;
        __declspec(property(get = get_Code)) UINT Code;

    protected:

        CabinetError m_type;
        UINT m_code;
    };
}
