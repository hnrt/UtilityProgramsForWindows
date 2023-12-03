#include "pch.h"
#include "hnrt/BCryptAlgHandle.h"
#include "hnrt/CryptographyException.h"
#include "hnrt/Buffer.h"


#pragma comment(lib, "Bcrypt")


using namespace hnrt;


String BCryptHandle::GetPropertyString(PCWSTR pszName) const
{
    Buffer<WCHAR> wcsValue(260);
    while (true)
    {
        ULONG valueLength = 0UL;
        NTSTATUS status = BCryptGetProperty(m_h, pszName, reinterpret_cast<PUCHAR>(&wcsValue[0]), static_cast<ULONG>(wcsValue.Len * sizeof(WCHAR)), &valueLength, 0);
        if (status == STATUS_SUCCESS)
        {
            return String(wcsValue, wcsValue.Len);
        }
        else if (status == STATUS_BUFFER_TOO_SMALL)
        {
            wcsValue.Resize(wcsValue.Len * 2);
        }
        else
        {
            throw CryptographyException(status, L"BCryptGetProperty(%s) failed with status of %s.", pszName, BCryptErrorLabel(status));
        }
    }
}


DWORD BCryptHandle::GetPropertyDWORD(PCWSTR pszName) const
{
    DWORD dwValue = 0UL;
    ULONG valueLength = 0UL;
    NTSTATUS status = BCryptGetProperty(m_h, pszName, reinterpret_cast<PUCHAR>(&dwValue), sizeof(dwValue), &valueLength, 0);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptGetProperty(%s) failed with status of %s.", pszName, BCryptErrorLabel(status));
    }
    else if (valueLength != sizeof(dwValue))
    {
        throw CryptographyException(status, L"BCryptGetProperty(%s) returned an unexpected value; actual=%lu expected=%zu", pszName, valueLength, sizeof(dwValue));
    }
    return dwValue;
}


std::vector<DWORD> BCryptHandle::GetPropertyArrayDWORD(PCWSTR pszName) const
{
    Buffer<DWORD> dwValues(16);
    while (true)
    {
        ULONG valueLength = 0UL;
        NTSTATUS status = BCryptGetProperty(m_h, pszName, reinterpret_cast<PUCHAR>(&dwValues[0]), static_cast<ULONG>(dwValues.Len * sizeof(DWORD)), &valueLength, 0);
        if (status == STATUS_SUCCESS)
        {
            std::vector<DWORD> list;
            ULONG count = valueLength / sizeof(DWORD);
            for (ULONG index = 0; index < count; index++)
            {
                list.push_back(dwValues[index]);
            }
            return list;
        }
        else if (status == STATUS_BUFFER_TOO_SMALL)
        {
            dwValues.Resize(dwValues.Len * 2);
        }
        else
        {
            throw CryptographyException(status, L"BCryptGetProperty(%s) failed with status of %s.", pszName, BCryptErrorLabel(status));
        }
    }
}


std::vector<DWORD> BCryptHandle::GetPropertyKeyLengths() const
{
    BCRYPT_KEY_LENGTHS_STRUCT keyLengths = { 0 };
    ULONG valueLength = 0UL;
    NTSTATUS status = BCryptGetProperty(m_h, BCRYPT_KEY_LENGTHS, reinterpret_cast<PUCHAR>(&keyLengths), sizeof(keyLengths), &valueLength, 0);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptGetProperty(BCRYPT_KEY_LENGTHS) failed with status of %s.", BCryptErrorLabel(status));
    }
    else if (valueLength != sizeof(keyLengths))
    {
        throw CryptographyException(status, L"BCryptGetProperty(BCRYPT_KEY_LENGTHS) returned an unexpected value; actual=%lu expected=%zu", valueLength, sizeof(keyLengths));
    }
    std::vector<DWORD> list;
    for (DWORD length = keyLengths.dwMinLength; length <= keyLengths.dwMaxLength; length += keyLengths.dwIncrement)
    {
        list.push_back(length);
    }
    return list;
}


void BCryptHandle::SetProperty(PCWSTR pszName, PCWSTR pszValue) const
{
    size_t size = (wcslen(pszValue) + 1) * sizeof(WCHAR);
    NTSTATUS status = BCryptSetProperty(m_h, pszName, reinterpret_cast<PUCHAR>(const_cast<PWSTR>(pszValue)), static_cast<ULONG>(size), 0);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptSetProperty(%s,%s) failed with status of %s.", pszName, pszValue, BCryptErrorLabel(status));
    }
}


String hnrt::BCryptErrorLabel(NTSTATUS status)
{
    switch (status)
    {
#define CASE(x) case x: return String(L#x)
    CASE(STATUS_SUCCESS);
    CASE(STATUS_AUTH_TAG_MISMATCH);
    CASE(STATUS_BUFFER_TOO_SMALL);
    CASE(STATUS_INVALID_BUFFER_SIZE);
    CASE(STATUS_INVALID_HANDLE);
    CASE(STATUS_INVALID_PARAMETER);
    CASE(STATUS_NOT_SUPPORTED);
#undef CASE
    default: return String(PRINTF, L"0x%08lX", status);
    }
}
