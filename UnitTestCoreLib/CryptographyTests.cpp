#include "pch.h"
#include "CppUnitTest.h"
#include "hnrt/BCryptAlgHandle.h"
#include "hnrt/BCryptKeyHandle.h"
#include "hnrt/BCryptAuthenticatedCipherModeInfo.h"
#include "hnrt/CryptographyException.h"
#include "hnrt/Hash.h"
#include "hnrt/StringUTF8.h"
#include "hnrt/Buffer.h"
#include "hnrt/Base64.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/StringUtils.h"
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
				Debug::Put(L"CryptographyTest01: BlockSizeList=[%zu]{%s}", bsList.size(), StringUtils::JoinBy(bsList, L","));
				Assert::AreEqual(1ULL, bsList.size());
				Assert::AreEqual(16UL, bsList[0]);
				std::vector<DWORD> kList = hAlg.KeyLengths;
				Debug::Put(L"CryptographyTest01: KeyLengths=[%zu]{%s}", kList.size(), StringUtils::JoinBy(kList, L","));
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
				MD5Hash keyHash(keyHint, keyHint.Len); // 128 bits (16 bytes)
				Buffer<BYTE> key(keyHash.ValueLength);
				memcpy_s(key, key.Len, keyHash.Value, keyHash.ValueLength);
				Debug::Put(L"Test_AES_128_CBC_Encrypt: key=%s", ToHex(keyHash));
				StringUTF8 ivHint("20231204");
				MD5Hash ivHash(ivHint, ivHint.Len); // 128 bits (16 bytes)
				Buffer<BYTE> iv(ivHash.ValueLength);
				memcpy_s(iv, iv.Len, ivHash.Value, ivHash.ValueLength);
				Debug::Put(L"Test_AES_128_CBC_Encrypt: iv=%s", ToHex(ivHash));
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
				SHA256Hash keyHash(keyHint, keyHint.Len); // 256 bits (32 bytes)
				Buffer<BYTE> key(keyHash.ValueLength);
				memcpy_s(key, key.Len, keyHash.Value, keyHash.ValueLength);
				Debug::Put(L"Test_AES_256_CBC_Encrypt: key=%s", ToHex(keyHash));
				StringUTF8 ivHint("20231204");
				MD5Hash ivHash(ivHint, ivHint.Len); // 128 bits (16 bytes)
				Buffer<BYTE> iv(ivHash.ValueLength);
				memcpy_s(iv, iv.Len, ivHash.Value, ivHash.ValueLength);
				Debug::Put(L"Test_AES_256_CBC_Encrypt: iv=%s", ToHex(ivHash));
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
				MD5Hash keyHash(keyHint, keyHint.Len); // 128 bits (16 bytes)
				Buffer<BYTE> key(keyHash.ValueLength);
				memcpy_s(key, key.Len, keyHash.Value, keyHash.ValueLength);
				Debug::Put(L"Test_AES_128_CBC_Decrypt: key=%s", ToHex(keyHash));
				StringUTF8 ivHint("20231204");
				MD5Hash ivHash(ivHint, ivHint.Len); // 128 bits (16 bytes)
				Buffer<BYTE> iv(ivHash.ValueLength);
				memcpy_s(iv, iv.Len, ivHash.Value, ivHash.ValueLength);
				Debug::Put(L"Test_AES_128_CBC_Decrypt: iv=%s", ToHex(ivHash));
				BCryptAlgHandle hAlg;
				hAlg.Open(BCRYPT_AES_ALGORITHM);
				hAlg.ChainingMode = BCRYPT_CHAIN_MODE_CBC; // Cipher Block Chaining Mode
				BCryptKeyHandle hKey;
				hKey.Generate(hAlg, key, key.Len);
				Debug::Put(L"Test_AES_128_CBC_Decrypt: KeyLength=%lu ChainingMode=%s BlockLength=%lu", hKey.KeyLength, hAlg.ChainingMode, hAlg.BlockLength);
				ByteString decrypted = hKey.Decrypt(d, d.Len, iv, iv.Len);
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
				SHA256Hash keyHash(keyHint, keyHint.Len); // 256 bits (32 bytes)
				Buffer<BYTE> key(keyHash.ValueLength);
				memcpy_s(key, key.Len, keyHash.Value, keyHash.ValueLength);
				Debug::Put(L"Test_AES_256_CBC_Decrypt: key=%s", ToHex(keyHash));
				StringUTF8 ivHint("20231204");
				MD5Hash ivHash(ivHint, ivHint.Len); // 128 bits (16 bytes)
				Buffer<BYTE> iv(ivHash.ValueLength);
				memcpy_s(iv, iv.Len, ivHash.Value, ivHash.ValueLength);
				Debug::Put(L"Test_AES_256_CBC_Decrypt: iv=%s", ToHex(ivHash));
				BCryptAlgHandle hAlg;
				hAlg.Open(BCRYPT_AES_ALGORITHM);
				hAlg.ChainingMode = BCRYPT_CHAIN_MODE_CBC; // Cipher Block Chaining Mode
				BCryptKeyHandle hKey;
				hKey.Generate(hAlg, key, key.Len);
				Debug::Put(L"Test_AES_256_CBC_Decrypt: KeyLength=%lu ChainingMode=%s BlockLength=%lu", hKey.KeyLength, hAlg.ChainingMode, hAlg.BlockLength);
				ByteString decrypted = hKey.Decrypt(d, d.Len, iv, iv.Len);
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
				MD5Hash keyHash(keyHint, keyHint.Len); // 128 bits
				Buffer<BYTE> key(keyHash.ValueLength);
				memcpy_s(key, key.Len, keyHash.Value, keyHash.ValueLength);
				Debug::Put(L"Test_AES_128_ECB_Encrypt: key=%s", ToHex(keyHash));
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
				SHA256Hash keyHash(keyHint, keyHint.Len); // 256 bits
				Buffer<BYTE> key(keyHash.ValueLength);
				memcpy_s(key, key.Len, keyHash.Value, keyHash.ValueLength);
				Debug::Put(L"Test_AES_256_ECB_Encrypt: key=%s", ToHex(keyHash));
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
				MD5Hash keyHash(keyHint, keyHint.Len); // 128 bits
				Buffer<BYTE> key(keyHash.ValueLength);
				memcpy_s(key, key.Len, keyHash.Value, keyHash.ValueLength);
				Debug::Put(L"Test_AES_128_ECB_Decrypt: key=%s", ToHex(keyHash));
				BCryptAlgHandle hAlg;
				hAlg.Open(BCRYPT_AES_ALGORITHM);
				hAlg.ChainingMode = BCRYPT_CHAIN_MODE_ECB; // Electronic Codebook Mode
				BCryptKeyHandle hKey;
				hKey.Generate(hAlg, key, key.Len);
				Debug::Put(L"Test_AES_128_ECB_Decrypt: KeyLength=%lu ChainingMode=%s BlockLength=%lu", hKey.KeyLength, hAlg.ChainingMode, hAlg.BlockLength);
				ByteString decrypted = hKey.Decrypt(d, d.Len);
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
				SHA256Hash keyHash(keyHint, keyHint.Len); // 256 bits
				Buffer<BYTE> key(keyHash.ValueLength);
				memcpy_s(key, key.Len, keyHash.Value, keyHash.ValueLength);
				Debug::Put(L"Test_AES_256_ECB_Decrypt: key=%s", ToHex(keyHash));
				BCryptAlgHandle hAlg;
				hAlg.Open(BCRYPT_AES_ALGORITHM);
				hAlg.ChainingMode = BCRYPT_CHAIN_MODE_ECB; // Electronic Codebook Mode
				BCryptKeyHandle hKey;
				hKey.Generate(hAlg, key, key.Len);
				Debug::Put(L"Test_AES_256_ECB_Decrypt: KeyLength=%lu ChainingMode=%s BlockLength=%lu", hKey.KeyLength, hAlg.ChainingMode, hAlg.BlockLength);
				ByteString decrypted = hKey.Decrypt(d, d.Len);
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

		TEST_METHOD(Test_AES_128_CFB)
		{
			Debug::Put(L"Test_AES_128_CFB: Started.");
			try
			{
				StringUTF8 text(L"大谷翔平17");
				Debug::Put(L"Test_AES_128_CFB: text=%s (%s)", String(CP_UTF8, text), ByteString(text, text.Len).ToHex());
				StringUTF8 keyHint("BENI");
				SHA256Hash keyHash(keyHint, keyHint.Len); // 256 bits (32 bytes)
				Buffer<BYTE> key(16);
				memcpy_s(key, key.Len, keyHash.Value, key.Len);
				Debug::Put(L"Test_AES_128_CFB: key=%s", ByteString(key, key.Len).ToHex());
				StringUTF8 ivHint("20231208");
				MD5Hash ivHash(ivHint, ivHint.Len); // 128 bits (16 bytes)
				Buffer<BYTE> iv(16);
				memcpy_s(iv, iv.Len, ivHash.Value, iv.Len);
				Debug::Put(L"Test_AES_128_CFB: iv=%s", ByteString(iv, iv.Len).ToHex());
				BCryptAlgHandle hAlg;
				hAlg.Open(BCRYPT_AES_ALGORITHM);
				hAlg.ChainingMode = BCRYPT_CHAIN_MODE_CFB; // Cipher Feedback Mode
				BCryptKeyHandle hKey;
				hKey.Generate(hAlg, key, key.Len);
				Debug::Put(L"Test_AES_128_CFB: KeyLength=%lu %s BlockLength=%lu", hKey.KeyLength, hAlg.ChainingMode, hAlg.BlockLength);
				ByteString encrypted = hKey.Encrypt(const_cast<PSTR>(text.Ptr), text.Len, iv, iv.Len);
				Debug::Put(L"Test_AES_128_CFB: encrypted=%s", encrypted.ToHex());
				memcpy_s(iv, iv.Len, ivHash.Value, iv.Len);
				ByteString decrypted = hKey.Decrypt(encrypted, encrypted.Len, iv, iv.Len);
				Debug::Put(L"Test_AES_128_CFB: decrypted=%s", decrypted.ToHex());
				StringUTF8 result((PCSTR)decrypted, decrypted.Len);
				Assert::IsTrue(result == text);
				Debug::Put(L"Test_AES_128_CFB: OK!");
			}
			catch (CryptographyException ce)
			{
				Assert::Fail(String(PRINTF, L"Test_AES_128_CFB: %s", ce.Message));
			}
			Debug::Put(L"Test_AES_128_CFB: Ended.");
		}

		TEST_METHOD(Test_AES_192_CFB)
		{
			Debug::Put(L"Test_AES_192_CFB: Started.");
			try
			{
				StringUTF8 text(L"大谷翔平17");
				Debug::Put(L"Test_AES_192_CFB: text=%s (%s)", String(CP_UTF8, text), ByteString(text, text.Len).ToHex());
				StringUTF8 keyHint("BENI");
				SHA256Hash keyHash(keyHint, keyHint.Len); // 256 bits (32 bytes)
				Buffer<BYTE> key(24);
				memcpy_s(key, key.Len, keyHash.Value, key.Len);
				Debug::Put(L"Test_AES_192_CFB: key=%s", ByteString(key, key.Len).ToHex());
				StringUTF8 ivHint("20231208");
				MD5Hash ivHash(ivHint, ivHint.Len); // 128 bits (16 bytes)
				Buffer<BYTE> iv(16);
				memcpy_s(iv, iv.Len, ivHash.Value, iv.Len);
				Debug::Put(L"Test_AES_192_CFB: iv=%s", ByteString(iv, iv.Len).ToHex());
				BCryptAlgHandle hAlg;
				hAlg.Open(BCRYPT_AES_ALGORITHM);
				hAlg.ChainingMode = BCRYPT_CHAIN_MODE_CFB; // Cipher Feedback Mode
				BCryptKeyHandle hKey;
				hKey.Generate(hAlg, key, key.Len);
				Debug::Put(L"Test_AES_192_CFB: KeyLength=%lu %s BlockLength=%lu", hKey.KeyLength, hAlg.ChainingMode, hAlg.BlockLength);
				ByteString encrypted = hKey.Encrypt(const_cast<PSTR>(text.Ptr), text.Len, iv, iv.Len);
				Debug::Put(L"Test_AES_192_CFB: encrypted=%s", encrypted.ToHex());
				memcpy_s(iv, iv.Len, ivHash.Value, iv.Len);
				ByteString decrypted = hKey.Decrypt(encrypted, encrypted.Len, iv, iv.Len);
				Debug::Put(L"Test_AES_192_CFB: decrypted=%s", decrypted.ToHex());
				StringUTF8 result((PCSTR)decrypted, decrypted.Len);
				Assert::IsTrue(result == text);
				Debug::Put(L"Test_AES_192_CFB: OK!");
			}
			catch (CryptographyException ce)
			{
				Assert::Fail(String(PRINTF, L"Test_AES_192_CFB: %s", ce.Message));
			}
			Debug::Put(L"Test_AES_192_CFB: Ended.");
		}

		TEST_METHOD(Test_AES_256_CFB)
		{
			Debug::Put(L"Test_AES_256_CFB: Started.");
			try
			{
				StringUTF8 text(L"大谷翔平17");
				Debug::Put(L"Test_AES_256_CFB: text=%s (%s)", String(CP_UTF8, text), ByteString(text, text.Len).ToHex());
				StringUTF8 keyHint("BENI");
				SHA256Hash keyHash(keyHint, keyHint.Len); // 256 bits (32 bytes)
				Buffer<BYTE> key(32);
				memcpy_s(key, key.Len, keyHash.Value, key.Len);
				Debug::Put(L"Test_AES_256_CFB: key=%s", ByteString(key, key.Len).ToHex());
				StringUTF8 ivHint("20231208");
				MD5Hash ivHash(ivHint, ivHint.Len); // 128 bits (16 bytes)
				Buffer<BYTE> iv(16);
				memcpy_s(iv, iv.Len, ivHash.Value, iv.Len);
				Debug::Put(L"Test_AES_256_CFB: iv=%s", ByteString(iv, iv.Len).ToHex());
				BCryptAlgHandle hAlg;
				hAlg.Open(BCRYPT_AES_ALGORITHM);
				hAlg.ChainingMode = BCRYPT_CHAIN_MODE_CFB; // Cipher Feedback Mode
				BCryptKeyHandle hKey;
				hKey.Generate(hAlg, key, key.Len);
				Debug::Put(L"Test_AES_256_CFB: KeyLength=%lu %s BlockLength=%lu", hKey.KeyLength, hAlg.ChainingMode, hAlg.BlockLength);
				ByteString encrypted = hKey.Encrypt(const_cast<PSTR>(text.Ptr), text.Len, iv, iv.Len);
				Debug::Put(L"Test_AES_256_CFB: encrypted=%s", encrypted.ToHex());
				memcpy_s(iv, iv.Len, ivHash.Value, iv.Len);
				ByteString decrypted = hKey.Decrypt(encrypted, encrypted.Len, iv, iv.Len);
				Debug::Put(L"Test_AES_256_CFB: decrypted=%s", decrypted.ToHex());
				StringUTF8 result((PCSTR)decrypted, decrypted.Len);
				Assert::IsTrue(result == text);
				Debug::Put(L"Test_AES_256_CFB: OK!");
			}
			catch (CryptographyException ce)
			{
				Assert::Fail(String(PRINTF, L"Test_AES_256_CFB: %s", ce.Message));
			}
			Debug::Put(L"Test_AES_256_CFB: Ended.");
		}

		TEST_METHOD(Test_AES_128_CCM)
		{
			Debug::Put(L"Test_AES_128_CCM: Started.");
			try
			{
				StringUTF8 text(L"大谷翔平17二刀流");
				Debug::Put(L"Test_AES_128_CCM: text=%s (%s)", String(CP_UTF8, text), ByteString(text, text.Len).ToHex());
				StringUTF8 keyHint("BENI");
				SHA256Hash keyHash(keyHint, keyHint.Len); // 256 bits (32 bytes)
				Buffer<BYTE> key(16);
				memcpy_s(key, key.Len, keyHash.Value, key.Len);
				Debug::Put(L"Test_AES_128_CCM: key=%s", ByteString(key, key.Len).ToHex());
				BCryptAlgHandle hAlg;
				hAlg.Open(BCRYPT_AES_ALGORITHM);
				hAlg.ChainingMode = BCRYPT_CHAIN_MODE_CCM; // Counter with CBC-MAC Mode
				BCryptKeyHandle hKey;
				hKey.Generate(hAlg, key, key.Len);
				Debug::Put(L"Test_AES_128_CCM: KeyLength=%lu %s BlockLength=%lu", hKey.KeyLength, hAlg.ChainingMode, hAlg.BlockLength);
				std::vector<DWORD> tagLengths = hAlg.AuthTagLengths;
				Debug::Put(L"Test_AES_128_CCM: AuthTagLengths[%zu]=%s", tagLengths.size(), StringUtils::JoinBy(tagLengths, L","));
				StringUTF8 nonceHint("20231208");
				MD5Hash nonceHash(nonceHint, nonceHint.Len); // 128 bits (16 bytes)
				BCryptAuthenticatedCipherModeInfo infoE;
				BCryptAuthenticatedCipherModeInfo infoD;
				infoE
					.SetNonce(nonceHash.Value, 12)
					.SetTagSize(tagLengths[tagLengths.size() - 1]);
				ByteString encrypted = hKey.Encrypt(const_cast<PSTR>(text.Ptr), text.Len, infoE, NULL, 0);
				Debug::Put(L"Test_AES_128_CCM: encrypted=%s", encrypted.ToHex());
				Debug::Put(L"Test_AES_128_CCM: tag=%s", ByteString(infoE.pbTag, infoE.cbTag).ToHex());
				Debug::Put(L"Test_AES_128_CCM: nonce=%s", ByteString(nonceHash.Value, 12).ToHex());
				infoD
					.SetNonce(nonceHash.Value, 12)
					.SetTag(infoE.pbTag, infoE.cbTag);
				ByteString decrypted = hKey.Decrypt(encrypted, encrypted.Len, infoD, NULL, 0);
				Debug::Put(L"Test_AES_128_CCM: decrypted=%s", decrypted.ToHex());
				Assert::IsTrue(StringUTF8((PCSTR)decrypted, decrypted.Len) == text);
				Debug::Put(L"Test_AES_128_CCM: OK!");
			}
			catch (CryptographyException ce)
			{
				Assert::Fail(String(PRINTF, L"Test_AES_128_CCM: %s", ce.Message));
			}
			Debug::Put(L"Test_AES_128_CCM: Ended.");
		}

