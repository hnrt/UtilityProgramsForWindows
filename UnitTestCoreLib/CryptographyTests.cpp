#include "pch.h"
#include "CppUnitTest.h"
#include "hnrt/BCryptAlgHandle.h"
#include "hnrt/BCryptKeyHandle.h"
#include "hnrt/CryptographyException.h"
#include "hnrt/Hash.h"
#include "hnrt/ByteDataFeeder.h"
#include "hnrt/StringUTF8.h"
#include "hnrt/Buffer.h"
#include "hnrt/Base64.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/Debug.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace hnrt;

namespace UnitTestCoreLib
{
	TEST_CLASS(CryptographyTests)
	{
	public:

		TEST_METHOD(Test01)
		{
			Debug::Put(L"CryptographyTest01: Started.");
			{
				BCryptAlgHandle hAlg;
				hAlg.Open(BCRYPT_AES_ALGORITHM);
				Debug::Put(L"CryptographyTest01: BCRYPT_AES_ALGORITHM=\"%s\"", hAlg.AlgorithmName);
				Assert::AreEqual(L"AES", hAlg.AlgorithmName);
				Debug::Put(L"CryptographyTest01: BlockLength=%lu", hAlg.BlockLength);
				Assert::AreEqual(16UL, hAlg.BlockLength);
				std::vector<DWORD> bsList = hAlg.BlockSizeList;
				String ss;
				for (size_t index = 0; index < bsList.size(); index++)
				{
					ss.AppendFormat(L",%lu", bsList[index]);
				}
				if (!ss.Len) ss = L",";
				Debug::Put(L"CryptographyTest01: BlockSizeList=[%zu]{%s}", bsList.size(), &ss[1]);
				Assert::AreEqual(1ULL, bsList.size());
				Assert::AreEqual(16UL, bsList[0]);
				std::vector<DWORD> kList = hAlg.KeyLengths;
				ss = L"";
				for (size_t index = 0; index < kList.size(); index++)
				{
					ss.AppendFormat(L",%lu", kList[index]);
				}
				if (!ss.Len) ss = L",";
				Debug::Put(L"CryptographyTest01: KeyLengths=[%zu]{%s}", kList.size(), &ss[1]);
				Assert::AreEqual(3ULL, kList.size());
				Assert::AreEqual(128UL, kList[0]);
				Assert::AreEqual(192UL, kList[1]);
				Assert::AreEqual(256UL, kList[2]);
			}
			Debug::Put(L"CryptographyTest01: Ended.");
		}

		TEST_METHOD(Test02)
		{
			Debug::Put(L"CryptographyTest02: Started.");
			{
				BCryptAlgHandle hAlg;
				hAlg.Open(BCRYPT_AES_ALGORITHM);
				BCryptKeyHandle hKey;
				unsigned char key128[16] = { 0 };
				hKey.Generate(hAlg, key128, sizeof(key128));
				Debug::Put(L"CryptographyTest02: (16 bytes) KeyLength=%lu", hKey.KeyLength);
				Assert::AreEqual(128UL, hKey.KeyLength);
				unsigned char key192[24] = { 0 };
				hKey.Generate(hAlg, key192, sizeof(key192));
				Debug::Put(L"CryptographyTest02: (24 bytes) KeyLength=%lu", hKey.KeyLength);
				Assert::AreEqual(192UL, hKey.KeyLength);
				unsigned char key256[32] = { 0 };
				hKey.Generate(hAlg, key256, sizeof(key256));
				Debug::Put(L"CryptographyTest02: (32 bytes) KeyLength=%lu", hKey.KeyLength);
				Assert::AreEqual(256UL, hKey.KeyLength);
			}
			Debug::Put(L"CryptographyTest02: Ended.");
		}

		TEST_METHOD(Test03)
		{
			Debug::Put(L"CryptographyTest03: Started.");
			try
			{
				StringUTF8 keyHint("AKB48");
				ByteDataFeeder bdf1(keyHint.Ptr, keyHint.Len, 32);
				SHA256Hash keyHash(bdf1);
				Debug::Put(L"CryptographyTest03: key=%s", keyHash.Text);
				StringUTF8 ivHint("Fortune Cookie");
				ByteDataFeeder bdf2(ivHint.Ptr, ivHint.Len, 32);
				MD5Hash ivHash(bdf2);
				Debug::Put(L"CryptographyTest03: iv=%s", ivHash.Text);
				BCryptAlgHandle hAlg;
				hAlg.Open(BCRYPT_AES_ALGORITHM);
				BCryptKeyHandle hKey;
				Buffer<BYTE> key(keyHash.ValueLength);
				memcpy_s(key, key.Len, keyHash.Value, keyHash.ValueLength);
				hKey.Generate(hAlg, key, key.Len);
				Buffer<BYTE> iv(ivHash.ValueLength);
				memcpy_s(iv, iv.Len, ivHash.Value, ivHash.ValueLength);
				StringUTF8 src("May the Force be with you!");
				Buffer<BYTE> buf(src.Len);
				memcpy_s(buf, buf.Len, src, src.Len);
				RefPtr<SecretBuffer> encrypted = hKey.Encrypt(buf, buf.Len, iv, iv.Len);
				Base64Encoder e;
				e.Append(encrypted->Ptr, encrypted->Len);
				e.End();
				Debug::Put(L"CryptographyTest03: encrypted=%s", e.Ptr);
				// echo -n "May the Force be with you!" | openssl aes-256-cbc -K KEY -iv IV -base64
				Assert::AreEqual(L"4Monw+ztEcNkfpr7iGWMZtPqxK/Gr9cKeAv1kzRaDxg=", e.Ptr);
				Base64Decoder d;
				Assert::IsTrue(d.Parse(e));
				memcpy_s(iv, iv.Len, ivHash.Value, ivHash.ValueLength);
				RefPtr<SecretBuffer> decrypted = hKey.Decrypt(d.Ptr, d.Len, iv, iv.Len);
				String dst(CP_UTF8, reinterpret_cast<PSTR>(decrypted->Ptr), src.Len);
				Debug::Put(L"CryptographyTest03: decrypted=%s", dst);
				Assert::AreEqual(L"May the Force be with you!", dst);
				Debug::Put(L"CryptographyTest03: OK!");
			}
			catch (CryptographyException ce)
			{
				Assert::Fail(String(L"CryptographyTest03: %s", ce.Message));
			}
			Debug::Put(L"CryptographyTest03: Ended.");
		}

		TEST_METHOD(Test04)
		{
			Debug::Put(L"CryptographyTest04: Started.");
			try
			{
				StringUTF8 keyHint("MAX/NANA");
				ByteDataFeeder bdf1(keyHint.Ptr, keyHint.Len, 32);
				SHA256Hash keyHash(bdf1);
				Debug::Put(L"CryptographyTest04: key=%s", keyHash.Text);
				StringUTF8 ivHint("20231204");
				ByteDataFeeder bdf2(ivHint.Ptr, ivHint.Len, 32);
				MD5Hash ivHash(bdf2);
				Debug::Put(L"CryptographyTest04: iv=%s", ivHash.Text);
				Base64Decoder d;
				// echo -n "澤岻奈々子(タクシナナコ)" | openssl aes-256-cbc -K d230b66ffcc15cb2f7ab7c30ce497a5e7d46059a428d0a1330c6af3de39fd001 -iv 525a2888da84049530cd95ca7083e717 -base64
				Assert::IsTrue(d.Parse(L"QJdJsUsPX/ZSE9FcfZULAGgb6JFvHDwcNbFJ+yett40Fgen0RCtcSEWJtqew9YB2"));
				BCryptAlgHandle hAlg;
				hAlg.Open(BCRYPT_AES_ALGORITHM);
				BCryptKeyHandle hKey;
				Buffer<BYTE> key(keyHash.ValueLength);
				memcpy_s(key, key.Len, keyHash.Value, keyHash.ValueLength);
				hKey.Generate(hAlg, key, key.Len);
				Buffer<BYTE> iv(ivHash.ValueLength);
				memcpy_s(iv, iv.Len, ivHash.Value, ivHash.ValueLength);
				RefPtr<SecretBuffer> decrypted = hKey.Decrypt(d.Ptr, d.Len, iv, iv.Len);
				UINT len = 5 * 3 + 1 + 6 * 3 + 1;
				String dst(CP_UTF8, reinterpret_cast<PSTR>(decrypted->Ptr), len);
				Debug::Put(L"CryptographyTest04: decrypted=%s", dst);
				Assert::AreEqual(L"澤岻奈々子(タクシナナコ)", dst);
				Debug::Put(L"CryptographyTest04: OK!");
			}
			catch (CryptographyException ce)
			{
				Assert::Fail(String(L"CryptographyTest04: %s", ce.Message));
			}
			Debug::Put(L"CryptographyTest04: Ended.");
		}
	};
}
