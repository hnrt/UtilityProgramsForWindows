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
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfWeek.type);
			Assert::AreEqual(1, dynamic_cast<const CronValueSingle&>(cron.DayOfWeek).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.DayOfWeek).step);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0002)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * 2");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfWeek.type);
			Assert::AreEqual(2, dynamic_cast<const CronValueSingle&>(cron.DayOfWeek).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.DayOfWeek).step);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0003)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * 7");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfWeek.type);
			Assert::AreEqual(7, dynamic_cast<const CronValueSingle&>(cron.DayOfWeek).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.DayOfWeek).step);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0101)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * 1-3");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfWeek.type);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.DayOfWeek).from);
			Assert::AreEqual(3, dynamic_cast<const CronValueRange&>(cron.DayOfWeek).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.DayOfWeek).step);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0201)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * 2,4,6");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfWeek.type);
			Assert::AreEqual(2, dynamic_cast<const CronValueSingle&>(cron.DayOfWeek).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.DayOfWeek).step);
			const CronValue* pValue2 = cron.DayOfWeek.next.Ptr;
			Assert::AreEqual(4, dynamic_cast<const CronValueSingle*>(pValue2)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue2)->step);
			const CronValue* pValue3 = pValue2->next.Ptr;
			Assert::AreEqual(6, dynamic_cast<const CronValueSingle*>(pValue3)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue3)->step);
			const CronValue* pValue4 = pValue3->next.Ptr;
			Assert::IsNull(pValue4);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0301)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * 6L");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_LAST_DAYOFWEEK == cron.DayOfWeek.type);
			Assert::AreEqual(6, dynamic_cast<const CronValueLastDayOfWeek&>(cron.DayOfWeek).dow);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0401)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * 4#2");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_NTH_DAYOFWEEK == cron.DayOfWeek.type);
			Assert::AreEqual(4, dynamic_cast<const CronValueNthDayOfWeek&>(cron.DayOfWeek).dow);
			Assert::AreEqual(2, dynamic_cast<const CronValueNthDayOfWeek&>(cron.DayOfWeek).nth);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0501)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * SUN");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfWeek.type);
			Assert::AreEqual(10001, dynamic_cast<const CronValueSingle&>(cron.DayOfWeek).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.DayOfWeek).step);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0502)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * MON");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfWeek.type);
			Assert::AreEqual(10002, dynamic_cast<const CronValueSingle&>(cron.DayOfWeek).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.DayOfWeek).step);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0503)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * TUE");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfWeek.type);
			Assert::AreEqual(10003, dynamic_cast<const CronValueSingle&>(cron.DayOfWeek).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.DayOfWeek).step);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0504)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * WED");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfWeek.type);
			Assert::AreEqual(10004, dynamic_cast<const CronValueSingle&>(cron.DayOfWeek).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.DayOfWeek).step);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0505)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * THU");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfWeek.type);
			Assert::AreEqual(10005, dynamic_cast<const CronValueSingle&>(cron.DayOfWeek).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.DayOfWeek).step);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0506)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * FRI");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfWeek.type);
			Assert::AreEqual(10006, dynamic_cast<const CronValueSingle&>(cron.DayOfWeek).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.DayOfWeek).step);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0507)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * SAT");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfWeek.type);
			Assert::AreEqual(10007, dynamic_cast<const CronValueSingle&>(cron.DayOfWeek).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.DayOfWeek).step);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0601)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * SUN-SAT");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfWeek.type);
			Assert::AreEqual(10001, dynamic_cast<const CronValueRange&>(cron.DayOfWeek).from);
			Assert::AreEqual(10007, dynamic_cast<const CronValueRange&>(cron.DayOfWeek).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.DayOfWeek).step);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0701)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * SUN,WED,SAT");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfWeek.type);
			Assert::AreEqual(10001, dynamic_cast<const CronValueSingle&>(cron.DayOfWeek).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.DayOfWeek).step);
			const CronValue* pValue2 = cron.DayOfWeek.next.Ptr;
			Assert::AreEqual(10004, dynamic_cast<const CronValueSingle*>(pValue2)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue2)->step);
			const CronValue* pValue3 = pValue2->next.Ptr;
			Assert::AreEqual(10007, dynamic_cast<const CronValueSingle*>(pValue3)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue3)->step);
			const CronValue* pValue4 = pValue3->next.Ptr;
			Assert::IsNull(pValue4);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}
	};
}
