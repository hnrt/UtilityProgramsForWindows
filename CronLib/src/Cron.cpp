#include "pch.h"
#include "hnrt/Cron.h"
#include "hnrt/CronParser.h"
#include "hnrt/CronEvaluation.h"
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
		if (st.wMilliseconds)
		{
			st.wMilliseconds = 0;
			FileTime ft(st);
			ft.AddSeconds(1);
			ft.ToSystemTime(st);
		}
	}
	else if (st.wMilliseconds || st.wSecond)
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
	FileTime(st).ToSystemTime(stReturn);
	return true;
}


bool Cron::CheckYear(SYSTEMTIME& st) const
{
	int target = st.wYear;
	int closest = INT_MAX;
	for (RefPtr<CronValue> pYear = m_pYear; pYear; pYear = pYear->Next)
	{
		if (pYear->Type == CRON_EMPTY || pYear->Type == CRON_ALL)
		{
			return true;
		}
		else if (pYear->Type == CRON_SINGLE)
		{
			if (CheckRange(pYear->Value, CronValue::Max(pYear->Element), pYear->Step, target, closest))
			{
				return true;
			}
		}
		else if (pYear->Type == CRON_RANGE)
		{
			if (CheckRange(pYear->From, pYear->To, pYear->Step, target, closest))
			{
				return true;
			}
		}
		else
		{
			return false;
		}
	}
	if (closest < INT_MAX)
	{
		st.wYear = closest;
		st.wMonth = 1;
		st.wDay = 1;
		st.wDayOfWeek = 0;
		st.wHour = 0;
		st.wMinute = 0;
		st.wSecond = 0;
		FileTime(st).ToSystemTime(st);
		return true;
	}
	else
	{
		return false;
	}
}


bool Cron::CheckMonth(SYSTEMTIME& st) const
{
	while (true)
	{
		int target = st.wMonth;
		int closest = INT_MAX;
		for (RefPtr<CronValue> pMonth = m_pMonth; pMonth; pMonth = pMonth->Next)
		{
			if (pMonth->Type == CRON_ALL)
			{
				return true;
			}
			else if (pMonth->Type == CRON_SINGLE)
			{
				if (CheckRange(CRON_NUMBER(pMonth->Value), CronValue::Max(pMonth->Element), pMonth->Step, target, closest))
				{
					return true;
				}
			}
			else if (pMonth->Type == CRON_RANGE)
			{
				if (CheckRange(CRON_NUMBER(pMonth->From), CRON_NUMBER(pMonth->To), pMonth->Step, target, closest))
				{
					return true;
				}
			}
			else
			{
				return false;
			}
		}
		if (closest < INT_MAX)
		{
			st.wMonth = closest;
			st.wDay = 1;
			st.wDayOfWeek = 0;
			st.wHour = 0;
			st.wMinute = 0;
			st.wSecond = 0;
			FileTime(st).ToSystemTime(st);
			return true;
		}
		if (!CheckNextYear(st))
		{
			return false;
		}
	}
}


