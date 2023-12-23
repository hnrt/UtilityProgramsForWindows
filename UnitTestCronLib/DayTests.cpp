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
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfMonth.Type);
			Assert::AreEqual(1, cron.DayOfMonth.Value);
			Assert::AreEqual(0, cron.DayOfMonth.Step);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0002)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * 23 * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfMonth.Type);
			Assert::AreEqual(23, cron.DayOfMonth.Value);
			Assert::AreEqual(0, cron.DayOfMonth.Step);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0003)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * 15/5 * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfMonth.Type);
			Assert::AreEqual(15, cron.DayOfMonth.Value);
			Assert::AreEqual(5, cron.DayOfMonth.Step);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0101)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * 1-9 * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfMonth.Type);
			Assert::AreEqual(1, cron.DayOfMonth.From);
			Assert::AreEqual(9, cron.DayOfMonth.To);
			Assert::AreEqual(1, cron.DayOfMonth.Step);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0102)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * 2-11 * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfMonth.Type);
			Assert::AreEqual(2, cron.DayOfMonth.From);
			Assert::AreEqual(11, cron.DayOfMonth.To);
			Assert::AreEqual(1, cron.DayOfMonth.Step);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0103)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * 10-20 * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfMonth.Type);
			Assert::AreEqual(10, cron.DayOfMonth.From);
			Assert::AreEqual(20, cron.DayOfMonth.To);
			Assert::AreEqual(1, cron.DayOfMonth.Step);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0104)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * 3-9/1 * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfMonth.Type);
			Assert::AreEqual(3, cron.DayOfMonth.From);
			Assert::AreEqual(9, cron.DayOfMonth.To);
			Assert::AreEqual(1, cron.DayOfMonth.Step);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0105)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * 10-19/3 * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfMonth.Type);
			Assert::AreEqual(10, cron.DayOfMonth.From);
			Assert::AreEqual(19, cron.DayOfMonth.To);
			Assert::AreEqual(3, cron.DayOfMonth.Step);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0106)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * 4-20/10 * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfMonth.Type);
			Assert::AreEqual(4, cron.DayOfMonth.From);
			Assert::AreEqual(20, cron.DayOfMonth.To);
			Assert::AreEqual(10, cron.DayOfMonth.Step);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0201)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * 1 * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfMonth.Type);
			Assert::AreEqual(1, cron.DayOfMonth.Value);
			Assert::AreEqual(0, cron.DayOfMonth.Step);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0202)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * 23 * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfMonth.Type);
			Assert::AreEqual(23, cron.DayOfMonth.Value);
			Assert::AreEqual(0, cron.DayOfMonth.Step);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0203)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * 15/5 * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfMonth.Type);
			Assert::AreEqual(15, cron.DayOfMonth.Value);
			Assert::AreEqual(5, cron.DayOfMonth.Step);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0301)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * 5-9 * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfMonth.Type);
			Assert::AreEqual(5, cron.DayOfMonth.From);
			Assert::AreEqual(9, cron.DayOfMonth.To);
			Assert::AreEqual(1, cron.DayOfMonth.Step);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0302)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * 6-11 * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfMonth.Type);
			Assert::AreEqual(6, cron.DayOfMonth.From);
			Assert::AreEqual(11, cron.DayOfMonth.To);
			Assert::AreEqual(1, cron.DayOfMonth.Step);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0303)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * 17-20 * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfMonth.Type);
			Assert::AreEqual(17, cron.DayOfMonth.From);
			Assert::AreEqual(20, cron.DayOfMonth.To);
			Assert::AreEqual(1, cron.DayOfMonth.Step);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0304)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * 8-9/1 * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfMonth.Type);
			Assert::AreEqual(8, cron.DayOfMonth.From);
			Assert::AreEqual(9, cron.DayOfMonth.To);
			Assert::AreEqual(1, cron.DayOfMonth.Step);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0305)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * 10-19/3 * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfMonth.Type);
			Assert::AreEqual(10, cron.DayOfMonth.From);
			Assert::AreEqual(19, cron.DayOfMonth.To);
			Assert::AreEqual(3, cron.DayOfMonth.Step);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0306)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * 1-23/15 * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_RANGE == cron.DayOfMonth.Type);
			Assert::AreEqual(1, cron.DayOfMonth.From);
			Assert::AreEqual(23, cron.DayOfMonth.To);
			Assert::AreEqual(15, cron.DayOfMonth.Step);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0401)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * L * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_LASTDAY == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0402)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * L * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_LASTDAY == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0501)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * W * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_WEEKDAY == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0502)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * W * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_WEEKDAY == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0601)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * 3W * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_CLOSEST_WEEKDAY == cron.DayOfMonth.Type);
			Assert::AreEqual(3, cron.DayOfMonth.Value);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0602)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * 27W * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_CLOSEST_WEEKDAY == cron.DayOfMonth.Type);
			Assert::AreEqual(27, cron.DayOfMonth.Value);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0701)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * 7,8 * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfMonth.Type);
			Assert::AreEqual(7, cron.DayOfMonth.Value);
			Assert::AreEqual(0, cron.DayOfMonth.Step);
			RefPtr<CronValue> pValue2 = cron.DayOfMonth.Next;
			Assert::AreEqual(8, pValue2->Value);
			Assert::AreEqual(0, pValue2->Step);
			RefPtr<CronValue> pValue3 = pValue2->Next;
			Assert::IsNull(pValue3.Ptr);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0702)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * 7,8,10,22 * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfMonth.Type);
			Assert::AreEqual(7, cron.DayOfMonth.Value);
			Assert::AreEqual(0, cron.DayOfMonth.Step);
			RefPtr<CronValue> pValue2 = cron.DayOfMonth.Next;
			Assert::AreEqual(8, pValue2->Value);
			Assert::AreEqual(0, pValue2->Step);
			RefPtr<CronValue> pValue3 = pValue2->Next;
			Assert::AreEqual(10, pValue3->Value);
			Assert::AreEqual(0, pValue3->Step);
			RefPtr<CronValue> pValue4 = pValue3->Next;
			Assert::AreEqual(22, pValue4->Value);
			Assert::AreEqual(0, pValue4->Step);
			RefPtr<CronValue> pValue5 = pValue4->Next;
			Assert::IsNull(pValue5.Ptr);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0801)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * 7,8 * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfMonth.Type);
			Assert::AreEqual(7, cron.DayOfMonth.Value);
			Assert::AreEqual(0, cron.DayOfMonth.Step);
			RefPtr<CronValue> pValue2 = cron.DayOfMonth.Next;
			Assert::AreEqual(8, pValue2->Value);
			Assert::AreEqual(0, pValue2->Step);
			RefPtr<CronValue> pValue3 = pValue2->Next;
			Assert::IsNull(pValue3.Ptr);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0802)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * 7,8,20,31 * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_SINGLE == cron.DayOfMonth.Type);
			Assert::AreEqual(7, cron.DayOfMonth.Value);
			Assert::AreEqual(0, cron.DayOfMonth.Step);
			RefPtr<CronValue> pValue2 = cron.DayOfMonth.Next;
			Assert::AreEqual(8, pValue2->Value);
			Assert::AreEqual(0, pValue2->Step);
			RefPtr<CronValue> pValue3 = pValue2->Next;
			Assert::AreEqual(20, pValue3->Value);
			Assert::AreEqual(0, pValue3->Step);
			RefPtr<CronValue> pValue4 = pValue3->Next;
			Assert::AreEqual(31, pValue4->Value);
			Assert::AreEqual(0, pValue4->Step);
			RefPtr<CronValue> pValue5 = pValue4->Next;
			Assert::IsNull(pValue5.Ptr);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}
	};
}
