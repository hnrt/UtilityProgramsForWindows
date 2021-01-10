#include "pch.h"
#include "CppUnitTest.h"
#include "hnrt/SecretFactory.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/Debug.h"
#include "hnrt/PasswordHolder.h"
#include "hnrt/Base64.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace hnrt;

namespace UnitTestCoreLib
{
    TEST_CLASS(SecretTests)
    {
    public:

	TEST_METHOD(Test1)
	{
	    PCWSTR pszSource = L"ŒxŽ@‚ÌDNAŒ^“o˜^ Šg‘å";
	    Buffer<char> input;
	    input.Resize(WideCharToMultiByte(CP_UTF8, 0, pszSource, -1, NULL, 0, NULL, NULL));
	    WideCharToMultiByte(CP_UTF8, 0, pszSource, -1, input.Ptr, (int)input.Len, NULL, NULL);

	    PCWSTR pszKey = L"some considered to be unnatural.";
	    PCWSTR pszIV = L"202008231156xxxx";
	    RefPtr<Secret> secret = SecretFactory::Create(pszKey, pszIV);

	    secret->Encrypt(input.Ptr, input.Len);
	    Buffer<char> output;
	    output.Resize(secret->Len);
	    memcpy_s(output.Ptr, output.Len, secret->Ptr, secret->Len);

	    secret->Decrypt(output.Ptr, output.Len);
	    Buffer<char> output2;
	    output2.Resize(secret->Len);
	    memcpy_s(output2.Ptr, output2.Len, secret->Ptr, secret->Len);

	    StringBuffer sb(300);
	    sb.Len = 0;
	    for (char* p = input.Ptr; p < input.Ptr + input.Len; p++)
	    {
		sb.AppendFormat(L" %02X", *p & 0xFF);
	    }
	    Debug::Put(L"ORG[%zu]:%s", input.Len, sb.Ptr);
	    sb.Len = 0;
	    for (char* p = output.Ptr; p < output.Ptr + output.Len; p++)
	    {
		sb.AppendFormat(L" %02X", *p & 0xFF);
	    }
	    Debug::Put(L"ENC[%zu]:%s", output.Len, sb.Ptr);
	    sb.Len = 0;
	    for (char* p = output2.Ptr; p < output2.Ptr + output2.Len; p++)
	    {
		sb.AppendFormat(L" %02X", *p & 0xFF);
	    }
	    Debug::Put(L"DEC[%zu]:%s", output2.Len, sb.Ptr);
	    Assert::AreEqual(input.Len, output2.Len);
	    Assert::IsTrue(!memcmp(output2.Ptr, input.Ptr, input.Len));
	}

	TEST_METHOD(Test2)
	{
	    StringBuffer sb(64);
	    PCWSTR pszInput = L"Abracadabra2020$";
	    sb.Len = 0;
	    sb.AppendFormat(L"%02X", pszInput[0]);
	    for (size_t i = 1; pszInput[i]; i++)
	    {
		sb.AppendFormat(L" %02X", pszInput[i]);
	    }
	    Debug::Put(L"Input=%s {%s}", pszInput, sb.Ptr);
	    static const unsigned char key[SECRET_KEY_LENGTH] =
	    {
		0x6f, 0x09, 0xc5, 0x65, 0x8c, 0x57, 0x42, 0x2b, 0xac, 0x84, 0x2d, 0x51, 0xa3, 0xa0, 0xc5, 0x40,
		0xe7, 0x85, 0x70, 0x43, 0x80, 0x21, 0x4b, 0x2d, 0x8e, 0x85, 0xb5, 0x09, 0xfd, 0xaf, 0xb3, 0x50
	    };
	    static const unsigned char iv[SECRET_IV_LENGTH] =
	    {
		0x1c, 0x0c, 0xbf, 0x99, 0x06, 0x75, 0x4b, 0xea, 0x9e, 0x84, 0xac, 0xfa, 0x3f, 0xc6, 0x3a, 0x55
	    };
	    PasswordHolder ph(key, iv);
	    ph.PlainText = pszInput;
	    PCWSTR pszEncrypted = ph.Encrypted;
	    PCWSTR pszPlain = ph.PlainText;
	    Assert::AreEqual(pszInput, pszPlain);
	    Base64Decoder dec;
	    Assert::IsTrue(dec.Parse(pszEncrypted));
	    sb.Len = 0;
	    sb.AppendFormat(L"%02X", dec.Ptr[0]);
	    for (size_t i = 1; i < dec.Len; i++)
	    {
		sb.AppendFormat(L" %02X", dec.Ptr[i]);
	    }
	    Debug::Put(L"Encrypted=%s {%s}", pszEncrypted, sb.Ptr);
	    sb.Len = 0;
	    sb.AppendFormat(L"%02X", pszPlain[0]);
	    for (size_t i = 1; pszPlain[i]; i++)
	    {
		sb.AppendFormat(L" %02X", pszPlain[i]);
	    }
	    Debug::Put(L"Output=%s {%s}", pszPlain, sb.Ptr);
	    ph.ClearPlainText();
	}

	TEST_METHOD(Test3)
	{
	    StringBuffer sb(64);
	    PCWSTR pszInput = L"a1s2hs0nb4n";
	    Debug::Put(L"Input=%s", pszInput);
	    static const unsigned char key[SECRET_KEY_LENGTH] =
	    {
		0x6f, 0x09, 0xc5, 0x65, 0x8c, 0x57, 0x42, 0x2b, 0xac, 0x84, 0x2d, 0x51, 0xa3, 0xa0, 0xc5, 0x40,
		0xe7, 0x85, 0x70, 0x43, 0x80, 0x21, 0x4b, 0x2d, 0x8e, 0x85, 0xb5, 0x09, 0xfd, 0xaf, 0xb3, 0x50
	    };
	    static const unsigned char iv[SECRET_IV_LENGTH] =
	    {
		0x1c, 0x0c, 0xbf, 0x99, 0x06, 0x75, 0x4b, 0xea, 0x9e, 0x84, 0xac, 0xfa, 0x3f, 0xc6, 0x3a, 0x55
	    };
	    RefPtr<Secret> secret = SecretFactory::Create(key, iv);
	    int cb = WideCharToMultiByte(CP_UTF8, 0, pszInput, -1, NULL, 0, NULL, NULL);
	    Buffer<char> tmp(cb);
	    WideCharToMultiByte(CP_UTF8, 0, pszInput, -1, tmp, cb, NULL, NULL);
	    secret->Encrypt(tmp.Ptr, tmp.Len);
	    Buffer<unsigned char> encrypted(secret->Len);
	    memcpy_s(encrypted.Ptr, encrypted.Len, secret->Ptr, secret->Len);
	    secret->Decrypt(encrypted.Ptr, encrypted.Len);
	    Buffer<unsigned char> decrypted(secret->Len);
	    memcpy_s(decrypted.Ptr, decrypted.Len, secret->Ptr, secret->Len);
	    Debug::Put(L" Original={%s}", DumpBytes(tmp.Ptr, tmp.Len, sb));
	    Debug::Put(L"Encrypted={%s}", DumpBytes(encrypted.Ptr, encrypted.Len, sb));
	    Debug::Put(L"Decrypted={%s}", DumpBytes(decrypted.Ptr, decrypted.Len, sb));
	    Assert::AreEqual(tmp.Len, decrypted.Len);
	    if (tmp.Len == decrypted.Len)
	    {
		Assert::IsTrue(!memcmp(tmp.Ptr, decrypted.Ptr, decrypted.Len));
	    }
	    Base64Encoder enc;
	    enc.Append(encrypted.Ptr, encrypted.Len);
	    enc.End();
	    Base64Decoder dec;
	    dec.Parse(enc.Ptr);
	    Debug::Put(L"base64 encoded=%s", enc.Ptr);
	    Debug::Put(L"base64  in={%s}", DumpBytes(encrypted.Ptr, encrypted.Len, sb));
	    Debug::Put(L"base64 out={%s}", DumpBytes(dec.Ptr, dec.Len, sb));
	    Assert::AreEqual(encrypted.Len, dec.Len);
	    if (encrypted.Len == dec.Len)
	    {
		Assert::IsTrue(!memcmp(encrypted.Ptr, dec.Ptr, dec.Len));
	    }
	    PasswordHolder ph(key, iv);
	    ph.PlainText = pszInput;
	    PCWSTR pszEncrypted = ph.Encrypted;
	    PCWSTR pszPlain = ph.PlainText;
	    Debug::Put(L"PH  in=%s", pszInput);
	    Debug::Put(L"PH enc=%s", pszEncrypted);
	    Debug::Put(L"PH out=%s", pszPlain);
	    Assert::AreEqual(pszInput, pszPlain);
	    ph.ClearPlainText();
	}

	static PCWSTR DumpBytes(const void* ptr, size_t len, StringBuffer& buf)
	{
	    if (!len)
	    {
		return L"";
	    }
	    const unsigned char* pCur = reinterpret_cast<const unsigned char*>(ptr);
	    const unsigned char* pEnd = pCur + len;
	    buf.Len = 0;
	    buf.AppendFormat(L"%02X", *pCur++);
	    while (pCur < pEnd)
	    {
		buf.AppendFormat(L" %02X", *pCur++);
	    }
	    return buf.Ptr;
	}
    };
}
