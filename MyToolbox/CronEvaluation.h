#pragma once


#include "CronValueEvaluation.h"


namespace hnrt
{
	class Cron;

	class CronEvaluation
	{
	public:

		CronEvaluation(const Cron&);
		CronEvaluation(const CronEvaluation&) = delete;
		~CronEvaluation() = default;
		void operator =(const CronEvaluation&) = delete;
		bool Run(int offset);
		operator const SYSTEMTIME&() const;

	private:

		bool GetNextYearMonthDayOfMonthHourMinuteSecond();
		bool GetNextMonthDayOfMonthHourMinuteSecond();
		bool GetNextDayOfMonthHourMinuteSecond();
		bool GetNextHourMinuteSecond();
		bool GetNextMinuteSecond();

		bool GetNextYear();
		bool GetNextMonth();
		bool GetDayOfMonthEqualToOrLaterThanToday();
		bool GetNextDayOfWeekEqualToOrLaterThanToday();
		bool GetNextDayOfMonth();
		bool GetNextDayOfWeek();
		bool GetNextHour();
		bool GetNextMinute();
		bool GetNextSecond();

		const Cron& m_cron;
		SYSTEMTIME m_et;
		SYSTEMTIME m_ct;
		RefPtr<CronValueEvaluation> m_pYearEval;
		RefPtr<CronValueEvaluation> m_pMonthEval;
		RefPtr<CronValueEvaluation> m_pDayOfMonthEval;
		RefPtr<CronValueEvaluation> m_pDayOfWeekEval;
		RefPtr<CronValueEvaluation> m_pHourEval;
		RefPtr<CronValueEvaluation> m_pMinuteEval;
		RefPtr<CronValueEvaluation> m_pSecondEval;
	};

	inline CronEvaluation::operator const SYSTEMTIME&() const
	{
		return m_et;
	}
}
