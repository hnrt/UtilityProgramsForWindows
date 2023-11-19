#pragma once


#include "hnrt/RefPtr.h"
#include "hnrt/Secret.h"
#include "hnrt/String.h"


namespace hnrt
{
    class PasswordHolder
    {
    public:

        PasswordHolder(const unsigned char key[SECRET_KEY_LENGTH], const unsigned char iv[SECRET_IV_LENGTH]);
        PasswordHolder(const PasswordHolder&) = delete;
        ~PasswordHolder();
        void operator =(const PasswordHolder&) = delete;
        const String& get_Encrypted() const;
        void set_Encrypted(const String& sz);
        const String& get_PlainText() const;
        void set_PlainText(const String& sz);
        void ClearPlainText();
        __declspec(property(get = get_Encrypted, put = set_Encrypted)) const String& Encrypted;
        __declspec(property(get = get_PlainText, put = set_PlainText)) const String& PlainText;

    private:

        mutable RefPtr<Secret> m_pSecret;
        mutable String m_szPlainText;
        String m_szEncrypted;
    };
}
