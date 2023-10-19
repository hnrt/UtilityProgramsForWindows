#include "pch.h"
#include "Cron.h"
#include "CronParser.h"
#include "hnrt/Exception.h"
#include "hnrt/String.h"
#include "hnrt/StringBuffer.h"


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


Cron::operator PCWSTR() const
{
	return ToString();
}


bool Cron::isSecondEnabled() const
{
	return m_bSecond;
}


void Cron::EnableSecond(bool bEnabled)
{
	if (!m_bSecond && bEnabled && !m_pSecond)
	{
		m_pSecond = CronValueAll::Create(CRON_SECOND);
	}
	m_bSecond = bEnabled;
}


void Cron::SetAll()
{
	Clear();
	m_pSecond = CronValueAll::Create(CRON_SECOND);
	m_pMinute = CronValueAll::Create(CRON_MINUTE);
	m_pHour = CronValueAll::Create(CRON_HOUR);
	m_pDayOfMonth = CronValueAll::Create(CRON_DAYOFMONTH);
	m_pMonth = CronValueAll::Create(CRON_MONTH);
	m_pDayOfWeek = CronValueAny::Create(CRON_DAYOFWEEK);
}


void Cron::Parse(PCWSTR psz)
{
	Clear();
	if (m_bSecond)
	{
		CronParser(psz).Run(m_pYear, m_pMonth, m_pDayOfMonth, m_pDayOfWeek, m_pHour, m_pMinute, m_pSecond);
	}
	else
	{
		m_pSecond = CronValueAll::Create(CRON_SECOND);
		CronParser(psz).Run(m_pYear, m_pMonth, m_pDayOfMonth, m_pDayOfWeek, m_pHour, m_pMinute);
	}
}


void Cron::ParseSecond(PCWSTR psz)
{
	CronValue::Free(m_pSecond);
	m_pSecond = CronParser(psz).RunOnlyFor(CRON_SECOND);
}


void Cron::ParseMinute(PCWSTR psz)
{
	CronValue::Free(m_pMinute);
	m_pMinute = CronParser(psz).RunOnlyFor(CRON_MINUTE);
}


void Cron::ParseHour(PCWSTR psz)
{
	CronValue::Free(m_pHour);
	m_pHour = CronParser(psz).RunOnlyFor(CRON_HOUR);
}


void Cron::ParseDayOfMonth(PCWSTR psz)
{
	CronValue::Free(m_pDayOfMonth);
	m_pDayOfMonth = CronParser(psz).RunOnlyFor(CRON_DAYOFMONTH);
}


void Cron::ParseMonth(PCWSTR psz)
{
	CronValue::Free(m_pMonth);
	m_pMonth = CronParser(psz).RunOnlyFor(CRON_MONTH);
}


void Cron::ParseDayOfWeek(PCWSTR psz)
{
	CronValue::Free(m_pDayOfWeek);
	m_pDayOfWeek = CronParser(psz).RunOnlyFor(CRON_DAYOFWEEK);
}


void Cron::ParseYear(PCWSTR psz)
{
	CronValue::Free(m_pYear);
	m_pYear = CronParser(psz).RunOnlyFor(CRON_YEAR);
}


PCWSTR Cron::ToString() const
{
	StringBuffer buf(260);
	if (m_bSecond && m_pSecond)
	{
		buf.AppendFormat(L" %s", m_pSecond->ToString());
	}
	if (m_pMinute)
	{
		buf.AppendFormat(L" %s", m_pMinute->ToString());
	}
	if (m_pHour)
	{
		buf.AppendFormat(L" %s", m_pHour->ToString());
	}
	if (m_pDayOfMonth)
	{
		buf.AppendFormat(L" %s", m_pDayOfMonth->ToString());
	}
	if (m_pMonth)
	{
		buf.AppendFormat(L" %s", m_pMonth->ToString());
	}
	if (m_pDayOfWeek)
	{
		buf.AppendFormat(L" %s", m_pDayOfWeek->ToString());
	}
	if (m_pYear)
	{
		buf.AppendFormat(L" %s", m_pYear->ToString());
	}
	return String::Trim(buf);
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
