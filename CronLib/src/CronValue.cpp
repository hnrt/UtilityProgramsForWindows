#include "pch.h"
#include "hnrt/CronValue.h"
#include "hnrt/CronTokenizer.h"
#include "hnrt/Buffer.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/String.h"
#include "hnrt/Time.h"
#include "hnrt/Exception.h"
#include "hnrt/Debug.h"


using namespace hnrt;


static String AppendIntegerOrWord(int value, CronElement element, String sz)
{
	int min = CronValue::Min(element);
	int max = CronValue::Max(element);
	if (min <= value && value <= max)
	{
		sz.AppendFormat(L"%d", value);
	}
	else if (element == CRON_MONTH)
	{
		value -= CRON_WORD_DISPLACEMENT;
		if (min <= value && value <= max)
		{
			sz += CronMonthWords[value - min];
		}
		else
		{
			throw Exception(L"CronValue::ToString: Bad value: %d", value + CRON_WORD_DISPLACEMENT);
		}
	}
	else if (element == CRON_DAYOFWEEK)
	{
		value -= CRON_WORD_DISPLACEMENT;
		if (min <= value && value <= max)
		{
			sz += CronDayOfWeekWords[value - min];
		}
		else
		{
			throw Exception(L"CronValue::ToString: Bad value: %d", value + CRON_WORD_DISPLACEMENT);
		}
	}
	else
	{
		throw Exception(L"CronValue::ToString: Bad value: %d", value);
	}
	return sz;
}


PCWSTR CronValue::Name(CronElement element)
{
	switch (element)
	{
	case CRON_YEAR: return L"Year";
	case CRON_MONTH: return L"Month";
	case CRON_DAYOFMONTH: return L"Day of the month";
	case CRON_DAYOFWEEK: return L"Day of the week";
	case CRON_HOUR: return L"Hour";
	case CRON_MINUTE: return L"Minute";
	case CRON_SECOND: return L"Second";
	case CRON_ELEMENT_UNSPECIFIED: return L"(unspecified)";
	default: throw Exception(L"CronValue::Name: Bad element: %d", element);
	}
}


int CronValue::Min(CronElement element)
{
	switch (element)
	{
	case CRON_YEAR: return 1970;
	case CRON_MONTH: return 1;
	case CRON_DAYOFMONTH: return 1;
	case CRON_DAYOFWEEK: return 1;
	case CRON_HOUR: return 0;
	case CRON_MINUTE: return 0;
	case CRON_SECOND: return 0;
	default: throw Exception(L"CronValue::Max: Bad element: %d", element);
	}
}


int CronValue::Max(CronElement element)
{
	switch (element)
	{
	case CRON_YEAR: return 2099;
	case CRON_MONTH: return 12;
	case CRON_DAYOFMONTH: return 31;
	case CRON_DAYOFWEEK: return 7;
	case CRON_HOUR: return 23;
	case CRON_MINUTE: return 59;
	case CRON_SECOND: return 59;
	default: throw Exception(L"CronValue::Max: Bad element: %d", element);
	}
}


RefPtr<CronValue> CronValue::InvalidValue()
{
	return RefPtr<CronValue>(new CronValue(CRON_ELEMENT_UNSPECIFIED, CRON_INVALID_VALUE));
}


RefPtr<CronValue> CronValue::CreateEmpty(CronElement element)
{
	return RefPtr<CronValue>(new CronValue(element, CRON_EMPTY));
}


RefPtr<CronValue> CronValue::CreateAll(CronElement element)
{
	return RefPtr<CronValue>(new CronValue(element, CRON_ALL));
}


RefPtr<CronValue> CronValue::CreateAny(CronElement element)
{
	return RefPtr<CronValue>(new CronValue(element, CRON_ANY));
}


RefPtr<CronValue> CronValue::CreateLastDay(CronElement element)
{
	return RefPtr<CronValue>(new CronValue(element, CRON_LASTDAY));
}


RefPtr<CronValue> CronValue::CreateWeekDay(CronElement element)
{
	return RefPtr<CronValue>(new CronValue(element, CRON_WEEKDAY));
}


RefPtr<CronValue> CronValue::CreateSingle(CronElement element, int value, int step)
{
	RefPtr<CronValue> ptr(new CronValue(element, CRON_SINGLE));
	ptr->m_Complex.single.value = value;
	ptr->m_Complex.single.step = step;
	return ptr;
}


