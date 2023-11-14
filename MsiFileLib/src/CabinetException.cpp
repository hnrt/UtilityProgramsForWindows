#include "pch.h"
#include <fdi.h>
#include "hnrt/CabinetException.h"


using namespace hnrt;


static PCWSTR ToMessage(int type)
{
    switch (type)
    {
    case FDIERROR_NONE:
        return L"No error.";

    case FDIERROR_CABINET_NOT_FOUND:
        return L"Cabinet not found.";

    case FDIERROR_NOT_A_CABINET:
        return L"Cabinet file does not have the correct format.";

    case FDIERROR_UNKNOWN_CABINET_VERSION:
        return L"Cabinet file has an unknown version number.";

    case FDIERROR_CORRUPT_CABINET:
        return L"Cabinet file is corrupt.";

    case FDIERROR_ALLOC_FAIL:
        return L"Could not allocate enough memory.";

    case FDIERROR_BAD_COMPR_TYPE:
        return L"Unknown compression type in a cabinet folder.";

    case FDIERROR_MDI_FAIL:
        return L"Failure decompressing data from a cabinet file.";

    case FDIERROR_TARGET_FILE:
        return L"Failure writing to target file.";

    case FDIERROR_RESERVE_MISMATCH:
        return L"Cabinets in a set do not have the same RESERVE sizes.";

    case FDIERROR_WRONG_CABINET:
        return L"Cabinet returned on fdintNEXT_CABINET is incorrect.";

    case FDIERROR_USER_ABORT:
        return L"Aborted by user.";

    case FDIERROR_EOF:
        return L"Unexpected end of file.";

    default:
        return L"Unknown error.";
    }
}


static CabinetError ToError(int type)
{
    switch (type)
    {
    case FDIERROR_NONE:
        return CabinetError::None;

    case FDIERROR_CABINET_NOT_FOUND:
        return CabinetError::CabinetNotFound;

    case FDIERROR_NOT_A_CABINET:
        return CabinetError::IncorrectFormat;

    case FDIERROR_UNKNOWN_CABINET_VERSION:
        return CabinetError::UnknownVersion;

    case FDIERROR_CORRUPT_CABINET:
        return CabinetError::Corrupt;

    case FDIERROR_ALLOC_FAIL:
        return CabinetError::OutOfMemory;

    case FDIERROR_BAD_COMPR_TYPE:
        return CabinetError::UnknownCompressionType;

    case FDIERROR_MDI_FAIL:
        return CabinetError::DecompressionFailure;

    case FDIERROR_TARGET_FILE:
        return CabinetError::TargetFileFailure;

    case FDIERROR_RESERVE_MISMATCH:
        return CabinetError::ReserveSizesMismatch;

    case FDIERROR_WRONG_CABINET:
        return CabinetError::WrongCabinet;

    case FDIERROR_USER_ABORT:
        return CabinetError::AbortedByUser;

    case FDIERROR_EOF:
        return CabinetError::UnexpectedEndOfFile;

    default:
        return CabinetError::UnknownError;
    }
}


CabinetException::CabinetException(int type, int code)
    : Exception(ToMessage(type))
    , m_type(ToError(type))
    , m_code(code)
{
}


CabinetException::CabinetException(const CabinetException& src)
    : Exception(src)
    , m_type(src.m_type)
    , m_code(src.m_code)
{
}


CabinetException& CabinetException::operator =(const CabinetException& src)
{
    m_Message = src.m_Message;
    m_type = src.m_type;
    m_code = src.m_code;
    return *this;
}
