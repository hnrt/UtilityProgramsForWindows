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
			const WCHAR s[16] = { L'A', L'B', L'C', L'‚ ', L'‚¢', L'‚¤', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0' };
			SIZE_T n = StrLen(s);
			Assert::AreEqual((SIZE_T)6, n);
		}

		TEST_METHOD(StrLen_02)
		{
			const WCHAR s[16] = { L'A', L'B', L'C', L'‚ ', L'‚¢', L'‚¤', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0' };
			SIZE_T n = StrLen(s, -100);
			Assert::AreEqual((SIZE_T)6, n);
		}

		TEST_METHOD(StrLen_03)
		{
			const WCHAR s[16] = { L'A', L'B', L'C', L'‚ ', L'‚¢', L'‚¤', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0' };
			SIZE_T n = StrLen(s, 0);
			Assert::AreEqual((SIZE_T)0, n);
		}

		TEST_METHOD(StrLen_04)
		{
			const WCHAR s[16] = { L'A', L'B', L'C', L'‚ ', L'‚¢', L'‚¤', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0' };
			SIZE_T n = StrLen(s, 5);
			Assert::AreEqual((SIZE_T)5, n);
		}

		TEST_METHOD(StrLen_05)
		{
			const WCHAR s[16] = { L'A', L'B', L'C', L'‚ ', L'‚¢', L'‚¤', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0' };
			SIZE_T n = StrLen(s, _countof(s));
			Assert::AreEqual((SIZE_T)6, n);
		}

		TEST_METHOD(StrLen_01A)
		{
			const CHAR s[16] = { 'A', 'B', 'C', '\x82', '\xa0', '\x82', '\xa2', '\x82', '\xa4', '\0', '\0', '\0', '\0', '\0', '\0', '\0' };
			SIZE_T n = StrLen(s);
			Assert::AreEqual((SIZE_T)9, n);
		}

		TEST_METHOD(StrLen_02A)
		{
			const CHAR s[16] = { 'A', 'B', 'C', '\x82', '\xa0', '\x82', '\xa2', '\x82', '\xa4', '\0', '\0', '\0', '\0', '\0', '\0', '\0' };
			SIZE_T n = StrLen(s, -100);
			Assert::AreEqual((SIZE_T)9, n);
		}

		TEST_METHOD(StrLen_03A)
		{
			const CHAR s[16] = { 'A', 'B', 'C', '\x82', '\xa0', '\x82', '\xa2', '\x82', '\xa4', '\0', '\0', '\0', '\0', '\0', '\0', '\0' };
			SIZE_T n = StrLen(s, 0);
			Assert::AreEqual((SIZE_T)0, n);
		}

		TEST_METHOD(StrLen_04A)
		{
			const CHAR s[16] = { 'A', 'B', 'C', '\x82', '\xa0', '\x82', '\xa2', '\x82', '\xa4', '\0', '\0', '\0', '\0', '\0', '\0', '\0' };
			SIZE_T n = StrLen(s, 5);
			Assert::AreEqual((SIZE_T)5, n);
		}

		TEST_METHOD(StrLen_05A)
		{
			const CHAR s[16] = { 'A', 'B', 'C', '\x82', '\xa0', '\x82', '\xa2', '\x82', '\xa4', '\0', '\0', '\0', '\0', '\0', '\0', '\0' };
			SIZE_T n = StrLen(s, _countof(s));
			Assert::AreEqual((SIZE_T)9, n);
		}

		TEST_METHOD(StrCpy_01)
		{
			const WCHAR s[16] = { L'A', L'B', L'C', L'‚ ', L'‚¢', L'‚¤', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0' };
			WCHAR t[16] = { L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@' };
			SIZE_T n = StrCpy(t, s);
			Assert::AreEqual((SIZE_T)6, n);
			Assert::AreEqual(L'A', t[0]);
			Assert::AreEqual(L'B', t[1]);
			Assert::AreEqual(L'C', t[2]);
			Assert::AreEqual(L'‚ ', t[3]);
			Assert::AreEqual(L'‚¢', t[4]);
			Assert::AreEqual(L'‚¤', t[5]);
			Assert::AreEqual(L'\0', t[6]);
			Assert::AreEqual(L'@', t[7]);
			Assert::AreEqual(L'@', t[8]);
			Assert::AreEqual(L'@', t[9]);
			Assert::AreEqual(L'@', t[10]);
			Assert::AreEqual(L'@', t[11]);
			Assert::AreEqual(L'@', t[12]);
			Assert::AreEqual(L'@', t[13]);
			Assert::AreEqual(L'@', t[14]);
			Assert::AreEqual(L'@', t[15]);
		}

		TEST_METHOD(StrCpy_02)
		{
			const WCHAR s[16] = { L'A', L'B', L'C', L'‚ ', L'‚¢', L'‚¤', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0' };
			WCHAR t[16] = { L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@' };
			SIZE_T n = StrCpy(t, s, -100);
			Assert::AreEqual((SIZE_T)6, n);
			Assert::AreEqual(L'A', t[0]);
			Assert::AreEqual(L'B', t[1]);
			Assert::AreEqual(L'C', t[2]);
			Assert::AreEqual(L'‚ ', t[3]);
			Assert::AreEqual(L'‚¢', t[4]);
			Assert::AreEqual(L'‚¤', t[5]);
			Assert::AreEqual(L'\0', t[6]);
			Assert::AreEqual(L'@', t[7]);
			Assert::AreEqual(L'@', t[8]);
			Assert::AreEqual(L'@', t[9]);
			Assert::AreEqual(L'@', t[10]);
			Assert::AreEqual(L'@', t[11]);
			Assert::AreEqual(L'@', t[12]);
			Assert::AreEqual(L'@', t[13]);
			Assert::AreEqual(L'@', t[14]);
			Assert::AreEqual(L'@', t[15]);
		}

		TEST_METHOD(StrCpy_03)
		{
			const WCHAR s[16] = { L'A', L'B', L'C', L'‚ ', L'‚¢', L'‚¤', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0' };
			WCHAR t[16] = { L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@' };
			SIZE_T n = StrCpy(t, s, 0);
			Assert::AreEqual((SIZE_T)0, n);
			Assert::AreEqual(L'\0', t[0]);
			Assert::AreEqual(L'@', t[1]);
			Assert::AreEqual(L'@', t[2]);
			Assert::AreEqual(L'@', t[3]);
			Assert::AreEqual(L'@', t[4]);
			Assert::AreEqual(L'@', t[5]);
			Assert::AreEqual(L'@', t[6]);
			Assert::AreEqual(L'@', t[7]);
			Assert::AreEqual(L'@', t[8]);
			Assert::AreEqual(L'@', t[9]);
			Assert::AreEqual(L'@', t[10]);
			Assert::AreEqual(L'@', t[11]);
			Assert::AreEqual(L'@', t[12]);
			Assert::AreEqual(L'@', t[13]);
			Assert::AreEqual(L'@', t[14]);
			Assert::AreEqual(L'@', t[15]);
		}

		TEST_METHOD(StrCpy_04)
		{
			const WCHAR s[16] = { L'A', L'B', L'C', L'‚ ', L'‚¢', L'‚¤', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0' };
			WCHAR t[16] = { L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@' };
			SIZE_T n = StrCpy(t, s, 5);
			Assert::AreEqual((SIZE_T)5, n);
			Assert::AreEqual(L'A', t[0]);
			Assert::AreEqual(L'B', t[1]);
			Assert::AreEqual(L'C', t[2]);
			Assert::AreEqual(L'‚ ', t[3]);
			Assert::AreEqual(L'‚¢', t[4]);
			Assert::AreEqual(L'\0', t[5]);
			Assert::AreEqual(L'@', t[6]);
			Assert::AreEqual(L'@', t[7]);
			Assert::AreEqual(L'@', t[8]);
			Assert::AreEqual(L'@', t[9]);
			Assert::AreEqual(L'@', t[10]);
			Assert::AreEqual(L'@', t[11]);
			Assert::AreEqual(L'@', t[12]);
			Assert::AreEqual(L'@', t[13]);
			Assert::AreEqual(L'@', t[14]);
			Assert::AreEqual(L'@', t[15]);
		}

		TEST_METHOD(StrCpy_05)
		{
			const WCHAR s[16] = { L'A', L'B', L'C', L'‚ ', L'‚¢', L'‚¤', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0' };
			WCHAR t[16] = { L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@' };
			SIZE_T n = StrCpy(t, s, _countof(s));
			Assert::AreEqual((SIZE_T)6, n);
			Assert::AreEqual(L'A', t[0]);
			Assert::AreEqual(L'B', t[1]);
			Assert::AreEqual(L'C', t[2]);
			Assert::AreEqual(L'‚ ', t[3]);
			Assert::AreEqual(L'‚¢', t[4]);
			Assert::AreEqual(L'‚¤', t[5]);
			Assert::AreEqual(L'\0', t[6]);
			Assert::AreEqual(L'@', t[7]);
			Assert::AreEqual(L'@', t[8]);
			Assert::AreEqual(L'@', t[9]);
			Assert::AreEqual(L'@', t[10]);
			Assert::AreEqual(L'@', t[11]);
			Assert::AreEqual(L'@', t[12]);
			Assert::AreEqual(L'@', t[13]);
			Assert::AreEqual(L'@', t[14]);
			Assert::AreEqual(L'@', t[15]);
		}

		TEST_METHOD(StrCpy_01A)
		{
			const CHAR s[16] = { 'A', 'B', 'C', '\x82', '\xa0', '\x82', '\xa2', '\x82', '\xa4', '\0', '\0', '\0', '\0', '\0', '\0', '\0' };
			CHAR t[16] = { '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@' };
			SIZE_T n = StrCpy(t, s);
			Assert::AreEqual((SIZE_T)9, n);
			Assert::AreEqual('A', t[0]);
			Assert::AreEqual('B', t[1]);
			Assert::AreEqual('C', t[2]);
			Assert::AreEqual('\x82', t[3]);
			Assert::AreEqual('\xa0', t[4]);
			Assert::AreEqual('\x82', t[5]);
			Assert::AreEqual('\xa2', t[6]);
			Assert::AreEqual('\x82', t[7]);
			Assert::AreEqual('\xa4', t[8]);
			Assert::AreEqual('\0', t[9]);
			Assert::AreEqual('@', t[10]);
			Assert::AreEqual('@', t[11]);
			Assert::AreEqual('@', t[12]);
			Assert::AreEqual('@', t[13]);
			Assert::AreEqual('@', t[14]);
			Assert::AreEqual('@', t[15]);
		}

		TEST_METHOD(StrCpy_02A)
		{
			const CHAR s[16] = { 'A', 'B', 'C', '\x82', '\xa0', '\x82', '\xa2', '\x82', '\xa4', '\0', '\0', '\0', '\0', '\0', '\0', '\0' };
			CHAR t[16] = { '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@' };
			SIZE_T n = StrCpy(t, s, -100);
			Assert::AreEqual((SIZE_T)9, n);
			Assert::AreEqual('A', t[0]);
			Assert::AreEqual('B', t[1]);
			Assert::AreEqual('C', t[2]);
			Assert::AreEqual('\x82', t[3]);
			Assert::AreEqual('\xa0', t[4]);
			Assert::AreEqual('\x82', t[5]);
			Assert::AreEqual('\xa2', t[6]);
			Assert::AreEqual('\x82', t[7]);
			Assert::AreEqual('\xa4', t[8]);
			Assert::AreEqual('\0', t[9]);
			Assert::AreEqual('@', t[10]);
			Assert::AreEqual('@', t[11]);
			Assert::AreEqual('@', t[12]);
			Assert::AreEqual('@', t[13]);
			Assert::AreEqual('@', t[14]);
			Assert::AreEqual('@', t[15]);
		}

		TEST_METHOD(StrCpy_03A)
		{
			const CHAR s[16] = { 'A', 'B', 'C', '\x82', '\xa0', '\x82', '\xa2', '\x82', '\xa4', '\0', '\0', '\0', '\0', '\0', '\0', '\0' };
			CHAR t[16] = { '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@' };
			SIZE_T n = StrCpy(t, s, 0);
			Assert::AreEqual((SIZE_T)0, n);
			Assert::AreEqual('\0', t[0]);
			Assert::AreEqual('@', t[1]);
			Assert::AreEqual('@', t[2]);
			Assert::AreEqual('@', t[3]);
			Assert::AreEqual('@', t[4]);
			Assert::AreEqual('@', t[5]);
			Assert::AreEqual('@', t[6]);
			Assert::AreEqual('@', t[7]);
			Assert::AreEqual('@', t[8]);
			Assert::AreEqual('@', t[9]);
			Assert::AreEqual('@', t[10]);
			Assert::AreEqual('@', t[11]);
			Assert::AreEqual('@', t[12]);
			Assert::AreEqual('@', t[13]);
			Assert::AreEqual('@', t[14]);
			Assert::AreEqual('@', t[15]);
		}

		TEST_METHOD(StrCpy_04A)
		{
			const CHAR s[16] = { 'A', 'B', 'C', '\x82', '\xa0', '\x82', '\xa2', '\x82', '\xa4', '\0', '\0', '\0', '\0', '\0', '\0', '\0' };
			CHAR t[16] = { '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@' };
			SIZE_T n = StrCpy(t, s, 5);
			Assert::AreEqual((SIZE_T)5, n);
			Assert::AreEqual('A', t[0]);
			Assert::AreEqual('B', t[1]);
			Assert::AreEqual('C', t[2]);
			Assert::AreEqual('\x82', t[3]);
			Assert::AreEqual('\xa0', t[4]);
			Assert::AreEqual('\0', t[5]);
			Assert::AreEqual('@', t[6]);
			Assert::AreEqual('@', t[7]);
			Assert::AreEqual('@', t[8]);
			Assert::AreEqual('@', t[9]);
			Assert::AreEqual('@', t[10]);
			Assert::AreEqual('@', t[11]);
			Assert::AreEqual('@', t[12]);
			Assert::AreEqual('@', t[13]);
			Assert::AreEqual('@', t[14]);
			Assert::AreEqual('@', t[15]);
		}

		TEST_METHOD(StrCpy_05A)
		{
			const CHAR s[16] = { 'A', 'B', 'C', '\x82', '\xa0', '\x82', '\xa2', '\x82', '\xa4', '\0', '\0', '\0', '\0', '\0', '\0', '\0' };
			CHAR t[16] = { '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@', '@' };
			SIZE_T n = StrCpy(t, s, _countof(t));
			Assert::AreEqual((SIZE_T)9, n);
			Assert::AreEqual('A', t[0]);
			Assert::AreEqual('B', t[1]);
			Assert::AreEqual('C', t[2]);
			Assert::AreEqual('\x82', t[3]);
			Assert::AreEqual('\xa0', t[4]);
			Assert::AreEqual('\x82', t[5]);
			Assert::AreEqual('\xa2', t[6]);
			Assert::AreEqual('\x82', t[7]);
			Assert::AreEqual('\xa4', t[8]);
			Assert::AreEqual('\0', t[9]);
			Assert::AreEqual('@', t[10]);
			Assert::AreEqual('@', t[11]);
			Assert::AreEqual('@', t[12]);
			Assert::AreEqual('@', t[13]);
			Assert::AreEqual('@', t[14]);
			Assert::AreEqual('@', t[15]);
		}

		TEST_METHOD(MemSet_01)
		{
			WCHAR s[8] = { L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@' };
			SIZE_T n = MemSet(s, L'A', 0);
			Assert::AreEqual((SIZE_T)0, n);
			Assert::AreEqual(L'@', s[0]);
			Assert::AreEqual(L'@', s[1]);
			Assert::AreEqual(L'@', s[2]);
			Assert::AreEqual(L'@', s[3]);
			Assert::AreEqual(L'@', s[4]);
			Assert::AreEqual(L'@', s[5]);
			Assert::AreEqual(L'@', s[6]);
			Assert::AreEqual(L'@', s[7]);
		}

		TEST_METHOD(MemSet_02)
		{
			WCHAR s[8] = { L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@' };
			SIZE_T n = MemSet(s, L'A', 4);
			Assert::AreEqual((SIZE_T)4, n);
			Assert::AreEqual(L'A', s[0]);
			Assert::AreEqual(L'A', s[1]);
			Assert::AreEqual(L'A', s[2]);
			Assert::AreEqual(L'A', s[3]);
			Assert::AreEqual(L'@', s[4]);
			Assert::AreEqual(L'@', s[5]);
			Assert::AreEqual(L'@', s[6]);
			Assert::AreEqual(L'@', s[7]);
		}

		TEST_METHOD(MemSet_01A)
		{
			CHAR s[8] = { '@', '@', '@', '@', '@', '@', '@', '@' };
			SIZE_T n = MemSet(s, 'A', 0);
			Assert::AreEqual((SIZE_T)0, n);
			Assert::AreEqual('@', s[0]);
			Assert::AreEqual('@', s[1]);
			Assert::AreEqual('@', s[2]);
			Assert::AreEqual('@', s[3]);
			Assert::AreEqual('@', s[4]);
			Assert::AreEqual('@', s[5]);
			Assert::AreEqual('@', s[6]);
			Assert::AreEqual('@', s[7]);
		}

		TEST_METHOD(MemSet_02A)
		{
			CHAR s[8] = { '@', '@', '@', '@', '@', '@', '@', '@' };
			SIZE_T n = MemSet(s, 'A', 4);
			Assert::AreEqual((SIZE_T)4, n);
			Assert::AreEqual('A', s[0]);
			Assert::AreEqual('A', s[1]);
			Assert::AreEqual('A', s[2]);
			Assert::AreEqual('A', s[3]);
			Assert::AreEqual('@', s[4]);
			Assert::AreEqual('@', s[5]);
			Assert::AreEqual('@', s[6]);
			Assert::AreEqual('@', s[7]);
		}

		TEST_METHOD(MemMove_01)
		{
			WCHAR s[16] = { L'A', L'B', L'C', L'‚ ', L'‚¢', L'‚¤', L'\0', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@', L'@' };
			SIZE_T n1 = MemMove(s + 1, s, 0);
			Assert::AreEqual((SIZE_T)0, n1);
			Assert::AreEqual(L'A', s[0]);
			Assert::AreEqual(L'B', s[1]);
			Assert::AreEqual(L'C', s[2]);
			Assert::AreEqual(L'‚ ', s[3]);
			Assert::AreEqual(L'‚¢', s[4]);
			Assert::AreEqual(L'‚¤', s[5]);
			Assert::AreEqual(L'\0', s[6]);
			Assert::AreEqual(L'@', s[7]);
			Assert::AreEqual(L'@', s[8]);
			Assert::AreEqual(L'@', s[9]);
			Assert::AreEqual(L'@', s[10]);
			Assert::AreEqual(L'@', s[11]);
			Assert::AreEqual(L'@', s[12]);
			Assert::AreEqual(L'@', s[13]);
			Assert::AreEqual(L'@', s[14]);
			Assert::AreEqual(L'@', s[15]);
			SIZE_T n2 = MemMove(s + 7, s, 7);
			Assert::AreEqual((SIZE_T)7, n2);
			Assert::AreEqual(L'A', s[0]);
			Assert::AreEqual(L'B', s[1]);
			Assert::AreEqual(L'C', s[2]);
			Assert::AreEqual(L'‚ ', s[3]);
			Assert::AreEqual(L'‚¢', s[4]);
			Assert::AreEqual(L'‚¤', s[5]);
			Assert::AreEqual(L'\0', s[6]);
			Assert::AreEqual(L'A', s[7]);
			Assert::AreEqual(L'B', s[8]);
			Assert::AreEqual(L'C', s[9]);
			Assert::AreEqual(L'‚ ', s[10]);
			Assert::AreEqual(L'‚¢', s[11]);
			Assert::AreEqual(L'‚¤', s[12]);
			Assert::AreEqual(L'\0', s[13]);
			Assert::AreEqual(L'@', s[14]);
			Assert::AreEqual(L'@', s[15]);
			SIZE_T n3 = MemMove(s + 1, s + 3, 3);
			Assert::AreEqual((SIZE_T)3, n3);
			Assert::AreEqual(L'A', s[0]);
			Assert::AreEqual(L'‚ ', s[1]);
			Assert::AreEqual(L'‚¢', s[2]);
			Assert::AreEqual(L'‚¤', s[3]);
			Assert::AreEqual(L'‚¢', s[4]);
			Assert::AreEqual(L'‚¤', s[5]);
			Assert::AreEqual(L'\0', s[6]);
			Assert::AreEqual(L'A', s[7]);
			Assert::AreEqual(L'B', s[8]);
			Assert::AreEqual(L'C', s[9]);
			Assert::AreEqual(L'‚ ', s[10]);
			Assert::AreEqual(L'‚¢', s[11]);
			Assert::AreEqual(L'‚¤', s[12]);
			Assert::AreEqual(L'\0', s[13]);
			Assert::AreEqual(L'@', s[14]);
			Assert::AreEqual(L'@', s[15]);
			SIZE_T n4 = MemMove(s + 3, s, 5);
			Assert::AreEqual((SIZE_T)5, n4);
			Assert::AreEqual(L'A', s[0]);
			Assert::AreEqual(L'‚ ', s[1]);
			Assert::AreEqual(L'‚¢', s[2]);
			Assert::AreEqual(L'A', s[3]);
			Assert::AreEqual(L'‚ ', s[4]);
			Assert::AreEqual(L'‚¢', s[5]);
			Assert::AreEqual(L'‚¤', s[6]);
			Assert::AreEqual(L'‚¢', s[7]);
			Assert::AreEqual(L'B', s[8]);
			Assert::AreEqual(L'C', s[9]);
			Assert::AreEqual(L'‚ ', s[10]);
			Assert::AreEqual(L'‚¢', s[11]);
			Assert::AreEqual(L'‚¤', s[12]);
			Assert::AreEqual(L'\0', s[13]);
			Assert::AreEqual(L'@', s[14]);
			Assert::AreEqual(L'@', s[15]);
		}
		TEST_METHOD(MemMove_01A)
		{
			CHAR s[16] = { 'A', 'B', 'C', '\x82', '\xa0', '\x82', '\xa2', '\x82', '\xa4', '\x0', '@', '@', '@', '@', '@', '@' };
			SIZE_T n1 = MemMove(s + 1, s, 0);
			Assert::AreEqual((SIZE_T)0, n1);
			Assert::AreEqual('A', s[0]);
			Assert::AreEqual('B', s[1]);
			Assert::AreEqual('C', s[2]);
			Assert::AreEqual('\x82', s[3]);
			Assert::AreEqual('\xa0', s[4]);
			Assert::AreEqual('\x82', s[5]);
			Assert::AreEqual('\xa2', s[6]);
			Assert::AreEqual('\x82', s[7]);
			Assert::AreEqual('\xa4', s[8]);
			Assert::AreEqual('\x0', s[9]);
			Assert::AreEqual('@', s[10]);
			Assert::AreEqual('@', s[11]);
			Assert::AreEqual('@', s[12]);
			Assert::AreEqual('@', s[13]);
			Assert::AreEqual('@', s[14]);
			Assert::AreEqual('@', s[15]);
			SIZE_T n2 = MemMove(s + 10, s + 3, 6);
			Assert::AreEqual((SIZE_T)6, n2);
			Assert::AreEqual('A', s[0]);
			Assert::AreEqual('B', s[1]);
			Assert::AreEqual('C', s[2]);
			Assert::AreEqual('\x82', s[3]);
			Assert::AreEqual('\xa0', s[4]);
			Assert::AreEqual('\x82', s[5]);
			Assert::AreEqual('\xa2', s[6]);
			Assert::AreEqual('\x82', s[7]);
			Assert::AreEqual('\xa4', s[8]);
			Assert::AreEqual('\x0', s[9]);
			Assert::AreEqual('\x82', s[10]);
			Assert::AreEqual('\xa0', s[11]);
			Assert::AreEqual('\x82', s[12]);
			Assert::AreEqual('\xa2', s[13]);
			Assert::AreEqual('\x82', s[14]);
			Assert::AreEqual('\xa4', s[15]);
			SIZE_T n3 = MemMove(s + 2, s + 3, 4);
			Assert::AreEqual((SIZE_T)4, n3);
			Assert::AreEqual('A', s[0]);
			Assert::AreEqual('B', s[1]);
			Assert::AreEqual('\x82', s[2]);
			Assert::AreEqual('\xa0', s[3]);
			Assert::AreEqual('\x82', s[4]);
			Assert::AreEqual('\xa2', s[5]);
			Assert::AreEqual('\xa2', s[6]);
			Assert::AreEqual('\x82', s[7]);
			Assert::AreEqual('\xa4', s[8]);
			Assert::AreEqual('\x0', s[9]);
			Assert::AreEqual('\x82', s[10]);
			Assert::AreEqual('\xa0', s[11]);
			Assert::AreEqual('\x82', s[12]);
			Assert::AreEqual('\xa2', s[13]);
			Assert::AreEqual('\x82', s[14]);
			Assert::AreEqual('\xa4', s[15]);
			SIZE_T n4 = MemMove(s + 4, s, 6);
			Assert::AreEqual((SIZE_T)6, n4);
			Assert::AreEqual('A', s[0]);
			Assert::AreEqual('B', s[1]);
			Assert::AreEqual('\x82', s[2]);
			Assert::AreEqual('\xa0', s[3]);
			Assert::AreEqual('A', s[4]);
			Assert::AreEqual('B', s[5]);
			Assert::AreEqual('\x82', s[6]);
			Assert::AreEqual('\xa0', s[7]);
			Assert::AreEqual('\x82', s[8]);
			Assert::AreEqual('\xa2', s[9]);
			Assert::AreEqual('\x82', s[10]);
			Assert::AreEqual('\xa0', s[11]);
			Assert::AreEqual('\x82', s[12]);
			Assert::AreEqual('\xa2', s[13]);
			Assert::AreEqual('\x82', s[14]);
			Assert::AreEqual('\xa4', s[15]);
		}

		TEST_METHOD(StrUpr_01)
		{
			WCHAR s[10] = { L'A', L'B', L'C', L'x', L'y', L'z', L'\0', L'd', L'e', L'f' };
			SIZE_T n = StrUpr(s);
			Assert::AreEqual((SIZE_T)6, n);
			Assert::AreEqual(L'A', s[0]);
			Assert::AreEqual(L'B', s[1]);
			Assert::AreEqual(L'C', s[2]);
			Assert::AreEqual(L'X', s[3]);
			Assert::AreEqual(L'Y', s[4]);
			Assert::AreEqual(L'Z', s[5]);
			Assert::AreEqual(L'\0', s[6]);
			Assert::AreEqual(L'd', s[7]);
			Assert::AreEqual(L'e', s[8]);
			Assert::AreEqual(L'f', s[9]);
		}

		TEST_METHOD(StrUpr_02)
		{
			WCHAR s[10] = { L'A', L'B', L'C', L'x', L'y', L'z', L'\0', L'd', L'e', L'f' };
			SIZE_T n = StrUpr(s, -100);
			Assert::AreEqual((SIZE_T)6, n);
			Assert::AreEqual(L'A', s[0]);
			Assert::AreEqual(L'B', s[1]);
			Assert::AreEqual(L'C', s[2]);
			Assert::AreEqual(L'X', s[3]);
			Assert::AreEqual(L'Y', s[4]);
			Assert::AreEqual(L'Z', s[5]);
			Assert::AreEqual(L'\0', s[6]);
			Assert::AreEqual(L'd', s[7]);
			Assert::AreEqual(L'e', s[8]);
			Assert::AreEqual(L'f', s[9]);
		}

		TEST_METHOD(StrUpr_03)
		{
			WCHAR s[10] = { L'A', L'B', L'C', L'x', L'y', L'z', L'\0', L'd', L'e', L'f' };
			SIZE_T n = StrUpr(s, 0);
			Assert::AreEqual((SIZE_T)0, n);
			Assert::AreEqual(L'\0', s[0]);
			Assert::AreEqual(L'B', s[1]);
			Assert::AreEqual(L'C', s[2]);
			Assert::AreEqual(L'x', s[3]);
			Assert::AreEqual(L'y', s[4]);
			Assert::AreEqual(L'z', s[5]);
			Assert::AreEqual(L'\0', s[6]);
			Assert::AreEqual(L'd', s[7]);
			Assert::AreEqual(L'e', s[8]);
			Assert::AreEqual(L'f', s[9]);
		}

		TEST_METHOD(StrUpr_04)
		{
			WCHAR s[10] = { L'A', L'B', L'C', L'x', L'y', L'z', L'\0', L'd', L'e', L'f' };
			SIZE_T n = StrUpr(s, 4);
			Assert::AreEqual((SIZE_T)4, n);
			Assert::AreEqual(L'A', s[0]);
			Assert::AreEqual(L'B', s[1]);
			Assert::AreEqual(L'C', s[2]);
			Assert::AreEqual(L'X', s[3]);
			Assert::AreEqual(L'\0', s[4]);
			Assert::AreEqual(L'z', s[5]);
			Assert::AreEqual(L'\0', s[6]);
			Assert::AreEqual(L'd', s[7]);
			Assert::AreEqual(L'e', s[8]);
			Assert::AreEqual(L'f', s[9]);
		}

		TEST_METHOD(StrUpr_05)
		{
			WCHAR s[10] = { L'A', L'B', L'C', L'x', L'y', L'z', L'\0', L'd', L'e', L'f' };
			SIZE_T n = StrUpr(s, _countof(s));
			Assert::AreEqual((SIZE_T)6, n);
			Assert::AreEqual(L'A', s[0]);
			Assert::AreEqual(L'B', s[1]);
			Assert::AreEqual(L'C', s[2]);
			Assert::AreEqual(L'X', s[3]);
			Assert::AreEqual(L'Y', s[4]);
			Assert::AreEqual(L'Z', s[5]);
			Assert::AreEqual(L'\0', s[6]);
			Assert::AreEqual(L'd', s[7]);
			Assert::AreEqual(L'e', s[8]);
			Assert::AreEqual(L'f', s[9]);
		}

		TEST_METHOD(StrUpr_06)
		{
			WCHAR s[10] = { L'A', L'B', L'C', L'x', L'y', L'z', L'\0', L'd', L'e', L'f' };
			SIZE_T n = StrCase(UPPERCASE, s, _countof(s));
			Assert::AreEqual((SIZE_T)6, n);
			Assert::AreEqual(L'A', s[0]);
			Assert::AreEqual(L'B', s[1]);
			Assert::AreEqual(L'C', s[2]);
			Assert::AreEqual(L'X', s[3]);
			Assert::AreEqual(L'Y', s[4]);
			Assert::AreEqual(L'Z', s[5]);
			Assert::AreEqual(L'\0', s[6]);
			Assert::AreEqual(L'd', s[7]);
			Assert::AreEqual(L'e', s[8]);
			Assert::AreEqual(L'f', s[9]);
		}

		TEST_METHOD(StrUpr_01A)
		{
			CHAR s[10] = { 'A', 'B', 'C', 'x', 'y', 'z', '\0', 'd', 'e', 'f' };
			SIZE_T n = StrUpr(s);
			Assert::AreEqual((SIZE_T)6, n);
			Assert::AreEqual('A', s[0]);
			Assert::AreEqual('B', s[1]);
			Assert::AreEqual('C', s[2]);
			Assert::AreEqual('X', s[3]);
			Assert::AreEqual('Y', s[4]);
			Assert::AreEqual('Z', s[5]);
			Assert::AreEqual('\0', s[6]);
			Assert::AreEqual('d', s[7]);
			Assert::AreEqual('e', s[8]);
			Assert::AreEqual('f', s[9]);
		}

		TEST_METHOD(StrUpr_02A)
		{
			CHAR s[10] = { 'A', 'B', 'C', 'x', 'y', 'z', '\0', 'd', 'e', 'f' };
			SIZE_T n = StrUpr(s, -100);
			Assert::AreEqual((SIZE_T)6, n);
			Assert::AreEqual('A', s[0]);
			Assert::AreEqual('B', s[1]);
			Assert::AreEqual('C', s[2]);
			Assert::AreEqual('X', s[3]);
			Assert::AreEqual('Y', s[4]);
			Assert::AreEqual('Z', s[5]);
			Assert::AreEqual('\0', s[6]);
			Assert::AreEqual('d', s[7]);
			Assert::AreEqual('e', s[8]);
			Assert::AreEqual('f', s[9]);
		}

		TEST_METHOD(StrUpr_03A)
		{
			CHAR s[10] = { 'A', 'B', 'C', 'x', 'y', 'z', '\0', 'd', 'e', 'f' };
			SIZE_T n = StrUpr(s, 0);
			Assert::AreEqual((SIZE_T)0, n);
			Assert::AreEqual('\0', s[0]);
			Assert::AreEqual('B', s[1]);
			Assert::AreEqual('C', s[2]);
			Assert::AreEqual('x', s[3]);
			Assert::AreEqual('y', s[4]);
			Assert::AreEqual('z', s[5]);
			Assert::AreEqual('\0', s[6]);
			Assert::AreEqual('d', s[7]);
			Assert::AreEqual('e', s[8]);
			Assert::AreEqual('f', s[9]);
		}

		TEST_METHOD(StrUpr_04A)
		{
			CHAR s[10] = { 'A', 'B', 'C', 'x', 'y', 'z', '\0', 'd', 'e', 'f' };
			SIZE_T n = StrUpr(s, 4);
			Assert::AreEqual((SIZE_T)4, n);
			Assert::AreEqual('A', s[0]);
			Assert::AreEqual('B', s[1]);
			Assert::AreEqual('C', s[2]);
			Assert::AreEqual('X', s[3]);
			Assert::AreEqual('\0', s[4]);
			Assert::AreEqual('z', s[5]);
			Assert::AreEqual('\0', s[6]);
			Assert::AreEqual('d', s[7]);
			Assert::AreEqual('e', s[8]);
			Assert::AreEqual('f', s[9]);
		}

		TEST_METHOD(StrUpr_05A)
		{
			CHAR s[10] = { 'A', 'B', 'C', 'x', 'y', 'z', '\0', 'd', 'e', 'f' };
			SIZE_T n = StrUpr(s, _countof(s));
			Assert::AreEqual((SIZE_T)6, n);
			Assert::AreEqual('A', s[0]);
			Assert::AreEqual('B', s[1]);
			Assert::AreEqual('C', s[2]);
			Assert::AreEqual('X', s[3]);
			Assert::AreEqual('Y', s[4]);
			Assert::AreEqual('Z', s[5]);
			Assert::AreEqual('\0', s[6]);
			Assert::AreEqual('d', s[7]);
			Assert::AreEqual('e', s[8]);
			Assert::AreEqual('f', s[9]);
		}

		TEST_METHOD(StrUpr_06A)
		{
			CHAR s[10] = { 'A', 'B', 'C', 'x', 'y', 'z', '\0', 'd', 'e', 'f' };
			SIZE_T n = StrCase(UPPERCASE, s, _countof(s));
			Assert::AreEqual((SIZE_T)6, n);
			Assert::AreEqual('A', s[0]);
			Assert::AreEqual('B', s[1]);
			Assert::AreEqual('C', s[2]);
			Assert::AreEqual('X', s[3]);
			Assert::AreEqual('Y', s[4]);
			Assert::AreEqual('Z', s[5]);
			Assert::AreEqual('\0', s[6]);
			Assert::AreEqual('d', s[7]);
			Assert::AreEqual('e', s[8]);
			Assert::AreEqual('f', s[9]);
		}

		TEST_METHOD(StrLwr_01)
		{
			WCHAR s[10] = { L'A', L'B', L'C', L'x', L'y', L'z', L'\0', L'D', L'E', L'F' };
			SIZE_T n = StrLwr(s);
			Assert::AreEqual((SIZE_T)6, n);
			Assert::AreEqual(L'a', s[0]);
			Assert::AreEqual(L'b', s[1]);
			Assert::AreEqual(L'c', s[2]);
			Assert::AreEqual(L'x', s[3]);
			Assert::AreEqual(L'y', s[4]);
			Assert::AreEqual(L'z', s[5]);
			Assert::AreEqual(L'\0', s[6]);
			Assert::AreEqual(L'D', s[7]);
			Assert::AreEqual(L'E', s[8]);
			Assert::AreEqual(L'F', s[9]);
		}

		TEST_METHOD(StrLwr_02)
		{
			WCHAR s[10] = { L'A', L'B', L'C', L'x', L'y', L'z', L'\0', L'D', L'E', L'F' };
			SIZE_T n = StrLwr(s, -100);
			Assert::AreEqual((SIZE_T)6, n);
			Assert::AreEqual(L'a', s[0]);
			Assert::AreEqual(L'b', s[1]);
			Assert::AreEqual(L'c', s[2]);
			Assert::AreEqual(L'x', s[3]);
			Assert::AreEqual(L'y', s[4]);
			Assert::AreEqual(L'z', s[5]);
			Assert::AreEqual(L'\0', s[6]);
			Assert::AreEqual(L'D', s[7]);
			Assert::AreEqual(L'E', s[8]);
			Assert::AreEqual(L'F', s[9]);
		}

		TEST_METHOD(StrLwr_03)
		{
			WCHAR s[10] = { L'A', L'B', L'C', L'x', L'y', L'z', L'\0', L'D', L'E', L'F' };
			SIZE_T n = StrLwr(s, 0);
			Assert::AreEqual((SIZE_T)0, n);
			Assert::AreEqual(L'\0', s[0]);
			Assert::AreEqual(L'B', s[1]);
			Assert::AreEqual(L'C', s[2]);
			Assert::AreEqual(L'x', s[3]);
			Assert::AreEqual(L'y', s[4]);
			Assert::AreEqual(L'z', s[5]);
			Assert::AreEqual(L'\0', s[6]);
			Assert::AreEqual(L'D', s[7]);
			Assert::AreEqual(L'E', s[8]);
			Assert::AreEqual(L'F', s[9]);
		}

		TEST_METHOD(StrLwr_04)
		{
			WCHAR s[10] = { L'A', L'B', L'C', L'x', L'y', L'z', L'\0', L'D', L'E', L'F' };
			SIZE_T n = StrLwr(s, 4);
			Assert::AreEqual((SIZE_T)4, n);
			Assert::AreEqual(L'a', s[0]);
			Assert::AreEqual(L'b', s[1]);
			Assert::AreEqual(L'c', s[2]);
			Assert::AreEqual(L'x', s[3]);
			Assert::AreEqual(L'\0', s[4]);
			Assert::AreEqual(L'z', s[5]);
			Assert::AreEqual(L'\0', s[6]);
			Assert::AreEqual(L'D', s[7]);
			Assert::AreEqual(L'E', s[8]);
			Assert::AreEqual(L'F', s[9]);
		}

		TEST_METHOD(StrLwr_05)
		{
			WCHAR s[10] = { L'A', L'B', L'C', L'x', L'y', L'z', L'\0', L'D', L'E', L'F' };
			SIZE_T n = StrLwr(s, _countof(s));
			Assert::AreEqual((SIZE_T)6, n);
			Assert::AreEqual(L'a', s[0]);
			Assert::AreEqual(L'b', s[1]);
			Assert::AreEqual(L'c', s[2]);
			Assert::AreEqual(L'x', s[3]);
			Assert::AreEqual(L'y', s[4]);
			Assert::AreEqual(L'z', s[5]);
			Assert::AreEqual(L'\0', s[6]);
			Assert::AreEqual(L'D', s[7]);
			Assert::AreEqual(L'E', s[8]);
			Assert::AreEqual(L'F', s[9]);
		}

		TEST_METHOD(StrLwr_06)
		{
			WCHAR s[10] = { L'A', L'B', L'C', L'x', L'y', L'z', L'\0', L'D', L'E', L'F' };
			SIZE_T n = StrCase(LOWERCASE, s, _countof(s));
			Assert::AreEqual((SIZE_T)6, n);
			Assert::AreEqual(L'a', s[0]);
			Assert::AreEqual(L'b', s[1]);
			Assert::AreEqual(L'c', s[2]);
			Assert::AreEqual(L'x', s[3]);
			Assert::AreEqual(L'y', s[4]);
			Assert::AreEqual(L'z', s[5]);
			Assert::AreEqual(L'\0', s[6]);
			Assert::AreEqual(L'D', s[7]);
			Assert::AreEqual(L'E', s[8]);
			Assert::AreEqual(L'F', s[9]);
		}

		TEST_METHOD(StrLwr_01A)
		{
			CHAR s[10] = { 'A', 'B', 'C', 'x', 'y', 'z', '\0', 'D', 'E', 'F' };
			SIZE_T n = StrLwr(s);
			Assert::AreEqual((SIZE_T)6, n);
			Assert::AreEqual('a', s[0]);
			Assert::AreEqual('b', s[1]);
			Assert::AreEqual('c', s[2]);
			Assert::AreEqual('x', s[3]);
			Assert::AreEqual('y', s[4]);
			Assert::AreEqual('z', s[5]);
			Assert::AreEqual('\0', s[6]);
			Assert::AreEqual('D', s[7]);
			Assert::AreEqual('E', s[8]);
			Assert::AreEqual('F', s[9]);
		}

		TEST_METHOD(StrLwr_02A)
		{
			CHAR s[10] = { 'A', 'B', 'C', 'x', 'y', 'z', '\0', 'D', 'E', 'F' };
			SIZE_T n = StrLwr(s, -100);
			Assert::AreEqual((SIZE_T)6, n);
			Assert::AreEqual('a', s[0]);
			Assert::AreEqual('b', s[1]);
			Assert::AreEqual('c', s[2]);
			Assert::AreEqual('x', s[3]);
			Assert::AreEqual('y', s[4]);
			Assert::AreEqual('z', s[5]);
			Assert::AreEqual('\0', s[6]);
			Assert::AreEqual('D', s[7]);
			Assert::AreEqual('E', s[8]);
			Assert::AreEqual('F', s[9]);
		}

		TEST_METHOD(StrLwr_03A)
		{
			CHAR s[10] = { 'A', 'B', 'C', 'x', 'y', 'z', '\0', 'D', 'E', 'F' };
			SIZE_T n = StrLwr(s, 0);
			Assert::AreEqual((SIZE_T)0, n);
			Assert::AreEqual('\0', s[0]);
			Assert::AreEqual('B', s[1]);
			Assert::AreEqual('C', s[2]);
			Assert::AreEqual('x', s[3]);
			Assert::AreEqual('y', s[4]);
			Assert::AreEqual('z', s[5]);
			Assert::AreEqual('\0', s[6]);
			Assert::AreEqual('D', s[7]);
			Assert::AreEqual('E', s[8]);
			Assert::AreEqual('F', s[9]);
		}

		TEST_METHOD(StrLwr_04A)
		{
			CHAR s[10] = { 'A', 'B', 'C', 'x', 'y', 'z', '\0', 'D', 'E', 'F' };
			SIZE_T n = StrLwr(s, 4);
			Assert::AreEqual((SIZE_T)4, n);
			Assert::AreEqual('a', s[0]);
			Assert::AreEqual('b', s[1]);
			Assert::AreEqual('c', s[2]);
			Assert::AreEqual('x', s[3]);
			Assert::AreEqual('\0', s[4]);
			Assert::AreEqual('z', s[5]);
			Assert::AreEqual('\0', s[6]);
			Assert::AreEqual('D', s[7]);
			Assert::AreEqual('E', s[8]);
			Assert::AreEqual('F', s[9]);
		}

		TEST_METHOD(StrLwr_05A)
		{
			CHAR s[10] = { 'A', 'B', 'C', 'x', 'y', 'z', '\0', 'D', 'E', 'F' };
			SIZE_T n = StrLwr(s, _countof(s));
			Assert::AreEqual((SIZE_T)6, n);
			Assert::AreEqual('a', s[0]);
			Assert::AreEqual('b', s[1]);
			Assert::AreEqual('c', s[2]);
			Assert::AreEqual('x', s[3]);
			Assert::AreEqual('y', s[4]);
			Assert::AreEqual('z', s[5]);
			Assert::AreEqual('\0', s[6]);
			Assert::AreEqual('D', s[7]);
			Assert::AreEqual('E', s[8]);
			Assert::AreEqual('F', s[9]);
		}

		TEST_METHOD(StrLwr_06A)
		{
			CHAR s[10] = { 'A', 'B', 'C', 'x', 'y', 'z', '\0', 'D', 'E', 'F' };
			SIZE_T n = StrCase(LOWERCASE, s, _countof(s));
			Assert::AreEqual((SIZE_T)6, n);
			Assert::AreEqual('a', s[0]);
			Assert::AreEqual('b', s[1]);
			Assert::AreEqual('c', s[2]);
			Assert::AreEqual('x', s[3]);
			Assert::AreEqual('y', s[4]);
			Assert::AreEqual('z', s[5]);
			Assert::AreEqual('\0', s[6]);
			Assert::AreEqual('D', s[7]);
			Assert::AreEqual('E', s[8]);
			Assert::AreEqual('F', s[9]);
		}

		TEST_METHOD(StrTrimScan_01)
		{
			WCHAR s[] = { L'A', L'B', L'C', L'X', L'Y', L'Z', L'\0' };
			int start = INT_MIN;
			int end = INT_MIN;
			StrTrimScan(s, start, end);
			Assert::AreEqual(0, start);
			Assert::AreEqual(6, end);
		}

		TEST_METHOD(StrTrimScan_02)
		{
			WCHAR s[] = { L'A', L'B', L'C', L'X', L'Y', L'Z', L'\0' };
			int start = INT_MIN;
			int end = INT_MIN;
			StrTrimScan(s, start, end, TRIM);
			Assert::AreEqual(0, start);
			Assert::AreEqual(6, end);
		}

		TEST_METHOD(StrTrimScan_03)
		{
			WCHAR s[] = { L'A', L'B', L'C', L'X', L'Y', L'Z', L'\0' };
			int start = INT_MIN;
			int end = INT_MIN;
			StrTrimScan(s, start, end, TRIM_HEAD);
			Assert::AreEqual(0, start);
			Assert::AreEqual(6, end);
		}

		TEST_METHOD(StrTrimScan_04)
		{
			WCHAR s[] = { L'A', L'B', L'C', L'X', L'Y', L'Z', L'\0' };
			int start = INT_MIN;
			int end = INT_MIN;
			StrTrimScan(s, start, end, TRIM_TAIL);
			Assert::AreEqual(0, start);
			Assert::AreEqual(6, end);
		}

		TEST_METHOD(StrTrimScan_11)
		{
			WCHAR s[] = { L' ', L'A', L'B', L'C', L' ', L'P', L'Q', L'R', L' ', L'X', L'Y', L'Z', L' ', L'\0'};
			int start = INT_MIN;
			int end = INT_MIN;
			StrTrimScan(s, start, end);
			Assert::AreEqual(1, start);
			Assert::AreEqual(12, end);
		}

		TEST_METHOD(StrTrimScan_12)
		{
			WCHAR s[] = { L' ', L'A', L'B', L'C', L' ', L'P', L'Q', L'R', L' ', L'X', L'Y', L'Z', L' ', L'\0' };
			int start = INT_MIN;
			int end = INT_MIN;
			StrTrimScan(s, start, end, TRIM);
			Assert::AreEqual(1, start);
			Assert::AreEqual(12, end);
		}

		TEST_METHOD(StrTrimScan_13)
		{
			WCHAR s[] = { L' ', L'A', L'B', L'C', L' ', L'P', L'Q', L'R', L' ', L'X', L'Y', L'Z', L' ', L'\0' };
			int start = INT_MIN;
			int end = INT_MIN;
			StrTrimScan(s, start, end, TRIM_HEAD);
			Assert::AreEqual(1, start);
			Assert::AreEqual(13, end);
		}

		TEST_METHOD(StrTrimScan_14)
		{
			WCHAR s[] = { L' ', L'A', L'B', L'C', L' ', L'P', L'Q', L'R', L' ', L'X', L'Y', L'Z', L' ', L'\0' };
			int start = INT_MIN;
			int end = INT_MIN;
			StrTrimScan(s, start, end, TRIM_TAIL);
			Assert::AreEqual(0, start);
			Assert::AreEqual(12, end);
		}

		TEST_METHOD(StrTrimScan_21)
		{
			WCHAR s[] = { L'\t', L'\n', L'\v', L'\f', L'\r', L' ', L'A', L'B', L'C', L'\t', L'\n', L'\v', L'\f', L'\r', L' ', L'P', L' ', L'R', L' ', L'X', L'Y', L'Z', L'\t', L'\n', L'\v', L'\f', L'\r', L' ', L'\0'};
			int start = INT_MIN;
			int end = INT_MIN;
			StrTrimScan(s, start, end);
			Assert::AreEqual(6, start);
			Assert::AreEqual(22, end);
		}

		TEST_METHOD(StrTrimScan_22)
		{
			WCHAR s[] = { L'\t', L'\n', L'\v', L'\f', L'\r', L' ', L'A', L'B', L'C', L'\t', L'\n', L'\v', L'\f', L'\r', L' ', L'P', L' ', L'R', L' ', L'X', L'Y', L'Z', L'\t', L'\n', L'\v', L'\f', L'\r', L' ', L'\0' };
			int start = INT_MIN;
			int end = INT_MIN;
			StrTrimScan(s, start, end, TRIM);
			Assert::AreEqual(6, start);
			Assert::AreEqual(22, end);
		}

		TEST_METHOD(StrTrimScan_23)
		{
			WCHAR s[] = { L'\t', L'\n', L'\v', L'\f', L'\r', L' ', L'A', L'B', L'C', L'\t', L'\n', L'\v', L'\f', L'\r', L' ', L'P', L' ', L'R', L' ', L'X', L'Y', L'Z', L'\t', L'\n', L'\v', L'\f', L'\r', L' ', L'\0' };
			int start = INT_MIN;
			int end = INT_MIN;
			StrTrimScan(s, start, end, TRIM_HEAD);
			Assert::AreEqual(6, start);
			Assert::AreEqual(28, end);
		}

		TEST_METHOD(StrTrimScan_24)
		{
			WCHAR s[] = { L'\t', L'\n', L'\v', L'\f', L'\r', L' ', L'A', L'B', L'C', L'\t', L'\n', L'\v', L'\f', L'\r', L' ', L'P', L' ', L'R', L' ', L'X', L'Y', L'Z', L'\t', L'\n', L'\v', L'\f', L'\r', L' ', L'\0' };
			int start = INT_MIN;
			int end = INT_MIN;
			StrTrimScan(s, start, end, TRIM_TAIL);
			Assert::AreEqual(0, start);
			Assert::AreEqual(22, end);
		}

		TEST_METHOD(StrTrimScan_01A)
		{
			CHAR s[] = { 'A', 'B', 'C', 'X', 'Y', 'Z', '\0' };
			int start = INT_MIN;
			int end = INT_MIN;
			StrTrimScan(s, start, end);
			Assert::AreEqual(0, start);
			Assert::AreEqual(6, end);
		}

		TEST_METHOD(StrTrimScan_02A)
		{
			CHAR s[] = { 'A', 'B', 'C', 'X', 'Y', 'Z', '\0' };
			int start = INT_MIN;
			int end = INT_MIN;
			StrTrimScan(s, start, end, TRIM);
			Assert::AreEqual(0, start);
			Assert::AreEqual(6, end);
		}

		TEST_METHOD(StrTrimScan_03A)
		{
			CHAR s[] = { 'A', 'B', 'C', 'X', 'Y', 'Z', '\0' };
			int start = INT_MIN;
			int end = INT_MIN;
			StrTrimScan(s, start, end, TRIM_HEAD);
			Assert::AreEqual(0, start);
			Assert::AreEqual(6, end);
		}

		TEST_METHOD(StrTrimScan_04A)
		{
			CHAR s[] = { 'A', 'B', 'C', 'X', 'Y', 'Z', '\0' };
			int start = INT_MIN;
			int end = INT_MIN;
			StrTrimScan(s, start, end, TRIM_TAIL);
			Assert::AreEqual(0, start);
			Assert::AreEqual(6, end);
		}

		TEST_METHOD(StrTrimScan_11A)
		{
			CHAR s[] = { ' ', 'A', 'B', 'C', ' ', 'P', 'Q', 'R', ' ', 'X', 'Y', 'Z', ' ', '\0' };
			int start = INT_MIN;
			int end = INT_MIN;
			StrTrimScan(s, start, end);
			Assert::AreEqual(1, start);
			Assert::AreEqual(12, end);
		}

		TEST_METHOD(StrTrimScan_12A)
		{
			CHAR s[] = { ' ', 'A', 'B', 'C', ' ', 'P', 'Q', 'R', ' ', 'X', 'Y', 'Z', ' ', '\0' };
			int start = INT_MIN;
			int end = INT_MIN;
			StrTrimScan(s, start, end, TRIM);
			Assert::AreEqual(1, start);
			Assert::AreEqual(12, end);
		}

		TEST_METHOD(StrTrimScan_13A)
		{
			CHAR s[] = { ' ', 'A', 'B', 'C', ' ', 'P', 'Q', 'R', ' ', 'X', 'Y', 'Z', ' ', '\0' };
			int start = INT_MIN;
			int end = INT_MIN;
			StrTrimScan(s, start, end, TRIM_HEAD);
			Assert::AreEqual(1, start);
			Assert::AreEqual(13, end);
		}

		TEST_METHOD(StrTrimScan_14A)
		{
			CHAR s[] = { ' ', 'A', 'B', 'C', ' ', 'P', 'Q', 'R', ' ', 'X', 'Y', 'Z', ' ', '\0' };
			int start = INT_MIN;
			int end = INT_MIN;
			StrTrimScan(s, start, end, TRIM_TAIL);
			Assert::AreEqual(0, start);
			Assert::AreEqual(12, end);
		}

		TEST_METHOD(StrTrimScan_21A)
		{
			CHAR s[] = { '\t', '\n', '\v', '\f', '\r', ' ', 'A', 'B', 'C', '\t', '\n', '\v', '\f', '\r', ' ', 'P', ' ', 'R', ' ', 'X', 'Y', 'Z', '\t', '\n', '\v', '\f', '\r', ' ', '\0' };
			int start = INT_MIN;
			int end = INT_MIN;
			StrTrimScan(s, start, end);
			Assert::AreEqual(6, start);
			Assert::AreEqual(22, end);
		}

		TEST_METHOD(StrTrimScan_22A)
		{
			CHAR s[] = { '\t', '\n', '\v', '\f', '\r', ' ', 'A', 'B', 'C', '\t', '\n', '\v', '\f', '\r', ' ', 'P', ' ', 'R', ' ', 'X', 'Y', 'Z', '\t', '\n', '\v', '\f', '\r', ' ', '\0' };
			int start = INT_MIN;
			int end = INT_MIN;
			StrTrimScan(s, start, end, TRIM);
			Assert::AreEqual(6, start);
			Assert::AreEqual(22, end);
		}

		TEST_METHOD(StrTrimScan_23A)
		{
			CHAR s[] = { '\t', '\n', '\v', '\f', '\r', ' ', 'A', 'B', 'C', '\t', '\n', '\v', '\f', '\r', ' ', 'P', ' ', 'R', ' ', 'X', 'Y', 'Z', '\t', '\n', '\v', '\f', '\r', ' ', '\0' };
			int start = INT_MIN;
			int end = INT_MIN;
			StrTrimScan(s, start, end, TRIM_HEAD);
			Assert::AreEqual(6, start);
			Assert::AreEqual(28, end);
		}

		TEST_METHOD(StrTrimScan_24A)
		{
			CHAR s[] = { '\t', '\n', '\v', '\f', '\r', ' ', 'A', 'B', 'C', '\t', '\n', '\v', '\f', '\r', ' ', 'P', ' ', 'R', ' ', 'X', 'Y', 'Z', '\t', '\n', '\v', '\f', '\r', ' ', '\0' };
			int start = INT_MIN;
			int end = INT_MIN;
			StrTrimScan(s, start, end, TRIM_TAIL);
			Assert::AreEqual(0, start);
			Assert::AreEqual(22, end);
		}

		TEST_METHOD(StrTrim_01)
		{
			WCHAR s[] = { L'A', L'B', L'C', L'\0' };
			WCHAR* t = StrTrim(s);
			Assert::AreEqual(s, t);
			Assert::AreEqual(L'A', s[0]);
			Assert::AreEqual(L'B', s[1]);
			Assert::AreEqual(L'C', s[2]);
			Assert::AreEqual(L'\0', s[3]);
		}

		TEST_METHOD(StrTrim_02)
		{
			WCHAR s[] = { L'A', L'B', L'C', L'\0' };
			WCHAR* t = StrTrim(s, TRIM);
			Assert::AreEqual(s, t);
			Assert::AreEqual(L'A', s[0]);
			Assert::AreEqual(L'B', s[1]);
			Assert::AreEqual(L'C', s[2]);
			Assert::AreEqual(L'\0', s[3]);
		}

		TEST_METHOD(StrTrim_03)
		{
			WCHAR s[] = { L'A', L'B', L'C', L'\0' };
			WCHAR* t = StrTrim(s, TRIM_HEAD);
			Assert::AreEqual(s, t);
			Assert::AreEqual(L'A', s[0]);
			Assert::AreEqual(L'B', s[1]);
			Assert::AreEqual(L'C', s[2]);
			Assert::AreEqual(L'\0', s[3]);
		}

		TEST_METHOD(StrTrim_04)
		{
			WCHAR s[] = { L'A', L'B', L'C', L'\0' };
			WCHAR* t = StrTrim(s, TRIM_TAIL);
			Assert::AreEqual(s, t);
			Assert::AreEqual(L'A', s[0]);
			Assert::AreEqual(L'B', s[1]);
			Assert::AreEqual(L'C', s[2]);
			Assert::AreEqual(L'\0', s[3]);
		}

		TEST_METHOD(StrTrim_11)
		{
			WCHAR s[] = { L' ', L'A', L'B', L'C', L' ', L'\0'};
			WCHAR* t = StrTrim(s);
			Assert::AreEqual(s, t);
			Assert::AreEqual(L'A', s[0]);
			Assert::AreEqual(L'B', s[1]);
			Assert::AreEqual(L'C', s[2]);
			Assert::AreEqual(L'\0', s[3]);
			Assert::AreEqual(L' ', s[4]);
			Assert::AreEqual(L'\0', s[5]);
		}

		TEST_METHOD(StrTrim_12)
		{
			WCHAR s[] = { L' ', L'A', L'B', L'C', L' ', L'\0' };
			WCHAR* t = StrTrim(s, TRIM);
			Assert::AreEqual(s, t);
			Assert::AreEqual(L'A', s[0]);
			Assert::AreEqual(L'B', s[1]);
			Assert::AreEqual(L'C', s[2]);
			Assert::AreEqual(L'\0', s[3]);
			Assert::AreEqual(L' ', s[4]);
			Assert::AreEqual(L'\0', s[5]);
		}

		TEST_METHOD(StrTrim_13)
		{
			WCHAR s[] = { L' ', L'A', L'B', L'C', L' ', L'\0' };
			WCHAR* t = StrTrim(s, TRIM_HEAD);
			Assert::AreEqual(s, t);
			Assert::AreEqual(L'A', s[0]);
			Assert::AreEqual(L'B', s[1]);
			Assert::AreEqual(L'C', s[2]);
			Assert::AreEqual(L' ', s[3]);
			Assert::AreEqual(L'\0', s[4]);
			Assert::AreEqual(L'\0', s[5]);
		}

		TEST_METHOD(StrTrim_14)
		{
			WCHAR s[] = { L' ', L'A', L'B', L'C', L' ', L'\0' };
			WCHAR* t = StrTrim(s, TRIM_TAIL);
			Assert::AreEqual(s, t);
			Assert::AreEqual(L' ', s[0]);
			Assert::AreEqual(L'A', s[1]);
			Assert::AreEqual(L'B', s[2]);
			Assert::AreEqual(L'C', s[3]);
			Assert::AreEqual(L'\0', s[4]);
			Assert::AreEqual(L'\0', s[5]);
		}

		TEST_METHOD(StrTrim_21)
		{
			WCHAR s[] = { L' ', L'\t', L'\n', L'\v', L'\f', L'\r', L'A', L'B', L'C', L'\t', L'\n', L'\v', L'\f', L'\r', L' ', L'\0' };
			WCHAR* t = StrTrim(s);
			Assert::AreEqual(s, t);
			Assert::AreEqual(L'A', s[0]);
			Assert::AreEqual(L'B', s[1]);
			Assert::AreEqual(L'C', s[2]);
			Assert::AreEqual(L'\0', s[3]);
			Assert::AreEqual(L'\f', s[4]);
			Assert::AreEqual(L'\r', s[5]);
			Assert::AreEqual(L'A', s[6]);
			Assert::AreEqual(L'B', s[7]);
			Assert::AreEqual(L'C', s[8]);
			Assert::AreEqual(L'\t', s[9]);
			Assert::AreEqual(L'\n', s[10]);
			Assert::AreEqual(L'\v', s[11]);
			Assert::AreEqual(L'\f', s[12]);
			Assert::AreEqual(L'\r', s[13]);
			Assert::AreEqual(L' ', s[14]);
			Assert::AreEqual(L'\0', s[15]);
		}

		TEST_METHOD(StrTrim_22)
		{
			WCHAR s[] = { L' ', L'\t', L'\n', L'\v', L'\f', L'\r', L'A', L'B', L'C', L'\t', L'\n', L'\v', L'\f', L'\r', L' ', L'\0' };
			WCHAR* t = StrTrim(s, TRIM);
			Assert::AreEqual(s, t);
			Assert::AreEqual(L'A', s[0]);
			Assert::AreEqual(L'B', s[1]);
			Assert::AreEqual(L'C', s[2]);
			Assert::AreEqual(L'\0', s[3]);
			Assert::AreEqual(L'\f', s[4]);
			Assert::AreEqual(L'\r', s[5]);
			Assert::AreEqual(L'A', s[6]);
			Assert::AreEqual(L'B', s[7]);
			Assert::AreEqual(L'C', s[8]);
			Assert::AreEqual(L'\t', s[9]);
			Assert::AreEqual(L'\n', s[10]);
			Assert::AreEqual(L'\v', s[11]);
			Assert::AreEqual(L'\f', s[12]);
			Assert::AreEqual(L'\r', s[13]);
			Assert::AreEqual(L' ', s[14]);
			Assert::AreEqual(L'\0', s[15]);
		}

		TEST_METHOD(StrTrim_23)
		{
			WCHAR s[] = { L' ', L'\t', L'\n', L'\v', L'\f', L'\r', L'A', L'B', L'C', L'\t', L'\n', L'\v', L'\f', L'\r', L' ', L'\0' };
			WCHAR* t = StrTrim(s, TRIM_HEAD);
			Assert::AreEqual(s, t);
			Assert::AreEqual(L'A', s[0]);
			Assert::AreEqual(L'B', s[1]);
			Assert::AreEqual(L'C', s[2]);
			Assert::AreEqual(L'\t', s[3]);
			Assert::AreEqual(L'\n', s[4]);
			Assert::AreEqual(L'\v', s[5]);
			Assert::AreEqual(L'\f', s[6]);
			Assert::AreEqual(L'\r', s[7]);
			Assert::AreEqual(L' ', s[8]);
			Assert::AreEqual(L'\0', s[9]);
			Assert::AreEqual(L'\n', s[10]);
			Assert::AreEqual(L'\v', s[11]);
			Assert::AreEqual(L'\f', s[12]);
			Assert::AreEqual(L'\r', s[13]);
			Assert::AreEqual(L' ', s[14]);
			Assert::AreEqual(L'\0', s[15]);
		}

		TEST_METHOD(StrTrim_24)
		{
			WCHAR s[] = { L' ', L'\t', L'\n', L'\v', L'\f', L'\r', L'A', L'B', L'C', L'\t', L'\n', L'\v', L'\f', L'\r', L' ', L'\0' };
			WCHAR* t = StrTrim(s, TRIM_TAIL);
			Assert::AreEqual(s, t);
			Assert::AreEqual(L' ', s[0]);
			Assert::AreEqual(L'\t', s[1]);
			Assert::AreEqual(L'\n', s[2]);
			Assert::AreEqual(L'\v', s[3]);
			Assert::AreEqual(L'\f', s[4]);
			Assert::AreEqual(L'\r', s[5]);
			Assert::AreEqual(L'A', s[6]);
			Assert::AreEqual(L'B', s[7]);
			Assert::AreEqual(L'C', s[8]);
			Assert::AreEqual(L'\0', s[9]);
			Assert::AreEqual(L'\n', s[10]);
			Assert::AreEqual(L'\v', s[11]);
			Assert::AreEqual(L'\f', s[12]);
			Assert::AreEqual(L'\r', s[13]);
			Assert::AreEqual(L' ', s[14]);
			Assert::AreEqual(L'\0', s[15]);
		}

		TEST_METHOD(StrTrim_01A)
		{
			CHAR s[] = { 'A', 'B', 'C', '\0' };
			CHAR* t = StrTrim(s);
			Assert::AreEqual(s, t);
			Assert::AreEqual('A', s[0]);
			Assert::AreEqual('B', s[1]);
			Assert::AreEqual('C', s[2]);
			Assert::AreEqual('\0', s[3]);
		}

		TEST_METHOD(StrTrim_02A)
		{
			CHAR s[] = { 'A', 'B', 'C', '\0' };
			CHAR* t = StrTrim(s, TRIM);
			Assert::AreEqual(s, t);
			Assert::AreEqual('A', s[0]);
			Assert::AreEqual('B', s[1]);
			Assert::AreEqual('C', s[2]);
			Assert::AreEqual('\0', s[3]);
		}

		TEST_METHOD(StrTrim_03A)
		{
			CHAR s[] = { 'A', 'B', 'C', '\0' };
			CHAR* t = StrTrim(s, TRIM_HEAD);
			Assert::AreEqual(s, t);
			Assert::AreEqual('A', s[0]);
			Assert::AreEqual('B', s[1]);
			Assert::AreEqual('C', s[2]);
			Assert::AreEqual('\0', s[3]);
		}

		TEST_METHOD(StrTrim_04A)
		{
			CHAR s[] = { 'A', 'B', 'C', '\0' };
			CHAR* t = StrTrim(s, TRIM_TAIL);
			Assert::AreEqual(s, t);
			Assert::AreEqual('A', s[0]);
			Assert::AreEqual('B', s[1]);
			Assert::AreEqual('C', s[2]);
			Assert::AreEqual('\0', s[3]);
		}

		TEST_METHOD(StrTrim_11A)
		{
			CHAR s[] = { ' ', 'A', 'B', 'C', ' ', '\0' };
			CHAR* t = StrTrim(s);
			Assert::AreEqual(s, t);
			Assert::AreEqual('A', s[0]);
			Assert::AreEqual('B', s[1]);
			Assert::AreEqual('C', s[2]);
			Assert::AreEqual('\0', s[3]);
			Assert::AreEqual(' ', s[4]);
			Assert::AreEqual('\0', s[5]);
		}

		TEST_METHOD(StrTrim_12A)
		{
			CHAR s[] = { ' ', 'A', 'B', 'C', ' ', '\0' };
			CHAR* t = StrTrim(s, TRIM);
			Assert::AreEqual(s, t);
			Assert::AreEqual('A', s[0]);
			Assert::AreEqual('B', s[1]);
			Assert::AreEqual('C', s[2]);
			Assert::AreEqual('\0', s[3]);
			Assert::AreEqual(' ', s[4]);
			Assert::AreEqual('\0', s[5]);
		}

		TEST_METHOD(StrTrim_13A)
		{
			CHAR s[] = { ' ', 'A', 'B', 'C', ' ', '\0' };
			CHAR* t = StrTrim(s, TRIM_HEAD);
			Assert::AreEqual(s, t);
			Assert::AreEqual('A', s[0]);
			Assert::AreEqual('B', s[1]);
			Assert::AreEqual('C', s[2]);
			Assert::AreEqual(' ', s[3]);
			Assert::AreEqual('\0', s[4]);
			Assert::AreEqual('\0', s[5]);
		}

		TEST_METHOD(StrTrim_14A)
		{
			CHAR s[] = { ' ', 'A', 'B', 'C', ' ', '\0' };
			CHAR* t = StrTrim(s, TRIM_TAIL);
			Assert::AreEqual(s, t);
			Assert::AreEqual(' ', s[0]);
			Assert::AreEqual('A', s[1]);
			Assert::AreEqual('B', s[2]);
			Assert::AreEqual('C', s[3]);
			Assert::AreEqual('\0', s[4]);
			Assert::AreEqual('\0', s[5]);
		}

		TEST_METHOD(StrTrim_21A)
		{
			CHAR s[] = { ' ', '\t', '\n', '\v', '\f', '\r', 'A', 'B', 'C', '\t', '\n', '\v', '\f', '\r', ' ', '\0' };
			CHAR* t = StrTrim(s);
			Assert::AreEqual(s, t);
			Assert::AreEqual('A', s[0]);
			Assert::AreEqual('B', s[1]);
			Assert::AreEqual('C', s[2]);
			Assert::AreEqual('\0', s[3]);
			Assert::AreEqual('\f', s[4]);
			Assert::AreEqual('\r', s[5]);
			Assert::AreEqual('A', s[6]);
			Assert::AreEqual('B', s[7]);
			Assert::AreEqual('C', s[8]);
			Assert::AreEqual('\t', s[9]);
			Assert::AreEqual('\n', s[10]);
			Assert::AreEqual('\v', s[11]);
			Assert::AreEqual('\f', s[12]);
			Assert::AreEqual('\r', s[13]);
			Assert::AreEqual(' ', s[14]);
			Assert::AreEqual('\0', s[15]);
		}

		TEST_METHOD(StrTrim_22A)
		{
			CHAR s[] = { ' ', '\t', '\n', '\v', '\f', '\r', 'A', 'B', 'C', '\t', '\n', '\v', '\f', '\r', ' ', '\0' };
			CHAR* t = StrTrim(s, TRIM);
			Assert::AreEqual(s, t);
			Assert::AreEqual('A', s[0]);
			Assert::AreEqual('B', s[1]);
			Assert::AreEqual('C', s[2]);
			Assert::AreEqual('\0', s[3]);
			Assert::AreEqual('\f', s[4]);
			Assert::AreEqual('\r', s[5]);
			Assert::AreEqual('A', s[6]);
			Assert::AreEqual('B', s[7]);
			Assert::AreEqual('C', s[8]);
			Assert::AreEqual('\t', s[9]);
			Assert::AreEqual('\n', s[10]);
			Assert::AreEqual('\v', s[11]);
			Assert::AreEqual('\f', s[12]);
			Assert::AreEqual('\r', s[13]);
			Assert::AreEqual(' ', s[14]);
			Assert::AreEqual('\0', s[15]);
		}

		TEST_METHOD(StrTrim_23A)
		{
			CHAR s[] = { ' ', '\t', '\n', '\v', '\f', '\r', 'A', 'B', 'C', '\t', '\n', '\v', '\f', '\r', ' ', '\0' };
			CHAR* t = StrTrim(s, TRIM_HEAD);
			Assert::AreEqual(s, t);
			Assert::AreEqual('A', s[0]);
			Assert::AreEqual('B', s[1]);
			Assert::AreEqual('C', s[2]);
			Assert::AreEqual('\t', s[3]);
			Assert::AreEqual('\n', s[4]);
			Assert::AreEqual('\v', s[5]);
			Assert::AreEqual('\f', s[6]);
			Assert::AreEqual('\r', s[7]);
			Assert::AreEqual(' ', s[8]);
			Assert::AreEqual('\0', s[9]);
			Assert::AreEqual('\n', s[10]);
			Assert::AreEqual('\v', s[11]);
			Assert::AreEqual('\f', s[12]);
			Assert::AreEqual('\r', s[13]);
			Assert::AreEqual(' ', s[14]);
			Assert::AreEqual('\0', s[15]);
		}

		TEST_METHOD(StrTrim_24A)
		{
			CHAR s[] = { ' ', '\t', '\n', '\v', '\f', '\r', 'A', 'B', 'C', '\t', '\n', '\v', '\f', '\r', ' ', '\0' };
			CHAR* t = StrTrim(s, TRIM_TAIL);
			Assert::AreEqual(s, t);
			Assert::AreEqual(' ', s[0]);
			Assert::AreEqual('\t', s[1]);
			Assert::AreEqual('\n', s[2]);
			Assert::AreEqual('\v', s[3]);
			Assert::AreEqual('\f', s[4]);
			Assert::AreEqual('\r', s[5]);
			Assert::AreEqual('A', s[6]);
			Assert::AreEqual('B', s[7]);
			Assert::AreEqual('C', s[8]);
			Assert::AreEqual('\0', s[9]);
			Assert::AreEqual('\n', s[10]);
			Assert::AreEqual('\v', s[11]);
			Assert::AreEqual('\f', s[12]);
			Assert::AreEqual('\r', s[13]);
			Assert::AreEqual(' ', s[14]);
			Assert::AreEqual('\0', s[15]);
		}

		TEST_METHOD(VaStrCatLen_01)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT, L"A", NULL);
			Assert::AreEqual((SIZE_T)1, n);
		}

		TEST_METHOD(VaStrCatLen_02)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT, L"A", L"BC", NULL);
			Assert::AreEqual((SIZE_T)3, n);
		}

		TEST_METHOD(VaStrCatLen_03)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT, L"A", L"BC", L"DEF", NULL);
			Assert::AreEqual((SIZE_T)6, n);
		}

		TEST_METHOD(VaStrCatLen_04)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT, L"A", L"BC", L"DEF", L"GHIJ", NULL);
			Assert::AreEqual((SIZE_T)10, n);
		}

		TEST_METHOD(VaStrCatLen_05)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT, L"A", L"BC", L"DEF", L"GHIJ", L"KLMNO", NULL);
			Assert::AreEqual((SIZE_T)15, n);
		}

		TEST_METHOD(VaStrCatLen_06)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT, L"A", L"BC", L"DEF", L"GHIJ", L"KLMNO", L"PQRSTU", NULL);
			Assert::AreEqual((SIZE_T)21, n);
		}

		TEST_METHOD(VaStrCatLen_07)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT, L"A", L"BC", L"DEF", L"GHIJ", L"KLMNO", L"PQRSTU", L"VWXYZ01", NULL);
			Assert::AreEqual((SIZE_T)28, n);
		}

		TEST_METHOD(VaStrCatLen_08)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT, L"A", L"BC", L"DEF", L"GHIJ", L"KLMNO", L"PQRSTU", L"VWXYZ01", L"23456", NULL);
			Assert::AreEqual((SIZE_T)33, n);
		}

		TEST_METHOD(VaStrCatLen_09)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT, L"A", L"BC", L"DEF", L"GHIJ", L"KLMNO", L"PQRSTU", L"VWXYZ01", L"23456", L"789", NULL);
			Assert::AreEqual((SIZE_T)36, n);
		}

		TEST_METHOD(VaStrCatLen_20)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT2, L"A", L"BC", L"DEF", L"GHIJ", L"KLMNO", L"PQRSTU", L"VWXYZ01", L"23456", L"789");
			Assert::AreEqual((SIZE_T)3, n);
		}

		TEST_METHOD(VaStrCatLen_30)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT3, L"A", L"BC", L"DEF", L"GHIJ", L"KLMNO", L"PQRSTU", L"VWXYZ01", L"23456", L"789");
			Assert::AreEqual((SIZE_T)6, n);
		}

		TEST_METHOD(VaStrCatLen_40)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT4, L"A", L"BC", L"DEF", L"GHIJ", L"KLMNO", L"PQRSTU", L"VWXYZ01", L"23456", L"789");
			Assert::AreEqual((SIZE_T)10, n);
		}

		TEST_METHOD(VaStrCatLen_50)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT5, L"A", L"BC", L"DEF", L"GHIJ", L"KLMNO", L"PQRSTU", L"VWXYZ01", L"23456", L"789");
			Assert::AreEqual((SIZE_T)15, n);
		}

		TEST_METHOD(VaStrCatLen_60)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT6, L"A", L"BC", L"DEF", L"GHIJ", L"KLMNO", L"PQRSTU", L"VWXYZ01", L"23456", L"789");
			Assert::AreEqual((SIZE_T)21, n);
		}

		TEST_METHOD(VaStrCatLen_70)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT7, L"A", L"BC", L"DEF", L"GHIJ", L"KLMNO", L"PQRSTU", L"VWXYZ01", L"23456", L"789");
			Assert::AreEqual((SIZE_T)28, n);
		}

		TEST_METHOD(VaStrCatLen_80)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT8, L"A", L"BC", L"DEF", L"GHIJ", L"KLMNO", L"PQRSTU", L"VWXYZ01", L"23456", L"789");
			Assert::AreEqual((SIZE_T)33, n);
		}

		TEST_METHOD(VaStrCatLen_90)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT9, L"A", L"BC", L"DEF", L"GHIJ", L"KLMNO", L"PQRSTU", L"VWXYZ01", L"23456", L"789");
			Assert::AreEqual((SIZE_T)36, n);
		}

		TEST_METHOD(VaStrCatLen_01A)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT, "A", NULL);
			Assert::AreEqual((SIZE_T)1, n);
		}

		TEST_METHOD(VaStrCatLen_02A)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT, "A", "BC", NULL);
			Assert::AreEqual((SIZE_T)3, n);
		}

		TEST_METHOD(VaStrCatLen_03A)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT, "A", "BC", "DEF", NULL);
			Assert::AreEqual((SIZE_T)6, n);
		}

		TEST_METHOD(VaStrCatLen_04A)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT, "A", "BC", "DEF", "GHIJ", NULL);
			Assert::AreEqual((SIZE_T)10, n);
		}

		TEST_METHOD(VaStrCatLen_05A)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT, "A", "BC", "DEF", "GHIJ", "KLMNO", NULL);
			Assert::AreEqual((SIZE_T)15, n);
		}

		TEST_METHOD(VaStrCatLen_06A)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT, "A", "BC", "DEF", "GHIJ", "KLMNO", "PQRSTU", NULL);
			Assert::AreEqual((SIZE_T)21, n);
		}

		TEST_METHOD(VaStrCatLen_07A)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT, "A", "BC", "DEF", "GHIJ", "KLMNO", "PQRSTU", "VWXYZ01", NULL);
			Assert::AreEqual((SIZE_T)28, n);
		}

		TEST_METHOD(VaStrCatLen_08A)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT, "A", "BC", "DEF", "GHIJ", "KLMNO", "PQRSTU", "VWXYZ01", "23456", NULL);
			Assert::AreEqual((SIZE_T)33, n);
		}

		TEST_METHOD(VaStrCatLen_09A)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT, "A", "BC", "DEF", "GHIJ", "KLMNO", "PQRSTU", "VWXYZ01", "23456", "789", NULL);
			Assert::AreEqual((SIZE_T)36, n);
		}

		TEST_METHOD(VaStrCatLen_20A)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT2, "A", "BC", "DEF", "GHIJ", "KLMNO", "PQRSTU", "VWXYZ01", "23456", "789");
			Assert::AreEqual((SIZE_T)3, n);
		}

		TEST_METHOD(VaStrCatLen_30A)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT3, "A", "BC", "DEF", "GHIJ", "KLMNO", "PQRSTU", "VWXYZ01", "23456", "789");
			Assert::AreEqual((SIZE_T)6, n);
		}

		TEST_METHOD(VaStrCatLen_40A)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT4, "A", "BC", "DEF", "GHIJ", "KLMNO", "PQRSTU", "VWXYZ01", "23456", "789");
			Assert::AreEqual((SIZE_T)10, n);
		}

		TEST_METHOD(VaStrCatLen_50A)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT5, "A", "BC", "DEF", "GHIJ", "KLMNO", "PQRSTU", "VWXYZ01", "23456", "789");
			Assert::AreEqual((SIZE_T)15, n);
		}

		TEST_METHOD(VaStrCatLen_60A)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT6, "A", "BC", "DEF", "GHIJ", "KLMNO", "PQRSTU", "VWXYZ01", "23456", "789");
			Assert::AreEqual((SIZE_T)21, n);
		}

		TEST_METHOD(VaStrCatLen_70A)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT7, "A", "BC", "DEF", "GHIJ", "KLMNO", "PQRSTU", "VWXYZ01", "23456", "789");
			Assert::AreEqual((SIZE_T)28, n);
		}

		TEST_METHOD(VaStrCatLen_80A)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT8, "A", "BC", "DEF", "GHIJ", "KLMNO", "PQRSTU", "VWXYZ01", "23456", "789");
			Assert::AreEqual((SIZE_T)33, n);
		}

		TEST_METHOD(VaStrCatLen_90A)
		{
			SIZE_T n = CallVaStrCatLen(CONCAT9, "A", "BC", "DEF", "GHIJ", "KLMNO", "PQRSTU", "VWXYZ01", "23456", "789");
			Assert::AreEqual((SIZE_T)36, n);
		}

		TEST_METHOD(VaStrCat_01)
		{
			WCHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT, L"A", NULL);
			Assert::AreEqual((SIZE_T)1, n);
			Assert::AreEqual(L"A", s);
		}

		TEST_METHOD(VaStrCat_02)
		{
			WCHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT, L"A", L"BC", NULL);
			Assert::AreEqual((SIZE_T)3, n);
			Assert::AreEqual(L"ABC", s);
		}

		TEST_METHOD(VaStrCat_03)
		{
			WCHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT, L"A", L"BC", L"DEF", NULL);
			Assert::AreEqual((SIZE_T)6, n);
			Assert::AreEqual(L"ABCDEF", s);
		}

		TEST_METHOD(VaStrCat_04)
		{
			WCHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT, L"A", L"BC", L"DEF", L"GHIJ", NULL);
			Assert::AreEqual((SIZE_T)10, n);
			Assert::AreEqual(L"ABCDEFGHIJ", s);
		}

		TEST_METHOD(VaStrCat_05)
		{
			WCHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT, L"A", L"BC", L"DEF", L"GHIJ", L"KLMNO", NULL);
			Assert::AreEqual((SIZE_T)15, n);
			Assert::AreEqual(L"ABCDEFGHIJKLMNO", s);
		}

		TEST_METHOD(VaStrCat_06)
		{
			WCHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT, L"A", L"BC", L"DEF", L"GHIJ", L"KLMNO", L"PQRSTU", NULL);
			Assert::AreEqual((SIZE_T)21, n);
			Assert::AreEqual(L"ABCDEFGHIJKLMNOPQRSTU", s);
		}

		TEST_METHOD(VaStrCat_07)
		{
			WCHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT, L"A", L"BC", L"DEF", L"GHIJ", L"KLMNO", L"PQRSTU", L"VWXYZ01", NULL);
			Assert::AreEqual((SIZE_T)28, n);
			Assert::AreEqual(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ01", s);
		}

		TEST_METHOD(VaStrCat_08)
		{
			WCHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT, L"A", L"BC", L"DEF", L"GHIJ", L"KLMNO", L"PQRSTU", L"VWXYZ01", L"23456", NULL);
			Assert::AreEqual((SIZE_T)33, n);
			Assert::AreEqual(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456", s);
		}

		TEST_METHOD(VaStrCat_09)
		{
			WCHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT, L"A", L"BC", L"DEF", L"GHIJ", L"KLMNO", L"PQRSTU", L"VWXYZ01", L"23456", L"789", NULL);
			Assert::AreEqual((SIZE_T)36, n);
			Assert::AreEqual(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", s);
		}

		TEST_METHOD(VaStrCat_20)
		{
			WCHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT2, L"A", L"BC", L"DEF", L"GHIJ", L"KLMNO", L"PQRSTU", L"VWXYZ01", L"23456", L"789");
			Assert::AreEqual((SIZE_T)3, n);
			Assert::AreEqual(L"ABC", s);
		}

		TEST_METHOD(VaStrCat_30)
		{
			WCHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT3, L"A", L"BC", L"DEF", L"GHIJ", L"KLMNO", L"PQRSTU", L"VWXYZ01", L"23456", L"789");
			Assert::AreEqual((SIZE_T)6, n);
			Assert::AreEqual(L"ABCDEF", s);
		}

		TEST_METHOD(VaStrCat_40)
		{
			WCHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT4, L"A", L"BC", L"DEF", L"GHIJ", L"KLMNO", L"PQRSTU", L"VWXYZ01", L"23456", L"789");
			Assert::AreEqual((SIZE_T)10, n);
			Assert::AreEqual(L"ABCDEFGHIJ", s);
		}

		TEST_METHOD(VaStrCat_50)
		{
			WCHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT5, L"A", L"BC", L"DEF", L"GHIJ", L"KLMNO", L"PQRSTU", L"VWXYZ01", L"23456", L"789");
			Assert::AreEqual((SIZE_T)15, n);
			Assert::AreEqual(L"ABCDEFGHIJKLMNO", s);
		}

		TEST_METHOD(VaStrCat_60)
		{
			WCHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT6, L"A", L"BC", L"DEF", L"GHIJ", L"KLMNO", L"PQRSTU", L"VWXYZ01", L"23456", L"789");
			Assert::AreEqual((SIZE_T)21, n);
			Assert::AreEqual(L"ABCDEFGHIJKLMNOPQRSTU", s);
		}

		TEST_METHOD(VaStrCat_70)
		{
			WCHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT7, L"A", L"BC", L"DEF", L"GHIJ", L"KLMNO", L"PQRSTU", L"VWXYZ01", L"23456", L"789");
			Assert::AreEqual((SIZE_T)28, n);
			Assert::AreEqual(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ01", s);
		}

		TEST_METHOD(VaStrCat_80)
		{
			WCHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT8, L"A", L"BC", L"DEF", L"GHIJ", L"KLMNO", L"PQRSTU", L"VWXYZ01", L"23456", L"789");
			Assert::AreEqual((SIZE_T)33, n);
			Assert::AreEqual(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456", s);
		}

		TEST_METHOD(VaStrCat_90)
		{
			WCHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT9, L"A", L"BC", L"DEF", L"GHIJ", L"KLMNO", L"PQRSTU", L"VWXYZ01", L"23456", L"789");
			Assert::AreEqual((SIZE_T)36, n);
			Assert::AreEqual(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", s);
		}

		TEST_METHOD(VaStrCat_01A)
		{
			CHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT, "A", NULL);
			Assert::AreEqual((SIZE_T)1, n);
			Assert::AreEqual("A", s);
		}

		TEST_METHOD(VaStrCat_02A)
		{
			CHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT, "A", "BC", NULL);
			Assert::AreEqual((SIZE_T)3, n);
			Assert::AreEqual("ABC", s);
		}

		TEST_METHOD(VaStrCat_03A)
		{
			CHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT, "A", "BC", "DEF", NULL);
			Assert::AreEqual((SIZE_T)6, n);
			Assert::AreEqual("ABCDEF", s);
		}

		TEST_METHOD(VaStrCat_04A)
		{
			CHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT, "A", "BC", "DEF", "GHIJ", NULL);
			Assert::AreEqual((SIZE_T)10, n);
			Assert::AreEqual("ABCDEFGHIJ", s);
		}

		TEST_METHOD(VaStrCat_05A)
		{
			CHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT, "A", "BC", "DEF", "GHIJ", "KLMNO", NULL);
			Assert::AreEqual((SIZE_T)15, n);
			Assert::AreEqual("ABCDEFGHIJKLMNO", s);
		}

		TEST_METHOD(VaStrCat_06A)
		{
			CHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT, "A", "BC", "DEF", "GHIJ", "KLMNO", "PQRSTU", NULL);
			Assert::AreEqual((SIZE_T)21, n);
			Assert::AreEqual("ABCDEFGHIJKLMNOPQRSTU", s);
		}

		TEST_METHOD(VaStrCat_07A)
		{
			CHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT, "A", "BC", "DEF", "GHIJ", "KLMNO", "PQRSTU", "VWXYZ01", NULL);
			Assert::AreEqual((SIZE_T)28, n);
			Assert::AreEqual("ABCDEFGHIJKLMNOPQRSTUVWXYZ01", s);
		}

		TEST_METHOD(VaStrCat_08A)
		{
			CHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT, "A", "BC", "DEF", "GHIJ", "KLMNO", "PQRSTU", "VWXYZ01", "23456", NULL);
			Assert::AreEqual((SIZE_T)33, n);
			Assert::AreEqual("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456", s);
		}

		TEST_METHOD(VaStrCat_09A)
		{
			CHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT, "A", "BC", "DEF", "GHIJ", "KLMNO", "PQRSTU", "VWXYZ01", "23456", "789", NULL);
			Assert::AreEqual((SIZE_T)36, n);
			Assert::AreEqual("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", s);
		}

		TEST_METHOD(VaStrCat_20A)
		{
			CHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT2, "A", "BC", "DEF", "GHIJ", "KLMNO", "PQRSTU", "VWXYZ01", "23456", "789");
			Assert::AreEqual((SIZE_T)3, n);
			Assert::AreEqual("ABC", s);
		}

		TEST_METHOD(VaStrCat_30A)
		{
			CHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT3, "A", "BC", "DEF", "GHIJ", "KLMNO", "PQRSTU", "VWXYZ01", "23456", "789");
			Assert::AreEqual((SIZE_T)6, n);
			Assert::AreEqual("ABCDEF", s);
		}

		TEST_METHOD(VaStrCat_40A)
		{
			CHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT4, "A", "BC", "DEF", "GHIJ", "KLMNO", "PQRSTU", "VWXYZ01", "23456", "789");
			Assert::AreEqual((SIZE_T)10, n);
			Assert::AreEqual("ABCDEFGHIJ", s);
		}

		TEST_METHOD(VaStrCat_50A)
		{
			CHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT5, "A", "BC", "DEF", "GHIJ", "KLMNO", "PQRSTU", "VWXYZ01", "23456", "789");
			Assert::AreEqual((SIZE_T)15, n);
			Assert::AreEqual("ABCDEFGHIJKLMNO", s);
		}

		TEST_METHOD(VaStrCat_60A)
		{
			CHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT6, "A", "BC", "DEF", "GHIJ", "KLMNO", "PQRSTU", "VWXYZ01", "23456", "789");
			Assert::AreEqual((SIZE_T)21, n);
			Assert::AreEqual("ABCDEFGHIJKLMNOPQRSTU", s);
		}

		TEST_METHOD(VaStrCat_70A)
		{
			CHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT7, "A", "BC", "DEF", "GHIJ", "KLMNO", "PQRSTU", "VWXYZ01", "23456", "789");
			Assert::AreEqual((SIZE_T)28, n);
			Assert::AreEqual("ABCDEFGHIJKLMNOPQRSTUVWXYZ01", s);
		}

		TEST_METHOD(VaStrCat_80A)
		{
			CHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT8, "A", "BC", "DEF", "GHIJ", "KLMNO", "PQRSTU", "VWXYZ01", "23456", "789");
			Assert::AreEqual((SIZE_T)33, n);
			Assert::AreEqual("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456", s);
		}

		TEST_METHOD(VaStrCat_90A)
		{
			CHAR s[40] = { 0 };
			SIZE_T n = CallVaStrCat(s, CONCAT9, "A", "BC", "DEF", "GHIJ", "KLMNO", "PQRSTU", "VWXYZ01", "23456", "789");
			Assert::AreEqual((SIZE_T)36, n);
			Assert::AreEqual("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", s);
		}

		TEST_METHOD(IndexOf_001)
		{
			int i = IndexOf(L"", L'\0');
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_002)
		{
			int i = IndexOf(L"", L'A');
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_011)
		{
			int i = IndexOf(L"", L'\0', -100);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_012)
		{
			int i = IndexOf(L"", L'A', -100);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_021)
		{
			int i = IndexOf(L"", L'\0', 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_022)
		{
			int i = IndexOf(L"", L'A', 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_031)
		{
			int i = IndexOf(L"", L'\0', 10);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_032)
		{
			int i = IndexOf(L"", L'A', 10);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_101)
		{
			int i = IndexOf(L"ABCABD", L'\0');
			Assert::AreEqual(6, i);
		}

		TEST_METHOD(IndexOf_102)
		{
			int i = IndexOf(L"ABCABD", L'A');
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_103)
		{
			int i = IndexOf(L"ABCABD", L'B');
			Assert::AreEqual(1, i);
		}

		TEST_METHOD(IndexOf_104)
		{
			int i = IndexOf(L"ABCABD", L'D');
			Assert::AreEqual(5, i);
		}

		TEST_METHOD(IndexOf_105)
		{
			int i = IndexOf(L"ABCABD", L'E');
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_111)
		{
			int i = IndexOf(L"ABCABD", L'\0', -100);
			Assert::AreEqual(6, i);
		}

		TEST_METHOD(IndexOf_112)
		{
			int i = IndexOf(L"ABCABD", L'A', -100);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_113)
		{
			int i = IndexOf(L"ABCABD", L'B', -100);
			Assert::AreEqual(1, i);
		}

		TEST_METHOD(IndexOf_114)
		{
			int i = IndexOf(L"ABCABD", L'D', -100);
			Assert::AreEqual(5, i);
		}

		TEST_METHOD(IndexOf_115)
		{
			int i = IndexOf(L"ABCABD", L'E', -100);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_121)
		{
			int i = IndexOf(L"ABCABD", L'\0', 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_122)
		{
			int i = IndexOf(L"ABCABD", L'A', 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_123)
		{
			int i = IndexOf(L"ABCABD", L'B', 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_124)
		{
			int i = IndexOf(L"ABCABD", L'D', 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_125)
		{
			int i = IndexOf(L"ABCABD", L'E', 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_131)
		{
			int i = IndexOf(L"ABCABD", L'\0', 3);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_132)
		{
			int i = IndexOf(L"ABCABD", L'A', 3);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_133)
		{
			int i = IndexOf(L"ABCABD", L'B', 3);
			Assert::AreEqual(1, i);
		}

		TEST_METHOD(IndexOf_134)
		{
			int i = IndexOf(L"ABCABD", L'C', 3);
			Assert::AreEqual(2, i);
		}

		TEST_METHOD(IndexOf_135)
		{
			int i = IndexOf(L"ABCABD", L'D', 3);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_141)
		{
			int i = IndexOf(L"ABCABD\0EE", L'\0', 10);
			Assert::AreEqual(6, i);
		}

		TEST_METHOD(IndexOf_142)
		{
			int i = IndexOf(L"ABCABD\0EE", L'A', 10);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_143)
		{
			int i = IndexOf(L"ABCABD\0EE", L'B', 10);
			Assert::AreEqual(1, i);
		}

		TEST_METHOD(IndexOf_144)
		{
			int i = IndexOf(L"ABCABD\0EE", L'D', 10);
			Assert::AreEqual(5, i);
		}

		TEST_METHOD(IndexOf_145)
		{
			int i = IndexOf(L"ABCABD\0EE", L'E', 10);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_201)
		{
			int i = IndexOf(L"", L"");
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_202)
		{
			int i = IndexOf(L"", L"A");
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_211)
		{
			int i = IndexOf(L"", L"", -100);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_212)
		{
			int i = IndexOf(L"", L"A", -100);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_221)
		{
			int i = IndexOf(L"", L"", 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_222)
		{
			int i = IndexOf(L"", L"A", 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_231)
		{
			int i = IndexOf(L"", L"", 10);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_232)
		{
			int i = IndexOf(L"", L"A", 10);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_301)
		{
			int i = IndexOf(L"ABCABCD", L"");
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_302)
		{
			int i = IndexOf(L"ABCABCD", L"", -100);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_303)
		{
			int i = IndexOf(L"ABCABCD", L"", 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_304)
		{
			int i = IndexOf(L"ABCABCD", L"", 3);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_305)
		{
			int i = IndexOf(L"ABCABCD", L"", 10);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_311)
		{
			int i = IndexOf(L"ABCABCD", L"ABC");
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_312)
		{
			int i = IndexOf(L"ABCABCD", L"ABC", -100);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_313)
		{
			int i = IndexOf(L"ABCABCD", L"ABC", 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_314)
		{
			int i = IndexOf(L"ABCABCD", L"ABC", 2);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_315)
		{
			int i = IndexOf(L"ABCABCD", L"ABC", 3);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_316)
		{
			int i = IndexOf(L"ABCABCD", L"ABC", 4);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_317)
		{
			int i = IndexOf(L"ABCABCD", L"ABC", 10);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_321)
		{
			int i = IndexOf(L"ABCABCD", L"BCA");
			Assert::AreEqual(1, i);
		}

		TEST_METHOD(IndexOf_322)
		{
			int i = IndexOf(L"ABCABCD", L"BCA", -100);
			Assert::AreEqual(1, i);
		}

		TEST_METHOD(IndexOf_323)
		{
			int i = IndexOf(L"ABCABCD", L"BCA", 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_324)
		{
			int i = IndexOf(L"ABCABCD", L"BCA", 3);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_325)
		{
			int i = IndexOf(L"ABCABCD", L"BCA", 4);
			Assert::AreEqual(1, i);
		}

		TEST_METHOD(IndexOf_326)
		{
			int i = IndexOf(L"ABCABCD", L"BCA", 5);
			Assert::AreEqual(1, i);
		}

		TEST_METHOD(IndexOf_327)
		{
			int i = IndexOf(L"ABCABCD", L"BCA", 10);
			Assert::AreEqual(1, i);
		}

		TEST_METHOD(IndexOf_331)
		{
			int i = IndexOf(L"ABCABCD", L"BCD");
			Assert::AreEqual(4, i);
		}

		TEST_METHOD(IndexOf_332)
		{
			int i = IndexOf(L"ABCABCD", L"BCD", -100);
			Assert::AreEqual(4, i);
		}

		TEST_METHOD(IndexOf_333)
		{
			int i = IndexOf(L"ABCABCD", L"BCD", 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_334)
		{
			int i = IndexOf(L"ABCABCD", L"BCD", 6);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_335)
		{
			int i = IndexOf(L"ABCABCD", L"BCD", 7);
			Assert::AreEqual(4, i);
		}

		TEST_METHOD(IndexOf_336)
		{
			int i = IndexOf(L"ABCABCD", L"BCD", 10);
			Assert::AreEqual(4, i);
		}

		TEST_METHOD(IndexOf_341)
		{
			int i = IndexOf(L"ABCABCD", L"BCE");
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_342)
		{
			int i = IndexOf(L"ABCABCD", L"XYZ");
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_001A)
		{
			int i = IndexOf("", '\0');
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_002A)
		{
			int i = IndexOf("", 'A');
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_011A)
		{
			int i = IndexOf("", '\0', -100);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_012A)
		{
			int i = IndexOf("", 'A', -100);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_021A)
		{
			int i = IndexOf("", '\0', 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_022A)
		{
			int i = IndexOf("", 'A', 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_031A)
		{
			int i = IndexOf("", '\0', 10);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_032A)
		{
			int i = IndexOf("", 'A', 10);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_101A)
		{
			int i = IndexOf("ABCABD", '\0');
			Assert::AreEqual(6, i);
		}

		TEST_METHOD(IndexOf_102A)
		{
			int i = IndexOf("ABCABD", 'A');
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_103A)
		{
			int i = IndexOf("ABCABD", 'B');
			Assert::AreEqual(1, i);
		}

		TEST_METHOD(IndexOf_104A)
		{
			int i = IndexOf("ABCABD", 'D');
			Assert::AreEqual(5, i);
		}

		TEST_METHOD(IndexOf_105A)
		{
			int i = IndexOf("ABCABD", 'E');
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_111A)
		{
			int i = IndexOf("ABCABD", '\0', -100);
			Assert::AreEqual(6, i);
		}

		TEST_METHOD(IndexOf_112A)
		{
			int i = IndexOf("ABCABD", 'A', -100);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_113A)
		{
			int i = IndexOf("ABCABD", 'B', -100);
			Assert::AreEqual(1, i);
		}

		TEST_METHOD(IndexOf_114A)
		{
			int i = IndexOf("ABCABD", 'D', -100);
			Assert::AreEqual(5, i);
		}

		TEST_METHOD(IndexOf_115A)
		{
			int i = IndexOf("ABCABD", 'E', -100);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_121A)
		{
			int i = IndexOf("ABCABD", '\0', 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_122A)
		{
			int i = IndexOf("ABCABD", 'A', 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_123A)
		{
			int i = IndexOf("ABCABD", 'B', 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_124A)
		{
			int i = IndexOf("ABCABD", 'D', 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_125A)
		{
			int i = IndexOf("ABCABD", 'E', 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_131A)
		{
			int i = IndexOf("ABCABD", '\0', 3);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_132A)
		{
			int i = IndexOf("ABCABD", 'A', 3);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_133A)
		{
			int i = IndexOf("ABCABD", 'B', 3);
			Assert::AreEqual(1, i);
		}

		TEST_METHOD(IndexOf_134A)
		{
			int i = IndexOf("ABCABD", 'C', 3);
			Assert::AreEqual(2, i);
		}

		TEST_METHOD(IndexOf_135A)
		{
			int i = IndexOf("ABCABD", 'D', 3);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_141A)
		{
			int i = IndexOf("ABCABD\0EE", '\0', 10);
			Assert::AreEqual(6, i);
		}

		TEST_METHOD(IndexOf_142A)
		{
			int i = IndexOf("ABCABD\0EE", 'A', 10);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_143A)
		{
			int i = IndexOf("ABCABD\0EE", 'B', 10);
			Assert::AreEqual(1, i);
		}

		TEST_METHOD(IndexOf_144A)
		{
			int i = IndexOf("ABCABD\0EE", 'D', 10);
			Assert::AreEqual(5, i);
		}

		TEST_METHOD(IndexOf_145A)
		{
			int i = IndexOf("ABCABD\0EE", 'E', 10);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_201A)
		{
			int i = IndexOf("", "");
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_202A)
		{
			int i = IndexOf("", "A");
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_211A)
		{
			int i = IndexOf("", "", -100);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_212A)
		{
			int i = IndexOf("", "A", -100);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_221A)
		{
			int i = IndexOf("", "", 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_222A)
		{
			int i = IndexOf("", "A", 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_231A)
		{
			int i = IndexOf("", "", 10);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_232A)
		{
			int i = IndexOf("", "A", 10);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_301A)
		{
			int i = IndexOf("ABCABCD", "");
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_302A)
		{
			int i = IndexOf("ABCABCD", "", -100);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_303A)
		{
			int i = IndexOf("ABCABCD", "", 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_304A)
		{
			int i = IndexOf("ABCABCD", "", 3);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_305A)
		{
			int i = IndexOf("ABCABCD", "", 10);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_311A)
		{
			int i = IndexOf("ABCABCD", "ABC");
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_312A)
		{
			int i = IndexOf("ABCABCD", "ABC", -100);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_313A)
		{
			int i = IndexOf("ABCABCD", "ABC", 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_314A)
		{
			int i = IndexOf("ABCABCD", "ABC", 2);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_315A)
		{
			int i = IndexOf("ABCABCD", "ABC", 3);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_316A)
		{
			int i = IndexOf("ABCABCD", "ABC", 4);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_317A)
		{
			int i = IndexOf("ABCABCD", "ABC", 10);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(IndexOf_321A)
		{
			int i = IndexOf("ABCABCD", "BCA");
			Assert::AreEqual(1, i);
		}

		TEST_METHOD(IndexOf_322A)
		{
			int i = IndexOf("ABCABCD", "BCA", -100);
			Assert::AreEqual(1, i);
		}

		TEST_METHOD(IndexOf_323A)
		{
			int i = IndexOf("ABCABCD", "BCA", 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_324A)
		{
			int i = IndexOf("ABCABCD", "BCA", 3);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_325A)
		{
			int i = IndexOf("ABCABCD", "BCA", 4);
			Assert::AreEqual(1, i);
		}

		TEST_METHOD(IndexOf_326A)
		{
			int i = IndexOf("ABCABCD", "BCA", 5);
			Assert::AreEqual(1, i);
		}

		TEST_METHOD(IndexOf_327A)
		{
			int i = IndexOf("ABCABCD", "BCA", 10);
			Assert::AreEqual(1, i);
		}

		TEST_METHOD(IndexOf_331A)
		{
			int i = IndexOf("ABCABCD", "BCD");
			Assert::AreEqual(4, i);
		}

		TEST_METHOD(IndexOf_332A)
		{
			int i = IndexOf("ABCABCD", "BCD", -100);
			Assert::AreEqual(4, i);
		}

		TEST_METHOD(IndexOf_333A)
		{
			int i = IndexOf("ABCABCD", "BCD", 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_334A)
		{
			int i = IndexOf("ABCABCD", "BCD", 6);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_335A)
		{
			int i = IndexOf("ABCABCD", "BCD", 7);
			Assert::AreEqual(4, i);
		}

		TEST_METHOD(IndexOf_336A)
		{
			int i = IndexOf("ABCABCD", "BCD", 10);
			Assert::AreEqual(4, i);
		}

		TEST_METHOD(IndexOf_341A)
		{
			int i = IndexOf("ABCABCD", "BCE");
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(IndexOf_342A)
		{
			int i = IndexOf("ABCABCD", "XYZ");
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_001)
		{
			int i = LastIndexOf(L"\0\0\0\0\0\0\0\0\0", L'\0');
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(LastIndexOf_002)
		{
			int i = LastIndexOf(L"\0\0\0\0\0\0\0\0\0", L'\0', -100);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(LastIndexOf_003)
		{
			int i = LastIndexOf(L"\0\0\0\0\0\0\0\0\0", L'\0', 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_004)
		{
			int i = LastIndexOf(L"\0\0\0\0\0\0\0\0\0", L'\0', 10);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(LastIndexOf_011)
		{
			int i = LastIndexOf(L"\0\0\0\0\0\0\0\0\0", L'A');
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_012)
		{
			int i = LastIndexOf(L"\0\0\0\0\0\0\0\0\0", L'A', -100);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_013)
		{
			int i = LastIndexOf(L"\0\0\0\0\0\0\0\0\0", L'A', 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_014)
		{
			int i = LastIndexOf(L"\0\0\0\0\0\0\0\0\0", L'A', 10);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_021)
		{
			int i = LastIndexOf(L"DCBACBA", L'A');
			Assert::AreEqual(6, i);
		}

		TEST_METHOD(LastIndexOf_022)
		{
			int i = LastIndexOf(L"DCBACBA", L'A', -100);
			Assert::AreEqual(6, i);
		}

		TEST_METHOD(LastIndexOf_023)
		{
			int i = LastIndexOf(L"DCBACBA", L'A', 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_024)
		{
			int i = LastIndexOf(L"DCBACBA", L'A', 3);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_025)
		{
			int i = LastIndexOf(L"DCBACBA", L'A', 4);
			Assert::AreEqual(3, i);
		}

		TEST_METHOD(LastIndexOf_026)
		{
			int i = LastIndexOf(L"DCBACBA\0A", L'A', 10);
			Assert::AreEqual(6, i);
		}

		TEST_METHOD(LastIndexOf_031)
		{
			int i = LastIndexOf(L"DCBACBA", L'B');
			Assert::AreEqual(5, i);
		}

		TEST_METHOD(LastIndexOf_032)
		{
			int i = LastIndexOf(L"DCBACBA", L'B', -100);
			Assert::AreEqual(5, i);
		}

		TEST_METHOD(LastIndexOf_033)
		{
			int i = LastIndexOf(L"DCBACBA", L'B', 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_034)
		{
			int i = LastIndexOf(L"DCBACBA", L'B', 4);
			Assert::AreEqual(2, i);
		}

		TEST_METHOD(LastIndexOf_035)
		{
			int i = LastIndexOf(L"DCBACBA\0B", L'B', 10);
			Assert::AreEqual(5, i);
		}

		TEST_METHOD(LastIndexOf_041)
		{
			int i = LastIndexOf(L"DCBACBA", L'D');
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(LastIndexOf_042)
		{
			int i = LastIndexOf(L"DCBACBA", L'D', -100);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(LastIndexOf_043)
		{
			int i = LastIndexOf(L"DCBACBA", L'D', 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_044)
		{
			int i = LastIndexOf(L"DCBACBA", L'D', 4);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(LastIndexOf_045)
		{
			int i = LastIndexOf(L"DCBACBA\0D", L'D', 10);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(LastIndexOf_051)
		{
			int i = LastIndexOf(L"DCBACBA", L'E');
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_052)
		{
			int i = LastIndexOf(L"DCBACBA", L'E', -100);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_053)
		{
			int i = LastIndexOf(L"DCBACBA", L'E', 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_054)
		{
			int i = LastIndexOf(L"DCBACBA", L'E', 4);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_055)
		{
			int i = LastIndexOf(L"DCBACBA\0E", L'E', 10);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_101)
		{
			int i = LastIndexOf(L"\0\0\0\0\0\0\0\0\0", L"");
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(LastIndexOf_102)
		{
			int i = LastIndexOf(L"\0\0\0\0\0\0\0\0\0", L"", -100);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(LastIndexOf_103)
		{
			int i = LastIndexOf(L"\0\0\0\0\0\0\0\0\0", L"", 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_104)
		{
			int i = LastIndexOf(L"\0\0\0\0\0\0\0\0\0", L"", 10);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(LastIndexOf_111)
		{
			int i = LastIndexOf(L"\0\0\0\0\0\0\0\0\0", L"A");
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_112)
		{
			int i = LastIndexOf(L"\0\0\0\0\0\0\0\0\0", L"A", -100);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_113)
		{
			int i = LastIndexOf(L"\0\0\0\0\0\0\0\0\0", L"A", 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_114)
		{
			int i = LastIndexOf(L"\0\0\0\0\0\0\0\0\0", L"A", 10);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_121)
		{
			int i = LastIndexOf(L"DCBACBA\0AA", L"A");
			Assert::AreEqual(6, i);
		}

		TEST_METHOD(LastIndexOf_122)
		{
			int i = LastIndexOf(L"DCBACBA\0AA", L"A", -100);
			Assert::AreEqual(6, i);
		}

		TEST_METHOD(LastIndexOf_123)
		{
			int i = LastIndexOf(L"DCBACBA\0AA", L"A", 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_124)
		{
			int i = LastIndexOf(L"DCBACBA\0AA", L"A", 3);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_125)
		{
			int i = LastIndexOf(L"DCBACBA\0AA", L"A", 4);
			Assert::AreEqual(3, i);
		}

		TEST_METHOD(LastIndexOf_126)
		{
			int i = LastIndexOf(L"DCBACBA\0AA", L"A", 10);
			Assert::AreEqual(6, i);
		}

		TEST_METHOD(LastIndexOf_131)
		{
			int i = LastIndexOf(L"DCBACBA\0BB", L"B");
			Assert::AreEqual(5, i);
		}

		TEST_METHOD(LastIndexOf_132)
		{
			int i = LastIndexOf(L"DCBACBA\0BB", L"B", -100);
			Assert::AreEqual(5, i);
		}

		TEST_METHOD(LastIndexOf_133)
		{
			int i = LastIndexOf(L"DCBACBA\0BB", L"B", 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_134)
		{
			int i = LastIndexOf(L"DCBACBA\0BB", L"B", 5);
			Assert::AreEqual(2, i);
		}

		TEST_METHOD(LastIndexOf_135)
		{
			int i = LastIndexOf(L"DCBACBA\0BB", L"B", 10);
			Assert::AreEqual(5, i);
		}

		TEST_METHOD(LastIndexOf_141)
		{
			int i = LastIndexOf(L"DCBACBA\0DD", L"D");
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(LastIndexOf_142)
		{
			int i = LastIndexOf(L"DCBACBA\0DD", L"D", -100);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(LastIndexOf_143)
		{
			int i = LastIndexOf(L"DCBACBA\0DD", L"D", 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_144)
		{
			int i = LastIndexOf(L"DCBACBA\0DD", L"D", 5);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(LastIndexOf_145)
		{
			int i = LastIndexOf(L"DCBACBA\0DD", L"D", 10);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(LastIndexOf_151)
		{
			int i = LastIndexOf(L"DCBACBA\0EE", L"E");
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_152)
		{
			int i = LastIndexOf(L"DCBACBA\0EE", L"E", -100);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_153)
		{
			int i = LastIndexOf(L"DCBACBA\0EE", L"E", 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_154)
		{
			int i = LastIndexOf(L"DCBACBA\0EE", L"E", 5);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_155)
		{
			int i = LastIndexOf(L"DCBACBA\0EE", L"E", 10);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_201)
		{
			int i = LastIndexOf(L"DCBACBA\0CBACBA", L"CBA");
			Assert::AreEqual(4, i);
		}

		TEST_METHOD(LastIndexOf_202)
		{
			int i = LastIndexOf(L"DCBACBA\0CBACBA", L"CBA", -100);
			Assert::AreEqual(4, i);
		}

		TEST_METHOD(LastIndexOf_203)
		{
			int i = LastIndexOf(L"DCBACBA\0CBACBA", L"CBA", 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_204)
		{
			int i = LastIndexOf(L"DCBACBA\0CBACBA", L"CBA", 6);
			Assert::AreEqual(1, i);
		}

		TEST_METHOD(LastIndexOf_205)
		{
			int i = LastIndexOf(L"DCBACBA\0CBACBAB", L"CBA", 16);
			Assert::AreEqual(4, i);
		}

		TEST_METHOD(LastIndexOf_211)
		{
			int i = LastIndexOf(L"DCBACBA\0DCBCBA", L"DCB");
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(LastIndexOf_212)
		{
			int i = LastIndexOf(L"DCBACBA\0DCBCBA", L"DCB", -100);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(LastIndexOf_213)
		{
			int i = LastIndexOf(L"DCBACBA\0DCBCBA", L"DCB", 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_214)
		{
			int i = LastIndexOf(L"DCBACBA\0DCBCBA", L"DCB", 6);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(LastIndexOf_215)
		{
			int i = LastIndexOf(L"DCBACBA\0DCBCBAB", L"DCB", 16);
			Assert::AreEqual(0, i);
		}

		TEST_METHOD(LastIndexOf_221)
		{
			int i = LastIndexOf(L"DCBACBA\0DCBCBA", L"ECB");
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_222)
		{
			int i = LastIndexOf(L"DCBACBA\0DCBCBA", L"ECB", -100);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_223)
		{
			int i = LastIndexOf(L"DCBACBA\0DCBCBA", L"ECB", 0);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_224)
		{
			int i = LastIndexOf(L"DCBACBA\0DCBCBA", L"ECB", 6);
			Assert::AreEqual(-1, i);
		}

		TEST_METHOD(LastIndexOf_225)
		{
			int i = LastIndexOf(L"DCBACBA\0DCBCBAB", L"ECB", 16);
			Assert::AreEqual(-1, i);
		}

		private:

			SIZE_T CallVaStrCatLen(StringOptions option, const WCHAR* str, ...)
			{
				va_list argptr;
				va_start(argptr, str);
				SIZE_T ret = VaStrCatLen(option, str, argptr);
				va_end(argptr);
				return ret;
			}

			SIZE_T CallVaStrCatLen(StringOptions option, const CHAR* str, ...)
			{
				va_list argptr;
				va_start(argptr, str);
				SIZE_T ret = VaStrCatLen(option, str, argptr);
				va_end(argptr);
				return ret;
			}

			SIZE_T CallVaStrCat(WCHAR* buf, StringOptions option, const WCHAR* str, ...)
			{
				va_list argptr;
				va_start(argptr, str);
				SIZE_T ret = VaStrCat(option, str, argptr, buf);
				va_end(argptr);
				return ret;
			}

			SIZE_T CallVaStrCat(CHAR* buf, StringOptions option, const CHAR* str, ...)
			{
				va_list argptr;
				va_start(argptr, str);
				SIZE_T ret = VaStrCat(option, str, argptr, buf);
				va_end(argptr);
				return ret;
			}
	};
}