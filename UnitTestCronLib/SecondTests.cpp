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
			Assert::IsTrue(CRON_SINGLE == cron.Second.type);
			Assert::AreEqual(1, dynamic_cast<const CronValueSingle&>(cron.Second).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Second).step);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0002)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"23 * * * * ?");
			Assert::IsTrue(CRON_SINGLE == cron.Second.type);
			Assert::AreEqual(23, dynamic_cast<const CronValueSingle&>(cron.Second).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Second).step);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0003)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"15/5 * * * * ?");
			Assert::IsTrue(CRON_SINGLE == cron.Second.type);
			Assert::AreEqual(15, dynamic_cast<const CronValueSingle&>(cron.Second).value);
			Assert::AreEqual(5, dynamic_cast<const CronValueSingle&>(cron.Second).step);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0101)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"0-9 * * * * ?");
			Assert::IsTrue(CRON_RANGE == cron.Second.type);
			Assert::AreEqual(0, dynamic_cast<const CronValueRange&>(cron.Second).from);
			Assert::AreEqual(9, dynamic_cast<const CronValueRange&>(cron.Second).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.Second).step);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0102)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"0-11 * * * * ?");
			Assert::IsTrue(CRON_RANGE == cron.Second.type);
			Assert::AreEqual(0, dynamic_cast<const CronValueRange&>(cron.Second).from);
			Assert::AreEqual(11, dynamic_cast<const CronValueRange&>(cron.Second).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.Second).step);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0103)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"30-40 * * * * ?");
			Assert::IsTrue(CRON_RANGE == cron.Second.type);
			Assert::AreEqual(30, dynamic_cast<const CronValueRange&>(cron.Second).from);
			Assert::AreEqual(40, dynamic_cast<const CronValueRange&>(cron.Second).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.Second).step);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0104)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"0-9/1 * * * * ?");
			Assert::IsTrue(CRON_RANGE == cron.Second.type);
			Assert::AreEqual(0, dynamic_cast<const CronValueRange&>(cron.Second).from);
			Assert::AreEqual(9, dynamic_cast<const CronValueRange&>(cron.Second).to);
			Assert::AreEqual(1, dynamic_cast<const CronValueRange&>(cron.Second).step);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0105)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"30-59/3 * * * * ?");
			Assert::IsTrue(CRON_RANGE == cron.Second.type);
			Assert::AreEqual(30, dynamic_cast<const CronValueRange&>(cron.Second).from);
			Assert::AreEqual(59, dynamic_cast<const CronValueRange&>(cron.Second).to);
			Assert::AreEqual(3, dynamic_cast<const CronValueRange&>(cron.Second).step);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0106)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"0-59/15 * * * * ?");
			Assert::IsTrue(CRON_RANGE == cron.Second.type);
			Assert::AreEqual(0, dynamic_cast<const CronValueRange&>(cron.Second).from);
			Assert::AreEqual(59, dynamic_cast<const CronValueRange&>(cron.Second).to);
			Assert::AreEqual(15, dynamic_cast<const CronValueRange&>(cron.Second).step);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0201)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"1,2 * * * * ?");
			Assert::IsTrue(CRON_SINGLE == cron.Second.type);
			Assert::AreEqual(1, dynamic_cast<const CronValueSingle&>(cron.Second).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Second).step);
			const CronValue* pValue2 = cron.Second.next.Ptr;
			Assert::AreEqual(2, dynamic_cast<const CronValueSingle*>(pValue2)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue2)->step);
			const CronValue* pValue3 = pValue2->next.Ptr;
			Assert::IsNull(pValue3);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}

		TEST_METHOD(Test0202)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"1,2,58,59 * * * * ?");
			Assert::IsTrue(CRON_SINGLE == cron.Second.type);
			Assert::AreEqual(1, dynamic_cast<const CronValueSingle&>(cron.Second).value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle&>(cron.Second).step);
			const CronValue* pValue2 = cron.Second.next.Ptr;
			Assert::AreEqual(2, dynamic_cast<const CronValueSingle*>(pValue2)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue2)->step);
			const CronValue* pValue3 = pValue2->next.Ptr;
			Assert::AreEqual(58, dynamic_cast<const CronValueSingle*>(pValue3)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue3)->step);
			const CronValue* pValue4 = pValue3->next.Ptr;
			Assert::AreEqual(59, dynamic_cast<const CronValueSingle*>(pValue4)->value);
			Assert::AreEqual(0, dynamic_cast<const CronValueSingle*>(pValue4)->step);
			const CronValue* pValue5 = pValue4->next.Ptr;
			Assert::IsNull(pValue5);
			Assert::IsTrue(CRON_ALL == cron.Minute.type);
			Assert::IsTrue(CRON_ALL == cron.Hour.type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.type);
			Assert::IsTrue(CRON_ALL == cron.Month.type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.type);
		}
	};
}
