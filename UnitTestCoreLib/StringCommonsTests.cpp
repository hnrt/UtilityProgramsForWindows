#include "pch.h"
#include "CppUnitTest.h"
#include "hnrt/StringCommons.h"
#include "hnrt/Debug.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace hnrt;

namespace UnitTestCoreLib
{
	TEST_CLASS(StringCommonsTests)
	{
	public:

		TEST_METHOD(StrLen_01)
		{
			WCHAR* p1 = nullptr;
			SIZE_T n1 = StrLen(p1);
			Assert::AreEqual(0ULL, n1);
			SIZE_T n2 = StrLen(p1, 0);
			Assert::AreEqual(0ULL, n2);
			SIZE_T n3 = StrLen(p1, 100);
			Assert::AreEqual(0ULL, n3);
			CHAR* p2 = nullptr;
			SIZE_T n4 = StrLen(p2);
			Assert::AreEqual(0ULL, n4);
			SIZE_T n5 = StrLen(p2, 0);
			Assert::AreEqual(0ULL, n5);
			SIZE_T n6 = StrLen(p2, 100);
			Assert::AreEqual(0ULL, n6);
		}

		TEST_METHOD(StrLen_02)
		{
			WCHAR* p1 = L"ABC";
			SIZE_T n1 = StrLen(p1, 0);
			Assert::AreEqual(0ULL, n1);
			CHAR* p2 = "ABC";
			SIZE_T n2 = StrLen(p2, 0);
			Assert::AreEqual(0ULL, n2);
		}

		TEST_METHOD(StrLen_03)
		{
			WCHAR s[16] = { L'A', L'B', L'C', L'‚ ', L'‚¢', L'‚¤', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			SIZE_T n1 = StrLen(s);
			Assert::AreEqual(6ULL, n1);
			SIZE_T n2 = StrLen(s, 16);
			Assert::AreEqual(6ULL, n2);
			SIZE_T n3 = StrLen(s, 4);
			Assert::AreEqual(4ULL, n3);
			CHAR t[16] = { 0 };
			WideCharToMultiByte(932, 0, s, -1, t, 16, NULL, NULL);
			SIZE_T n4 = StrLen(t);
			Assert::AreEqual(9ULL, n4);
			SIZE_T n5 = StrLen(t, 16);
			Assert::AreEqual(9ULL, n5);
			SIZE_T n6 = StrLen(t, 5);
			Assert::AreEqual(5ULL, n6);
		}

		TEST_METHOD(StrCopy_01)
		{
			WCHAR* p1 = nullptr;
			WCHAR* q1 = nullptr;
			SIZE_T n1 = StrCopy(q1, p1);
			Assert::AreEqual(0ULL, n1);
			SIZE_T n2 = StrCopy(q1, p1, 0);
			Assert::AreEqual(0ULL, n2);
			SIZE_T n3 = StrCopy(q1, p1, 10);
			Assert::AreEqual(0ULL, n3);
			CHAR* p2 = nullptr;
			CHAR* q2 = nullptr;
			SIZE_T n4 = StrCopy(q2, p2);
			Assert::AreEqual(0ULL, n4);
			SIZE_T n5 = StrCopy(q2, p2, 0);
			Assert::AreEqual(0ULL, n5);
			SIZE_T n6 = StrCopy(q2, p2, 10);
			Assert::AreEqual(0ULL, n6);
		}

		TEST_METHOD(StrCopy_02)
		{
			WCHAR* p1 = nullptr;
			WCHAR t1[16] = { 0 };
			SIZE_T n1 = StrCopy(t1, p1);
			Assert::AreEqual(0ULL, n1);
			SIZE_T n2 = StrCopy(t1, p1, 0);
			Assert::AreEqual(0ULL, n2);
			SIZE_T n3 = StrCopy(t1, p1, 10);
			Assert::AreEqual(0ULL, n3);
			CHAR* p2 = nullptr;
			CHAR t2[16] = { 0 };
			SIZE_T n4 = StrCopy(t2, p2);
			Assert::AreEqual(0ULL, n4);
			SIZE_T n5 = StrCopy(t2, p2, 0);
			Assert::AreEqual(0ULL, n5);
			SIZE_T n6 = StrCopy(t2, p2, 10);
			Assert::AreEqual(0ULL, n6);
		}

		TEST_METHOD(StrCopy_03)
		{
			WCHAR s1[16] = { L'A', L'B', L'C', L'‚ ', L'‚¢', L'‚¤', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			WCHAR* q1 = nullptr;
			SIZE_T n1 = StrCopy(q1, s1);
			Assert::AreEqual(0ULL, n1);
			SIZE_T n2 = StrCopy(q1, s1, 0);
			Assert::AreEqual(0ULL, n2);
			SIZE_T n3 = StrCopy(q1, s1, 10);
			Assert::AreEqual(0ULL, n3);
			CHAR s2[16] = { 0 };
			CHAR* q2 = nullptr;
			WideCharToMultiByte(932, 0, s1, -1, s2, 16, NULL, NULL);
			SIZE_T n4 = StrCopy(q2, s2);
			Assert::AreEqual(0ULL, n4);
			SIZE_T n5 = StrCopy(q2, s2, 0);
			Assert::AreEqual(0ULL, n5);
			SIZE_T n6 = StrCopy(q2, s2, 10);
			Assert::AreEqual(0ULL, n6);
		}

		TEST_METHOD(StrCopy_04)
		{
			WCHAR s1[16] = { L'A', L'B', L'C', L'‚ ', L'‚¢', L'‚¤', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			WCHAR t1[16] = { 0 };
			wmemset(t1, 0xBEEF, 16);
			SIZE_T n1 = StrCopy(t1, s1);
			Assert::AreEqual(6ULL, n1);
			Assert::AreEqual(L'A', t1[0]);
			Assert::AreEqual(L'B', t1[1]);
			Assert::AreEqual(L'C', t1[2]);
			Assert::AreEqual(L'‚ ', t1[3]);
			Assert::AreEqual(L'‚¢', t1[4]);
			Assert::AreEqual(L'‚¤', t1[5]);
			Assert::AreEqual(L'\0', t1[6]);
			Assert::AreEqual(L'\xbeef', t1[7]);
			Assert::AreEqual(L'\xbeef', t1[8]);
			Assert::AreEqual(L'\xbeef', t1[9]);
			Assert::AreEqual(L'\xbeef', t1[10]);
			Assert::AreEqual(L'\xbeef', t1[11]);
			Assert::AreEqual(L'\xbeef', t1[12]);
			Assert::AreEqual(L'\xbeef', t1[13]);
			Assert::AreEqual(L'\xbeef', t1[14]);
			Assert::AreEqual(L'\xbeef', t1[15]);
			wmemset(t1, 0xBEEF, 16);
			SIZE_T n2 = StrCopy(t1, s1, 0);
			Assert::AreEqual(0ULL, n2);
			Assert::AreEqual(L'\0', t1[0]);
			Assert::AreEqual(L'\xbeef', t1[1]);
			Assert::AreEqual(L'\xbeef', t1[2]);
			Assert::AreEqual(L'\xbeef', t1[3]);
			Assert::AreEqual(L'\xbeef', t1[4]);
			Assert::AreEqual(L'\xbeef', t1[5]);
			Assert::AreEqual(L'\xbeef', t1[6]);
			Assert::AreEqual(L'\xbeef', t1[7]);
			Assert::AreEqual(L'\xbeef', t1[8]);
			Assert::AreEqual(L'\xbeef', t1[9]);
			Assert::AreEqual(L'\xbeef', t1[10]);
			Assert::AreEqual(L'\xbeef', t1[11]);
			Assert::AreEqual(L'\xbeef', t1[12]);
			Assert::AreEqual(L'\xbeef', t1[13]);
			Assert::AreEqual(L'\xbeef', t1[14]);
			Assert::AreEqual(L'\xbeef', t1[15]);
			wmemset(t1, 0xBEEF, 16);
			SIZE_T n3 = StrCopy(t1, s1, 4);
			Assert::AreEqual(4ULL, n3);
			Assert::AreEqual(L'A', t1[0]);
			Assert::AreEqual(L'B', t1[1]);
			Assert::AreEqual(L'C', t1[2]);
			Assert::AreEqual(L'‚ ', t1[3]);
			Assert::AreEqual(L'\0', t1[4]);
			Assert::AreEqual(L'\xbeef', t1[5]);
			Assert::AreEqual(L'\xbeef', t1[6]);
			Assert::AreEqual(L'\xbeef', t1[7]);
			Assert::AreEqual(L'\xbeef', t1[8]);
			Assert::AreEqual(L'\xbeef', t1[9]);
			Assert::AreEqual(L'\xbeef', t1[10]);
			Assert::AreEqual(L'\xbeef', t1[11]);
			Assert::AreEqual(L'\xbeef', t1[12]);
			Assert::AreEqual(L'\xbeef', t1[13]);
			Assert::AreEqual(L'\xbeef', t1[14]);
			Assert::AreEqual(L'\xbeef', t1[15]);
			wmemset(t1, 0xBEEF, 16);
			SIZE_T n4 = StrCopy(t1, s1, 10);
			Assert::AreEqual(6ULL, n4);
			Assert::AreEqual(L'A', t1[0]);
			Assert::AreEqual(L'B', t1[1]);
			Assert::AreEqual(L'C', t1[2]);
			Assert::AreEqual(L'‚ ', t1[3]);
			Assert::AreEqual(L'‚¢', t1[4]);
			Assert::AreEqual(L'‚¤', t1[5]);
			Assert::AreEqual(L'\0', t1[6]);
			Assert::AreEqual(L'\xbeef', t1[7]);
			Assert::AreEqual(L'\xbeef', t1[8]);
			Assert::AreEqual(L'\xbeef', t1[9]);
			Assert::AreEqual(L'\xbeef', t1[10]);
			Assert::AreEqual(L'\xbeef', t1[11]);
			Assert::AreEqual(L'\xbeef', t1[12]);
			Assert::AreEqual(L'\xbeef', t1[13]);
			Assert::AreEqual(L'\xbeef', t1[14]);
			Assert::AreEqual(L'\xbeef', t1[15]);
			CHAR s2[16] = { 0 };
			CHAR t2[16] = { 0 };
			WideCharToMultiByte(932, 0, s1, -1, s2, 16, NULL, NULL);
			memset(t2, 0xEE, 16);
			SIZE_T n5 = StrCopy(t2, s2);
			Assert::AreEqual(9ULL, n5);
			Assert::AreEqual('A', t2[0]);
			Assert::AreEqual('B', t2[1]);
			Assert::AreEqual('C', t2[2]);
			Assert::AreEqual('\x82', t2[3]);
			Assert::AreEqual('\xa0', t2[4]);
			Assert::AreEqual('\x82', t2[5]);
			Assert::AreEqual('\xa2', t2[6]);
			Assert::AreEqual('\x82', t2[7]);
			Assert::AreEqual('\xa4', t2[8]);
			Assert::AreEqual('\0', t2[9]);
			Assert::AreEqual('\xee', t2[10]);
			Assert::AreEqual('\xee', t2[11]);
			Assert::AreEqual('\xee', t2[12]);
			Assert::AreEqual('\xee', t2[13]);
			Assert::AreEqual('\xee', t2[14]);
			Assert::AreEqual('\xee', t2[15]);
			memset(t2, 0xEE, 16);
			SIZE_T n6 = StrCopy(t2, s2, 0);
			Assert::AreEqual(0ULL, n6);
			Assert::AreEqual('\0', t2[0]);
			Assert::AreEqual('\xee', t2[1]);
			Assert::AreEqual('\xee', t2[2]);
			Assert::AreEqual('\xee', t2[3]);
			Assert::AreEqual('\xee', t2[4]);
			Assert::AreEqual('\xee', t2[5]);
			Assert::AreEqual('\xee', t2[6]);
			Assert::AreEqual('\xee', t2[7]);
			Assert::AreEqual('\xee', t2[8]);
			Assert::AreEqual('\xee', t2[9]);
			Assert::AreEqual('\xee', t2[10]);
			Assert::AreEqual('\xee', t2[11]);
			Assert::AreEqual('\xee', t2[12]);
			Assert::AreEqual('\xee', t2[13]);
			Assert::AreEqual('\xee', t2[14]);
			Assert::AreEqual('\xee', t2[15]);
			memset(t2, 0xEE, 16);
			SIZE_T n7 = StrCopy(t2, s2, 5);
			Assert::AreEqual(5ULL, n7);
			Assert::AreEqual('A', t2[0]);
			Assert::AreEqual('B', t2[1]);
			Assert::AreEqual('C', t2[2]);
			Assert::AreEqual('\x82', t2[3]);
			Assert::AreEqual('\xa0', t2[4]);
			Assert::AreEqual('\0', t2[5]);
			Assert::AreEqual('\xee', t2[6]);
			Assert::AreEqual('\xee', t2[7]);
			Assert::AreEqual('\xee', t2[8]);
			Assert::AreEqual('\xee', t2[9]);
			Assert::AreEqual('\xee', t2[10]);
			Assert::AreEqual('\xee', t2[11]);
			Assert::AreEqual('\xee', t2[12]);
			Assert::AreEqual('\xee', t2[13]);
			Assert::AreEqual('\xee', t2[14]);
			Assert::AreEqual('\xee', t2[15]);
			memset(t2, 0xEE, 16);
			SIZE_T n8 = StrCopy(t2, s2, 16);
			Assert::AreEqual(9ULL, n8);
			Assert::AreEqual('A', t2[0]);
			Assert::AreEqual('B', t2[1]);
			Assert::AreEqual('C', t2[2]);
			Assert::AreEqual('\x82', t2[3]);
			Assert::AreEqual('\xa0', t2[4]);
			Assert::AreEqual('\x82', t2[5]);
			Assert::AreEqual('\xa2', t2[6]);
			Assert::AreEqual('\x82', t2[7]);
			Assert::AreEqual('\xa4', t2[8]);
			Assert::AreEqual('\0', t2[9]);
			Assert::AreEqual('\xee', t2[10]);
			Assert::AreEqual('\xee', t2[11]);
			Assert::AreEqual('\xee', t2[12]);
			Assert::AreEqual('\xee', t2[13]);
			Assert::AreEqual('\xee', t2[14]);
			Assert::AreEqual('\xee', t2[15]);
		}
	};
}