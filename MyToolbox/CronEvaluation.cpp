#include "pch.h"
#include "CronEvaluation.h"
#include "Cron.h"
#include "hnrt/Time.h"
#include "hnrt/Debug.h"


using namespace hnrt;


CronEvaluation::CronEvaluation(const Cron& cron)
	: m_cron(cron)
	, m_et()
	, m_ct()
	, m_pYearEval()
	, m_pMonthEval()
	, m_pDayOfMonthEval()
	, m_pDayOfWeekEval()
	, m_pHourEval()
	, m_pMinuteEval()
	, m_pSecondEval()
{
}


bool CronEvaluation::Run(int offset)
{
	if (!m_cron.m_pSecond || m_cron.m_pSecond->type == CRON_INVALID_VALUE)
	{
		return false;
	}
	if (!m_cron.m_pMinute || m_cron.m_pMinute->type == CRON_INVALID_VALUE)
	{
		return false;
	}
	if (!m_cron.m_pHour || m_cron.m_pHour->type == CRON_INVALID_VALUE)
	{
		return false;
	}
	if (!m_cron.m_pDayOfMonth || m_cron.m_pDayOfMonth->type == CRON_INVALID_VALUE)
	{
		return false;
	}
	if (!m_cron.m_pMonth || m_cron.m_pMonth->type == CRON_INVALID_VALUE)
	{
		return false;
	}
	if (!m_cron.m_pDayOfWeek || m_cron.m_pDayOfWeek->type == CRON_INVALID_VALUE)
	{
		return false;
	}
	if (!m_cron.m_pYear || m_cron.m_pYear->type == CRON_INVALID_VALUE)
	{
		return false;
	}
	m_pYearEval = m_cron.m_pYear->Evaluate(0);
	m_pMonthEval = m_cron.m_pMonth->Evaluate(0);
	m_pDayOfMonthEval = m_cron.m_pDayOfMonth->Evaluate(0);
	m_pDayOfWeekEval = m_cron.m_pDayOfWeek->Evaluate(0);
	m_pHourEval = m_cron.m_pHour->Evaluate(0);
	m_pMinuteEval = m_cron.m_pMinute->Evaluate(0);
	m_pSecondEval = m_cron.m_pSecond->Evaluate(0);
	memset(&m_et, 0, sizeof(m_et));
	FileTime ft;
	ft.AddMinutes(offset);
	ft.ToSystemTime(m_ct);
	if (!m_pYearEval->EqualToOrGreaterThan(m_ct.wYear, m_et.wYear))
	{
		return false;
	}
	else if (m_ct.wYear < m_et.wYear)
	{
		return GetNextMonthDayOfMonthHourMinuteSecond();
	}
	else if (!m_pMonthEval->EqualToOrGreaterThan(m_ct.wMonth, m_et.wMonth))
	{
		return GetNextYearMonthDayOfMonthHourMinuteSecond();
	}
	else if (m_ct.wMonth < m_et.wMonth)
	{
		return GetNextDayOfMonthHourMinuteSecond();
	}
	else if (!GetDayOfMonthEqualToOrLaterThanToday())
	{
		return GetNextMonthDayOfMonthHourMinuteSecond();
	}
	else if (m_ct.wDay < m_et.wDay)
	{
		return GetNextHourMinuteSecond();
	}
	else if (!m_pHourEval->EqualToOrGreaterThan(m_ct.wHour, m_et.wHour))
	{
		return GetNextDayOfMonthHourMinuteSecond();
	}
	else if (m_ct.wHour < m_et.wHour)
	{
		return GetNextMinuteSecond();
	}
	else if (!m_pMinuteEval->EqualToOrGreaterThan(m_ct.wMinute, m_et.wMinute))
	{
		return GetNextHourMinuteSecond();
	}
	else if (m_ct.wMinute < m_et.wMinute)
	{
		return GetNextSecond();
	}
	else if (!m_pSecondEval->EqualToOrGreaterThan(m_ct.wSecond, m_et.wSecond))
	{
		return GetNextMinuteSecond();
	}
	return true;
}


bool CronEvaluation::GetNextYearMonthDayOfMonthHourMinuteSecond()
{
	if (!GetNextYear())
	{
		return false;
	}
	return GetNextMonthDayOfMonthHourMinuteSecond();
}


bool CronEvaluation::GetNextMonthDayOfMonthHourMinuteSecond()
{
	if (!GetNextMonth())
	{
		return GetNextYearMonthDayOfMonthHourMinuteSecond();
	}
	return GetNextDayOfMonthHourMinuteSecond();
}


bool CronEvaluation::GetNextDayOfMonthHourMinuteSecond()
{
	if (!GetNextDayOfMonth())
	{
		return GetNextMonthDayOfMonthHourMinuteSecond();
	}
	return GetNextHourMinuteSecond();
}


bool CronEvaluation::GetNextHourMinuteSecond()
{
	if (!GetNextHour())
	{
		return GetNextDayOfMonthHourMinuteSecond();
	}
	return GetNextMinuteSecond();
}


bool CronEvaluation::GetNextMinuteSecond()
{
	while (true)
	{
		if (!GetNextMinute())
		{
			return GetNextHourMinuteSecond();
		}
		if (GetNextSecond())
		{
			return true;
		}
	}
}


bool CronEvaluation::GetNextYear()
{
	return m_pYearEval->Next(m_et.wYear);
}


