#include "pch.h"
#include "MsiUtils.h"
#include "hnrt/MsiException.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/String.h"


using namespace hnrt;


String MsiUtils::GetString(MSIHANDLE hRecord, UINT uField)
{
    WCHAR tmp[1] = { 0 };
    DWORD cch = 0;
    UINT rc = MsiRecordGetStringW(hRecord, uField, tmp, &cch);
    if (rc != ERROR_MORE_DATA)
    {
        throw MsiException(rc, L"Failed to get string from field#%u.", uField);
    }
    StringBuffer buf(static_cast<size_t>(cch) + 1);
    cch = static_cast<DWORD>(buf.Cap);
    rc = MsiRecordGetStringW(hRecord, uField, buf, &cch);
    if (rc)
    {
        throw MsiException(rc, L"Failed to get string from field#%u.", uField);
    }
    return String(buf);
}


void MsiUtils::ReadStream(MSIHANDLE hRecord, UINT uField, ByteString& data)
{
    DWORD dwBytes = 0;
    UINT rc = MsiRecordReadStream(hRecord, uField, NULL, &dwBytes);
    if (rc)
    {
        throw MsiException(rc, L"Failed to read stream from field#%u.", uField);
    }
    data.Resize(dwBytes);
    rc = MsiRecordReadStream(hRecord, uField, data, &dwBytes);
    if (rc)
    {
        throw MsiException(rc, L"Failed to read stream from field#%u.", uField);
    }
}
