#pragma once


#include "CronValue.h"


namespace hnrt
{
	class Cron
	{
	public:

		Cron();
		Cron(const Cron&) = delete;
		~Cron();
		void operator =(const Cron&) = delete;
		operator PCWSTR() const;
		void Clear();
		bool isSecondEnabled() const;
		void EnableSecond(bool bEnabled = true);
		void SetAll();
		void Parse(PCWSTR);
		void ParseSecond(PCWSTR);
		void ParseMinute(PCWSTR);
		void ParseHour(PCWSTR);
		void ParseDayOfMonth(PCWSTR);
		void ParseMonth(PCWSTR);
		void ParseDayOfWeek(PCWSTR);
		void ParseYear(PCWSTR);
		PCWSTR ToString() const;
		const CronValue& GetSecond() const;
		const CronValue& GetMinute() const;
		const CronValue& GetHour() const;
		const CronValue& GetDayOfMonth() const;
		const CronValue& GetMonth() const;
		const CronValue& GetDayOfWeek() const;
		const CronValue& GetYear() const;

		__declspec(property(get = GetSecond)) const CronValue& Second;
		__declspec(property(get = GetMinute)) const CronValue& Minute;
		__declspec(property(get = GetHour)) const CronValue& Hour;
		__declspec(property(get = GetDayOfMonth)) const CronValue& DayOfMonth;
		__declspec(property(get = GetMonth)) const CronValue& Month;
		__declspec(property(get = GetDayOfWeek)) const CronValue& DayOfWeek;
		__declspec(property(get = GetYear)) const CronValue& Year;
		__declspec(property(get = isSecondEnabled, put = EnableSecond)) bool SecondEnabled;

	private:

		CronValue* m_pSecond; // 0-59 ,-*/
		CronValue* m_pMinute; // 0-59 ,-*/
		CronValue* m_pHour; // 0-23 ,-*/
		CronValue* m_pDayOfMonth; // 1-31 ,-*/LWC
		CronValue* m_pMonth; // 0-11 or JAN-DEC ,-*/
		CronValue* m_pDayOfWeek; // 1-7 or SUN-SAT ,-*/LC#
		CronValue* m_pYear; // empty or 1970-2099 ,-*/
		bool m_bSecond;
	};
}