#if 0
		// It looks to produce STATUS_INVALID_PARAMETER error in any ways...
		TEST_METHOD(Test_AES_128_CCM_CHAIN_CALLS)
		{
			Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: Started.");
			try
			{
				StringUTF8 text(L"二刀流大谷翔平ドジャースに10年総額7億ドルで移籍、2024年シーズンは50/50を達成しワールドシリーズ制覇。ナショナルリーグMVPに選出され大活躍");
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: text=%s (%s)", String(CP_UTF8, text), ByteString(text, text.Len).ToHex());
				Buffer<BYTE> src(((text.Len + 15) / 16) * 16);
				memcpy_s(src, src.Len, text, text.Len);
				memset(src + text.Len, 0, src.Len - text.Len);
				ByteString src1(src + 16 * 0, 16 * 2);
				ByteString src2(src + 16 * 2, 16 * 2);
				ByteString src3(src + 16 * 4, src.Len - 16 * 4);
				StringUTF8 keyHint("BENI");
				SHA256Hash keyHash(keyHint, keyHint.Len); // 256 bits (32 bytes)
				Buffer<BYTE> key(16);
				memcpy_s(key, key.Len, keyHash.Value, key.Len);
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: key=%s", ByteString(key, key.Len).ToHex());
				BCryptAlgHandle hAlg;
				hAlg.Open(BCRYPT_AES_ALGORITHM);
				hAlg.ChainingMode = BCRYPT_CHAIN_MODE_CCM; // Counter with CBC-MAC Mode
				BCryptKeyHandle hKey;
				hKey.Generate(hAlg, key, key.Len);
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: KeyLength=%lu %s BlockLength=%lu", hKey.KeyLength, hAlg.ChainingMode, hAlg.BlockLength);
				std::vector<DWORD> tagLengths = hAlg.AuthTagLengths;
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: AuthTagLengths[%zu]={%s}", tagLengths.size(), StringUtils::JoinBy(tagLengths, L","));
				StringUTF8 nonceHint("20231208");
				MD5Hash nonceHash(nonceHint, nonceHint.Len); // 128 bits (16 bytes)
				Buffer<BYTE> ivE(16);
				Buffer<BYTE> ivD(16);
				memset(ivE, 0, ivE.Len);
				memset(ivD, 0, ivD.Len);
				char aad[4] = { 0x32, 0x30, 0x32, 0x35 };
				BCryptAuthenticatedCipherModeInfo infoE;
				BCryptAuthenticatedCipherModeInfo infoD;
				// ENCRYPTION #1
				infoE
					.SetNonce(nonceHash.Value, 12)
					.SetTagSize(tagLengths[0])
					.SetMacContextSize(tagLengths[tagLengths.size() - 1])
					.SetAuthData(aad, sizeof(aad))
					.SetFlags(BCRYPT_AUTH_MODE_CHAIN_CALLS_FLAG);
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: nonce[%lu]=%s", infoE.cbNonce, ByteString(infoE.pbNonce, infoE.cbNonce).ToHex());
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: tag.size=%lu", infoE.cbTag);
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: macctx.size=%lu", infoE.cbMacContext);
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: src1[%zu]=%s", src1.Len, src1.ToHex());
				ByteString encrypted1 = hKey.Encrypt(src1.Ptr, src1.Len, infoE, ivE.Ptr, ivE.Len);
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: encrypted1[%zu]=%s", encrypted1.Len, encrypted1.ToHex());
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: tag=%s", ByteString(infoE.pbTag, infoE.cbTag).ToHex());
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: iv=%s", ByteString(ivE, ivE.Len).ToHex());
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: src2[%zu]=%s", src2.Len, src2.ToHex());
				// ENCRYPTION #2
				ByteString encrypted2 = hKey.Encrypt(src2.Ptr, src2.Len, infoE, ivE, ivE.Len);
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: encrypted2[%zu]=%s", encrypted2.Len, encrypted2.ToHex());
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: tag=%s", ByteString(infoE.pbTag, infoE.cbTag).ToHex());
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: iv=%s", ByteString(ivE, ivE.Len).ToHex());
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: src3[%zu]=%s", src3.Len, src3.ToHex());
				// ENCRYPTION #3
				infoE
					.ResetFlags(BCRYPT_AUTH_MODE_CHAIN_CALLS_FLAG);
				ByteString encrypted3 = hKey.Encrypt(src3.Ptr, src3.Len, infoE, ivE, ivE.Len);
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: encrypted3[%zu]=%s", encrypted3.Len, encrypted3.ToHex());
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: tag=%s", ByteString(infoE.pbTag, infoE.cbTag).ToHex());
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: iv=%s", ByteString(ivE, ivE.Len).ToHex());
				// DECRYPTION #1
				infoD
					.SetNonce(nonceHash.Value, 12)
					.SetTagSize(infoE.cbTag)
					.SetMacContextSize(tagLengths[tagLengths.size() - 1])
					.SetAuthData(aad, sizeof(aad))
					.SetFlags(BCRYPT_AUTH_MODE_CHAIN_CALLS_FLAG);
				ByteString decrypted1 = hKey.Decrypt((PUCHAR)encrypted1, encrypted1.Len, infoD, ivD, ivD.Len);
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: decrypted1=%s", decrypted1.ToHex());
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: tag=%s", ByteString(infoD.pbTag, infoD.cbTag).ToHex());
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: iv=%s", ByteString(ivD, ivD.Len).ToHex());
				// DECRYPTION #2
				ByteString decrypted2 = hKey.Decrypt((PUCHAR)encrypted2, encrypted2.Len, infoD, ivD, ivD.Len);
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: decrypted2=%s", decrypted2.ToHex());
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: tag=%s", ByteString(infoD.pbTag, infoD.cbTag).ToHex());
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: iv=%s", ByteString(ivD, ivD.Len).ToHex());
				// DECRYPTION #3
				infoD
					.SetTag(infoE.pbTag, infoE.cbTag)
					.ResetFlags(BCRYPT_AUTH_MODE_CHAIN_CALLS_FLAG);
				ByteString decrypted3 = hKey.Decrypt((PUCHAR)encrypted3, encrypted3.Len, infoD, ivD, ivD.Len);
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: decrypted3=%s", decrypted3.ToHex());
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: tag=%s", ByteString(infoD.pbTag, infoD.cbTag).ToHex());
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: iv=%s", ByteString(ivD, ivD.Len).ToHex());
				ByteString decrypted = decrypted1;
				decrypted += decrypted2;
				decrypted += decrypted3;
				Assert::IsTrue(StringUTF8(decrypted, decrypted.Len) == text);
				Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: OK!");
			}
			catch (CryptographyException ce)
			{
				Assert::Fail(String(PRINTF, L"Test_AES_128_CCM_CHAIN_CALLS: %s", ce.Message));
			}
			Debug::Put(L"Test_AES_128_CCM_CHAIN_CALLS: Ended.");
		}
