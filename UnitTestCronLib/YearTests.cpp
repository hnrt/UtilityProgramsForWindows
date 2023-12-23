#include "pch.h"
#include "CppUnitTest.h"
#include "hnrt/Cron.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace hnrt;

namespace UnitTestCronLib
{
	TEST_CLASS(YearTests)
	{
	public:

		TEST_METHOD(Test0001)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * * ? 2023");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Year.Type);
			Assert::AreEqual(2023, cron.Year.Value);
			Assert::AreEqual(0, cron.Year.Step);
		}

		TEST_METHOD(Test0002)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * * ? 2023/4");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Year.Type);
			Assert::AreEqual(2023, cron.Year.Value);
			Assert::AreEqual(4, cron.Year.Step);
		}

		TEST_METHOD(Test0101)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * * ? 2023,2024,2026");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Year.Type);
			Assert::AreEqual(2023, cron.Year.Value);
			Assert::AreEqual(0, cron.Year.Step);
			RefPtr<CronValue> pValue2 = cron.Year.Next;
			Assert::AreEqual(2024, pValue2->Value);
			Assert::AreEqual(0, pValue2->Step);
			RefPtr<CronValue> pValue3 = pValue2->Next;
			Assert::AreEqual(2026, pValue3->Value);
			Assert::AreEqual(0, pValue3->Step);
			RefPtr<CronValue> pValue4 = pValue3->Next;
			Assert::IsNull(pValue4.Ptr);
		}

		TEST_METHOD(Test0201)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * * ? 2023-2030");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_RANGE == cron.Year.Type);
			Assert::AreEqual(2023, cron.Year.From);
			Assert::AreEqual(2030, cron.Year.To);
			Assert::AreEqual(1, cron.Year.Step);
		}

		TEST_METHOD(Test0202)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * * ? 2020-2030/3");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_RANGE == cron.Year.Type);
			Assert::AreEqual(2020, cron.Year.From);
			Assert::AreEqual(2030, cron.Year.To);
			Assert::AreEqual(3, cron.Year.Step);
		}

		TEST_METHOD(Test0301)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * * ? 2023");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Year.Type);
			Assert::AreEqual(2023, cron.Year.Value);
			Assert::AreEqual(0, cron.Year.Step);
		}

		TEST_METHOD(Test0302)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * * ? 2023/4");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Year.Type);
			Assert::AreEqual(2023, cron.Year.Value);
			Assert::AreEqual(4, cron.Year.Step);
		}

		TEST_METHOD(Test0401)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * * ? 2023,2024,2026");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_SINGLE == cron.Year.Type);
			Assert::AreEqual(2023, cron.Year.Value);
			Assert::AreEqual(0, cron.Year.Step);
			RefPtr<CronValue> pValue2 = cron.Year.Next;
			Assert::AreEqual(2024, pValue2->Value);
			Assert::AreEqual(0, pValue2->Step);
			RefPtr<CronValue> pValue3 = pValue2->Next;
			Assert::AreEqual(2026, pValue3->Value);
			Assert::AreEqual(0, pValue3->Step);
			RefPtr<CronValue> pValue4 = pValue3->Next;
			Assert::IsNull(pValue4.Ptr);
		}

		TEST_METHOD(Test0501)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * * ? 2023-2030");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_RANGE == cron.Year.Type);
			Assert::AreEqual(2023, cron.Year.From);
			Assert::AreEqual(2030, cron.Year.To);
			Assert::AreEqual(1, cron.Year.Step);
		}

		TEST_METHOD(Test0502)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * * ? 2020-2030/3");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_RANGE == cron.Year.Type);
			Assert::AreEqual(2020, cron.Year.From);
			Assert::AreEqual(2030, cron.Year.To);
			Assert::AreEqual(3, cron.Year.Step);
		}
	};
}
