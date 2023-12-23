#include "pch.h"
#include "CppUnitTest.h"
#include "hnrt/Cron.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace hnrt;

namespace UnitTestCronLib
{
	TEST_CLASS(HourTests)
	{
	public:

		TEST_METHOD(Test0001)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * 1 * * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Hour.Type);
			Assert::AreEqual(1, cron.Hour.Value);
			Assert::AreEqual(0, cron.Hour.Step);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0002)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * 23 * * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Hour.Type);
			Assert::AreEqual(23, cron.Hour.Value);
			Assert::AreEqual(0, cron.Hour.Step);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0003)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * 15/5 * * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Hour.Type);
			Assert::AreEqual(15, cron.Hour.Value);
			Assert::AreEqual(5, cron.Hour.Step);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0101)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * 0-9 * * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_RANGE == cron.Hour.Type);
			Assert::AreEqual(0, cron.Hour.From);
			Assert::AreEqual(9, cron.Hour.To);
			Assert::AreEqual(1, cron.Hour.Step);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0102)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * 0-11 * * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_RANGE == cron.Hour.Type);
			Assert::AreEqual(0, cron.Hour.From);
			Assert::AreEqual(11, cron.Hour.To);
			Assert::AreEqual(1, cron.Hour.Step);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0103)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * 10-20 * * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_RANGE == cron.Hour.Type);
			Assert::AreEqual(10, cron.Hour.From);
			Assert::AreEqual(20, cron.Hour.To);
			Assert::AreEqual(1, cron.Hour.Step);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0104)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * 0-9/1 * * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_RANGE == cron.Hour.Type);
			Assert::AreEqual(0, cron.Hour.From);
			Assert::AreEqual(9, cron.Hour.To);
			Assert::AreEqual(1, cron.Hour.Step);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0105)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * 10-19/3 * * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_RANGE == cron.Hour.Type);
			Assert::AreEqual(10, cron.Hour.From);
			Assert::AreEqual(19, cron.Hour.To);
			Assert::AreEqual(3, cron.Hour.Step);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0106)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * 0-20/10 * * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_RANGE == cron.Hour.Type);
			Assert::AreEqual(0, cron.Hour.From);
			Assert::AreEqual(20, cron.Hour.To);
			Assert::AreEqual(10, cron.Hour.Step);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0201)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* 1 * * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Hour.Type);
			Assert::AreEqual(1, cron.Hour.Value);
			Assert::AreEqual(0, cron.Hour.Step);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0202)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* 23  * * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Hour.Type);
			Assert::AreEqual(23, cron.Hour.Value);
			Assert::AreEqual(0, cron.Hour.Step);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0203)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* 15/5 * * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Hour.Type);
			Assert::AreEqual(15, cron.Hour.Value);
			Assert::AreEqual(5, cron.Hour.Step);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0301)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* 0-9 * * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_RANGE == cron.Hour.Type);
			Assert::AreEqual(0, cron.Hour.From);
			Assert::AreEqual(9, cron.Hour.To);
			Assert::AreEqual(1, cron.Hour.Step);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0302)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* 0-11 * * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_RANGE == cron.Hour.Type);
			Assert::AreEqual(0, cron.Hour.From);
			Assert::AreEqual(11, cron.Hour.To);
			Assert::AreEqual(1, cron.Hour.Step);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0303)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* 10-20 * * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_RANGE == cron.Hour.Type);
			Assert::AreEqual(10, cron.Hour.From);
			Assert::AreEqual(20, cron.Hour.To);
			Assert::AreEqual(1, cron.Hour.Step);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0304)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* 0-9/1 * * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_RANGE == cron.Hour.Type);
			Assert::AreEqual(0, cron.Hour.From);
			Assert::AreEqual(9, cron.Hour.To);
			Assert::AreEqual(1, cron.Hour.Step);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0305)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* 10-19/3 * * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_RANGE == cron.Hour.Type);
			Assert::AreEqual(10, cron.Hour.From);
			Assert::AreEqual(19, cron.Hour.To);
			Assert::AreEqual(3, cron.Hour.Step);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0306)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* 0-23/15 * * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_RANGE == cron.Hour.Type);
			Assert::AreEqual(0, cron.Hour.From);
			Assert::AreEqual(23, cron.Hour.To);
			Assert::AreEqual(15, cron.Hour.Step);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0401)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * 0,6 * * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Hour.Type);
			Assert::AreEqual(0, cron.Hour.Value);
			Assert::AreEqual(0, cron.Hour.Step);
			RefPtr<CronValue> pValue2 = cron.Hour.Next;
			Assert::AreEqual(6, pValue2->Value);
			Assert::AreEqual(0, pValue2->Step);
			RefPtr<CronValue> pValue3 = pValue2->Next;
			Assert::IsNull(pValue3.Ptr);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0402)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * 0,6,22,23 * * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Hour.Type);
			Assert::AreEqual(0, cron.Hour.Value);
			Assert::AreEqual(0, cron.Hour.Step);
			RefPtr<CronValue> pValue2 = cron.Hour.Next;
			Assert::AreEqual(6, pValue2->Value);
			Assert::AreEqual(0, pValue2->Step);
			RefPtr<CronValue> pValue3 = pValue2->Next;
			Assert::AreEqual(22, pValue3->Value);
			Assert::AreEqual(0, pValue3->Step);
			RefPtr<CronValue> pValue4 = pValue3->Next;
			Assert::AreEqual(23, pValue4->Value);
			Assert::AreEqual(0, pValue4->Step);
			RefPtr<CronValue> pValue5 = pValue4->Next;
			Assert::IsNull(pValue5.Ptr);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0501)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* 5,6 * * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Hour.Type);
			Assert::AreEqual(5, cron.Hour.Value);
			Assert::AreEqual(0, cron.Hour.Step);
			RefPtr<CronValue> pValue2 = cron.Hour.Next;
			Assert::AreEqual(6, pValue2->Value);
			Assert::AreEqual(0, pValue2->Step);
			RefPtr<CronValue> pValue3 = pValue2->Next;
			Assert::IsNull(pValue3.Ptr);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0502)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* 5,6,7,23 * * ?");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Hour.Type);
			Assert::AreEqual(5, cron.Hour.Value);
			Assert::AreEqual(0, cron.Hour.Step);
			RefPtr<CronValue> pValue2 = cron.Hour.Next;
			Assert::AreEqual(6, pValue2->Value);
			Assert::AreEqual(0, pValue2->Step);
			RefPtr<CronValue> pValue3 = pValue2->Next;
			Assert::AreEqual(7, pValue3->Value);
			Assert::AreEqual(0, pValue3->Step);
			RefPtr<CronValue> pValue4 = pValue3->Next;
			Assert::AreEqual(23, pValue4->Value);
			Assert::AreEqual(0, pValue4->Step);
			RefPtr<CronValue> pValue5 = pValue4->Next;
			Assert::IsNull(pValue5.Ptr);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}
	};
}
