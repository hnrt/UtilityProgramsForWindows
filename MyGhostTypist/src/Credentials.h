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

        static RefPtr<Credentials> Create(LONG version = 2L);

        Credentials(const Credentials&) = delete;
        ~Credentials() = default;
        void operator =(const Credentials&) = delete;
        RefPtr<Credentials> Clone() const;
        void ClearPlainText();
        const String& get_Username() const;
        void set_Username(const String& sz);
        const String& get_Password() const;
        void set_Password(const String& sz);
        const String& get_EncryptedPassword() const;
        void set_EncryptedPassword(const String& sz);
        const String& get_Key() const;
        void set_Key(const String& sz);
        void set_Callback(CredentialsCallback* pCallback);

        __declspec(property(get = get_Username, put = set_Username)) const String& Username;
        __declspec(property(get = get_Password, put = set_Password)) const String& Password;
        __declspec(property(get = get_EncryptedPassword, put = set_EncryptedPassword)) const String& EncryptedPassword;
        __declspec(property(get = get_Key, put = set_Key)) const String& Key;
        __declspec(property(put = set_Callback)) CredentialsCallback* Callback;

    private:

        Credentials();
        Credentials(const unsigned char key[SECRET_KEY_LENGTH], const unsigned char iv[SECRET_IV_LENGTH]);

        String m_szUsername;
        PasswordHolder m_Password;
        String m_szKey;
        CredentialsCallback* m_pCallback;
    };

    inline void Credentials::ClearPlainText()
    {
        m_Password.ClearPlainText();
    }

    inline const String& Credentials::get_Username() const
    {
        return m_szUsername;
    }

    inline const String& Credentials::get_Password() const
    {
        return m_Password.PlainText;
    }

    inline const String& Credentials::get_EncryptedPassword() const
    {
        return m_Password.Encrypted;
    }

    inline const String& Credentials::get_Key() const
    {
        return m_szKey;
    }

    inline void Credentials::set_Callback(CredentialsCallback* pCallback)
    {
        m_pCallback = pCallback;
    }
}
