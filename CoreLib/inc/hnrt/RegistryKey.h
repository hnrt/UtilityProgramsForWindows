#pragma once


#include <Windows.h>


namespace hnrt
{
    class RegistryKey
    {
    public:

        RegistryKey();
        RegistryKey(const RegistryKey&) = delete;
        ~RegistryKey();
        void operator =(const RegistryKey&) = delete;
        operator HKEY() const;
        HKEY* operator &();
        LSTATUS Create(HKEY hKey, PCWSTR pszSubKey, DWORD dwOptions = 0, REGSAM samDesired = KEY_ALL_ACCESS, const PSECURITY_ATTRIBUTES pSecurityAttributes = nullptr, PDWORD pdwDisposition = nullptr);
        LSTATUS Open(HKEY hKey, PCWSTR pszSubKey, DWORD dwOptions = 0, REGSAM samDesired = KEY_ALL_ACCESS);
        LSTATUS Close();

    protected:

        HKEY m_h;
    };

    inline RegistryKey::operator HKEY() const
    {
        return m_h;
    }
}
