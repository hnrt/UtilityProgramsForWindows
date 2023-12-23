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
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(1, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0002)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * 12 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(12, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0003)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * 5/5 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(5, cron.Month.Value);
			Assert::AreEqual(5, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0101)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * 1-9 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_RANGE == cron.Month.Type);
			Assert::AreEqual(1, cron.Month.From);
			Assert::AreEqual(9, cron.Month.To);
			Assert::AreEqual(1, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0102)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * 1-11 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_RANGE == cron.Month.Type);
			Assert::AreEqual(1, cron.Month.From);
			Assert::AreEqual(11, cron.Month.To);
			Assert::AreEqual(1, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0103)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * 10-12 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_RANGE == cron.Month.Type);
			Assert::AreEqual(10, cron.Month.From);
			Assert::AreEqual(12, cron.Month.To);
			Assert::AreEqual(1, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0104)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * 1-9/1 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_RANGE == cron.Month.Type);
			Assert::AreEqual(1, cron.Month.From);
			Assert::AreEqual(9, cron.Month.To);
			Assert::AreEqual(1, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0105)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * 10-12/2 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_RANGE == cron.Month.Type);
			Assert::AreEqual(10, cron.Month.From);
			Assert::AreEqual(12, cron.Month.To);
			Assert::AreEqual(2, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0106)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * 1-12/10 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_RANGE == cron.Month.Type);
			Assert::AreEqual(1, cron.Month.From);
			Assert::AreEqual(12, cron.Month.To);
			Assert::AreEqual(10, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0201)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * 1 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(1, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0202)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * 12 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(12, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0203)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * 10/2 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10, cron.Month.Value);
			Assert::AreEqual(2, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0301)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * 5-9 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_RANGE == cron.Month.Type);
			Assert::AreEqual(5, cron.Month.From);
			Assert::AreEqual(9, cron.Month.To);
			Assert::AreEqual(1, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0302)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * 3-11 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_RANGE == cron.Month.Type);
			Assert::AreEqual(3, cron.Month.From);
			Assert::AreEqual(11, cron.Month.To);
			Assert::AreEqual(1, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0303)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * 10-12 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_RANGE == cron.Month.Type);
			Assert::AreEqual(10, cron.Month.From);
			Assert::AreEqual(12, cron.Month.To);
			Assert::AreEqual(1, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0304)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * 4-8/1 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_RANGE == cron.Month.Type);
			Assert::AreEqual(4, cron.Month.From);
			Assert::AreEqual(8, cron.Month.To);
			Assert::AreEqual(1, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0305)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * 10-12/2 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_RANGE == cron.Month.Type);
			Assert::AreEqual(10, cron.Month.From);
			Assert::AreEqual(12, cron.Month.To);
			Assert::AreEqual(2, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0306)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * 1-12/10 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_RANGE == cron.Month.Type);
			Assert::AreEqual(1, cron.Month.From);
			Assert::AreEqual(12, cron.Month.To);
			Assert::AreEqual(10, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0401)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * JAN ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10001, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0402)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * FEB ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10002, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0403)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * MAR ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10003, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0404)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * APR ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10004, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0405)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * MAY ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10005, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0406)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * JUN ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10006, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0407)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * JUL ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10007, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0408)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * AUG ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10008, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0409)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * SEP ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10009, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0410)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * OCT ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10010, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0411)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * NOV ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10011, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0412)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * DEC ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10012, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0413)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * JUL/3 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10007, cron.Month.Value);
			Assert::AreEqual(3, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0501)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * APR-SEP ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_RANGE == cron.Month.Type);
			Assert::AreEqual(10004, cron.Month.From);
			Assert::AreEqual(10009, cron.Month.To);
			Assert::AreEqual(1, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0502)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * FEB-10 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_RANGE == cron.Month.Type);
			Assert::AreEqual(10002, cron.Month.From);
			Assert::AreEqual(10, cron.Month.To);
			Assert::AreEqual(1, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0503)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * 2-OCT ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_RANGE == cron.Month.Type);
			Assert::AreEqual(2, cron.Month.From);
			Assert::AreEqual(10010, cron.Month.To);
			Assert::AreEqual(1, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0504)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * APR-SEP/2 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_RANGE == cron.Month.Type);
			Assert::AreEqual(10004, cron.Month.From);
			Assert::AreEqual(10009, cron.Month.To);
			Assert::AreEqual(2, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0505)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * FEB-10/3 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_RANGE == cron.Month.Type);
			Assert::AreEqual(10002, cron.Month.From);
			Assert::AreEqual(10, cron.Month.To);
			Assert::AreEqual(3, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0506)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * 2-OCT/4 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_RANGE == cron.Month.Type);
			Assert::AreEqual(2, cron.Month.From);
			Assert::AreEqual(10010, cron.Month.To);
			Assert::AreEqual(4, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0601)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * JAN ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10001, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0602)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * FEB ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10002, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0603)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * MAR ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10003, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0604)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * APR ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10004, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0605)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * MAY ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10005, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0606)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * JUN ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10006, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0607)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * JUL ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10007, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0608)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * AUG ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10008, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0609)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * SEP ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10009, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0610)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * OCT ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10010, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0611)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * NOV ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10011, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0612)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * DEC ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10012, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0613)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * JUL/3 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10007, cron.Month.Value);
			Assert::AreEqual(3, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0701)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * APR-SEP ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_RANGE == cron.Month.Type);
			Assert::AreEqual(10004, cron.Month.From);
			Assert::AreEqual(10009, cron.Month.To);
			Assert::AreEqual(1, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0702)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * FEB-10 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_RANGE == cron.Month.Type);
			Assert::AreEqual(10002, cron.Month.From);
			Assert::AreEqual(10, cron.Month.To);
			Assert::AreEqual(1, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0703)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * 2-OCT ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_RANGE == cron.Month.Type);
			Assert::AreEqual(2, cron.Month.From);
			Assert::AreEqual(10010, cron.Month.To);
			Assert::AreEqual(1, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0704)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * APR-SEP/2 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_RANGE == cron.Month.Type);
			Assert::AreEqual(10004, cron.Month.From);
			Assert::AreEqual(10009, cron.Month.To);
			Assert::AreEqual(2, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0705)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * FEB-10/3 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_RANGE == cron.Month.Type);
			Assert::AreEqual(10002, cron.Month.From);
			Assert::AreEqual(10, cron.Month.To);
			Assert::AreEqual(3, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0706)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * 2-OCT/4 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_RANGE == cron.Month.Type);
			Assert::AreEqual(2, cron.Month.From);
			Assert::AreEqual(10010, cron.Month.To);
			Assert::AreEqual(4, cron.Month.Step);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0801)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * 7,8 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(7, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			RefPtr<CronValue> pValue2 = cron.Month.Next;
			Assert::AreEqual(8, pValue2->Value);
			Assert::AreEqual(0, pValue2->Step);
			RefPtr<CronValue> pValue3 = pValue2->Next;
			Assert::IsNull(pValue3.Ptr);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0802)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * 7,8,10,12 ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(7, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			RefPtr<CronValue> pValue2 = cron.Month.Next;
			Assert::AreEqual(8, pValue2->Value);
			Assert::AreEqual(0, pValue2->Step);
			RefPtr<CronValue> pValue3 = pValue2->Next;
			Assert::AreEqual(10, pValue3->Value);
			Assert::AreEqual(0, pValue3->Step);
			RefPtr<CronValue> pValue4 = pValue3->Next;
			Assert::AreEqual(12, pValue4->Value);
			Assert::AreEqual(0, pValue4->Step);
			RefPtr<CronValue> pValue5 = pValue4->Next;
			Assert::IsNull(pValue5.Ptr);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0901)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * 7,8 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(7, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			RefPtr<CronValue> pValue2 = cron.Month.Next;
			Assert::AreEqual(8, pValue2->Value);
			Assert::AreEqual(0, pValue2->Step);
			RefPtr<CronValue> pValue3 = pValue2->Next;
			Assert::IsNull(pValue3.Ptr);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0902)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * 7,8,10,12 ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(7, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			RefPtr<CronValue> pValue2 = cron.Month.Next;
			Assert::AreEqual(8, pValue2->Value);
			Assert::AreEqual(0, pValue2->Step);
			RefPtr<CronValue> pValue3 = pValue2->Next;
			Assert::AreEqual(10, pValue3->Value);
			Assert::AreEqual(0, pValue3->Step);
			RefPtr<CronValue> pValue4 = pValue3->Next;
			Assert::AreEqual(12, pValue4->Value);
			Assert::AreEqual(0, pValue4->Step);
			RefPtr<CronValue> pValue5 = pValue4->Next;
			Assert::IsNull(pValue5.Ptr);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test1001)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * JUL,AUG ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10007, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			RefPtr<CronValue> pValue2 = cron.Month.Next;
			Assert::AreEqual(10008, pValue2->Value);
			Assert::AreEqual(0, pValue2->Step);
			RefPtr<CronValue> pValue3 = pValue2->Next;
			Assert::IsNull(pValue3.Ptr);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test1002)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * JUL,AUG,OCT,DEC ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10007, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			RefPtr<CronValue> pValue2 = cron.Month.Next;
			Assert::AreEqual(10008, pValue2->Value);
			Assert::AreEqual(0, pValue2->Step);
			RefPtr<CronValue> pValue3 = pValue2->Next;
			Assert::AreEqual(10010, pValue3->Value);
			Assert::AreEqual(0, pValue3->Step);
			RefPtr<CronValue> pValue4 = pValue3->Next;
			Assert::AreEqual(10012, pValue4->Value);
			Assert::AreEqual(0, pValue4->Step);
			RefPtr<CronValue> pValue5 = pValue4->Next;
			Assert::IsNull(pValue5.Ptr);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test1101)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * JUL,AUG ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10007, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			RefPtr<CronValue> pValue2 = cron.Month.Next;
			Assert::AreEqual(10008, pValue2->Value);
			Assert::AreEqual(0, pValue2->Step);
			RefPtr<CronValue> pValue3 = pValue2->Next;
			Assert::IsNull(pValue3.Ptr);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test1102)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * JUL,AUG,OCT,DEC ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Month.Type);
			Assert::AreEqual(10007, cron.Month.Value);
			Assert::AreEqual(0, cron.Month.Step);
			RefPtr<CronValue> pValue2 = cron.Month.Next;
			Assert::AreEqual(10008, pValue2->Value);
			Assert::AreEqual(0, pValue2->Step);
			RefPtr<CronValue> pValue3 = pValue2->Next;
			Assert::AreEqual(10010, pValue3->Value);
			Assert::AreEqual(0, pValue3->Step);
			RefPtr<CronValue> pValue4 = pValue3->Next;
			Assert::AreEqual(10012, pValue4->Value);
			Assert::AreEqual(0, pValue4->Step);
			RefPtr<CronValue> pValue5 = pValue4->Next;
			Assert::IsNull(pValue5.Ptr);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}
	};
}
