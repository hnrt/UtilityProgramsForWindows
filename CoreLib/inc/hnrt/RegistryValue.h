#pragma once


#include "hnrt/String.h"


namespace hnrt
{
    union RegistryValueHolder
    {
        DWORD dw;
        ULONGLONG qw;
        PWSTR psz;
        PBYTE ptr;
    };

    class RegistryValue
    {
    public:

        static LSTATUS SetDWORD(HKEY hKey, PCWSTR pszName, DWORD dwValue);
        static LSTATUS SetQWORD(HKEY hKey, PCWSTR pszName, ULONGLONG qwValue);
        static LSTATUS SetSZ(HKEY hKey, PCWSTR pszName, PCWSTR pszValue);
        static LSTATUS SetEXPANDSZ(HKEY hKey, PCWSTR pszName, PCWSTR pszValue);
        static LSTATUS SetMULTISZ(HKEY hKey, PCWSTR pszName, PCWSTR pszValues[], SIZE_T nValues);
        static LSTATUS SetBINARY(HKEY hKey, PCWSTR pszName, const void* pValue, SIZE_T cbValue);
        static DWORD GetDWORD(HKEY hKey, PCWSTR pszName, DWORD dwDefaultValue = 0);
        static ULONGLONG GetQWORD(HKEY hKey, PCWSTR pszName, ULONGLONG qwDefaultValue = 0);
        static String GetSZ(HKEY hKey, PCWSTR pszName, const String& szDefaultValue = String::Empty);
        static String GetEXPANDSZ(HKEY hKey, PCWSTR pszName, const String& szDefaultValue = String::Empty);
        static String GetString(HKEY hKey, PCWSTR pszName, const String& szDefaultValue = String::Empty);

        RegistryValue();
        RegistryValue(DWORD);
        RegistryValue(ULONGLONG);
        RegistryValue(PCWSTR, DWORD);
        RegistryValue(const RegistryValue&) = delete;
        ~RegistryValue();
        void operator =(const RegistryValue&) = delete;
        operator DWORD() const;
        operator ULONGLONG() const;
        operator PCWSTR() const;
        PCWSTR operator [](SIZE_T index) const;
        operator const void*() const;
        void Clear();
        LSTATUS Query(HKEY hKey, PCWSTR pszName);
        PCWSTR Expand();
        DWORD get_Type() const;
        DWORD get_Size() const;
        long get_Int32() const;
        unsigned long get_UInt32() const;
        long long get_Int64() const;
        unsigned long long get_UInt64() const;
        PCWSTR get_SZ() const;
        const void* get_BINARY() const;

        __declspec(property(get = get_Type)) DWORD Type;
        __declspec(property(get = get_Size)) DWORD Size;
        __declspec(property(get = get_Int32)) long Int32;
        __declspec(property(get = get_UInt32)) unsigned long UInt32;
        __declspec(property(get = get_Int64)) long long Int64;
        __declspec(property(get = get_UInt64)) unsigned long long UInt64;
        __declspec(property(get = get_SZ)) PCWSTR SZ;
        __declspec(property(get = get_BINARY)) const void* BINARY;

    protected:

        DWORD m_dwType;
        DWORD m_dwSize;
        RegistryValueHolder m_value;
    };

    inline DWORD RegistryValue::get_Type() const
    {
        return m_dwType;
    }

    inline DWORD RegistryValue::get_Size() const
    {
        return m_dwSize;
    }

    inline long RegistryValue::get_Int32() const
    {
        return static_cast<long>(operator DWORD());
    }

    inline unsigned long RegistryValue::get_UInt32() const
    {
        return operator DWORD();
    }

    inline long long RegistryValue::get_Int64() const
    {
        return static_cast<long long>(operator ULONGLONG());
    }

    inline unsigned long long RegistryValue::get_UInt64() const
    {
        return operator ULONGLONG();
    }

    inline PCWSTR RegistryValue::get_SZ() const
    {
        return operator PCWSTR();
    }

    inline const void* RegistryValue::get_BINARY() const
    {
        return operator const void*();
    }
}
