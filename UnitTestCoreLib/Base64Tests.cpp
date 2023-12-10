#include "pch.h"
#include "CppUnitTest.h"
#include "hnrt/Base64.h"
#include "hnrt/Debug.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace hnrt;

namespace UnitTestCoreLib
{
    TEST_CLASS(Base64Tests)
    {
    public:

		TEST_METHOD(Test0)
		{
			Base64Encoder e;
			e.End();
			Debug::Put(L"\"\"=%s", e.Ptr);

			Base64Decoder d;
			Assert::IsTrue(d.Parse(e.Ptr));
			Assert::AreEqual((size_t)0, d.Len);
		}

		TEST_METHOD(Test1)
		{
			Base64Encoder e;
			e.Append("A", 1);
			e.End();
			Debug::Put(L"A=%s", e.Ptr);

			Base64Decoder d;
			Assert::IsTrue(d.Parse(e.Ptr));
			Assert::AreEqual((size_t)1, d.Len);
			Assert::AreEqual(static_cast<unsigned char>('A'), d[0]);
		}

		TEST_METHOD(Test2)
		{
			Base64Encoder e;
			e.Append("AB", 2);
			e.End();
			Debug::Put(L"AB=%s", e.Ptr);

			Base64Decoder d;
			Assert::IsTrue(d.Parse(e.Ptr));
			Assert::AreEqual((size_t)2, d.Len);
			Assert::AreEqual(static_cast<unsigned char>('A'), d[0]);
			Assert::AreEqual(static_cast<unsigned char>('B'), d[1]);
		}

		TEST_METHOD(Test3)
		{
			Base64Encoder e;
			e.Append("ABC", 3);
			e.End();
			Debug::Put(L"ABC=%s", e.Ptr);

			Base64Decoder d;
			Assert::IsTrue(d.Parse(e.Ptr));
			Assert::AreEqual((size_t)3, d.Len);
			Assert::AreEqual(static_cast<unsigned char>('A'), d[0]);
			Assert::AreEqual(static_cast<unsigned char>('B'), d[1]);
			Assert::AreEqual(static_cast<unsigned char>('C'), d[2]);
		}

		TEST_METHOD(Test4)
		{
			Base64Encoder e;
			e.Append("ABCD", 4);
			e.End();
			Debug::Put(L"ABCD=%s", e.Ptr);

			Base64Decoder d;
			Assert::IsTrue(d.Parse(e.Ptr));
			Assert::AreEqual((size_t)4, d.Len);
			Assert::AreEqual(static_cast<unsigned char>('A'), d[0]);
			Assert::AreEqual(static_cast<unsigned char>('B'), d[1]);
			Assert::AreEqual(static_cast<unsigned char>('C'), d[2]);
			Assert::AreEqual(static_cast<unsigned char>('D'), d[3]);
		}

		TEST_METHOD(Test5)
		{
			Base64Encoder e;
			e.Append("ABCDE", 5);
			e.End();
			Debug::Put(L"ABCDE=%s", e.Ptr);

			Base64Decoder d;
			Assert::IsTrue(d.Parse(e.Ptr));
			Assert::AreEqual((size_t)5, d.Len);
			Assert::AreEqual(static_cast<unsigned char>('A'), d[0]);
			Assert::AreEqual(static_cast<unsigned char>('B'), d[1]);
			Assert::AreEqual(static_cast<unsigned char>('C'), d[2]);
			Assert::AreEqual(static_cast<unsigned char>('D'), d[3]);
			Assert::AreEqual(static_cast<unsigned char>('E'), d[4]);
		}

		TEST_METHOD(Test6)
		{
			Base64Encoder e;
			e.Append("ABCDEF", 6);
			e.End();
			Debug::Put(L"ABCDEF=%s", e.Ptr);

			Base64Decoder d;
			Assert::IsTrue(d.Parse(e.Ptr));
			Assert::AreEqual((size_t)6, d.Len);
			Assert::AreEqual(static_cast<unsigned char>('A'), d[0]);
			Assert::AreEqual(static_cast<unsigned char>('B'), d[1]);
			Assert::AreEqual(static_cast<unsigned char>('C'), d[2]);
			Assert::AreEqual(static_cast<unsigned char>('D'), d[3]);
			Assert::AreEqual(static_cast<unsigned char>('E'), d[4]);
			Assert::AreEqual(static_cast<unsigned char>('F'), d[5]);
		}

		TEST_METHOD(Test7)
		{
			Base64Encoder e;
			e.Append("ABCDEFG", 7);
			e.End();
			Debug::Put(L"ABCDEFG=%s", e.Ptr);

			Base64Decoder d;
			Assert::IsTrue(d.Parse(e.Ptr));
			Assert::AreEqual((size_t)7, d.Len);
			Assert::AreEqual(static_cast<unsigned char>('A'), d[0]);
			Assert::AreEqual(static_cast<unsigned char>('B'), d[1]);
			Assert::AreEqual(static_cast<unsigned char>('C'), d[2]);
			Assert::AreEqual(static_cast<unsigned char>('D'), d[3]);
			Assert::AreEqual(static_cast<unsigned char>('E'), d[4]);
			Assert::AreEqual(static_cast<unsigned char>('F'), d[5]);
			Assert::AreEqual(static_cast<unsigned char>('G'), d[6]);
		}

		TEST_METHOD(Test8)
		{
			Base64Encoder e;
			e.Append("ABCDEFGH", 8);
			e.End();
			Debug::Put(L"ABCDEFGH=%s", e.Ptr);

			Base64Decoder d;
			Assert::IsTrue(d.Parse(e.Ptr));
			Assert::AreEqual((size_t)8, d.Len);
			Assert::AreEqual(static_cast<unsigned char>('A'), d[0]);
			Assert::AreEqual(static_cast<unsigned char>('B'), d[1]);
			Assert::AreEqual(static_cast<unsigned char>('C'), d[2]);
			Assert::AreEqual(static_cast<unsigned char>('D'), d[3]);
			Assert::AreEqual(static_cast<unsigned char>('E'), d[4]);
			Assert::AreEqual(static_cast<unsigned char>('F'), d[5]);
			Assert::AreEqual(static_cast<unsigned char>('G'), d[6]);
			Assert::AreEqual(static_cast<unsigned char>('H'), d[7]);
		}

		TEST_METHOD(Test9)
		{
			Base64Encoder e;
			e.Append("ABCDEFGHI", 9);
			e.End();
			Debug::Put(L"ABCDEFGHI=%s", e.Ptr);

			Base64Decoder d;
			Assert::IsTrue(d.Parse(e.Ptr));
			Assert::AreEqual((size_t)9, d.Len);
			Assert::AreEqual(static_cast<unsigned char>('A'), d[0]);
			Assert::AreEqual(static_cast<unsigned char>('B'), d[1]);
			Assert::AreEqual(static_cast<unsigned char>('C'), d[2]);
			Assert::AreEqual(static_cast<unsigned char>('D'), d[3]);
			Assert::AreEqual(static_cast<unsigned char>('E'), d[4]);
			Assert::AreEqual(static_cast<unsigned char>('F'), d[5]);
			Assert::AreEqual(static_cast<unsigned char>('G'), d[6]);
			Assert::AreEqual(static_cast<unsigned char>('H'), d[7]);
			Assert::AreEqual(static_cast<unsigned char>('I'), d[8]);
		}
	};
}