bool Cron::CheckDayOfMonth(SYSTEMTIME& st) const
{
	if (m_pDayOfWeek->Type == CRON_ANY)
	{
		while (true)
		{
			int target = st.wDay;
			int closest = INT_MAX;
			for (RefPtr<CronValue> pDayOfMonth = m_pDayOfMonth; pDayOfMonth; pDayOfMonth = pDayOfMonth->Next)
			{
				if (pDayOfMonth->Type == CRON_ALL)
				{
					return true;
				}
				else if (pDayOfMonth->Type == CRON_SINGLE)
				{
					if (CheckRange(pDayOfMonth->Value, CronValue::Max(pDayOfMonth->Element), pDayOfMonth->Step, target, closest))
					{
						return true;
					}
				}
				else if (pDayOfMonth->Type == CRON_RANGE)
				{
					if (CheckRange(pDayOfMonth->From, pDayOfMonth->To, pDayOfMonth->Step, target, closest))
					{
						return true;
					}
				}
				else if (pDayOfMonth->Type == CRON_WEEKDAY)
				{
					WORD y = st.wYear;
					WORD m = st.wMonth;
					WORD d = st.wDay;
					GetWeekDay(y, m, d);
					if (y == st.wYear && m == st.wMonth)
					{
						if (d == st.wDay)
						{
							return true;
						}
						else if (closest > d)
						{
							closest = d;
						}
					}
				}
				else if (pDayOfMonth->Type == CRON_CLOSEST_WEEKDAY)
				{
					WORD y = st.wYear;
					WORD m = st.wMonth;
					WORD d = pDayOfMonth->Value;
					GetWeekDay(y, m, d);
					if (y == st.wYear && m == st.wMonth)
					{
						if (d == target)
						{
							return true;
						}
						else if (d > target && closest > d)
						{
							closest = d;
						}
					}
				}
				else if (pDayOfMonth->Type == CRON_LASTDAY)
				{
					int d = GetLastDayOfMonth(st.wYear, st.wMonth);
					if (closest > d)
					{
						closest = d;
					}
				}
				else
				{
					return false;
				}
			}
			if (closest < INT_MAX)
			{
				st.wDay = closest;
				st.wDayOfWeek = 0;
				st.wHour = 0;
				st.wMinute = 0;
				st.wSecond = 0;
				FileTime(st).ToSystemTime(st);
				return true;
			}
			if (!CheckNextMonth(st))
			{
				return false;
			}
		}
	}
	else if (m_pDayOfMonth->Type == CRON_ANY)
	{
		while (true)
		{
			int target = st.wDayOfWeek;
			int closest = INT_MAX;
			int max = GetLastDayOfMonth(st.wYear, st.wMonth);
			for (RefPtr<CronValue> pDayOfWeek = m_pDayOfWeek; pDayOfWeek; pDayOfWeek = pDayOfWeek->Next)
			{
				if (pDayOfWeek->Type == CRON_ALL)
				{
					return true;
				}
				else if (pDayOfWeek->Type == CRON_SINGLE)
				{
					int value = CRON_NUMBER(pDayOfWeek->Value) - 1;
					if (value == target)
					{
						return true;
					}
					else
					{
						int d = static_cast<int>(st.wDay) + ((value + 7 - target) % 7);
						if (d <= max && closest > d)
						{
							closest = d;
						}
					}
				}
				else if (pDayOfWeek->Type == CRON_RANGE)
				{
					int value = CRON_NUMBER(pDayOfWeek->From) - 1;
					int to = CRON_NUMBER(pDayOfWeek->To) - 1;
					while (true)
					{
						if (value == target)
						{
							return true;
						}
						else
						{
							int d = static_cast<int>(st.wDay) + ((value + 7 - target) % 7);
							if (d <= max && closest > d)
							{
								closest = d;
							}
						}
						if (value == to)
						{
							break;
						}
						value = (value + 1) % 7;
					}
				}
				else if (pDayOfWeek->Type == CRON_LAST_DAYOFWEEK)
				{
					WORD y = st.wYear;
					WORD m = st.wMonth;
					WORD d = st.wDay;
					GetLastDayOfMonth(y, m, d, st.wDayOfWeek);
					if (y == st.wYear && m == st.wMonth)
					{
						if (d == st.wDay)
						{
							return true;
						}
						else if (closest > d)
						{
							closest = d;
						}
					}
				}
				else if (pDayOfWeek->Type == CRON_NTH_DAYOFWEEK)
				{
					WORD y = st.wYear;
					WORD m = st.wMonth;
					WORD d = st.wDay;
					hnrt::GetDayOfWeek(y, m, d, pDayOfWeek->Value, pDayOfWeek->Nth);
					if (y == st.wYear && m == st.wMonth)
					{
						if (d == st.wDay)
						{
							return true;
						}
						else if (closest > d)
						{
							closest = d;
						}
					}
				}
				else
				{
					return false;
				}
			}
			if (closest < INT_MAX)
			{
				st.wDay = closest;
				st.wDayOfWeek = 0;
				st.wHour = 0;
				st.wMinute = 0;
				st.wSecond = 0;
				FileTime(st).ToSystemTime(st);
				return true;
			}
			if (!CheckNextMonth(st))
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
}


bool Cron::CheckHour(SYSTEMTIME& st) const
{
	while (true)
	{
		int target = st.wHour;
		int closest = INT_MAX;
		for (RefPtr<CronValue> pHour = m_pHour; pHour; pHour = pHour->Next)
		{
			if (pHour->Type == CRON_ALL)
			{
				return true;
			}
			if (pHour->Type == CRON_SINGLE)
			{
				if (CheckRange(pHour->Value, CronValue::Max(pHour->Element), pHour->Step, target, closest))
				{
					return true;
				}
			}
			else if (pHour->Type == CRON_RANGE)
			{
				if (CheckRange(pHour->From, pHour->To, pHour->Step, target, closest))
				{
					return true;
				}
			}
			else
			{
				return false;
			}
		}
		if (closest < INT_MAX)
		{
			st.wHour = closest;
			st.wMinute = 0;
			st.wSecond = 0;
			return true;
		}
		if (!CheckNextDayOfMonth(st))
		{
			return false;
		}
	}
}


bool Cron::CheckMinute(SYSTEMTIME& st) const
{
	while (true)
	{
		int target = st.wMinute;
		int closest = INT_MAX;
		for (RefPtr<CronValue> pMinute = m_pMinute; pMinute; pMinute = pMinute->Next)
		{
			if (pMinute->Type == CRON_ALL)
			{
				return true;
			}
			if (pMinute->Type == CRON_SINGLE)
			{
				if (CheckRange(pMinute->Value, CronValue::Max(pMinute->Element), pMinute->Step, target, closest))
				{
					return true;
				}
			}
			else if (pMinute->Type == CRON_RANGE)
			{
				if (CheckRange(pMinute->From, pMinute->To, pMinute->Step, target, closest))
				{
					return true;
				}
			}
			else
			{
				return false;
			}
		}
		if (closest < INT_MAX)
		{
			st.wMinute = closest;
			st.wSecond = 0;
			return true;
		}
		if (!CheckNextHour(st))
		{
			return false;
		}
	}
}


bool Cron::CheckSecond(SYSTEMTIME& st) const
{
	while (true)
	{
		int target = st.wSecond;
		int closest = INT_MAX;
		for (RefPtr<CronValue> pSecond = m_pSecond; pSecond; pSecond = pSecond->Next)
		{
			if (pSecond->Type == CRON_ALL)
			{
				return true;
			}
			if (pSecond->Type == CRON_SINGLE)
			{
				if (CheckRange(pSecond->Value, CronValue::Max(pSecond->Element), pSecond->Step, target, closest))
				{
					return true;
				}
			}
			else if (pSecond->Type == CRON_RANGE)
			{
				if (CheckRange(pSecond->From, pSecond->To, pSecond->Step, target, closest))
				{
					return true;
				}
			}
			else
			{
				return false;
			}
		}
		if (closest < INT_MAX)
		{
			st.wSecond = closest;
			return true;
		}
		if (!CheckNextMinute(st))
		{
			return false;
		}
	}
}


bool Cron::CheckRange(int from, int to, int step, int target, int& closest) const
{
	if (step < 1)
	{
		step = to;
	}
	for (int next = from; next <= to; next += step)
	{
		if (next == target)
		{
			return true;
		}
		else if (next > target)
		{
			if (closest > next)
			{
				closest = next;
			}
			return false;
		}
	}
	return false;
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
		st.wMilliseconds = 0;
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
		st.wMilliseconds = 0;
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
		st.wMilliseconds = 0;
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
		st.wMilliseconds = 0;
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
		st.wMilliseconds = 0;
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
