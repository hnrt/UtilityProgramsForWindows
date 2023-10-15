#include "pch.h"
#include "Cron.h"
#include "CronP.h"
#include "hnrt/Exception.h"


using namespace hnrt;


Cron::Cron()
	: m_pSecond(nullptr)
	, m_pMinute(nullptr)
	, m_pHour(nullptr)
	, m_pDayOfMonth(nullptr)
	, m_pMonth(nullptr)
	, m_pDayOfWeek(nullptr)
	, m_pYear(nullptr)
	, m_bSecond(true)
{
}


Cron::~Cron()
{
	Clear();
}


void Cron::Clear()
{
	CronValue::Free(m_pSecond);
	CronValue::Free(m_pMinute);
	CronValue::Free(m_pHour);
	CronValue::Free(m_pDayOfMonth);
	CronValue::Free(m_pMonth);
	CronValue::Free(m_pDayOfWeek);
	CronValue::Free(m_pYear);
}


bool Cron::isSecondEnabled() const
{
	return m_bSecond;
}


void Cron::EnableSecond(bool bEnabled)
{
	m_bSecond = bEnabled;
}


void Cron::Parse(PCWSTR psz)
{
	Clear();

	CronParser parser(psz);

	if (m_bSecond)
	{
		m_pSecond = parser.Run(CRON_SECOND, CRON_WC_STEP);
	}
	else
	{
		m_pSecond = CronValueAll::Create(CRON_SECOND);
	}
	m_pMinute = parser.Run(CRON_MINUTE, CRON_WC_STEP);
	m_pHour = parser.Run(CRON_HOUR, CRON_WC_STEP);
	m_pDayOfMonth = parser.Run(CRON_DAYOFMONTH, CRON_WC_STEP | CRON_WC_ANY | CRON_WC_LASTDAY | CRON_WC_WEEKDAY);
	m_pMonth = parser.Run(CRON_MONTH, CRON_WC_STEP, MonthWords);
	m_pDayOfWeek = parser.Run(CRON_DAYOFWEEK, CRON_WC_ANY | CRON_WC_LASTDAY | CRON_WC_NTH, DayOfWeekWords);
	if (!parser.isEOF())
	{
		m_pYear = parser.Run(CRON_YEAR, CRON_WC_STEP);
		if (!parser.isEOF())
		{
			throw Exception(L"One or more extra characters follow.");
		}
	}
	if (m_pDayOfMonth->type == CRON_ANY && m_pDayOfWeek->type == CRON_ANY)
	{
		throw Exception(L"? cannot be specified for both day of the month and of the week.");
	}
	if (CountCronValues(m_pDayOfWeek, CRON_NTH_DAYOFWEEK) > 1)
	{
		throw Exception(L"# cannot be specified two or more times.");
	}
}


void Cron::ParseSecond(PCWSTR psz)
{
	CronValue::Free(m_pSecond);
	m_pSecond = CronParser(psz).Run(CRON_SECOND, CRON_WC_STEP);
}


void Cron::ParseMinute(PCWSTR psz)
{
	CronValue::Free(m_pMinute);
	m_pMinute = CronParser(psz).Run(CRON_MINUTE, CRON_WC_STEP);
}


void Cron::ParseHour(PCWSTR psz)
{
	CronValue::Free(m_pHour);
	m_pHour = CronParser(psz).Run(CRON_HOUR, CRON_WC_STEP);
}


void Cron::ParseDayOfMonth(PCWSTR psz)
{
	CronValue::Free(m_pDayOfMonth);
	m_pDayOfMonth = CronParser(psz).Run(CRON_DAYOFMONTH, CRON_WC_STEP | CRON_WC_ANY | CRON_WC_LASTDAY | CRON_WC_WEEKDAY);
}


void Cron::ParseMonth(PCWSTR psz)
{
	CronValue::Free(m_pMonth);
	m_pMonth = CronParser(psz).Run(CRON_MONTH, CRON_WC_STEP, MonthWords);
}


void Cron::ParseDayOfWeek(PCWSTR psz)
{
	CronValue::Free(m_pDayOfWeek);
	m_pDayOfWeek = CronParser(psz).Run(CRON_DAYOFWEEK, CRON_WC_ANY | CRON_WC_LASTDAY | CRON_WC_NTH, DayOfWeekWords);
}


void Cron::ParseYear(PCWSTR psz)
{
	CronValue::Free(m_pYear);
	CronParser parser(psz);
	if (!parser.isEOF())
	{
		m_pYear = CronParser(psz).Run(CRON_YEAR, CRON_WC_STEP);
	}
}


const CronValue& Cron::GetSecond() const
{
	if (m_pSecond)
	{
		return *m_pSecond;
	}
	else
	{
		return *CronValue::Invalid();
	}
}


const CronValue& Cron::GetMinute() const
{
	if (m_pMinute)
	{
		return *m_pMinute;
	}
	else
	{
		return *CronValue::Invalid();
	}
}


const CronValue& Cron::GetHour() const
{
	if (m_pHour)
	{
		return *m_pHour;
	}
	else
	{
		return *CronValue::Invalid();
	}
}


const CronValue& Cron::GetDayOfMonth() const
{
	if (m_pDayOfMonth)
	{
		return *m_pDayOfMonth;
	}
	else
	{
		return *CronValue::Invalid();
	}
}


const CronValue& Cron::GetMonth() const
{
	if (m_pMonth)
	{
		return *m_pMonth;
	}
	else
	{
		return *CronValue::Invalid();
	}
}


const CronValue& Cron::GetDayOfWeek() const
{
	if (m_pDayOfWeek)
	{
		return *m_pDayOfWeek;
	}
	else
	{
		return *CronValue::Invalid();
	}
}


const CronValue& Cron::GetYear() const
{
	if (m_pYear)
	{
		return *m_pYear;
	}
	else
	{
		return *CronValue::Invalid();
	}
}
