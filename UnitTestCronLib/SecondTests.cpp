#include "pch.h"
#include "CppUnitTest.h"
#include "hnrt/Cron.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace hnrt;

namespace UnitTestCronLib
{
	TEST_CLASS(SecondTests)
	{
	public:

		TEST_METHOD(Test0001)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"1 * * * * ?");
			Assert::IsTrue(CRON_SINGLE == cron.Second.Type);
			Assert::AreEqual(1, cron.Second.Value);
			Assert::AreEqual(0, cron.Second.Step);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0002)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"23 * * * * ?");
			Assert::IsTrue(CRON_SINGLE == cron.Second.Type);
			Assert::AreEqual(23, cron.Second.Value);
			Assert::AreEqual(0, cron.Second.Step);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0003)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"15/5 * * * * ?");
			Assert::IsTrue(CRON_SINGLE == cron.Second.Type);
			Assert::AreEqual(15, cron.Second.Value);
			Assert::AreEqual(5, cron.Second.Step);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0004)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"*/5 * * * * ?");
			Assert::IsTrue(CRON_SINGLE == cron.Second.Type);
			Assert::AreEqual(0, cron.Second.Value);
			Assert::AreEqual(5, cron.Second.Step);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0101)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"0-9 * * * * ?");
			Assert::IsTrue(CRON_RANGE == cron.Second.Type);
			Assert::AreEqual(0, cron.Second.From);
			Assert::AreEqual(9, cron.Second.To);
			Assert::AreEqual(1, cron.Second.Step);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0102)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"0-11 * * * * ?");
			Assert::IsTrue(CRON_RANGE == cron.Second.Type);
			Assert::AreEqual(0, cron.Second.From);
			Assert::AreEqual(11, cron.Second.To);
			Assert::AreEqual(1, cron.Second.Step);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0103)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"30-40 * * * * ?");
			Assert::IsTrue(CRON_RANGE == cron.Second.Type);
			Assert::AreEqual(30, cron.Second.From);
			Assert::AreEqual(40, cron.Second.To);
			Assert::AreEqual(1, cron.Second.Step);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0104)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"0-9/1 * * * * ?");
			Assert::IsTrue(CRON_RANGE == cron.Second.Type);
			Assert::AreEqual(0, cron.Second.From);
			Assert::AreEqual(9, cron.Second.To);
			Assert::AreEqual(1, cron.Second.Step);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0105)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"30-59/3 * * * * ?");
			Assert::IsTrue(CRON_RANGE == cron.Second.Type);
			Assert::AreEqual(30, cron.Second.From);
			Assert::AreEqual(59, cron.Second.To);
			Assert::AreEqual(3, cron.Second.Step);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0106)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"0-59/15 * * * * ?");
			Assert::IsTrue(CRON_RANGE == cron.Second.Type);
			Assert::AreEqual(0, cron.Second.From);
			Assert::AreEqual(59, cron.Second.To);
			Assert::AreEqual(15, cron.Second.Step);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0201)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"1,2 * * * * ?");
			Assert::IsTrue(CRON_SINGLE == cron.Second.Type);
			Assert::AreEqual(1, cron.Second.Value);
			Assert::AreEqual(0, cron.Second.Step);
			RefPtr<CronValue> pValue2 = cron.Second.Next;
			Assert::AreEqual(2, pValue2->Value);
			Assert::AreEqual(0, pValue2->Step);
			RefPtr<CronValue> pValue3 = pValue2->Next;
			Assert::IsNull(pValue3.Ptr);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0202)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"1,2,58,59 * * * * ?");
			Assert::IsTrue(CRON_SINGLE == cron.Second.Type);
			Assert::AreEqual(1, cron.Second.Value);
			Assert::AreEqual(0, cron.Second.Step);
			RefPtr<CronValue> pValue2 = cron.Second.Next;
			Assert::AreEqual(2, pValue2->Value);
			Assert::AreEqual(0, pValue2->Step);
			RefPtr<CronValue> pValue3 = pValue2->Next;
			Assert::AreEqual(58, pValue3->Value);
			Assert::AreEqual(0, pValue3->Step);
			RefPtr<CronValue> pValue4 = pValue3->Next;
			Assert::AreEqual(59, pValue4->Value);
			Assert::AreEqual(0, pValue4->Step);
			RefPtr<CronValue> pValue5 = pValue4->Next;
			Assert::IsNull(pValue5.Ptr);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}
	};
}
