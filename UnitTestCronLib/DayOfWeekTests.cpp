#include "pch.h"
#include "CppUnitTest.h"
#include "hnrt/Cron.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace hnrt;

namespace UnitTestCronLib
{
	TEST_CLASS(DayOfWeekTests)
	{
	public:

		TEST_METHOD(Test0001)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * 1");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfWeek.Type);
			Assert::AreEqual(1, cron.DayOfWeek.Value);
			Assert::AreEqual(0, cron.DayOfWeek.Step);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0002)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * 2");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfWeek.Type);
			Assert::AreEqual(2, cron.DayOfWeek.Value);
			Assert::AreEqual(0, cron.DayOfWeek.Step);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0003)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * 7");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfWeek.Type);
			Assert::AreEqual(7, cron.DayOfWeek.Value);
			Assert::AreEqual(0, cron.DayOfWeek.Step);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0101)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * 1-3");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfWeek.Type);
			Assert::AreEqual(1, cron.DayOfWeek.From);
			Assert::AreEqual(3, cron.DayOfWeek.To);
			Assert::AreEqual(1, cron.DayOfWeek.Step);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0201)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * 2,4,6");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfWeek.Type);
			Assert::AreEqual(2, cron.DayOfWeek.Value);
			Assert::AreEqual(0, cron.DayOfWeek.Step);
			RefPtr<CronValue> pValue2 = cron.DayOfWeek.Next;
			Assert::AreEqual(4, pValue2->Value);
			Assert::AreEqual(0, pValue2->Step);
			RefPtr<CronValue> pValue3 = pValue2->Next;
			Assert::AreEqual(6, pValue3->Value);
			Assert::AreEqual(0, pValue3->Step);
			RefPtr<CronValue> pValue4 = pValue3->Next;
			Assert::IsNull(pValue4.Ptr);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0301)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * 6L");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_LAST_DAYOFWEEK == cron.DayOfWeek.Type);
			Assert::AreEqual(6, cron.DayOfWeek.Value);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0401)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * 4#2");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_NTH_DAYOFWEEK == cron.DayOfWeek.Type);
			Assert::AreEqual(4, cron.DayOfWeek.Value);
			Assert::AreEqual(2, cron.DayOfWeek.Nth);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0501)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * SUN");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfWeek.Type);
			Assert::AreEqual(10001, cron.DayOfWeek.Value);
			Assert::AreEqual(0, cron.DayOfWeek.Step);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0502)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * MON");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfWeek.Type);
			Assert::AreEqual(10002, cron.DayOfWeek.Value);
			Assert::AreEqual(0, cron.DayOfWeek.Step);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0503)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * TUE");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfWeek.Type);
			Assert::AreEqual(10003, cron.DayOfWeek.Value);
			Assert::AreEqual(0, cron.DayOfWeek.Step);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0504)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * WED");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfWeek.Type);
			Assert::AreEqual(10004, cron.DayOfWeek.Value);
			Assert::AreEqual(0, cron.DayOfWeek.Step);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0505)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * THU");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfWeek.Type);
			Assert::AreEqual(10005, cron.DayOfWeek.Value);
			Assert::AreEqual(0, cron.DayOfWeek.Step);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0506)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * FRI");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfWeek.Type);
			Assert::AreEqual(10006, cron.DayOfWeek.Value);
			Assert::AreEqual(0, cron.DayOfWeek.Step);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0507)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * SAT");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfWeek.Type);
			Assert::AreEqual(10007, cron.DayOfWeek.Value);
			Assert::AreEqual(0, cron.DayOfWeek.Step);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0601)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * SUN-SAT");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfWeek.Type);
			Assert::AreEqual(10001, cron.DayOfWeek.From);
			Assert::AreEqual(10007, cron.DayOfWeek.To);
			Assert::AreEqual(1, cron.DayOfWeek.Step);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0701)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * SUN,WED,SAT");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfWeek.Type);
			Assert::AreEqual(10001, cron.DayOfWeek.Value);
			Assert::AreEqual(0, cron.DayOfWeek.Step);
			RefPtr<CronValue> pValue2 = cron.DayOfWeek.Next;
			Assert::AreEqual(10004, pValue2->Value);
			Assert::AreEqual(0, pValue2->Step);
			RefPtr<CronValue> pValue3 = pValue2->Next;
			Assert::AreEqual(10007, pValue3->Value);
			Assert::AreEqual(0, pValue3->Step);
			RefPtr<CronValue> pValue4 = pValue3->Next;
			Assert::IsNull(pValue4.Ptr);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}
	};
}
