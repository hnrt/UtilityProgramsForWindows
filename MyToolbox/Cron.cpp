#include "pch.h"
#include "Cron.h"
#include "CronParser.h"
#include "CronEvaluation.h"
#include "hnrt/Exception.h"
#include "hnrt/String.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/Debug.h"


using namespace hnrt;


Cron::Cron()
	: m_pSecond()
	, m_pMinute()
	, m_pHour()
	, m_pDayOfMonth()
	, m_pMonth()
	, m_pDayOfWeek()
	, m_pYear()
	, m_bSecond()
{
}


Cron::~Cron()
{
	Clear();
}


void Cron::Clear()
{
	m_pSecond = nullptr;
	m_pMinute = nullptr;
	m_pHour = nullptr;
	m_pDayOfMonth = nullptr;
	m_pMonth = nullptr;
	m_pDayOfWeek = nullptr;
	m_pYear = nullptr;
}


void Cron::SetAll()
{
	m_pSecond = CronValueAll::Create(CRON_SECOND);
	m_pMinute = CronValueAll::Create(CRON_MINUTE);
	m_pHour = CronValueAll::Create(CRON_HOUR);
	m_pDayOfMonth = CronValueAll::Create(CRON_DAYOFMONTH);
	m_pMonth = CronValueAll::Create(CRON_MONTH);
	m_pDayOfWeek = CronValueAny::Create(CRON_DAYOFWEEK);
	m_pYear = CronValueEmpty::Create(CRON_YEAR);
}


void Cron::Parse(PCWSTR psz)
{
	CronParser(psz).Run(*this);
}


void Cron::ParseSecond(PCWSTR psz)
{
	m_pSecond = CronParser(psz).RunOnlyFor(CRON_SECOND);
}


void Cron::ParseMinute(PCWSTR psz)
{
	m_pMinute = CronParser(psz).RunOnlyFor(CRON_MINUTE);
}


void Cron::ParseHour(PCWSTR psz)
{
	m_pHour = CronParser(psz).RunOnlyFor(CRON_HOUR);
}


void Cron::ParseDayOfMonth(PCWSTR psz)
{
	m_pDayOfMonth = CronParser(psz).RunOnlyFor(CRON_DAYOFMONTH);
}


void Cron::ParseMonth(PCWSTR psz)
{
	m_pMonth = CronParser(psz).RunOnlyFor(CRON_MONTH);
}


void Cron::ParseDayOfWeek(PCWSTR psz)
{
	m_pDayOfWeek = CronParser(psz).RunOnlyFor(CRON_DAYOFWEEK);
}


void Cron::ParseYear(PCWSTR psz)
{
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


bool Cron::GetNextTime(int offset, SYSTEMTIME& stReturn)
{
	CronEvaluation eval(*this);
	if (eval.Run(offset))
	{
		stReturn = eval;
		return true;
	}
	else
	{
		return false;
	}
}


const CronValue& Cron::GetSecond() const
{
	if (m_pSecond)
	{
		return *(m_pSecond.Ptr);
	}
	else
	{
		return *(CronValue::InvalidValue().Ptr);
	}
}


const CronValue& Cron::GetMinute() const
{
	if (m_pMinute)
	{
		return *(m_pMinute.Ptr);
	}
	else
	{
		return *(CronValue::InvalidValue().Ptr);
	}
}


const CronValue& Cron::GetHour() const
{
	if (m_pHour)
	{
		return *(m_pHour.Ptr);
	}
	else
	{
		return *(CronValue::InvalidValue().Ptr);
	}
}


const CronValue& Cron::GetDayOfMonth() const
{
	if (m_pDayOfMonth)
	{
		return *(m_pDayOfMonth.Ptr);
	}
	else
	{
		return *(CronValue::InvalidValue().Ptr);
	}
}


const CronValue& Cron::GetMonth() const
{
	if (m_pMonth)
	{
		return *(m_pMonth.Ptr);
	}
	else
	{
		return *(CronValue::InvalidValue().Ptr);
	}
}


const CronValue& Cron::GetDayOfWeek() const
{
	if (m_pDayOfWeek)
	{
		return *(m_pDayOfWeek.Ptr);
	}
	else
	{
		return *(CronValue::InvalidValue().Ptr);
	}
}


const CronValue& Cron::GetYear() const
{
	if (m_pYear)
	{
		return *(m_pYear.Ptr);
	}
	else
	{
		return *(CronValue::InvalidValue().Ptr);
	}
}
