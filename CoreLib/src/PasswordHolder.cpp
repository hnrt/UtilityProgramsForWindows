#include "pch.h"
#include "hnrt/PasswordHolder.h"
#include "hnrt/SecretFactory.h"
#include "hnrt/Base64.h"
#include "hnrt/MultibyteString.h"
#include "hnrt/Win32Exception.h"


using namespace hnrt;


PasswordHolder::PasswordHolder(const unsigned char key[SECRET_KEY_LENGTH], const unsigned char iv[SECRET_IV_LENGTH])
    : m_pSecret(SecretFactory::Create(key, iv))
    , m_szPlainText()
    , m_szEncrypted()
{
}


PasswordHolder::~PasswordHolder()
{
    m_szPlainText.ZeroFill();
    m_szEncrypted.ZeroFill();
}


const String& PasswordHolder::get_Encrypted() const
{
    return m_szEncrypted;
}


void PasswordHolder::set_Encrypted(const String& sz)
{
    m_szEncrypted.ZeroFill();
    m_szEncrypted = sz;
}


const String& PasswordHolder::get_PlainText() const
{
    if (!m_szPlainText.Len)
    {
        if (m_szEncrypted.Len)
        {
            Base64Decoder dec;
            if (!dec.Parse(m_szEncrypted))
            {
                throw Exception(L"PasswordHolder failed base64 decoding.");
            }
            m_pSecret->Decrypt(dec.Ptr, dec.Len);
            int cch = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<LPCCH>(m_pSecret->Ptr), -1, NULL, 0);
            if (cch < 0)
            {
                m_pSecret->ClearBuffer();
                throw Win32Exception(GetLastError(), L"PasswordHolder failed text encoding conversion to UTF-16.");
            }
            m_szPlainText = String(CP_UTF8, reinterpret_cast<LPCCH>(m_pSecret->Ptr));
            m_pSecret->ClearBuffer();
        }
    }
    return m_szPlainText;
}


void PasswordHolder::set_PlainText(const String& sz)
{
    ClearPlainText();
    MultibyteString deserialized = sz.ToUTF8();
    m_pSecret->Encrypt(deserialized.Ptr, deserialized.Len + 1);
    memset(const_cast<PCHAR>(deserialized.Ptr), 0, deserialized.Len);
    Base64Encoder enc;
    enc.Append(m_pSecret->Ptr, m_pSecret->Len);
    enc.End();
    m_szEncrypted = enc.Ptr;
}


void PasswordHolder::ClearPlainText()
{
    m_szPlainText.ZeroFill();
}
