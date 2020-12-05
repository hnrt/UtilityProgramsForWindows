#pragma once


#include "hnrt/RefObj.h"
#include "hnrt/RefPtr.h"
#include "hnrt/PasswordHolder.h"


namespace hnrt
{
    class Credentials;

    class CredentialsCallback
    {
    public:

        virtual void OnCredentialsUpdate(Credentials&) = 0;
    };

    class Credentials
        : public RefObj
    {
    public:

        static RefPtr<Credentials> Create();

        Credentials(const Credentials&) = delete;
        ~Credentials() = default;
        void operator =(const Credentials&) = delete;
        RefPtr<Credentials> Clone() const;
        void ClearPlainText();
        PCWSTR get_Username() const;
        void set_Username(PCWSTR psz);
        PCWSTR get_Password() const;
        void set_Password(PCWSTR psz);
        PCWSTR get_EncryptedPassword() const;
        void set_EncryptedPassword(PCWSTR psz);
        PCWSTR get_Key() const;
        void set_Key(PCWSTR psz);
        void set_Callback(CredentialsCallback* pCallback);

        __declspec(property(get = get_Username, put = set_Username)) PCWSTR Username;
        __declspec(property(get = get_Password, put = set_Password)) PCWSTR Password;
        __declspec(property(get = get_EncryptedPassword, put = set_EncryptedPassword)) PCWSTR EncryptedPassword;
        __declspec(property(get = get_Key, put = set_Key)) PCWSTR Key;
        __declspec(property(put = set_Callback)) CredentialsCallback* Callback;

    private:

        Credentials();

        PCWSTR m_pszUsername;
        PasswordHolder m_Password;
        PCWSTR m_pszKey;
        CredentialsCallback* m_pCallback;
    };

    inline void Credentials::ClearPlainText()
    {
        m_Password.ClearPlainText();
    }

    inline PCWSTR Credentials::get_Username() const
    {
        return m_pszUsername;
    }

    inline PCWSTR Credentials::get_Password() const
    {
        return m_Password.PlainText;
    }

    inline PCWSTR Credentials::get_EncryptedPassword() const
    {
        return m_Password.Encrypted;
    }

    inline PCWSTR Credentials::get_Key() const
    {
        return m_pszKey;
    }

    inline void Credentials::set_Callback(CredentialsCallback* pCallback)
    {
        m_pCallback = pCallback;
    }
}
