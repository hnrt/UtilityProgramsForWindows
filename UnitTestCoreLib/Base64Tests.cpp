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
	TEST_METHOD(Test1)
	{
	    Base64Encoder e;
	    e.Append("A", 1);
	    e.End();
	    Debug::Put(L"A=%s", e.Ptr);

	    Base64Decoder d;
	    Assert::IsTrue(d.Parse(e.Ptr));
	    Assert::AreEqual((size_t)1, d.Len);
	    Assert::AreEqual(static_cast<unsigned char>('A'), d.Ptr[0]);
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
	    Assert::AreEqual(static_cast<unsigned char>('A'), d.Ptr[0]);
	    Assert::AreEqual(static_cast<unsigned char>('B'), d.Ptr[1]);
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
	    Assert::AreEqual(static_cast<unsigned char>('A'), d.Ptr[0]);
	    Assert::AreEqual(static_cast<unsigned char>('B'), d.Ptr[1]);
	    Assert::AreEqual(static_cast<unsigned char>('C'), d.Ptr[2]);
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
	    Assert::AreEqual(static_cast<unsigned char>('A'), d.Ptr[0]);
	    Assert::AreEqual(static_cast<unsigned char>('B'), d.Ptr[1]);
	    Assert::AreEqual(static_cast<unsigned char>('C'), d.Ptr[2]);
	    Assert::AreEqual(static_cast<unsigned char>('D'), d.Ptr[3]);
	}
    };
}