bool CronEvaluation::GetNextMonth()
{
	return m_pMonthEval->Next(m_et.wMonth);
}


bool CronEvaluation::GetDayOfMonthEqualToOrLaterThanToday()
{
	if (m_cron.m_pDayOfMonth->type == CRON_ANY)
	{
		return GetNextDayOfWeekEqualToOrLaterThanToday();
	}
	else if (m_cron.m_pDayOfMonth->type == CRON_WEEKDAY)
	{
		m_et.wDay = m_ct.wDay;
		GetWeekDay(m_et.wYear, m_et.wMonth, m_et.wDay);
		return true;
	}
	else if (m_cron.m_pDayOfMonth->type == CRON_LASTDAY)
	{
		GetLastDayOfMonth(m_et.wYear, m_et.wMonth, m_et.wDay);
		return true;
	}
	else
	{
		return m_pDayOfMonthEval->EqualToOrGreaterThan(m_ct.wDay, m_et.wDay);
	}
}


bool CronEvaluation::GetNextDayOfWeekEqualToOrLaterThanToday()
{
	if (m_cron.m_pDayOfWeek->type == CRON_ANY)
	{
		return false;
	}
	if (m_cron.m_pDayOfWeek->type == CRON_LAST_DAYOFWEEK)
	{
		m_et.wDay = m_ct.wDay;
		GetLastDayOfMonth(m_et.wYear, m_et.wMonth, m_et.wDay, CRON_NUMBER(m_cron.m_pDayOfWeek->lastdow.dow) - 1);
	}
	else if (m_cron.m_pDayOfWeek->type == CRON_NTH_DAYOFWEEK)
	{
		m_et.wDay = m_ct.wDay;
		GetDayOfWeek(m_et.wYear, m_et.wMonth, m_et.wDay, CRON_NUMBER(m_cron.m_pDayOfWeek->nthdow.dow) - 1, m_cron.m_pDayOfWeek->nthdow.nth);
	}
	else
	{
		SYSTEMTIME tt = m_ct;
		FileTime ft(tt);
		while (true)
		{
			WORD next = ~0;
			if (m_pDayOfWeekEval->Next(next))
			{
				if (next == tt.wDayOfWeek + 1)
				{
					m_et.wYear = tt.wYear;
					m_et.wMonth = tt.wMonth;
					m_et.wDay = tt.wDay;
					break;
				}
			}
			else
			{
				ft.AddDays(1);
				ft.ToSystemTime(tt); // sets tt.wDayOfWeek
			}
		}
	}
	return true;
}


bool CronEvaluation::GetNextDayOfMonth()
{
	if (m_cron.m_pDayOfMonth->type == CRON_ANY)
	{
		return GetNextDayOfWeek();
	}
	else if (m_cron.m_pDayOfMonth->type == CRON_WEEKDAY)
	{
		m_et.wDay = 1;
		GetWeekDay(m_et.wYear, m_et.wMonth, m_et.wDay);
		return true;
	}
	else if (m_cron.m_pDayOfMonth->type == CRON_LASTDAY)
	{
		GetLastDayOfMonth(m_et.wYear, m_et.wMonth, m_et.wDay);
		return true;
	}
	else
	{
		return m_pDayOfMonthEval->Next(m_et.wDay);
	}
}


bool CronEvaluation::GetNextDayOfWeek()
{
	if (m_cron.m_pDayOfWeek->type == CRON_ANY)
	{
		return false;
	}
	SYSTEMTIME tt = { 0 };
	tt.wYear = m_et.wYear;
	tt.wMonth = m_et.wMonth;
	tt.wDay = m_ct.wDay;
	FileTime ft(tt);
	ft.AddDays(1);
	ft.ToSystemTime(tt); // sets tt.wDayOfWeek
	m_et.wYear = tt.wYear;
	m_et.wMonth = tt.wMonth;
	m_et.wDay = tt.wDay;
	if (m_cron.m_pDayOfWeek->type == CRON_LAST_DAYOFWEEK)
	{
		GetLastDayOfMonth(m_et.wYear, m_et.wMonth, m_et.wDay, CRON_NUMBER(m_cron.m_pDayOfWeek->lastdow.dow) - 1);
	}
	else if (m_cron.m_pDayOfWeek->type == CRON_NTH_DAYOFWEEK)
	{
		GetDayOfWeek(m_et.wYear, m_et.wMonth, m_et.wDay, CRON_NUMBER(m_cron.m_pDayOfWeek->nthdow.dow) - 1, m_cron.m_pDayOfWeek->nthdow.nth);
	}
	else
	{
		while (true)
		{
			WORD next = ~0;
			if (m_pDayOfWeekEval->Next(next))
			{
				if (next == tt.wDayOfWeek + 1)
				{
					m_et.wYear = tt.wYear;
					m_et.wMonth = tt.wMonth;
					m_et.wDay = tt.wDay;
					break;
				}
			}
			else
			{
				ft.AddDays(1);
				ft.ToSystemTime(tt); // sets tt.wDayOfWeek
			}
		}
	}
	return true;
}


bool CronEvaluation::GetNextHour()
{
	return m_pHourEval->Next(m_et.wHour);
}


bool CronEvaluation::GetNextMinute()
{
	return m_pMinuteEval->Next(m_et.wMinute);
}


bool CronEvaluation::GetNextSecond()
{
	return m_pSecondEval->Next(m_et.wSecond);
}
