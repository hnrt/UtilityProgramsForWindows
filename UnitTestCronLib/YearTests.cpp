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
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_SINGLE == cron.Year.type);
			Assert::AreEqual(2023, dynamic_cast<const CronValueSingle&>(cron.Year).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Year).step);
		}

		TEST_METHOD(Test0002)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * * ? 2023/4");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_SINGLE == cron.Year.type);
			Assert::AreEqual(2023, dynamic_cast<const CronValueSingle&>(cron.Year).value);
			Assert::AreEqual(4, dynamic_cast<const CronValueSingle&>(cron.Year).step);
		}

		TEST_METHOD(Test0101)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * * ? 2023,2024,2026");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_SINGLE == cron.Year.type);
			Assert::AreEqual(2023, dynamic_cast<const CronValueSingle&>(cron.Year).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Year).step);
			const CronValue* pValue2 = cron.Year.next.Ptr;
			Assert::AreEqual(2024, dynamic_cast<const CronValueSingle*>(pValue2)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue2)->step);
			const CronValue* pValue3 = pValue2->next.Ptr;
			Assert::AreEqual(2026, dynamic_cast<const CronValueSingle*>(pValue3)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue3)->step);
			const CronValue* pValue4 = pValue3->next.Ptr;
			Assert::IsNull(pValue4);
		}

		TEST_METHOD(Test0201)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * * ? 2023-2030");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_RANGE == cron.Year.type);
			Assert::AreEqual(2023, dynamic_cast<const CronValueRange&>(cron.Year).from);
			Assert::AreEqual(2030, dynamic_cast<const CronValueRange&>(cron.Year).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.Year).step);
		}

		TEST_METHOD(Test0202)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * * ? 2020-2030/3");
			Assert::IsTrue(CRON_ALL == cron.Second.type);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_RANGE == cron.Year.type);
			Assert::AreEqual(2020, dynamic_cast<const CronValueRange&>(cron.Year).from);
			Assert::AreEqual(2030, dynamic_cast<const CronValueRange&>(cron.Year).to);
			Assert::AreEqual(3, dynamic_cast<const CronValueRange&>(cron.Year).step);
		}

		TEST_METHOD(Test0301)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * * ? 2023");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_SINGLE == cron.Year.type);
			Assert::AreEqual(2023, dynamic_cast<const CronValueSingle&>(cron.Year).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Year).step);
		}

		TEST_METHOD(Test0302)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * * ? 2023/4");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_SINGLE == cron.Year.type);
			Assert::AreEqual(2023, dynamic_cast<const CronValueSingle&>(cron.Year).value);
			Assert::AreEqual(4, dynamic_cast<const CronValueSingle&>(cron.Year).step);
		}

		TEST_METHOD(Test0401)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * * ? 2023,2024,2026");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_SINGLE == cron.Year.type);
			Assert::AreEqual(2023, dynamic_cast<const CronValueSingle&>(cron.Year).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Year).step);
			const CronValue* pValue2 = cron.Year.next.Ptr;
			Assert::AreEqual(2024, dynamic_cast<const CronValueSingle*>(pValue2)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue2)->step);
			const CronValue* pValue3 = pValue2->next.Ptr;
			Assert::AreEqual(2026, dynamic_cast<const CronValueSingle*>(pValue3)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue3)->step);
			const CronValue* pValue4 = pValue3->next.Ptr;
			Assert::IsNull(pValue4);
		}

		TEST_METHOD(Test0501)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * * ? 2023-2030");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_RANGE == cron.Year.type);
			Assert::AreEqual(2023, dynamic_cast<const CronValueRange&>(cron.Year).from);
			Assert::AreEqual(2030, dynamic_cast<const CronValueRange&>(cron.Year).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.Year).step);
		}

		TEST_METHOD(Test0502)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * * ? 2020-2030/3");
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_RANGE == cron.Year.type);
			Assert::AreEqual(2020, dynamic_cast<const CronValueRange&>(cron.Year).from);
			Assert::AreEqual(2030, dynamic_cast<const CronValueRange&>(cron.Year).to);
			Assert::AreEqual(3, dynamic_cast<const CronValueRange&>(cron.Year).step);
		}
	};
}
