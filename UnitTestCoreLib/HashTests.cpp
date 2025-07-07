#include "pch.h"
#include "CppUnitTest.h"
#include "hnrt/Hash.h"
#include "hnrt/MultibyteString.h"
#include "hnrt/ByteString.h"
#include "hnrt/ByteDataFeeder.h"
#include "hnrt/Debug.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace hnrt;

namespace UnitTestCoreLib
{
	TEST_CLASS(HashTests)
	{
	public:

		TEST_METHOD(TestMD5)
		{
			const char* data0 = Data0();
			const char* data1 = Data1();
			const wchar_t* data2 = Data2();
			MD5Hash hash0(data0, strlen(data0));
			Debug::Put(L"TestMD5: \"%s\"=%s", String(CP_ACP, data0), ToHex(hash0));
			Assert::AreEqual(L"d41d8cd98f00b204e9800998ecf8427e", ToHex(hash0));
			MD5Hash hash1(data1, strlen(data1));
			Debug::Put(L"TestMD5: \"%s\"=%s", String(CP_ACP, data1), ToHex(hash1));
			Assert::AreEqual(L"df756a3769fcab0a261880957590c768", ToHex(hash1));
			MultibyteString sz2(CP_UTF8, data2);
			MD5Hash hash2(sz2.Ptr, sz2.Len);
			Debug::Put(L"TestMD5: \"%s\"=%s", data2, ToHex(hash2));
			Assert::AreEqual(L"00110af8b4393ef3f72c50be5b332bec", ToHex(hash2));
		}

		TEST_METHOD(TestSHA1)
		{
			const char* data0 = Data0();
			const char* data1 = Data1();
			const wchar_t* data2 = Data2();
			SHA1Hash hash0(data0, strlen(data0));
			Debug::Put(L"TestSHA1: \"%s\"=%s", String(CP_ACP, data0), ToHex(hash0));
			Assert::AreEqual(L"da39a3ee5e6b4b0d3255bfef95601890afd80709", ToHex(hash0));
			SHA1Hash hash1(data1, strlen(data1));
			Debug::Put(L"TestSHA1: \"%s\"=%s", String(CP_ACP, data1), ToHex(hash1));
			Assert::AreEqual(L"00cffe7312bf9ca73584f24bdf7df1d028340397", ToHex(hash1));
			MultibyteString sz2(CP_UTF8, data2);
			SHA1Hash hash2(sz2.Ptr, sz2.Len);
			Debug::Put(L"TestSHA1: \"%s\"=%s", data2, ToHex(hash2));
			Assert::AreEqual(L"c12140a0ffb4e56481b4fe0a7a25040c2eafa9ca", ToHex(hash2));
		}

		TEST_METHOD(TestSHA256)
		{
			const char* data0 = Data0();
			const char* data1 = Data1();
			const wchar_t* data2 = Data2();
			SHA256Hash hash0(data0, strlen(data0));
			Debug::Put(L"TestSHA256: \"%s\"=%s", String(CP_ACP, data0), ToHex(hash0));
			Assert::AreEqual(L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", ToHex(hash0));
			SHA256Hash hash1(data1, strlen(data1));
			Debug::Put(L"TestSHA256: \"%s\"=%s", String(CP_ACP, data1), ToHex(hash1));
			Assert::AreEqual(L"ffca2587cfd4846e4cb975b503c9eb940f94566aa394e8bd571458b9da5097d5", ToHex(hash1));
			MultibyteString sz2(CP_UTF8, data2);
			SHA256Hash hash2(sz2.Ptr, sz2.Len);
			Debug::Put(L"TestSHA256: \"%s\"=%s", data2, ToHex(hash2));
			Assert::AreEqual(L"77710aedc74ecfa33685e33a6c7df5cc83004da1bdcef7fb280f5c2b2e97e0a5", ToHex(hash2));
		}

		TEST_METHOD(TestSHA384)
		{
			const char* data0 = Data0();
			const char* data1 = Data1();
			const wchar_t* data2 = Data2();
			SHA384Hash hash0(data0, strlen(data0));
			Debug::Put(L"TestSHA384: \"%s\"=%s", String(CP_ACP, data0), ToHex(hash0));
			Assert::AreEqual(L"38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b", ToHex(hash0));
			SHA384Hash hash1(data1, strlen(data1));
			Debug::Put(L"TestSHA384: \"%s\"=%s", String(CP_ACP, data1), ToHex(hash1));
			Assert::AreEqual(L"c7d71b1ba81d0dd028e79c7e75cf2f83169c14ba732ca5a2ad731151584e9de843c1a314077d62b96b03367f72e126d8", ToHex(hash1));
			MultibyteString sz2(CP_UTF8, data2);
			SHA384Hash hash2(sz2.Ptr, sz2.Len);
			Debug::Put(L"TestSHA384: \"%s\"=%s", data2, ToHex(hash2));
			Assert::AreEqual(L"d0b1adaab86b27d5d30373aa7f9642cd0695cfa4cd438f66b4006f7f807f1f8f77349e77d1d48be1afadd078d7385bde", ToHex(hash2));
		}

		TEST_METHOD(TestSHA512)
		{
			const char* data0 = Data0();
			const char* data1 = Data1();
			const wchar_t* data2 = Data2();
			SHA512Hash hash0(data0, strlen(data0));
			Debug::Put(L"TestSHA512: \"%s\"=%s", String(CP_ACP, data0), ToHex(hash0));
			Assert::AreEqual(L"cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e", ToHex(hash0));
			SHA512Hash hash1(data1, strlen(data1));
			Debug::Put(L"TestSHA512: \"%s\"=%s", String(CP_ACP, data1), ToHex(hash1));
			Assert::AreEqual(L"3045575cf3b873dd656f5f3426e04a4acd11950bb2538772ee14867002b408e21ff18ef7f7b2cab484a3c1c0be3f8acc4aed536a427353c7748dc365fc1a8646", ToHex(hash1));
			MultibyteString sz2(CP_UTF8, data2);
			SHA512Hash hash2(sz2.Ptr, sz2.Len);
			Debug::Put(L"TestSHA512: \"%s\"=%s", data2, ToHex(hash2));
			Assert::AreEqual(L"3df44aa21ecaedcf5b27b2d9ac97347ef57344534798de884593bed4ff920c79213d491eb5b61c07780cc485be830a1a2f6ef1347e02216d0cb560afd3f392f5", ToHex(hash2));
		}

	private:

		static const char* Data0()
		{
			return "";
		}

		static const char* Data1()
		{
			return "A quick brown fox jumps over the lazy dog.";
		}

		static const wchar_t* Data2()
		{
			return L"“ú–{Œê";
		}

		String ToHex(Hash& hash)
		{
			return ByteString(hash.Value, hash.ValueLength).ToHex(LOWERCASE);
		}
	};
}
