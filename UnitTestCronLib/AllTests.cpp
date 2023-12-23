#include "pch.h"
#include "CppUnitTest.h"
#include "hnrt/Cron.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace hnrt;

namespace UnitTestCronLib
{
	TEST_CLASS(AllTests)
	{
	public:
		
		TEST_METHOD(Test0001)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * * * ?");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
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
			cron.Parse(L"* * * * * ? *");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_ALL == cron.Year.Type);
		}

		TEST_METHOD(Test0003)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * *");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0004)
		{
			Cron cron;
			cron.SecondEnabled = true;
			cron.Parse(L"* * * ? * * *");
			Assert::IsTrue(CRON_ALL == cron.Second.Type);
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_ALL == cron.Year.Type);
		}

		TEST_METHOD(Test0101)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * * * ?");
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
			cron.SecondEnabled = false;
			cron.Parse(L"* * * * ? *");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_ALL == cron.Year.Type);
		}

		TEST_METHOD(Test0103)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * ? * *");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_EMPTY == cron.Year.Type);
		}

		TEST_METHOD(Test0104)
		{
			Cron cron;
			cron.SecondEnabled = false;
			cron.Parse(L"* * ? * * *");
			Assert::IsTrue(CRON_ALL == cron.Minute.Type);
			Assert::IsTrue(CRON_ALL == cron.Hour.Type);
			Assert::IsTrue(CRON_ANY == cron.DayOfMonth.Type);
			Assert::IsTrue(CRON_ALL == cron.Month.Type);
			Assert::IsTrue(CRON_ALL == cron.DayOfWeek.Type);
			Assert::IsTrue(CRON_ALL == cron.Year.Type);
		}
	};
}