#endif

		TEST_METHOD(Test_AES_128_GCM)
		{
			Debug::Put(L"Test_AES_128_GCM: Started.");
			try
			{
				StringUTF8 text(L"大谷翔平17二刀流");
				Debug::Put(L"Test_AES_128_GCM: text=%s (%s)", String(CP_UTF8, text), ByteString(text, text.Len).ToHex());
				StringUTF8 keyHint("BENI");
				SHA256Hash keyHash(keyHint, keyHint.Len); // 256 bits (32 bytes)
				Buffer<BYTE> key(16);
				memcpy_s(key, key.Len, keyHash.Value, key.Len);
				Debug::Put(L"Test_AES_128_GCM: key=%s", ByteString(key, key.Len).ToHex());
				BCryptAlgHandle hAlg;
				hAlg.Open(BCRYPT_AES_ALGORITHM);
				hAlg.ChainingMode = BCRYPT_CHAIN_MODE_GCM; // Galois/Counter Mode
				BCryptKeyHandle hKey;
				hKey.Generate(hAlg, key, key.Len);
				Debug::Put(L"Test_AES_128_GCM: KeyLength=%lu %s BlockLength=%lu", hKey.KeyLength, hAlg.ChainingMode, hAlg.BlockLength);
				std::vector<DWORD> tagLengths = hAlg.AuthTagLengths;
				Debug::Put(L"Test_AES_128_GCM: AuthTagLengths[%zu]=%s", tagLengths.size(), StringUtils::JoinBy(tagLengths, L","));
				StringUTF8 nonceHint("20231208");
				MD5Hash nonceHash(nonceHint, nonceHint.Len); // 128 bits (16 bytes)
				BCryptAuthenticatedCipherModeInfo infoE;
				BCryptAuthenticatedCipherModeInfo infoD;
				infoE
					.SetNonce(nonceHash.Value, 12)
					.SetTagSize(tagLengths[tagLengths.size() - 1]);
				Debug::Put(L"Test_AES_128_GCM: nonce=%s", ByteString(infoE.pbNonce, infoE.cbNonce).ToHex());
				ByteString encrypted = hKey.Encrypt(const_cast<PSTR>(text.Ptr), text.Len, infoE, NULL, 0);
				Debug::Put(L"Test_AES_128_GCM: encrypted=%s", encrypted.ToHex());
				Debug::Put(L"Test_AES_128_GCM: tag=%s", ByteString(infoE.pbTag, infoE.cbTag).ToHex());
				infoD
					.SetNonce(nonceHash.Value, 12)
					.SetTag(infoE.pbTag, infoE.cbTag);
				ByteString decrypted = hKey.Decrypt(encrypted, encrypted.Len, infoD, NULL, 0);
				Debug::Put(L"Test_AES_128_GCM: decrypted[%zu]=%s", decrypted.Len, decrypted.ToHex());
				Assert::IsTrue(StringUTF8((PCSTR)decrypted, decrypted.Len) == text);
				Debug::Put(L"Test_AES_128_GCM: OK!");
			}
			catch (CryptographyException ce)
			{
				Assert::Fail(String(PRINTF, L"Test_AES_128_GCM: %s", ce.Message));
			}
			Debug::Put(L"Test_AES_128_GCM: Ended.");
		}

		TEST_METHOD(Test_AES_128_GCM_CHAIN_CALLS)
		{
			Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: Started.");
			try
			{
				StringUTF8 text(L"二刀流大谷翔平ドジャースに10年総額7億ドルで移籍");
				Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: text=%s (%s)", String(CP_UTF8, text), ByteString(text, text.Len).ToHex());
				StringUTF8 keyHint("BENI");
				SHA256Hash keyHash(keyHint, keyHint.Len); // 256 bits (32 bytes)
				Buffer<BYTE> key(16);
				memcpy_s(key, key.Len, keyHash.Value, key.Len);
				Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: key=%s", ByteString(key, key.Len).ToHex());
				BCryptAlgHandle hAlg;
				hAlg.Open(BCRYPT_AES_ALGORITHM);
				hAlg.ChainingMode = BCRYPT_CHAIN_MODE_GCM; // Galois/Counter Mode
				BCryptKeyHandle hKey;
				hKey.Generate(hAlg, key, key.Len);
				Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: KeyLength=%lu %s BlockLength=%lu", hKey.KeyLength, hAlg.ChainingMode, hAlg.BlockLength);
				std::vector<DWORD> tagLengths = hAlg.AuthTagLengths;
				Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: AuthTagLengths[%zu]=%s", tagLengths.size(), StringUtils::JoinBy(tagLengths, L","));
				StringUTF8 nonceHint("20231208");
				MD5Hash nonceHash(nonceHint, nonceHint.Len); // 128 bits (16 bytes)
				Buffer<BYTE> ivE(16);
				Buffer<BYTE> ivD(16);
				memset(ivE, 0, ivE.Len);
				memset(ivD, 0, ivD.Len);
				char aad[4] = { 0x32, 0x30, 0x32, 0x35 };
				BCryptAuthenticatedCipherModeInfo infoE;
				BCryptAuthenticatedCipherModeInfo infoD;
				// ENCRYPTION #1
				infoE
					.SetNonce(nonceHash.Value, 12)
					.SetTagSize(tagLengths[0])
					.SetMacContextSize(tagLengths[tagLengths.size() - 1])
					.SetAuthData(aad, sizeof(aad))
					.SetFlags(BCRYPT_AUTH_MODE_CHAIN_CALLS_FLAG);
				Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: nonce=%s", ByteString(infoE.pbNonce, infoE.cbNonce).ToHex());
				ByteString encrypted1 = hKey.Encrypt(const_cast<PSTR>(text.Ptr) + 16 * 0, 16, infoE, ivE, ivE.Len);
				Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: encrypted1[%zu]=%s", encrypted1.Len, encrypted1.ToHex());
				Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: tag=%s", ByteString(infoE.pbTag, infoE.cbTag).ToHex());
				Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: iv=%s", ByteString(ivE, ivE.Len).ToHex());
				Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: flags=%lx", infoE.dwFlags);
				ByteString tag1(infoE.pbTag, infoE.cbTag);
				// ENCRYPTION #2
				ByteString encrypted2 = hKey.Encrypt(const_cast<PSTR>(text.Ptr) + 16 * 1, 16, infoE, ivE, ivE.Len);
				Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: encrypted2[%zu]=%s", encrypted2.Len, encrypted2.ToHex());
				Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: tag=%s", ByteString(infoE.pbTag, infoE.cbTag).ToHex());
				Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: iv=%s", ByteString(ivE, ivE.Len).ToHex());
				Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: flags=%lx", infoE.dwFlags);
				ByteString tag2(infoE.pbTag, infoE.cbTag);
				// ENCRYPTION #3
				infoE
					.ResetFlags(BCRYPT_AUTH_MODE_CHAIN_CALLS_FLAG);
				ByteString encrypted3 = hKey.Encrypt(const_cast<PSTR>(text.Ptr) + 16 * 2, text.Len - 16 * 2, infoE, ivE, ivE.Len);
				Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: encrypted3[%zu]=%s", encrypted3.Len, encrypted3.ToHex());
				Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: tag=%s", ByteString(infoE.pbTag, infoE.cbTag).ToHex());
				Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: iv=%s", ByteString(ivE, ivE.Len).ToHex());
				Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: flags=%lx", infoE.dwFlags);
				ByteString tag3(infoE.pbTag, infoE.cbTag);
				ByteString encrypted = encrypted1;
				encrypted += encrypted2;
				encrypted += encrypted3;
				Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: encrypted[%zu]=%s", encrypted.Len, encrypted.ToHex());
				// DECRYPTION #1
				infoD
					.SetNonce(nonceHash.Value, 12)
					.SetTag(infoE.pbTag, infoE.cbTag)
					.SetMacContextSize(tagLengths[tagLengths.size() - 1])
					.SetAuthData(aad, sizeof(aad))
					.SetFlags(BCRYPT_AUTH_MODE_CHAIN_CALLS_FLAG);
				ByteString decrypted1 = hKey.Decrypt((PUCHAR)encrypted + 16 * 0, 16, infoD, ivD, ivD.Len);
				Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: decrypted1[%zu]=%s", decrypted1.Len, decrypted1.ToHex());
				Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: iv=%s", ByteString(ivD, ivD.Len).ToHex());
				// DECRYPTION #2
				ByteString decrypted2 = hKey.Decrypt((PUCHAR)encrypted + 16 * 1, 16, infoD, ivD, ivD.Len);
				Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: decrypted2[%zu]=%s", decrypted2.Len, decrypted2.ToHex());
				Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: iv=%s", ByteString(ivD, ivD.Len).ToHex());
				// DECRYPTION #3
				infoD
					.ResetFlags(BCRYPT_AUTH_MODE_CHAIN_CALLS_FLAG);
				ByteString decrypted3 = hKey.Decrypt((PUCHAR)encrypted + 16 * 2, encrypted.Len - 16 * 2, infoD, ivD, ivD.Len);
				Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: decrypted3[%zu]=%s", decrypted3.Len, decrypted3.ToHex());
				Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: iv=%s", ByteString(ivD, ivD.Len).ToHex());
				ByteString decrypted = decrypted1;
				decrypted += decrypted2;
				decrypted += decrypted3;
				Assert::IsTrue(StringUTF8(decrypted, decrypted.Len) == text);
				Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: OK!");
			}
			catch (CryptographyException ce)
			{
				Assert::Fail(String(PRINTF, L"Test_AES_128_GCM_CHAIN_CALLS: %s", ce.Message));
			}
			Debug::Put(L"Test_AES_128_GCM_CHAIN_CALLS: Ended.");
		}

	private:

		String ToHex(Hash& hash)
		{
			return ByteString(hash.Value, hash.ValueLength).ToHex();
		}
	};
}
