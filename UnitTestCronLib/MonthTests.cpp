#include "pch.h"
#include "CppUnitTest.h"
#include "hnrt/Cron.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace hnrt;

namespace UnitTestCronLib
{
	TEST_CLASS(MonthTests)
	{
	public:

		TEST_METHOD(Test0001)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * 1 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(1, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0002)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * 12 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(12, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0003)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * 5/5 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(5, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(5, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0101)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * 1-9 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_RANGE == cron.Month.type);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.Month).from);
			Assert::AreEqual(9, dynamic_cast<const CronValueRange&>(cron.Month).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0102)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * 1-11 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_RANGE == cron.Month.type);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.Month).from);
			Assert::AreEqual(11, dynamic_cast<const CronValueRange&>(cron.Month).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0103)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * 10-12 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_RANGE == cron.Month.type);
			Assert::AreEqual(10, dynamic_cast<const CronValueRange&>(cron.Month).from);
			Assert::AreEqual(12, dynamic_cast<const CronValueRange&>(cron.Month).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0104)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * 1-9/1 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_RANGE == cron.Month.type);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.Month).from);
			Assert::AreEqual(9, dynamic_cast<const CronValueRange&>(cron.Month).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0105)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * 10-12/2 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_RANGE == cron.Month.type);
			Assert::AreEqual(10, dynamic_cast<const CronValueRange&>(cron.Month).from);
			Assert::AreEqual(12, dynamic_cast<const CronValueRange&>(cron.Month).to);
			Assert::AreEqual(2, dynamic_cast<const CronValueRange&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0106)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * 1-12/10 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_RANGE == cron.Month.type);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.Month).from);
			Assert::AreEqual(12, dynamic_cast<const CronValueRange&>(cron.Month).to);
			Assert::AreEqual(10, dynamic_cast<const CronValueRange&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0201)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * 1 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(1, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0202)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * 12 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(12, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0203)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * 10/2 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(2, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0301)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * 5-9 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_RANGE == cron.Month.type);
			Assert::AreEqual(5, dynamic_cast<const CronValueRange&>(cron.Month).from);
			Assert::AreEqual(9, dynamic_cast<const CronValueRange&>(cron.Month).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0302)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * 3-11 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_RANGE == cron.Month.type);
			Assert::AreEqual(3, dynamic_cast<const CronValueRange&>(cron.Month).from);
			Assert::AreEqual(11, dynamic_cast<const CronValueRange&>(cron.Month).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0303)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * 10-12 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_RANGE == cron.Month.type);
			Assert::AreEqual(10, dynamic_cast<const CronValueRange&>(cron.Month).from);
			Assert::AreEqual(12, dynamic_cast<const CronValueRange&>(cron.Month).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0304)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * 4-8/1 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_RANGE == cron.Month.type);
			Assert::AreEqual(4, dynamic_cast<const CronValueRange&>(cron.Month).from);
			Assert::AreEqual(8, dynamic_cast<const CronValueRange&>(cron.Month).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0305)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * 10-12/2 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_RANGE == cron.Month.type);
			Assert::AreEqual(10, dynamic_cast<const CronValueRange&>(cron.Month).from);
			Assert::AreEqual(12, dynamic_cast<const CronValueRange&>(cron.Month).to);
			Assert::AreEqual(2, dynamic_cast<const CronValueRange&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0306)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * 1-12/10 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_RANGE == cron.Month.type);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.Month).from);
			Assert::AreEqual(12, dynamic_cast<const CronValueRange&>(cron.Month).to);
			Assert::AreEqual(10, dynamic_cast<const CronValueRange&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0401)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * JAN ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10001, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0402)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * FEB ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10002, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0403)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * MAR ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10003, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0404)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * APR ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10004, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0405)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * MAY ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10005, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0406)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * JUN ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10006, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0407)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * JUL ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10007, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0408)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * AUG ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10008, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0409)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * SEP ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10009, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0410)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * OCT ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10010, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0411)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * NOV ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10011, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0412)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * DEC ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10012, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0413)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * JUL/3 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10007, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(3, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0501)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * APR-SEP ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_RANGE == cron.Month.type);
			Assert::AreEqual(10004, dynamic_cast<const CronValueRange&>(cron.Month).from);
			Assert::AreEqual(10009, dynamic_cast<const CronValueRange&>(cron.Month).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0502)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * FEB-10 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_RANGE == cron.Month.type);
			Assert::AreEqual(10002, dynamic_cast<const CronValueRange&>(cron.Month).from);
			Assert::AreEqual(10, dynamic_cast<const CronValueRange&>(cron.Month).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0503)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * 2-OCT ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_RANGE == cron.Month.type);
			Assert::AreEqual(2, dynamic_cast<const CronValueRange&>(cron.Month).from);
			Assert::AreEqual(10010, dynamic_cast<const CronValueRange&>(cron.Month).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0504)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * APR-SEP/2 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_RANGE == cron.Month.type);
			Assert::AreEqual(10004, dynamic_cast<const CronValueRange&>(cron.Month).from);
			Assert::AreEqual(10009, dynamic_cast<const CronValueRange&>(cron.Month).to);
			Assert::AreEqual(2, dynamic_cast<const CronValueRange&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0505)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * FEB-10/3 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_RANGE == cron.Month.type);
			Assert::AreEqual(10002, dynamic_cast<const CronValueRange&>(cron.Month).from);
			Assert::AreEqual(10, dynamic_cast<const CronValueRange&>(cron.Month).to);
			Assert::AreEqual(3, dynamic_cast<const CronValueRange&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0506)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * 2-OCT/4 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_RANGE == cron.Month.type);
			Assert::AreEqual(2, dynamic_cast<const CronValueRange&>(cron.Month).from);
			Assert::AreEqual(10010, dynamic_cast<const CronValueRange&>(cron.Month).to);
			Assert::AreEqual(4, dynamic_cast<const CronValueRange&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0601)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * JAN ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10001, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0602)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * FEB ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10002, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0603)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * MAR ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10003, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0604)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * APR ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10004, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0605)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * MAY ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10005, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0606)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * JUN ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10006, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0607)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * JUL ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10007, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0608)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * AUG ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10008, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0609)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * SEP ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10009, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0610)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * OCT ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10010, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0611)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * NOV ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10011, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0612)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * DEC ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10012, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0613)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * JUL/3 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10007, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(3, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0701)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * APR-SEP ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_RANGE == cron.Month.type);
			Assert::AreEqual(10004, dynamic_cast<const CronValueRange&>(cron.Month).from);
			Assert::AreEqual(10009, dynamic_cast<const CronValueRange&>(cron.Month).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0702)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * FEB-10 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_RANGE == cron.Month.type);
			Assert::AreEqual(10002, dynamic_cast<const CronValueRange&>(cron.Month).from);
			Assert::AreEqual(10, dynamic_cast<const CronValueRange&>(cron.Month).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0703)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * 2-OCT ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_RANGE == cron.Month.type);
			Assert::AreEqual(2, dynamic_cast<const CronValueRange&>(cron.Month).from);
			Assert::AreEqual(10010, dynamic_cast<const CronValueRange&>(cron.Month).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0704)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * APR-SEP/2 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_RANGE == cron.Month.type);
			Assert::AreEqual(10004, dynamic_cast<const CronValueRange&>(cron.Month).from);
			Assert::AreEqual(10009, dynamic_cast<const CronValueRange&>(cron.Month).to);
			Assert::AreEqual(2, dynamic_cast<const CronValueRange&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0705)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * FEB-10/3 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_RANGE == cron.Month.type);
			Assert::AreEqual(10002, dynamic_cast<const CronValueRange&>(cron.Month).from);
			Assert::AreEqual(10, dynamic_cast<const CronValueRange&>(cron.Month).to);
			Assert::AreEqual(3, dynamic_cast<const CronValueRange&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0706)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * 2-OCT/4 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_RANGE == cron.Month.type);
			Assert::AreEqual(2, dynamic_cast<const CronValueRange&>(cron.Month).from);
			Assert::AreEqual(10010, dynamic_cast<const CronValueRange&>(cron.Month).to);
			Assert::AreEqual(4, dynamic_cast<const CronValueRange&>(cron.Month).step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0801)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * 7,8 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(7, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			const CronValue* pValue2 = cron.Month.next.Ptr;
			Assert::AreEqual(8, dynamic_cast<const CronValueSingle*>(pValue2)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue2)->step);
			const CronValue* pValue3 = pValue2->next.Ptr;
			Assert::IsNull(pValue3);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0802)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * 7,8,10,12 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(7, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			const CronValue* pValue2 = cron.Month.next.Ptr;
			Assert::AreEqual(8, dynamic_cast<const CronValueSingle*>(pValue2)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue2)->step);
			const CronValue* pValue3 = pValue2->next.Ptr;
			Assert::AreEqual(10, dynamic_cast<const CronValueSingle*>(pValue3)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue3)->step);
			const CronValue* pValue4 = pValue3->next.Ptr;
			Assert::AreEqual(12, dynamic_cast<const CronValueSingle*>(pValue4)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue4)->step);
			const CronValue* pValue5 = pValue4->next.Ptr;
			Assert::IsNull(pValue5);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0901)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * 7,8 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(7, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			const CronValue* pValue2 = cron.Month.next.Ptr;
			Assert::AreEqual(8, dynamic_cast<const CronValueSingle*>(pValue2)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue2)->step);
			const CronValue* pValue3 = pValue2->next.Ptr;
			Assert::IsNull(pValue3);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0902)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * 7,8,10,12 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(7, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			const CronValue* pValue2 = cron.Month.next.Ptr;
			Assert::AreEqual(8, dynamic_cast<const CronValueSingle*>(pValue2)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue2)->step);
			const CronValue* pValue3 = pValue2->next.Ptr;
			Assert::AreEqual(10, dynamic_cast<const CronValueSingle*>(pValue3)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue3)->step);
			const CronValue* pValue4 = pValue3->next.Ptr;
			Assert::AreEqual(12, dynamic_cast<const CronValueSingle*>(pValue4)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue4)->step);
			const CronValue* pValue5 = pValue4->next.Ptr;
			Assert::IsNull(pValue5);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test1001)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * JUL,AUG ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10007, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			const CronValue* pValue2 = cron.Month.next.Ptr;
			Assert::AreEqual(10008, dynamic_cast<const CronValueSingle*>(pValue2)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue2)->step);
			const CronValue* pValue3 = pValue2->next.Ptr;
			Assert::IsNull(pValue3);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test1002)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * JUL,AUG,OCT,DEC ?");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10007, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			const CronValue* pValue2 = cron.Month.next.Ptr;
			Assert::AreEqual(10008, dynamic_cast<const CronValueSingle*>(pValue2)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue2)->step);
			const CronValue* pValue3 = pValue2->next.Ptr;
			Assert::AreEqual(10010, dynamic_cast<const CronValueSingle*>(pValue3)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue3)->step);
			const CronValue* pValue4 = pValue3->next.Ptr;
			Assert::AreEqual(10012, dynamic_cast<const CronValueSingle*>(pValue4)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue4)->step);
			const CronValue* pValue5 = pValue4->next.Ptr;
			Assert::IsNull(pValue5);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test1101)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * JUL,AUG ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10007, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			const CronValue* pValue2 = cron.Month.next.Ptr;
			Assert::AreEqual(10008, dynamic_cast<const CronValueSingle*>(pValue2)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue2)->step);
			const CronValue* pValue3 = pValue2->next.Ptr;
			Assert::IsNull(pValue3);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test1102)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * JUL,AUG,OCT,DEC ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.type);
			Assert::AreEqual(10007, dynamic_cast<const CronValueSingle&>(cron.Month).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Month).step);
			const CronValue* pValue2 = cron.Month.next.Ptr;
			Assert::AreEqual(10008, dynamic_cast<const CronValueSingle*>(pValue2)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue2)->step);
			const CronValue* pValue3 = pValue2->next.Ptr;
			Assert::AreEqual(10010, dynamic_cast<const CronValueSingle*>(pValue3)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue3)->step);
			const CronValue* pValue4 = pValue3->next.Ptr;
			Assert::AreEqual(10012, dynamic_cast<const CronValueSingle*>(pValue4)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue4)->step);
			const CronValue* pValue5 = pValue4->next.Ptr;
			Assert::IsNull(pValue5);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}
	};
}
