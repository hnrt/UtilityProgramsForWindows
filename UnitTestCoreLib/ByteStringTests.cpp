#include "pch.h"
#include "CppUnitTest.h"
#include "hnrt/ByteString.h"
#include "hnrt/Debug.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace hnrt;

namespace UnitTestCoreLib
{
	TEST_CLASS(ByteStringTests)
	{
	public:

		TEST_METHOD(Test01)
		{
			ByteString bs;
			Debug::Put(L"ByteStringTest01: Ptr=%p Len=%zu", bs.Ptr, bs.Len);
			Assert::AreEqual(nullptr, bs);
			Assert::AreEqual(0ULL, bs.Len);
		}

		TEST_METHOD(Test02)
		{
			ByteString bs(100);
			Debug::Put(L"ByteStringTest02: Ptr=%p Len=%zu", bs.Ptr, bs.Len);
			Assert::AreNotEqual(nullptr, bs);
			Assert::AreEqual(100ULL, bs.Len);
		}

		TEST_METHOD(Test03)
		{
			ByteString bs1;
			ByteString bs2;
			Assert::IsTrue(bs1 == bs2);
			Assert::IsFalse(bs1 != bs2);
			Assert::IsFalse(bs1 > bs2);
			Assert::IsTrue(bs1 >= bs2);
			Assert::IsFalse(bs1 < bs2);
			Assert::IsTrue(bs1 <= bs2);
		}

		TEST_METHOD(Test04)
		{
			ByteString bs1("ABC", 3);
			ByteString bs2("ABC", 3);
			Assert::IsTrue(bs1 == bs2);
			Assert::IsFalse(bs1 != bs2);
			Assert::IsFalse(bs1 > bs2);
			Assert::IsTrue(bs1 >= bs2);
			Assert::IsFalse(bs1 < bs2);
			Assert::IsTrue(bs1 <= bs2);
		}

		TEST_METHOD(Test05)
		{
			ByteString bs1("ABC", 3);
			ByteString bs2("BBC", 3);
			Assert::IsFalse(bs1 == bs2);
			Assert::IsTrue(bs1 != bs2);
			Assert::IsFalse(bs1 > bs2);
			Assert::IsFalse(bs1 >= bs2);
			Assert::IsTrue(bs1 < bs2);
			Assert::IsTrue(bs1 <= bs2);
		}

		TEST_METHOD(Test06)
		{
			ByteString bs1("AB", 2);
			ByteString bs2("ABC", 3);
			Assert::IsFalse(bs1 == bs2);
			Assert::IsTrue(bs1 != bs2);
			Assert::IsFalse(bs1 > bs2);
			Assert::IsFalse(bs1 >= bs2);
			Assert::IsTrue(bs1 < bs2);
			Assert::IsTrue(bs1 <= bs2);
		}

		TEST_METHOD(Test07)
		{
			ByteString bs1;
			ByteString bs2("ABC", 3);
			Assert::IsFalse(bs1 == bs2);
			Assert::IsTrue(bs1 != bs2);
			Assert::IsFalse(bs1 > bs2);
			Assert::IsFalse(bs1 >= bs2);
			Assert::IsTrue(bs1 < bs2);
			Assert::IsTrue(bs1 <= bs2);
		}

		TEST_METHOD(Test08)
		{
			ByteString bs1("BBC", 3);
			ByteString bs2("ABC", 3);
			Assert::IsFalse(bs1 == bs2);
			Assert::IsTrue(bs1 != bs2);
			Assert::IsTrue(bs1 > bs2);
			Assert::IsTrue(bs1 >= bs2);
			Assert::IsFalse(bs1 < bs2);
			Assert::IsFalse(bs1 <= bs2);
		}

		TEST_METHOD(Test09)
		{
			ByteString bs1("ABC", 3);
			ByteString bs2("AB", 2);
			Assert::IsFalse(bs1 == bs2);
			Assert::IsTrue(bs1 != bs2);
			Assert::IsTrue(bs1 > bs2);
			Assert::IsTrue(bs1 >= bs2);
			Assert::IsFalse(bs1 < bs2);
			Assert::IsFalse(bs1 <= bs2);
		}

		TEST_METHOD(Test10)
		{
			ByteString bs1("ABC", 3);
			ByteString bs2;
			Assert::IsFalse(bs1 == bs2);
			Assert::IsTrue(bs1 != bs2);
			Assert::IsTrue(bs1 > bs2);
			Assert::IsTrue(bs1 >= bs2);
			Assert::IsFalse(bs1 < bs2);
			Assert::IsFalse(bs1 <= bs2);
		}

		TEST_METHOD(Test11)
		{
			ByteString bs = ByteString::FromHex(L"112233AABBCCDDEEFF");
			unsigned char expected[] = { 0x11,0x22,0x33,0xaa,0xbb,0xcc,0xdd,0xee,0xff };
			Assert::IsTrue(bs == ByteString(expected, 9));
		}

		TEST_METHOD(Test12)
		{
			ByteString bs = ByteString::FromHex(L"112233aabbccddeeff");
			unsigned char expected[] = { 0x11,0x22,0x33,0xaa,0xbb,0xcc,0xdd,0xee,0xff };
			Assert::IsTrue(bs == ByteString(expected, 9));
		}

		TEST_METHOD(Test13)
		{
			ByteString bs(3);
			PBYTE a = bs;
			Assert::AreEqual((BYTE)0, a[0]);
			Assert::AreEqual((BYTE)0, a[1]);
			Assert::AreEqual((BYTE)0, a[2]);
			bs.Fill(0x43);
			Assert::AreEqual((BYTE)0x43, a[0]);
			Assert::AreEqual((BYTE)0x43, a[1]);
			Assert::AreEqual((BYTE)0x43, a[2]);
		}
	};
}
