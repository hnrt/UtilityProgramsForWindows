#include "pch.h"
#include "CppUnitTest.h"
#include "hnrt/Cron.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace hnrt;

namespace UnitTestCronLib
{
	TEST_CLASS(DayTests)
	{
	public:

		TEST_METHOD(Test0001)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * 1 * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfMonth.type);
			Assert::AreEqual(1, dynamic_cast<const CronValueSingle&>(cron.DayOfMonth).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.DayOfMonth).step);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0002)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * 23 * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfMonth.type);
			Assert::AreEqual(23, dynamic_cast<const CronValueSingle&>(cron.DayOfMonth).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.DayOfMonth).step);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0003)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * 15/5 * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfMonth.type);
			Assert::AreEqual(15, dynamic_cast<const CronValueSingle&>(cron.DayOfMonth).value);
			Assert::AreEqual(5, dynamic_cast<const CronValueSingle&>(cron.DayOfMonth).step);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0101)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * 1-9 * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfMonth.type);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).from);
			Assert::AreEqual(9, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).step);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0102)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * 2-11 * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfMonth.type);
			Assert::AreEqual(2, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).from);
			Assert::AreEqual(11, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).step);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0103)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * 10-20 * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfMonth.type);
			Assert::AreEqual(10, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).from);
			Assert::AreEqual(20, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).step);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0104)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * 3-9/1 * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfMonth.type);
			Assert::AreEqual(3, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).from);
			Assert::AreEqual(9, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).step);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0105)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * 10-19/3 * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfMonth.type);
			Assert::AreEqual(10, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).from);
			Assert::AreEqual(19, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).to);
			Assert::AreEqual(3, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).step);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0106)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * 4-20/10 * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfMonth.type);
			Assert::AreEqual(4, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).from);
			Assert::AreEqual(20, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).to);
			Assert::AreEqual(10, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).step);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0201)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * 1 * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfMonth.type);
			Assert::AreEqual(1, dynamic_cast<const CronValueSingle&>(cron.DayOfMonth).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.DayOfMonth).step);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0202)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * 23 * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfMonth.type);
			Assert::AreEqual(23, dynamic_cast<const CronValueSingle&>(cron.DayOfMonth).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.DayOfMonth).step);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0203)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * 15/5 * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfMonth.type);
			Assert::AreEqual(15, dynamic_cast<const CronValueSingle&>(cron.DayOfMonth).value);
			Assert::AreEqual(5, dynamic_cast<const CronValueSingle&>(cron.DayOfMonth).step);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0301)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * 5-9 * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfMonth.type);
			Assert::AreEqual(5, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).from);
			Assert::AreEqual(9, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).step);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0302)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * 6-11 * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfMonth.type);
			Assert::AreEqual(6, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).from);
			Assert::AreEqual(11, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).step);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0303)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * 17-20 * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfMonth.type);
			Assert::AreEqual(17, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).from);
			Assert::AreEqual(20, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).step);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0304)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * 8-9/1 * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfMonth.type);
			Assert::AreEqual(8, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).from);
			Assert::AreEqual(9, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).step);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0305)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * 10-19/3 * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfMonth.type);
			Assert::AreEqual(10, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).from);
			Assert::AreEqual(19, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).to);
			Assert::AreEqual(3, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).step);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0306)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * 1-23/15 * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfMonth.type);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).from);
			Assert::AreEqual(23, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).to);
			Assert::AreEqual(15, dynamic_cast<const CronValueRange&>(cron.DayOfMonth).step);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0401)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * L * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_LASTDAY == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0402)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * L * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_LASTDAY == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0501)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * W * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_WEEKDAY == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0502)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * W * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_WEEKDAY == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0601)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * 3W * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_CLOSEST_WEEKDAY == cron.DayOfMonth.type);
			Assert::AreEqual(3, dynamic_cast<const CronValueClosestWeekDay&>(cron.DayOfMonth).dom);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0602)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * 27W * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_CLOSEST_WEEKDAY == cron.DayOfMonth.type);
			Assert::AreEqual(27, dynamic_cast<const CronValueClosestWeekDay&>(cron.DayOfMonth).dom);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0701)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * 7,8 * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfMonth.type);
			Assert::AreEqual(7, dynamic_cast<const CronValueSingle&>(cron.DayOfMonth).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.DayOfMonth).step);
			const CronValue* pValue2 = cron.DayOfMonth.next.Ptr;
			Assert::AreEqual(8, dynamic_cast<const CronValueSingle*>(pValue2)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue2)->step);
			const CronValue* pValue3 = pValue2->next.Ptr;
			Assert::IsNull(pValue3);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0702)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * 7,8,10,22 * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfMonth.type);
			Assert::AreEqual(7, dynamic_cast<const CronValueSingle&>(cron.DayOfMonth).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.DayOfMonth).step);
			const CronValue* pValue2 = cron.DayOfMonth.next.Ptr;
			Assert::AreEqual(8, dynamic_cast<const CronValueSingle*>(pValue2)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue2)->step);
			const CronValue* pValue3 = pValue2->next.Ptr;
			Assert::AreEqual(10, dynamic_cast<const CronValueSingle*>(pValue3)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue3)->step);
			const CronValue* pValue4 = pValue3->next.Ptr;
			Assert::AreEqual(22, dynamic_cast<const CronValueSingle*>(pValue4)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue4)->step);
			const CronValue* pValue5 = pValue4->next.Ptr;
			Assert::IsNull(pValue5);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0801)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * 7,8 * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfMonth.type);
			Assert::AreEqual(7, dynamic_cast<const CronValueSingle&>(cron.DayOfMonth).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.DayOfMonth).step);
			const CronValue* pValue2 = cron.DayOfMonth.next.Ptr;
			Assert::AreEqual(8, dynamic_cast<const CronValueSingle*>(pValue2)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue2)->step);
			const CronValue* pValue3 = pValue2->next.Ptr;
			Assert::IsNull(pValue3);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0802)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * 7,8,20,31 * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfMonth.type);
			Assert::AreEqual(7, dynamic_cast<const CronValueSingle&>(cron.DayOfMonth).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.DayOfMonth).step);
			const CronValue* pValue2 = cron.DayOfMonth.next.Ptr;
			Assert::AreEqual(8, dynamic_cast<const CronValueSingle*>(pValue2)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue2)->step);
			const CronValue* pValue3 = pValue2->next.Ptr;
			Assert::AreEqual(20, dynamic_cast<const CronValueSingle*>(pValue3)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue3)->step);
			const CronValue* pValue4 = pValue3->next.Ptr;
			Assert::AreEqual(31, dynamic_cast<const CronValueSingle*>(pValue4)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue4)->step);
			const CronValue* pValue5 = pValue4->next.Ptr;
			Assert::IsNull(pValue5);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}
	};
}