RefPtr<CronValue> CronValue::CreateRange(CronElement element, int from, int to, int step)
{
	RefPtr<CronValue> ptr(new CronValue(element, CRON_RANGE));
	ptr->m_Complex.range.from = from;
	ptr->m_Complex.range.to = to;
	ptr->m_Complex.range.step = step;
	return ptr;
}


RefPtr<CronValue> CronValue::CreateClosestWeekDay(CronElement element, int dom)
{
	RefPtr<CronValue> ptr(new CronValue(element, CRON_CLOSEST_WEEKDAY));
	ptr->m_Complex.dom.value = dom;
	return ptr;
}


RefPtr<CronValue> CronValue::CreateNthDayOfWeek(CronElement element, int dow, int nth)
{
	RefPtr<CronValue> ptr(new CronValue(element, CRON_NTH_DAYOFWEEK));
	ptr->m_Complex.dow.value = dow;
	ptr->m_Complex.dow.nth = nth;
	return ptr;
}


RefPtr<CronValue> CronValue::CreateLastDayOfWeek(CronElement element, int dow)
{
	RefPtr<CronValue> ptr(new CronValue(element, CRON_LAST_DAYOFWEEK));
	ptr->m_Complex.dow.value = dow;
	return ptr;
}


CronValue::CronValue(CronElement element, CronValueType type)
	: RefObj()
	, m_Element(element)
	, m_Type(type)
	, m_Complex()
	, m_pNext()
{
	DBGPUT(L"CronValue<%s>@%p::ctor", Name(m_Element), this);
}


CronValue::~CronValue()
{
	DBGPUT(L"CronValue<%s>@%p::dtor", Name(m_Element), this);
}


void CronValue::Append(RefPtr<CronValue> pValue)
{
	if (m_pNext)
	{
		m_pNext->Append(pValue);
	}
	else
	{
		m_pNext = pValue;
	}
}


String CronValue::ToString() const
{
	String sz;
	for (RefPtr<CronValue> pCur = Self; pCur; pCur = pCur->Next)
	{
		if (sz.Len)
		{
			sz.AppendFormat(L",");
		}
		switch (pCur->Type)
		{
		case CRON_EMPTY:
			break;
		case CRON_ALL:
			sz.AppendFormat(L"*");
			break;
		case CRON_ANY:
			sz.AppendFormat(L"?");
			break;
		case CRON_SINGLE:
		{
			sz = AppendIntegerOrWord(pCur->Value, pCur->Element, sz);
			if (pCur->Step > 1)
			{
				sz.AppendFormat(L"/%d", pCur->Step);
			}
			break;
		}
		case CRON_RANGE:
		{
			sz = AppendIntegerOrWord(pCur->From, pCur->Element, sz);
			sz.AppendFormat(L"-");
			sz = AppendIntegerOrWord(pCur->To, pCur->Element, sz);
			if (pCur->Step > 1)
			{
				sz.AppendFormat(L"/%d", pCur->Step);
			}
			break;
		}
		case CRON_LASTDAY:
			sz.AppendFormat(L"L");
			break;
		case CRON_WEEKDAY:
			sz.AppendFormat(L"W");
			break;
		case CRON_CLOSEST_WEEKDAY:
			sz.AppendFormat(L"%dW", pCur->Value);
			break;
		case CRON_NTH_DAYOFWEEK:
		{
			sz = AppendIntegerOrWord(pCur->Value, pCur->Element, sz);
			sz.AppendFormat(L"#%d", pCur->Nth);
			break;
		}
		case CRON_LAST_DAYOFWEEK:
			sz.AppendFormat(L"%dL", pCur->Value);
			break;
		default:
			break;
		}
	}
	return sz;
}


int CronValue::Count(CronValueType type) const
{
	int count = 0;
	for (RefPtr<CronValue> pCur = Self; pCur; pCur = pCur->Next)
	{
		if (pCur->Type == type)
		{
			count++;
		}
	}
	return count;
}


