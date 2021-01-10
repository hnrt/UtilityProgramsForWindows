#include "pch.h"
#include "hnrt/PasswordHolder.h"
#include "hnrt/SecretFactory.h"
#include "hnrt/Base64.h"
#include "hnrt/Win32Exception.h"


using namespace hnrt;


PasswordHolder::PasswordHolder(const unsigned char key[SECRET_KEY_LENGTH], const unsigned char iv[SECRET_IV_LENGTH])
    : m_pSecret(SecretFactory::Create(key, iv))
    , m_PlainText()
    , m_Encrypted()
{
}


PasswordHolder::~PasswordHolder()
{
    wmemset(m_PlainText.Ptr, 0, m_PlainText.Len);
    wmemset(m_Encrypted.Ptr, 0, m_Encrypted.Len);
}


PCWSTR PasswordHolder::get_Encrypted() const
{
    return m_Encrypted.Ptr;
}


void PasswordHolder::set_Encrypted(PCWSTR psz)
{
    m_Encrypted.Resize(wcslen(psz) + 1);
    wcscpy_s(m_Encrypted.Ptr, m_Encrypted.Len, psz);
}


PCWSTR PasswordHolder::get_PlainText() const
{
    if (!m_PlainText.Len)
    {
        if (m_Encrypted.Len)
        {
            Base64Decoder dec;
            if (!dec.Parse(m_Encrypted.Ptr))
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
            m_PlainText.Resize(cch);
            MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<LPCCH>(m_pSecret->Ptr), -1, m_PlainText.Ptr, cch);
            m_pSecret->ClearBuffer();
        }
    }
    return m_PlainText.Ptr;
}


void PasswordHolder::set_PlainText(PCWSTR psz)
{
    ClearPlainText();
    int cb = WideCharToMultiByte(CP_UTF8, 0, psz, -1, NULL, 0, NULL, NULL);
    if (cb < 0)
    {
        throw Win32Exception(GetLastError(), L"PasswordHolder failed text encoding conversion to UTF-8.");
    }
    Buffer<char> tmp(cb);
    WideCharToMultiByte(CP_UTF8, 0, psz, -1, tmp, cb, NULL, NULL);
    m_pSecret->Encrypt(tmp.Ptr, tmp.Len);
    memset(tmp.Ptr, 0, tmp.Len);
    Base64Encoder enc;
    enc.Append(m_pSecret->Ptr, m_pSecret->Len);
    enc.End();
    m_Encrypted.Resize(enc.Len + 1);
    wcscpy_s(m_Encrypted.Ptr, m_Encrypted.Len, enc.Ptr);
}


void PasswordHolder::ClearPlainText()
{
    if (m_PlainText.Len)
    {
        wmemset(m_PlainText.Ptr, 0, m_PlainText.Len);
        m_PlainText.Resize(0);
    }
}
