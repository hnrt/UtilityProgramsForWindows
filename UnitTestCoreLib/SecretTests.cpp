#include "pch.h"
#include "CppUnitTest.h"
#include "hnrt/SecretFactory.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/Debug.h"
#include "hnrt/PasswordHolder.h"
#include "hnrt/Base64.h"
#include "hnrt/MultibyteString.h"
#include "hnrt/Buffer.h"

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
			MultibyteString input(CP_UTF8, pszSource);

			PCWSTR pszKey = L"some considered to be unnatural.";
			PCWSTR pszIV = L"202008231156xxxx";
			RefPtr<Secret> secret = SecretFactory::Create(pszKey, pszIV);

			secret->Encrypt(input.Ptr, input.Len + 1);
			Buffer<char> output(secret->Len);
			memcpy_s(output.Ptr, output.Len, secret->Ptr, secret->Len);

			secret->Decrypt(output.Ptr, output.Len);
			Buffer<char> output2(secret->Len);
			memcpy_s(output2.Ptr, output2.Len, secret->Ptr, secret->Len);

			Debug::Put(L"ORG[%zu]: %s", input.Len + 1, ByteString(input.Ptr, input.Len + 1).ToHex());
			Debug::Put(L"ENC[%zu]: %s", output.Len, ByteString(output.Ptr, output.Len).ToHex());
			Debug::Put(L"DEC[%zu]: %s", output2.Len, ByteString(output2.Ptr, output2.Len).ToHex());

			Assert::AreEqual(input.Len + 1, output2.Len);
			Assert::IsTrue(!memcmp(output2.Ptr, input.Ptr, input.Len + 1));
		}

		TEST_METHOD(Test2)
		{
			String szInput(L"Abracadabra2020$");
			Debug::Put(L"Input=%s (%s)", szInput, ByteString(szInput.Ptr, (szInput.Len + 1) * sizeof(WCHAR)).ToHex());
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
			ph.PlainText = szInput;
			Assert::AreEqual(szInput.Ptr, ph.PlainText.Ptr);
			Base64Decoder dec;
			Assert::IsTrue(dec.Parse(ph.Encrypted));
			Debug::Put(L"Encrypted=%s (%s)", ph.Encrypted, ByteString(ph.Encrypted, ph.Encrypted.Len).ToHex());
			Debug::Put(L"Output=%s (%s)", ph.PlainText, ByteString(ph.PlainText.Ptr, (ph.PlainText.Len + 1) * sizeof(WCHAR)).ToHex());
			ph.ClearPlainText();
		}

		TEST_METHOD(Test3)
		{
			String szInput = L"a1s2hs0nb4n";
			Debug::Put(L"Input=%s", szInput);
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
			MultibyteString tmp(CP_UTF8, szInput);
			secret->Encrypt(tmp.Ptr, tmp.Len + 1);
			Buffer<unsigned char> encrypted(secret->Len);
			memcpy_s(encrypted.Ptr, encrypted.Len, secret->Ptr, secret->Len);
			secret->Decrypt(encrypted.Ptr, encrypted.Len);
			Buffer<unsigned char> decrypted(secret->Len);
			memcpy_s(decrypted.Ptr, decrypted.Len, secret->Ptr, secret->Len);
			Debug::Put(L" Original={%s}", ByteString(tmp.Ptr, tmp.Len).ToHex());
			Debug::Put(L"Encrypted={%s}", ByteString(encrypted.Ptr, encrypted.Len).ToHex());
			Debug::Put(L"Decrypted={%s}", ByteString(decrypted.Ptr, decrypted.Len).ToHex());
			Assert::AreEqual(tmp.Len + 1, decrypted.Len);
			Assert::IsTrue(!memcmp(tmp.Ptr, decrypted.Ptr, decrypted.Len));
			Base64Encoder enc;
			enc.Append(encrypted.Ptr, encrypted.Len);
			enc.End();
			Base64Decoder dec;
			dec.Parse(enc.Ptr);
			Debug::Put(L"base64 encoded=%s", enc.Ptr);
			Debug::Put(L"base64  in={%s}", ByteString(encrypted.Ptr, encrypted.Len).ToHex());
			Debug::Put(L"base64 out={%s}", ByteString(dec.Ptr, dec.Len).ToHex());
			Assert::AreEqual(encrypted.Len, dec.Len);
			Assert::IsTrue(!memcmp(encrypted.Ptr, dec.Ptr, dec.Len));
			PasswordHolder ph(key, iv);
			ph.PlainText = szInput;
			Debug::Put(L"PH  in=%s", szInput);
			Debug::Put(L"PH enc=%s", ph.Encrypted);
			Debug::Put(L"PH out=%s", ph.PlainText);
			Assert::AreEqual(szInput.Ptr, ph.PlainText.Ptr);
			ph.ClearPlainText();
		}
    };
}