int CronValue::GetNext(SYSTEMTIME& st) const
{
	int candidate = INT_MAX;
	int target;
	switch (m_Element)
	{
	case CRON_DAYOFWEEK:
	{
		target = st.wDayOfWeek;
		WORD wLastDay = GetLastDayOfMonth(st.wYear, st.wMonth);
		for (RefPtr<CronValue> pValue = Self; pValue; pValue = pValue->Next)
		{
			if (pValue->Type == CRON_ALL)
			{
				return 1;
			}
			else if (pValue->Type == CRON_SINGLE)
			{
				if (CheckDayOfWeekRange(CRON_NUMBER(pValue->Value) - 1, -1, st.wDay, wLastDay, target, candidate))
				{
					return 1;
				}
			}
			else if (pValue->Type == CRON_RANGE)
			{
				if (CheckDayOfWeekRange(CRON_NUMBER(pValue->From) - 1, CRON_NUMBER(pValue->To) -1, st.wDay, wLastDay, target, candidate))
				{
					return 1;
				}
			}
			else if (pValue->Type == CRON_LAST_DAYOFWEEK)
			{
				if (CheckLastDayOfWeek(st.wYear, st.wMonth, st.wDay, target, candidate))
				{
					return 1;
				}
			}
			else if (pValue->Type == CRON_NTH_DAYOFWEEK)
			{
				if (CheckNthDayOfWeek(st.wYear, st.wMonth, st.wDay, pValue->Value, pValue->Nth, target, candidate))
				{
					return 1;
				}
			}
		}
		if (candidate == INT_MAX)
		{
			return 0;
		}
		st.wDay = candidate;
		st.wDayOfWeek = 0;
		st.wHour = 0;
		st.wMinute = 0;
		st.wSecond = 0;
		FileTime(st).ToSystemTime(st);
		return -1;
	}
	case CRON_YEAR:
		target = st.wYear;
		break;
	case CRON_MONTH:
		target = st.wMonth;
		break;
	case CRON_DAYOFMONTH:
		target = st.wDay;
		break;
	case CRON_HOUR:
		target = st.wHour;
		break;
	case CRON_MINUTE:
		target = st.wMinute;
		break;
	case CRON_SECOND:
		target = st.wSecond;
		break;
	default:
		return 0;
	}
	for (RefPtr<CronValue> pValue = Self; pValue; pValue = pValue->Next)
	{
		if (pValue->Type == CRON_ALL || pValue->Type == CRON_EMPTY)
		{
			return 1;
		}
		else if (pValue->Type == CRON_SINGLE)
		{
			if (CheckRange(CRON_NUMBER(pValue->Value), CronValue::Max(pValue->Element), pValue->Step, target, candidate))
			{
				return 1;
			}
		}
		else if (pValue->Type == CRON_RANGE)
		{
			if (CheckRange(CRON_NUMBER(pValue->From), CRON_NUMBER(pValue->To), pValue->Step, target, candidate))
			{
				return 1;
			}
		}
		else if (pValue->Type == CRON_WEEKDAY)
		{
			if (CheckWeekDay(st.wYear, st.wMonth, st.wDay, target, candidate))
			{
				return 1;
			}
		}
		else if (pValue->Type == CRON_CLOSEST_WEEKDAY)
		{
			if (CheckClosestWeekDay(st.wYear, st.wMonth, pValue->Value, target, candidate))
			{
				return 1;
			}
		}
		else if (pValue->Type == CRON_LASTDAY)
		{
			if (CheckLastDay(st.wYear, st.wMonth, target, candidate))
			{
				return 1;
			}
		}
	}
	if (candidate == INT_MAX)
	{
		return 0;
	}
	switch (m_Element)
	{
	case CRON_YEAR:
		st.wYear = candidate;
		st.wMonth = 1;
		st.wDay = 1;
		st.wDayOfWeek = 0;
		st.wHour = 0;
		st.wMinute = 0;
		st.wSecond = 0;
		FileTime(st).ToSystemTime(st);
		break;
	case CRON_MONTH:
		st.wMonth = candidate;
		st.wDay = 1;
		st.wDayOfWeek = 0;
		st.wHour = 0;
		st.wMinute = 0;
		st.wSecond = 0;
		FileTime(st).ToSystemTime(st);
		break;
	case CRON_DAYOFMONTH:
		st.wDay = candidate;
		st.wDayOfWeek = 0;
		st.wHour = 0;
		st.wMinute = 0;
		st.wSecond = 0;
		FileTime(st).ToSystemTime(st);
		break;
	case CRON_HOUR:
		st.wHour = candidate;
		st.wMinute = 0;
		st.wSecond = 0;
		break;
	case CRON_MINUTE:
		st.wMinute = candidate;
		st.wSecond = 0;
		break;
	case CRON_SECOND:
		st.wSecond = candidate;
		break;
	default:
		return 0;
	}
	return -1;
}


bool CronValue::CheckRange(int from, int to, int step, int target, int& candidate)
{
	if (step > 0)
	{
		for (int next = from; next <= to; next += step)
		{
			if (next == target)
			{
				return true;
			}
			else if (next > target)
			{
				if (candidate > next)
				{
					candidate = next;
				}
				return false;
			}
		}
	}
	else if (from == target)
	{
		return true;
	}
	else if (from > target)
	{
		if (candidate > from)
		{
			candidate = from;
		}
	}
	return false;
}


