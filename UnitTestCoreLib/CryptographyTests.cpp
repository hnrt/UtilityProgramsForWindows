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

		TEST_METHOD(Test_AES_128_CBC_Encrypt)
		{
			Debug::Put(L"Test_AES_128_CBC_Encrypt: Started.");
			try
			{
				StringUTF8 d(L"澤岻奈々子(タクシナナコ)");
				StringUTF8 keyHint("MAX/NANA");
				ByteDataFeeder bdf1(keyHint.Ptr, keyHint.Len, 32);
				MD5Hash keyHash(bdf1); // 128 bits (16 bytes)
				Buffer<BYTE> key(keyHash.ValueLength);
				memcpy_s(key, key.Len, keyHash.Value, keyHash.ValueLength);
				Debug::Put(L"Test_AES_128_CBC_Encrypt: key=%s", keyHash.Text);
				StringUTF8 ivHint("20231204");
				ByteDataFeeder bdf2(ivHint.Ptr, ivHint.Len, 32);
				MD5Hash ivHash(bdf2); // 128 bits (16 bytes)
				Buffer<BYTE> iv(ivHash.ValueLength);
				memcpy_s(iv, iv.Len, ivHash.Value, ivHash.ValueLength);
				Debug::Put(L"Test_AES_128_CBC_Encrypt: iv=%s", ivHash.Text);
				BCryptAlgHandle hAlg;
				hAlg.Open(BCRYPT_AES_ALGORITHM);
				hAlg.ChainingMode = BCRYPT_CHAIN_MODE_CBC; // Cipher Block Chaining Mode
				BCryptKeyHandle hKey;
				hKey.Generate(hAlg, key, key.Len);
				Debug::Put(L"Test_AES_128_CBC_Encrypt: KeyLength=%lu ChainingMode=%s BlockLength=%lu", hKey.KeyLength, hAlg.ChainingMode, hAlg.BlockLength);
				ByteString encrypted = hKey.Encrypt(const_cast<PSTR>(d.Ptr), d.Len, iv, iv.Len);
				Base64Encoder e;
				e.Append(encrypted, encrypted.Len);
				e.End();
				Debug::Put(L"Test_AES_128_CBC_Encrypt: encrypted=%s", e.Ptr);
				// echo -n "澤岻奈々子(タクシナナコ)"|openssl aes-128-cbc -K 3baa74d5f939a874604238623ed2906d -iv 525a2888da84049530cd95ca7083e717 -base64
				Assert::AreEqual(L"JWtX4f0wPFTE4x3a2DUhLXc8tzFuuamhMqCT6GC1waj9NQgnpV4p9mG5k0SjAPbK", e.Ptr);
				Debug::Put(L"Test_AES_128_CBC_Encrypt: OK!");
			}
			catch (CryptographyException ce)
			{
				Assert::Fail(String(L"Test_AES_128_CBC_Encrypt: %s", ce.Message));
			}
			Debug::Put(L"Test_AES_128_CBC_Encrypt: Ended.");
		}

		TEST_METHOD(Test_AES_256_CBC_Encrypt)
		{
			Debug::Put(L"Test_AES_256_CBC_Encrypt: Started.");
			try
			{
				StringUTF8 d(L"澤岻奈々子(タクシナナコ)");
				StringUTF8 keyHint("MAX/NANA");
				ByteDataFeeder bdf1(keyHint.Ptr, keyHint.Len, 32);
				SHA256Hash keyHash(bdf1); // 256 bits (32 bytes)
				Buffer<BYTE> key(keyHash.ValueLength);
				memcpy_s(key, key.Len, keyHash.Value, keyHash.ValueLength);
				Debug::Put(L"Test_AES_256_CBC_Encrypt: key=%s", keyHash.Text);
				StringUTF8 ivHint("20231204");
				ByteDataFeeder bdf2(ivHint.Ptr, ivHint.Len, 32);
				MD5Hash ivHash(bdf2); // 128 bits (16 bytes)
				Buffer<BYTE> iv(ivHash.ValueLength);
				memcpy_s(iv, iv.Len, ivHash.Value, ivHash.ValueLength);
				Debug::Put(L"Test_AES_256_CBC_Encrypt: iv=%s", ivHash.Text);
				BCryptAlgHandle hAlg;
				hAlg.Open(BCRYPT_AES_ALGORITHM);
				hAlg.ChainingMode = BCRYPT_CHAIN_MODE_CBC; // Cipher Block Chaining Mode
				BCryptKeyHandle hKey;
				hKey.Generate(hAlg, key, key.Len);
				Debug::Put(L"Test_AES_256_CBC_Encrypt: KeyLength=%lu ChainingMode=%s BlockLength=%lu", hKey.KeyLength, hAlg.ChainingMode, hAlg.BlockLength);
				ByteString encrypted = hKey.Encrypt(const_cast<PSTR>(d.Ptr), d.Len, iv, iv.Len);
				Base64Encoder e;
				e.Append(encrypted, encrypted.Len);
				e.End();
				Debug::Put(L"Test_AES_256_CBC_Encrypt: encrypted=%s", e.Ptr);
				// echo -n "澤岻奈々子(タクシナナコ)"|openssl aes-256-cbc -K d230b66ffcc15cb2f7ab7c30ce497a5e7d46059a428d0a1330c6af3de39fd001 -iv 525a2888da84049530cd95ca7083e717 -base64
				Assert::AreEqual(L"QJdJsUsPX/ZSE9FcfZULAGgb6JFvHDwcNbFJ+yett40Fgen0RCtcSEWJtqew9YB2", e.Ptr);
				Debug::Put(L"Test_AES_256_CBC_Encrypt: OK!");
			}
			catch (CryptographyException ce)
			{
				Assert::Fail(String(L"Test_AES_256_CBC_Encrypt: %s", ce.Message));
			}
			Debug::Put(L"Test_AES_256_CBC_Encrypt: Ended.");
		}

		TEST_METHOD(Test_AES_128_CBC_Decrypt)
		{
			Debug::Put(L"Test_AES_128_CBC_Decrypt: Started.");
			try
			{
				Base64Decoder d;
				// echo -n "澤岻奈々子(タクシナナコ)"|openssl aes-128-cbc -K 3baa74d5f939a874604238623ed2906d -iv 525a2888da84049530cd95ca7083e717 -base64
				Assert::IsTrue(d.Parse(L"JWtX4f0wPFTE4x3a2DUhLXc8tzFuuamhMqCT6GC1waj9NQgnpV4p9mG5k0SjAPbK"));
				StringUTF8 keyHint("MAX/NANA");
				ByteDataFeeder bdf1(keyHint.Ptr, keyHint.Len, 32);
				MD5Hash keyHash(bdf1); // 128 bits (16 bytes)
				Buffer<BYTE> key(keyHash.ValueLength);
				memcpy_s(key, key.Len, keyHash.Value, keyHash.ValueLength);
				Debug::Put(L"Test_AES_128_CBC_Decrypt: key=%s", keyHash.Text);
				StringUTF8 ivHint("20231204");
				ByteDataFeeder bdf2(ivHint.Ptr, ivHint.Len, 32);
				MD5Hash ivHash(bdf2); // 128 bits (16 bytes)
				Buffer<BYTE> iv(ivHash.ValueLength);
				memcpy_s(iv, iv.Len, ivHash.Value, ivHash.ValueLength);
				Debug::Put(L"Test_AES_128_CBC_Decrypt: iv=%s", ivHash.Text);
				BCryptAlgHandle hAlg;
				hAlg.Open(BCRYPT_AES_ALGORITHM);
				hAlg.ChainingMode = BCRYPT_CHAIN_MODE_CBC; // Cipher Block Chaining Mode
				BCryptKeyHandle hKey;
				hKey.Generate(hAlg, key, key.Len);
				Debug::Put(L"Test_AES_128_CBC_Decrypt: KeyLength=%lu ChainingMode=%s BlockLength=%lu", hKey.KeyLength, hAlg.ChainingMode, hAlg.BlockLength);
				ByteString decrypted = hKey.Decrypt(d.Ptr, d.Len, iv, iv.Len);
				String sz(CP_UTF8, decrypted, decrypted.Len);
				Debug::Put(L"Test_AES_128_CBC_Decrypt: decrypted=%s", sz);
				Assert::AreEqual(L"澤岻奈々子(タクシナナコ)", sz);
				Debug::Put(L"Test_AES_128_CBC_Decrypt: OK!");
			}
			catch (CryptographyException ce)
			{
				Assert::Fail(String(L"Test_AES_128_CBC_Decrypt: %s", ce.Message));
			}
			Debug::Put(L"Test_AES_128_CBC_Decrypt: Ended.");
		}

		TEST_METHOD(Test_AES_256_CBC_Decrypt)
		{
			Debug::Put(L"Test_AES_256_CBC_Decrypt: Started.");
			try
			{
				Base64Decoder d;
				// echo -n "澤岻奈々子(タクシナナコ)"|openssl aes-256-cbc -K d230b66ffcc15cb2f7ab7c30ce497a5e7d46059a428d0a1330c6af3de39fd001 -iv 525a2888da84049530cd95ca7083e717 -base64
				Assert::IsTrue(d.Parse(L"QJdJsUsPX/ZSE9FcfZULAGgb6JFvHDwcNbFJ+yett40Fgen0RCtcSEWJtqew9YB2"));
				StringUTF8 keyHint("MAX/NANA");
				ByteDataFeeder bdf1(keyHint.Ptr, keyHint.Len, 32);
				SHA256Hash keyHash(bdf1); // 256 bits (32 bytes)
				Buffer<BYTE> key(keyHash.ValueLength);
				memcpy_s(key, key.Len, keyHash.Value, keyHash.ValueLength);
				Debug::Put(L"Test_AES_256_CBC_Decrypt: key=%s", keyHash.Text);
				StringUTF8 ivHint("20231204");
				ByteDataFeeder bdf2(ivHint.Ptr, ivHint.Len, 32);
				MD5Hash ivHash(bdf2); // 128 bits (16 bytes)
				Buffer<BYTE> iv(ivHash.ValueLength);
				memcpy_s(iv, iv.Len, ivHash.Value, ivHash.ValueLength);
				Debug::Put(L"Test_AES_256_CBC_Decrypt: iv=%s", ivHash.Text);
				BCryptAlgHandle hAlg;
				hAlg.Open(BCRYPT_AES_ALGORITHM);
				hAlg.ChainingMode = BCRYPT_CHAIN_MODE_CBC; // Cipher Block Chaining Mode
				BCryptKeyHandle hKey;
				hKey.Generate(hAlg, key, key.Len);
				Debug::Put(L"Test_AES_256_CBC_Decrypt: KeyLength=%lu ChainingMode=%s BlockLength=%lu", hKey.KeyLength, hAlg.ChainingMode, hAlg.BlockLength);
				ByteString decrypted = hKey.Decrypt(d.Ptr, d.Len, iv, iv.Len);
				String sz(CP_UTF8, decrypted, decrypted.Len);
				Debug::Put(L"Test_AES_256_CBC_Decrypt: decrypted=%s", sz);
				Assert::AreEqual(L"澤岻奈々子(タクシナナコ)", sz);
				Debug::Put(L"Test_AES_256_CBC_Decrypt: OK!");
			}
			catch (CryptographyException ce)
			{
				Assert::Fail(String(L"Test_AES_256_CBC_Decrypt: %s", ce.Message));
			}
			Debug::Put(L"Test_AES_256_CBC_Decrypt: Ended.");
		}

		TEST_METHOD(Test_AES_128_ECB_Encrypt)
		{
			Debug::Put(L"Test_AES_128_ECB_Encrypt: Started.");
			try
			{
				StringUTF8 d(L"日本語123");
				StringUTF8 keyHint("BENI");
				ByteDataFeeder bdf(keyHint.Ptr, keyHint.Len, 32);
				MD5Hash keyHash(bdf); // 128 bits
				Buffer<BYTE> key(keyHash.ValueLength);
				memcpy_s(key, key.Len, keyHash.Value, keyHash.ValueLength);
				Debug::Put(L"Test_AES_128_ECB_Encrypt: key=%s", keyHash.Text);
				BCryptAlgHandle hAlg;
				hAlg.Open(BCRYPT_AES_ALGORITHM);
				hAlg.ChainingMode = BCRYPT_CHAIN_MODE_ECB; // Electronic Codebook Mode
				BCryptKeyHandle hKey;
				hKey.Generate(hAlg, key, key.Len);
				Debug::Put(L"Test_AES_128_ECB_Encrypt: KeyLength=%lu ChainingMode=%s BlockLength=%lu", hKey.KeyLength, hAlg.ChainingMode, hAlg.BlockLength);
				ByteString encrypted = hKey.Encrypt(const_cast<PSTR>(d.Ptr), d.Len);
				Base64Encoder e;
				e.Append(encrypted, encrypted.Len);
				e.End();
				Debug::Put(L"Test_AES_128_ECB_Encrypt: encrypted=%s", e.Ptr);
				//echo -n "日本語123"|openssl aes-128-ecb -K 056c5ef8a258a9faaced455d1bba59b1 -base64
				Assert::AreEqual(L"D7maX+hCE+90+eEQvoNIDQ==", e.Ptr);
				Debug::Put(L"Test_AES_128_ECB_Encrypt: OK!");
			}
			catch (CryptographyException ce)
			{
				Assert::Fail(String(PRINTF, L"Test_AES_128_ECB_Encrypt: %s", ce.Message));
			}
			Debug::Put(L"Test_AES_128_ECB_Encrypt: Ended.");
		}

		TEST_METHOD(Test_AES_256_ECB_Encrypt)
		{
			Debug::Put(L"Test_AES_256_ECB_Encrypt: Started.");
			try
			{
				StringUTF8 d(L"日本語123");
				StringUTF8 keyHint("BENI");
				ByteDataFeeder bdf(keyHint.Ptr, keyHint.Len, 32);
				SHA256Hash keyHash(bdf); // 256 bits
				Buffer<BYTE> key(keyHash.ValueLength);
				memcpy_s(key, key.Len, keyHash.Value, keyHash.ValueLength);
				Debug::Put(L"Test_AES_256_ECB_Encrypt: key=%s", keyHash.Text);
				BCryptAlgHandle hAlg;
				hAlg.Open(BCRYPT_AES_ALGORITHM);
				hAlg.ChainingMode = BCRYPT_CHAIN_MODE_ECB; // Electronic Codebook Mode
				BCryptKeyHandle hKey;
				hKey.Generate(hAlg, key, key.Len);
				Debug::Put(L"Test_AES_256_ECB_Encrypt: KeyLength=%lu ChainingMode=%s BlockLength=%lu", hKey.KeyLength, hAlg.ChainingMode, hAlg.BlockLength);
				ByteString encrypted = hKey.Encrypt(const_cast<PSTR>(d.Ptr), d.Len);
				Base64Encoder e;
				e.Append(encrypted, encrypted.Len);
				e.End();
				Debug::Put(L"Test_AES_256_ECB_Encrypt: encrypted=%s", e.Ptr);
				//echo -n "日本語123"|openssl aes-256-ecb -K 50aaddee463b2eb7e53f9a83251ee15092582855663e04b0cf36f42b87eb84a2 -base64
				Assert::AreEqual(L"+cqKQajlMuYqTS+qBVAQCg==", e.Ptr);
				Debug::Put(L"Test_AES_256_ECB_Encrypt: OK!");
			}
			catch (CryptographyException ce)
			{
				Assert::Fail(String(PRINTF, L"Test_AES_256_ECB_Encrypt: %s", ce.Message));
			}
			Debug::Put(L"Test_AES_256_ECB_Encrypt: Ended.");
		}

		TEST_METHOD(Test_AES_128_ECB_Decrypt)
		{
			Debug::Put(L"Test_AES_128_ECB_Decrypt: Started.");
			try
			{
				Base64Decoder d;
				//echo -n "日本語123"|openssl aes-128-ecb -K 056c5ef8a258a9faaced455d1bba59b1 -base64
				Assert::IsTrue(d.Parse(L"D7maX+hCE+90+eEQvoNIDQ=="));
				StringUTF8 keyHint("BENI");
				ByteDataFeeder bdf(keyHint.Ptr, keyHint.Len, 32);
				MD5Hash keyHash(bdf); // 128 bits
				Buffer<BYTE> key(keyHash.ValueLength);
				memcpy_s(key, key.Len, keyHash.Value, keyHash.ValueLength);
				Debug::Put(L"Test_AES_128_ECB_Decrypt: key=%s", keyHash.Text);
				BCryptAlgHandle hAlg;
				hAlg.Open(BCRYPT_AES_ALGORITHM);
				hAlg.ChainingMode = BCRYPT_CHAIN_MODE_ECB; // Electronic Codebook Mode
				BCryptKeyHandle hKey;
				hKey.Generate(hAlg, key, key.Len);
				Debug::Put(L"Test_AES_128_ECB_Decrypt: KeyLength=%lu ChainingMode=%s BlockLength=%lu", hKey.KeyLength, hAlg.ChainingMode, hAlg.BlockLength);
				ByteString decrypted = hKey.Decrypt(d.Ptr, d.Len);
				String sz(CP_UTF8, decrypted, decrypted.Len);
				Debug::Put(L"Test_AES_128_ECB_Decrypt: decrypted=%s", sz);
				Assert::AreEqual(L"日本語123", sz);
				Debug::Put(L"Test_AES_128_ECB_Decrypt: OK!");
			}
			catch (CryptographyException ce)
			{
				Assert::Fail(String(PRINTF, L"Test_AES_128_ECB_Decrypt: %s", ce.Message));
			}
			Debug::Put(L"Test_AES_128_ECB_Decrypt: Ended.");
		}

		TEST_METHOD(Test_AES_256_ECB_Decrypt)
		{
			Debug::Put(L"Test_AES_256_ECB_Decrypt: Started.");
			try
			{
				Base64Decoder d;
				//echo -n "日本語123"|openssl aes-256-ecb -K 50aaddee463b2eb7e53f9a83251ee15092582855663e04b0cf36f42b87eb84a2 -base64
				Assert::IsTrue(d.Parse(L"+cqKQajlMuYqTS+qBVAQCg=="));
				StringUTF8 keyHint("BENI");
				ByteDataFeeder bdf(keyHint.Ptr, keyHint.Len, 32);
				SHA256Hash keyHash(bdf); // 256 bits
				Buffer<BYTE> key(keyHash.ValueLength);
				memcpy_s(key, key.Len, keyHash.Value, keyHash.ValueLength);
				Debug::Put(L"Test_AES_256_ECB_Decrypt: key=%s", keyHash.Text);
				BCryptAlgHandle hAlg;
				hAlg.Open(BCRYPT_AES_ALGORITHM);
				hAlg.ChainingMode = BCRYPT_CHAIN_MODE_ECB; // Electronic Codebook Mode
				BCryptKeyHandle hKey;
				hKey.Generate(hAlg, key, key.Len);
				Debug::Put(L"Test_AES_256_ECB_Decrypt: KeyLength=%lu ChainingMode=%s BlockLength=%lu", hKey.KeyLength, hAlg.ChainingMode, hAlg.BlockLength);
				ByteString decrypted = hKey.Decrypt(d.Ptr, d.Len);
				String sz(CP_UTF8, decrypted, decrypted.Len);
				Debug::Put(L"Test_AES_256_ECB_Decrypt: decrypted=%s", sz);
				Assert::AreEqual(L"日本語123", sz);
				Debug::Put(L"Test_AES_256_ECB_Decrypt: OK!");
			}
			catch (CryptographyException ce)
			{
				Assert::Fail(String(PRINTF, L"Test_AES_256_ECB_Decrypt: %s", ce.Message));
			}
			Debug::Put(L"Test_AES_256_ECB_Decrypt: Ended.");
		}
	};
}
