#include "pch.h"
#include "hnrt/Cron.h"
#include "hnrt/CronParser.h"
#include "hnrt/Exception.h"
#include "hnrt/String.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/Time.h"
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
	m_pSecond = CronValue::CreateAll(CRON_SECOND);
	m_pMinute = CronValue::CreateAll(CRON_MINUTE);
	m_pHour = CronValue::CreateAll(CRON_HOUR);
	m_pDayOfMonth = CronValue::CreateAll(CRON_DAYOFMONTH);
	m_pMonth = CronValue::CreateAll(CRON_MONTH);
	m_pDayOfWeek = CronValue::CreateAny(CRON_DAYOFWEEK);
	m_pYear = CronValue::CreateEmpty(CRON_YEAR);
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


String Cron::ToString() const
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
	return String(TRIM, buf);
}


bool Cron::GetNextTime(const SYSTEMTIME& stBase, SYSTEMTIME& stReturn)
{
	SYSTEMTIME st = stBase;
	if (m_bSecond)
	{
		st.wMilliseconds = 0;
		FileTime ft(st);
		ft.AddSeconds(1);
		ft.ToSystemTime(st);
	}
	else
	{
		st.wMilliseconds = 0;
		st.wSecond = 0;
		FileTime ft(st);
		ft.AddMinutes(1);
		ft.ToSystemTime(st);
	}
	if (!CheckYear(st))
	{
		return false;
	}
	if (!CheckMonth(st))
	{
		return false;
	}
	if (!CheckDayOfMonth(st))
	{
		return false;
	}
	if (!CheckHour(st))
	{
		return false;
	}
	if (!CheckMinute(st))
	{
		return false;
	}
	if (m_bSecond && !CheckSecond(st))
	{
		return false;
	}
	stReturn = st;
	return true;
}


bool Cron::CheckYear(SYSTEMTIME& st) const
{
	if (m_pYear->GetNext(st))
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool Cron::CheckMonth(SYSTEMTIME& st) const
{
	while (!m_pMonth->GetNext(st))
	{
		if (!CheckNextYear(st))
		{
			return false;
		}
	}
	return true;
}


bool Cron::CheckDayOfMonth(SYSTEMTIME& st) const
{
	if (m_pDayOfWeek->Type == CRON_ANY)
	{
		while (!m_pDayOfMonth->GetNext(st))
		{
			if (!CheckNextMonth(st))
			{
				return false;
			}
		}
	}
	else if (m_pDayOfMonth->Type == CRON_ANY)
	{
		while (!m_pDayOfWeek->GetNext(st))
		{
			if (!CheckNextMonth(st))
			{
				return false;
			}
		}
	}
	else
	{
		throw Exception(L"Cron::CheckDayOfMonth: Either DayOfMonth or DayOfWeek must be of ANY.");
	}
	return true;
}


bool Cron::CheckHour(SYSTEMTIME& st) const
{
	while (!m_pHour->GetNext(st))
	{
		if (!CheckNextDayOfMonth(st))
		{
			return false;
		}
	}
	return true;
}


bool Cron::CheckMinute(SYSTEMTIME& st) const
{
	while (!m_pMinute->GetNext(st))
	{
		if (!CheckNextHour(st))
		{
			return false;
		}
	}
	return true;
}


bool Cron::CheckSecond(SYSTEMTIME& st) const
{
	while (!m_pSecond->GetNext(st))
	{
		if (!CheckNextMinute(st))
		{
			return false;
		}
	}
	return true;
}


bool Cron::CheckNextYear(SYSTEMTIME& st) const
{
	if (st.wYear < CronValue::Max(CRON_YEAR))
	{
		st.wYear++;
		st.wMonth = 1;
		st.wDay = 1;
		st.wDayOfWeek = 0;
		st.wHour = 0;
		st.wMinute = 0;
		st.wSecond = 0;
		FileTime(st).ToSystemTime(st);
	}
	else
	{
		return false;
	}
	return CheckYear(st);
}


bool Cron::CheckNextMonth(SYSTEMTIME& st) const
{
	if (st.wMonth < 12)
	{
		st.wMonth++;
		st.wDay = 1;
		st.wDayOfWeek = 0;
		st.wHour = 0;
		st.wMinute = 0;
		st.wSecond = 0;
		FileTime(st).ToSystemTime(st);
	}
	else if (!CheckNextYear(st))
	{
		return false;
	}
	return CheckMonth(st);
}


bool Cron::CheckNextDayOfMonth(SYSTEMTIME& st) const
{
	if (st.wDay < GetLastDayOfMonth(st.wYear, st.wMonth))
	{
		st.wDay++;
		st.wDayOfWeek = 0;
		st.wHour = 0;
		st.wMinute = 0;
		st.wSecond = 0;
		FileTime(st).ToSystemTime(st);
	}
	else if (!CheckNextMonth(st))
	{
		return false;
	}
	return CheckDayOfMonth(st);
}


bool Cron::CheckNextHour(SYSTEMTIME& st) const
{
	if (st.wHour < 23)
	{
		st.wHour++;
		st.wMinute = 0;
		st.wSecond = 0;
	}
	else if (!CheckNextDayOfMonth(st))
	{
		return false;
	}
	return CheckHour(st);
}


bool Cron::CheckNextMinute(SYSTEMTIME& st) const
{
	if (st.wMinute < 59)
	{
		st.wMinute++;
		st.wSecond = 0;
	}
	else if (!CheckNextHour(st))
	{
		return false;
	}
	return CheckMinute(st);
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
