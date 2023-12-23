#include "pch.h"
#include "CppUnitTest.h"
#include "hnrt/Cron.h"
#include "hnrt/Time.h"
#include "hnrt/Debug.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace hnrt;

namespace UnitTestCronLib
{
	TEST_CLASS(EvalTests)
	{
	public:

		TEST_METHOD(Test0001)
		{
			static const WCHAR szExpression[] = { L"* * * * * ?" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 0 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test0001:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test0001: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)23, st2.wDay);
			Assert::AreEqual((WORD)22, st2.wHour);
			Assert::AreEqual((WORD)42, st2.wMinute);
			Assert::AreEqual((WORD)37, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test0002)
		{
			static const WCHAR szExpression[] = { L"* * * * * ? *" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test0002:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test0002: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)23, st2.wDay);
			Assert::AreEqual((WORD)22, st2.wHour);
			Assert::AreEqual((WORD)42, st2.wMinute);
			Assert::AreEqual((WORD)38, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test0003)
		{
			static const WCHAR szExpression[] = { L"* * * * ? *" };
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 0, 0 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test0003:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test0003: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)23, st2.wDay);
			Assert::AreEqual((WORD)22, st2.wHour);
			Assert::AreEqual((WORD)42, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test0004)
		{
			static const WCHAR szExpression[] = { L"* * * * ? *" };
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 0 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test0004:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test0004: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)23, st2.wDay);
			Assert::AreEqual((WORD)22, st2.wHour);
			Assert::AreEqual((WORD)43, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test0005)
		{
			static const WCHAR szExpression[] = { L"* * * * ? *" };
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 0, 1 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test0005:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test0005: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)23, st2.wDay);
			Assert::AreEqual((WORD)22, st2.wHour);
			Assert::AreEqual((WORD)43, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test0006)
		{
			static const WCHAR szExpression[] = { L"* * * * ? *" };
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 1, 1 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test0006:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test0006: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)23, st2.wDay);
			Assert::AreEqual((WORD)22, st2.wHour);
			Assert::AreEqual((WORD)43, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test0101)
		{
			static const WCHAR szExpression[] = { L"* * * * * ? 2023" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test0101:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test0101: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)23, st2.wDay);
			Assert::AreEqual((WORD)22, st2.wHour);
			Assert::AreEqual((WORD)42, st2.wMinute);
			Assert::AreEqual((WORD)38, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test0102)
		{
			static const WCHAR szExpression[] = { L"* * * * * ? 2024" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test0102:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test0102: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2024, st2.wYear);
			Assert::AreEqual((WORD)1, st2.wMonth);
			Assert::AreEqual((WORD)1, st2.wDay);
			Assert::AreEqual((WORD)0, st2.wHour);
			Assert::AreEqual((WORD)0, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test0103)
		{
			static const WCHAR szExpression[] = { L"* * * * * ? 2022" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test0103:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test0103: out=%s", SystemTimeToString(st2));
			Assert::IsFalse(bResult);
		}

		TEST_METHOD(Test0201)
		{
			static const WCHAR szExpression[] = { L"* * * * * ? 2021,2023" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test0201:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test0201: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)23, st2.wDay);
			Assert::AreEqual((WORD)22, st2.wHour);
			Assert::AreEqual((WORD)42, st2.wMinute);
			Assert::AreEqual((WORD)38, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test0202)
		{
			static const WCHAR szExpression[] = { L"* * * * * ? 2024,2028" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test0202:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test0202: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2024, st2.wYear);
			Assert::AreEqual((WORD)1, st2.wMonth);
			Assert::AreEqual((WORD)1, st2.wDay);
			Assert::AreEqual((WORD)0, st2.wHour);
			Assert::AreEqual((WORD)0, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test0203)
		{
			static const WCHAR szExpression[] = { L"* * * * * ? 2021,2022" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test0203:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test0203: out=%s", SystemTimeToString(st2));
			Assert::IsFalse(bResult);
		}

		TEST_METHOD(Test0301)
		{
			static const WCHAR szExpression[] = { L"* * * * * ? 2021-2025/2" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test0301:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test0301: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)23, st2.wDay);
			Assert::AreEqual((WORD)22, st2.wHour);
			Assert::AreEqual((WORD)42, st2.wMinute);
			Assert::AreEqual((WORD)38, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test0302)
		{
			static const WCHAR szExpression[] = { L"* * * * * ? 2020-2028/4" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test0302:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test0302: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2024, st2.wYear);
			Assert::AreEqual((WORD)1, st2.wMonth);
			Assert::AreEqual((WORD)1, st2.wDay);
			Assert::AreEqual((WORD)0, st2.wHour);
			Assert::AreEqual((WORD)0, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test0303)
		{
			static const WCHAR szExpression[] = { L"* * * * * ? 2011-2017/3" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test0303:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test0303: out=%s", SystemTimeToString(st2));
			Assert::IsFalse(bResult);
		}

		TEST_METHOD(Test1001)
		{
			static const WCHAR szExpression[] = { L"* * * * 11 ? 2023" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 11, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test1001:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test1001: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)11, st2.wMonth);
			Assert::AreEqual((WORD)23, st2.wDay);
			Assert::AreEqual((WORD)22, st2.wHour);
			Assert::AreEqual((WORD)42, st2.wMinute);
			Assert::AreEqual((WORD)38, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test1002)
		{
			static const WCHAR szExpression[] = { L"* * * * 10 ? 2023,2024" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 11, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test1002:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test1002: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2024, st2.wYear);
			Assert::AreEqual((WORD)10, st2.wMonth);
			Assert::AreEqual((WORD)1, st2.wDay);
			Assert::AreEqual((WORD)0, st2.wHour);
			Assert::AreEqual((WORD)0, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test2001)
		{
			static const WCHAR szExpression[] = { L"* * * 1/2 11 ? 2023" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 11, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test2001:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test2001: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)11, st2.wMonth);
			Assert::AreEqual((WORD)23, st2.wDay);
			Assert::AreEqual((WORD)22, st2.wHour);
			Assert::AreEqual((WORD)42, st2.wMinute);
			Assert::AreEqual((WORD)38, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test2002)
		{
			static const WCHAR szExpression[] = { L"* * * 2/2 11 ? 2023" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 11, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test2002:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test2002: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)11, st2.wMonth);
			Assert::AreEqual((WORD)24, st2.wDay);
			Assert::AreEqual((WORD)0, st2.wHour);
			Assert::AreEqual((WORD)0, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test2003)
		{
			static const WCHAR szExpression[] = { L"* * * L 12 ? 2023" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test2003:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test2003: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)31, st2.wDay);
			Assert::AreEqual((WORD)0, st2.wHour);
			Assert::AreEqual((WORD)0, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test2004)
		{
			static const WCHAR szExpression[] = { L"* * * W 12 ? 2023" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test2004:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test2004: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)25, st2.wDay);
			Assert::AreEqual((WORD)0, st2.wHour);
			Assert::AreEqual((WORD)0, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test2005)
		{
			static const WCHAR szExpression[] = { L"* * * 26W 12 ? *" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test2005:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test2005: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)26, st2.wDay);
			Assert::AreEqual((WORD)0, st2.wHour);
			Assert::AreEqual((WORD)0, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test2006)
		{
			static const WCHAR szExpression[] = { L"* * * 7W 12 ? *" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test2006:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test2006: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2024, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)9, st2.wDay);
			Assert::AreEqual((WORD)0, st2.wHour);
			Assert::AreEqual((WORD)0, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test3001)
		{
			static const WCHAR szExpression[] = { L"* * * ? 12 SAT 2023" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test3001:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test3001: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)23, st2.wDay);
			Assert::AreEqual((WORD)22, st2.wHour);
			Assert::AreEqual((WORD)42, st2.wMinute);
			Assert::AreEqual((WORD)38, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test3002)
		{
			static const WCHAR szExpression[] = { L"* * * ? 12 SUN 2023" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test3002:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test3002: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)24, st2.wDay);
			Assert::AreEqual((WORD)0, st2.wHour);
			Assert::AreEqual((WORD)0, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test3003)
		{
			static const WCHAR szExpression[] = { L"* * * ? 12 MON 2023" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test3003:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test3003: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)25, st2.wDay);
			Assert::AreEqual((WORD)0, st2.wHour);
			Assert::AreEqual((WORD)0, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test3004)
		{
			static const WCHAR szExpression[] = { L"* * * ? 12 MON-SAT 2023" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test3004:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test3004: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)23, st2.wDay);
			Assert::AreEqual((WORD)22, st2.wHour);
			Assert::AreEqual((WORD)42, st2.wMinute);
			Assert::AreEqual((WORD)38, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}
	};
}
