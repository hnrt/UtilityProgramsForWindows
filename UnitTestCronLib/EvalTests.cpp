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
			Assert::AreEqual((WORD)38, st2.wSecond);
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
			Assert::AreEqual((WORD)43, st2.wMinute);
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
			static const WCHAR szExpression[] = { L"* * * ? 12 MON,TUE,WED,THU,FRI,SAT 2023" };
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

		TEST_METHOD(Test3005)
		{
			static const WCHAR szExpression[] = { L"* * * ? 12 MON-SAT 2023" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test3005:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test3005: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)23, st2.wDay);
			Assert::AreEqual((WORD)22, st2.wHour);
			Assert::AreEqual((WORD)42, st2.wMinute);
			Assert::AreEqual((WORD)38, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test3006)
		{
			static const WCHAR szExpression[] = { L"* * * ? 12 7#4 2023" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test3006:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test3006: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)23, st2.wDay);
			Assert::AreEqual((WORD)22, st2.wHour);
			Assert::AreEqual((WORD)42, st2.wMinute);
			Assert::AreEqual((WORD)38, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test3007)
		{
			static const WCHAR szExpression[] = { L"* * * ? * 7#5" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test3007:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test3007: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)30, st2.wDay);
			Assert::AreEqual((WORD)0, st2.wHour);
			Assert::AreEqual((WORD)0, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
			st2.wHour = 23;
			st2.wMinute = 59;
			st2.wSecond = 59;
			SYSTEMTIME st3 = { 0 };
			bResult = cron.GetNextTime(st2, st3);
			Debug::Put(L"Test3007:  in=%s [%s]=%s", SystemTimeToString(st2), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test3007: out=%s", SystemTimeToString(st3));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2024, st3.wYear);
			Assert::AreEqual((WORD)3, st3.wMonth);
			Assert::AreEqual((WORD)30, st3.wDay);
			Assert::AreEqual((WORD)0, st3.wHour);
			Assert::AreEqual((WORD)0, st3.wMinute);
			Assert::AreEqual((WORD)0, st3.wSecond);
			Assert::AreEqual((WORD)0, st3.wMilliseconds);
		}

		TEST_METHOD(Test3008)
		{
			static const WCHAR szExpression[] = { L"* * * ? * 1#1" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test3008:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test3008: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2024, st2.wYear);
			Assert::AreEqual((WORD)1, st2.wMonth);
			Assert::AreEqual((WORD)7, st2.wDay);
			Assert::AreEqual((WORD)0, st2.wHour);
			Assert::AreEqual((WORD)0, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test3009)
		{
			static const WCHAR szExpression[] = { L"* * * ? * 2#2" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test3009:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test3009: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2024, st2.wYear);
			Assert::AreEqual((WORD)1, st2.wMonth);
			Assert::AreEqual((WORD)8, st2.wDay);
			Assert::AreEqual((WORD)0, st2.wHour);
			Assert::AreEqual((WORD)0, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test3010)
		{
			static const WCHAR szExpression[] = { L"* * * ? * 3#3" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test3010:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test3010: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2024, st2.wYear);
			Assert::AreEqual((WORD)1, st2.wMonth);
			Assert::AreEqual((WORD)16, st2.wDay);
			Assert::AreEqual((WORD)0, st2.wHour);
			Assert::AreEqual((WORD)0, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test3011)
		{
			static const WCHAR szExpression[] = { L"* * * ? * 4#4" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test3011:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test3011: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)27, st2.wDay);
			Assert::AreEqual((WORD)0, st2.wHour);
			Assert::AreEqual((WORD)0, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test3012)
		{
			static const WCHAR szExpression[] = { L"* * * ? * 5#5" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test3012:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test3012: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2024, st2.wYear);
			Assert::AreEqual((WORD)2, st2.wMonth);
			Assert::AreEqual((WORD)29, st2.wDay);
			Assert::AreEqual((WORD)0, st2.wHour);
			Assert::AreEqual((WORD)0, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test3021)
		{
			static const WCHAR szExpression[] = { L"* * * ? * 1L" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 27, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test3021:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test3021: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)31, st2.wDay);
			Assert::AreEqual((WORD)0, st2.wHour);
			Assert::AreEqual((WORD)0, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test3022)
		{
			static const WCHAR szExpression[] = { L"* * * ? * 2L" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 27, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test3022:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test3022: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2024, st2.wYear);
			Assert::AreEqual((WORD)1, st2.wMonth);
			Assert::AreEqual((WORD)29, st2.wDay);
			Assert::AreEqual((WORD)0, st2.wHour);
			Assert::AreEqual((WORD)0, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test3023)
		{
			static const WCHAR szExpression[] = { L"* * * ? * 3L" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 27, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test3023:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test3023: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2024, st2.wYear);
			Assert::AreEqual((WORD)1, st2.wMonth);
			Assert::AreEqual((WORD)30, st2.wDay);
			Assert::AreEqual((WORD)0, st2.wHour);
			Assert::AreEqual((WORD)0, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test3024)
		{
			static const WCHAR szExpression[] = { L"* * * ? * 4L" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 27, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test3024:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test3024: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)27, st2.wDay);
			Assert::AreEqual((WORD)22, st2.wHour);
			Assert::AreEqual((WORD)42, st2.wMinute);
			Assert::AreEqual((WORD)38, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test3025)
		{
			static const WCHAR szExpression[] = { L"* * * ? * 5L" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 27, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test3025:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test3025: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)28, st2.wDay);
			Assert::AreEqual((WORD)0, st2.wHour);
			Assert::AreEqual((WORD)0, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test3026)
		{
			static const WCHAR szExpression[] = { L"* * * ? * 6L" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 27, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test3026:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test3026: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)29, st2.wDay);
			Assert::AreEqual((WORD)0, st2.wHour);
			Assert::AreEqual((WORD)0, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test3027)
		{
			static const WCHAR szExpression[] = { L"* * * ? * 7L" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 27, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test3027:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test3027: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)30, st2.wDay);
			Assert::AreEqual((WORD)0, st2.wHour);
			Assert::AreEqual((WORD)0, st2.wMinute);
			Assert::AreEqual((WORD)0, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test4001)
		{
			static const WCHAR szExpression[] = { L"* * 20/2 * * ?" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test4001:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test4001: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)23, st2.wDay);
			Assert::AreEqual((WORD)22, st2.wHour);
			Assert::AreEqual((WORD)42, st2.wMinute);
			Assert::AreEqual((WORD)38, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test4002)
		{
			static const WCHAR szExpression[] = { L"* * 20,21,22,23 * * ?" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test4002:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test4002: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)23, st2.wDay);
			Assert::AreEqual((WORD)22, st2.wHour);
			Assert::AreEqual((WORD)42, st2.wMinute);
			Assert::AreEqual((WORD)38, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test4003)
		{
			static const WCHAR szExpression[] = { L"* * 20-23 * * ?" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test4003:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test4003: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)23, st2.wDay);
			Assert::AreEqual((WORD)22, st2.wHour);
			Assert::AreEqual((WORD)42, st2.wMinute);
			Assert::AreEqual((WORD)38, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test5001)
		{
			static const WCHAR szExpression[] = { L"* 41,42,43 * * * ?" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test5001:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test5001: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)23, st2.wDay);
			Assert::AreEqual((WORD)22, st2.wHour);
			Assert::AreEqual((WORD)42, st2.wMinute);
			Assert::AreEqual((WORD)38, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
		}

		TEST_METHOD(Test6001)
		{
			static const WCHAR szExpression[] = { L"39,41 * * * * ?" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test6001:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test6001: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)23, st2.wDay);
			Assert::AreEqual((WORD)22, st2.wHour);
			Assert::AreEqual((WORD)42, st2.wMinute);
			Assert::AreEqual((WORD)39, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
			SYSTEMTIME st3 = { 0 };
			bResult = cron.GetNextTime(st2, st3);
			Debug::Put(L"Test6001:  in=%s [%s]=%s", SystemTimeToString(st2), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test6001: out=%s", SystemTimeToString(st3));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st3.wYear);
			Assert::AreEqual((WORD)12, st3.wMonth);
			Assert::AreEqual((WORD)23, st3.wDay);
			Assert::AreEqual((WORD)22, st3.wHour);
			Assert::AreEqual((WORD)42, st3.wMinute);
			Assert::AreEqual((WORD)41, st3.wSecond);
			Assert::AreEqual((WORD)0, st3.wMilliseconds);
			SYSTEMTIME st4 = { 0 };
			bResult = cron.GetNextTime(st3, st4);
			Debug::Put(L"Test6001:  in=%s [%s]=%s", SystemTimeToString(st3), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test6001: out=%s", SystemTimeToString(st4));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st4.wYear);
			Assert::AreEqual((WORD)12, st4.wMonth);
			Assert::AreEqual((WORD)23, st4.wDay);
			Assert::AreEqual((WORD)22, st4.wHour);
			Assert::AreEqual((WORD)43, st4.wMinute);
			Assert::AreEqual((WORD)39, st4.wSecond);
			Assert::AreEqual((WORD)0, st4.wMilliseconds);
		}

		TEST_METHOD(Test7001)
		{
			static const WCHAR szExpression[] = { L"39,41 42 22 ? * SAT" };
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(szExpression);
			SYSTEMTIME st1 = { 2023, 12, 0, 23, 22, 42, 37, 111 };
			FileTime(st1).ToSystemTime(st1);
			SYSTEMTIME st2 = { 0 };
			bool bResult = cron.GetNextTime(st1, st2);
			Debug::Put(L"Test7001:  in=%s [%s]=%s", SystemTimeToString(st1), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test7001: out=%s", SystemTimeToString(st2));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st2.wYear);
			Assert::AreEqual((WORD)12, st2.wMonth);
			Assert::AreEqual((WORD)23, st2.wDay);
			Assert::AreEqual((WORD)22, st2.wHour);
			Assert::AreEqual((WORD)42, st2.wMinute);
			Assert::AreEqual((WORD)39, st2.wSecond);
			Assert::AreEqual((WORD)0, st2.wMilliseconds);
			SYSTEMTIME st3 = { 0 };
			bResult = cron.GetNextTime(st2, st3);
			Debug::Put(L"Test7001:  in=%s [%s]=%s", SystemTimeToString(st2), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test7001: out=%s", SystemTimeToString(st3));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st3.wYear);
			Assert::AreEqual((WORD)12, st3.wMonth);
			Assert::AreEqual((WORD)23, st3.wDay);
			Assert::AreEqual((WORD)22, st3.wHour);
			Assert::AreEqual((WORD)42, st3.wMinute);
			Assert::AreEqual((WORD)41, st3.wSecond);
			Assert::AreEqual((WORD)0, st3.wMilliseconds);
			SYSTEMTIME st4 = { 0 };
			bResult = cron.GetNextTime(st3, st4);
			Debug::Put(L"Test7001:  in=%s [%s]=%s", SystemTimeToString(st3), szExpression, bResult ? L"true" : L"false");
			Debug::Put(L"Test7001: out=%s", SystemTimeToString(st4));
			Assert::IsTrue(bResult);
			Assert::AreEqual((WORD)2023, st4.wYear);
			Assert::AreEqual((WORD)12, st4.wMonth);
			Assert::AreEqual((WORD)30, st4.wDay);
			Assert::AreEqual((WORD)22, st4.wHour);
			Assert::AreEqual((WORD)42, st4.wMinute);
			Assert::AreEqual((WORD)39, st4.wSecond);
			Assert::AreEqual((WORD)0, st4.wMilliseconds);
		}
	};
}
