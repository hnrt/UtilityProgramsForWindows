#include "pch.h"
#include "CppUnitTest.h"
#include "hnrt/SecretBuffer.h"
#include "hnrt/Debug.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace hnrt;

namespace UnitTestCoreLib
{
	TEST_CLASS(SecretBufferTests)
	{
	public:

		TEST_METHOD(Test01)
		{
			SecretBuffer buf;
			Debug::Put(L"SecretBufferTest01: Ptr=%p Len=%zu", buf.Ptr, buf.Len);
			Assert::AreEqual((unsigned char*)nullptr, buf.Ptr);
			Assert::AreEqual(0ULL, buf.Len);
		}

		TEST_METHOD(Test02)
		{
			SecretBuffer* pBuf = new SecretBuffer;
			Debug::Put(L"SecretBufferTest02: Ptr=%p Len=%zu", pBuf->Ptr, pBuf->Len);
			Assert::AreEqual((unsigned char*)nullptr, pBuf->Ptr);
			Assert::AreEqual(0ULL, pBuf->Len);
			Assert::AreEqual(0UL, pBuf->Release());
		}

		TEST_METHOD(Test03)
		{
			SecretBuffer buf(100);
			Debug::Put(L"SecretBufferTest03: Ptr=%p Len=%zu", buf.Ptr, buf.Len);
			Assert::AreNotEqual((unsigned char*)nullptr, buf.Ptr);
			Assert::AreEqual(100ULL, buf.Len);
		}
	};
}