bool CronValue::CheckWeekDay(WORD wYear, WORD wMonth, WORD wDay, int target, int& candidate)
{
	WORD y = wYear;
	WORD m = wMonth;
	WORD d = wDay;
	GetWeekDay(y, m, d);
	if (y == wYear && m == wMonth)
	{
		if (d == target)
		{
			return true;
		}
		else if (d > target && candidate > d)
		{
			candidate = d;
		}
	}
	return false;
}


bool CronValue::CheckClosestWeekDay(WORD wYear, WORD wMonth, int day, int target, int& candidate)
{
	WORD y = wYear;
	WORD m = wMonth;
	WORD d = static_cast<WORD>(day);
	GetWeekDay(y, m, d);
	if (y == wYear && m == wMonth)
	{
		if (d == target)
		{
			return true;
		}
		else if (d > target && candidate > d)
		{
			candidate = d;
		}
	}
	return false;
}


bool CronValue::CheckLastDay(WORD wYear, WORD wMonth, int target, int& candidate)
{
	int d = GetLastDayOfMonth(wYear, wMonth);
	if (d == target)
	{
		return true;
	}
	else if (candidate > d)
	{
		candidate = d;
	}
	return false;
}


bool CronValue::CheckDayOfWeekRange(int from, int to, WORD wDay, WORD wLastDay, int target, int& candidate)
{
	if (to < 0)
	{
		to = from;
	}
	for (int next = from; ; next = (next + 1) % 7)
	{
		if (next == target)
		{
			return true;
		}
		else
		{
			int d = static_cast<int>(wDay) + ((next + 7 - target) % 7);
			if (d <= wLastDay && candidate > d)
			{
				candidate = d;
			}
		}
		if (next == to)
		{
			break;
		}
	}
	return false;
}


bool CronValue::CheckLastDayOfWeek(WORD wYear, WORD wMonth, WORD wDay, int target, int& candidate)
{
	WORD y = wYear;
	WORD m = wMonth;
	WORD d = wDay;
	GetLastDayOfMonth(y, m, d, target);
	if (y == wYear && m == wMonth)
	{
		if (d == wDay)
		{
			return true;
		}
		else if (candidate > d)
		{
			candidate = d;
		}
	}
	return false;
}


bool CronValue::CheckNthDayOfWeek(WORD wYear, WORD wMonth, WORD wDay, int dow, int nth, int target, int& candidate)
{
	WORD y = wYear;
	WORD m = wMonth;
	WORD d = wDay;
	GetDayOfWeek(y, m, d, dow, nth);
	if (y == wYear && m == wMonth)
	{
		if (d == wDay)
		{
			return true;
		}
		else if (candidate > d)
		{
			candidate = d;
		}
	}
	return false;
}


int CronValue::get_Value() const
{
	if (m_Type == CRON_SINGLE)
	{
		return m_Complex.single.value;
	}
	else if (m_Type == CRON_CLOSEST_WEEKDAY)
	{
		return m_Complex.dom.value;
	}
	else if (m_Type == CRON_NTH_DAYOFWEEK || m_Type == CRON_LAST_DAYOFWEEK)
	{
		return m_Complex.dow.value;
	}
	else
	{
		throw Exception(L"CronValue::get_Value: Bad access: element=%d", m_Element);
	}
}

int CronValue::get_Step() const
{
	if (m_Type == CRON_SINGLE)
	{
		return m_Complex.single.step;
	}
	else if (m_Type == CRON_RANGE)
	{
		return m_Complex.range.step;
	}
	else
	{
		throw Exception(L"CronValue::get_Step: Bad access: element=%d", m_Element);
	}
}


int CronValue::get_From() const
{
	if (m_Type == CRON_RANGE)
	{
		return m_Complex.range.from;
	}
	else
	{
		throw Exception(L"CronValue::get_From: Bad access: element=%d", m_Element);
	}
}


int CronValue::get_To() const
{
	if (m_Type == CRON_RANGE)
	{
		return m_Complex.range.to;
	}
	else
	{
		throw Exception(L"CronValue::get_To: Bad access: element=%d", m_Element);
	}
}


int CronValue::get_Nth() const
{
	if (m_Type == CRON_NTH_DAYOFWEEK)
	{
		return m_Complex.dow.nth;
	}
	else
	{
		throw Exception(L"CronValue::get_Nth: Bad access: element=%d", m_Element);
	}
}
