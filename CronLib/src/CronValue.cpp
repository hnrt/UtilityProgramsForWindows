#include "pch.h"
#include "hnrt/CronValue.h"
#include "hnrt/CronTokenizer.h"
#include "hnrt/Buffer.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/String.h"
#include "hnrt/Time.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/ErrorMessage.h"
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


// Num = Max - Min + 1
int CronValue::Num(CronElement element)
{
	switch (element)
	{
	case CRON_YEAR: return 2099 - 1970 + 1;
	case CRON_MONTH: return 12 - 1 + 1;
	case CRON_DAYOFMONTH: return 31 - 1 + 1;
	case CRON_DAYOFWEEK: return 7 - 1 + 1;
	case CRON_HOUR: return 23 - 0 + 1;
	case CRON_MINUTE: return 59 - 0 + 1;
	case CRON_SECOND: return 59 - 0 + 1;
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


bool CronValue::GetNext(SYSTEMTIME& st) const
{
	int candidate = INT_MAX;
	int target;
	switch (m_Element)
	{
	case CRON_DAYOFWEEK:
		target = st.wDayOfWeek + 1;
		break;
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
		throw Exception(L"CronValue::GetNext: Unknown element: %d", m_Element);
	}
	for (RefPtr<CronValue> pValue = Self; pValue; pValue = pValue->Next)
	{
		if (pValue->Type == CRON_ALL || pValue->Type == CRON_EMPTY)
		{
			return true;
		}
		else if (pValue->Type == CRON_SINGLE)
		{
			if (CheckRange(CRON_NUMBER(pValue->Value), Max(pValue->Element), pValue->Step, target, candidate))
			{
				return true;
			}
		}
		else if (pValue->Type == CRON_RANGE)
		{
			if (CheckRange(CRON_NUMBER(pValue->From), CRON_NUMBER(pValue->To), pValue->Step, target, candidate))
			{
				return true;
			}
		}
		else if (pValue->Type == CRON_WEEKDAY)
		{
			if (CheckWeekDay(st.wYear, st.wMonth, st.wDay, candidate))
			{
				return true;
			}
		}
		else if (pValue->Type == CRON_CLOSEST_WEEKDAY)
		{
			if (CheckClosestWeekDay(st.wYear, st.wMonth, pValue->Value, target, candidate))
			{
				return true;
			}
		}
		else if (pValue->Type == CRON_LASTDAY)
		{
			if (CheckLastDay(st.wYear, st.wMonth, target, candidate))
			{
				return true;
			}
		}
		else if (pValue->Type == CRON_LAST_DAYOFWEEK)
		{
			if (CheckLastDayOfWeek(st.wYear, st.wMonth, st.wDay, pValue->Value, candidate))
			{
				return true;
			}
		}
		else if (pValue->Type == CRON_NTH_DAYOFWEEK)
		{
			if (CheckNthDayOfWeek(st.wYear, st.wMonth, st.wDay, pValue->Nth, pValue->Value, candidate))
			{
				return true;
			}
		}
	}
	if (candidate == INT_MAX)
	{
		return false;
	}
	try
	{
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
			if (st.wDay > GetLastDayOfMonth(st.wYear, st.wMonth))
			{
				return false;
			}
			st.wDayOfWeek = 0;
			st.wHour = 0;
			st.wMinute = 0;
			st.wSecond = 0;
			FileTime(st).ToSystemTime(st);
			break;
		case CRON_DAYOFWEEK:
			st.wDay += candidate;
			if (st.wDay > GetLastDayOfMonth(st.wYear, st.wMonth))
			{
				return false;
			}
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
			// SHOULD NEVER REACH HERE
			Debug::Put(L"CronValue::GetNext: Unknown element: %d", m_Element);
			return false;
		}
		return true;
	}
	catch (Win32Exception e)
	{
		Debug::Put(L"CronValue::GetNext: %s (%s)", e.Message, ErrorMessage::Get(e.Error));
		return false;
	}
}


bool CronValue::CheckRange(int from, int to, int step, int target, int& candidate) const
{
	if (m_Element == CRON_DAYOFWEEK)
	{
		if (step > 0)
		{
			if (to < 0)
			{
				to = from;
			}
			else if (to < from)
			{
				to += Num(CRON_DAYOFWEEK);
			}
			for (int next = from; next <= to; next += step)
			{
				int normalized = Normalize(next);
				if (normalized == target)
				{
					candidate = 0;
					return true;
				}
				int delta = (normalized + 7 - target) % 7;
				if (delta < candidate)
				{
					candidate = delta;
				}
			}
		}
		else
		{
			if (from == target)
			{
				candidate = 0;
				return true;
			}
			int delta = (from + Num(CRON_DAYOFWEEK) - target) % Num(CRON_DAYOFWEEK);
			if (delta < candidate)
			{
				candidate = delta;
			}
		}
	}
	else if (step > 0)
	{
		if (m_Element == CRON_YEAR)
		{
			if (to < from)
			{
				to = from;
			}
		}
		else if (to < from)
		{
			to += Num(m_Element);
		}
		for (int next = from; next <= to; next += step)
		{
			int normalized = Normalize(next);
			if (normalized == target)
			{
				candidate = normalized;
				return true;
			}
			else if (target < normalized && normalized < candidate)
			{
				candidate = normalized;
			}
		}
	}
	else if (from == target)
	{
		candidate = target;
		return true;
	}
	else if (target < from && from < candidate)
	{
		candidate = from;
	}
	return false;
}


int CronValue::Normalize(int value) const
{
	return ((value - Min(m_Element)) % Num(m_Element)) + Min(m_Element);
}


bool CronValue::CheckWeekDay(WORD wYear, WORD wMonth, WORD wDay, int& candidate)
{
	WORD y = wYear;
	WORD m = wMonth;
	WORD d = wDay;
	GetWeekDay(y, m, d);
	if (y == wYear && m == wMonth)
	{
		if (d == wDay)
		{
			candidate = wDay;
			return true;
		}
		else if (wDay < d && d < candidate)
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
			candidate = target;
			return true;
		}
		else if (target < d && d < candidate)
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
		candidate = target;
		return true;
	}
	else if (target < d && d < candidate)
	{
		candidate = d;
	}
	return false;
}


bool CronValue::CheckLastDayOfWeek(WORD wYear, WORD wMonth, WORD wDay, int target, int& candidate)
{
	WORD y = wYear;
	WORD m = wMonth;
	WORD d = wDay;
	GetLastDayOfMonth(y, m, d, target - 1);
	if (y == wYear && m == wMonth)
	{
		if (d == wDay)
		{
			candidate = 0;
			return true;
		}
		int delta = d - wDay;
		if (candidate > delta)
		{
			candidate = delta;
		}
	}
	return false;
}


bool CronValue::CheckNthDayOfWeek(WORD wYear, WORD wMonth, WORD wDay, int nth, int target, int& candidate)
{
	WORD y = wYear;
	WORD m = wMonth;
	WORD d = wDay;
	GetDayOfWeek(y, m, d, target - 1, nth);
	if (y == wYear && m == wMonth)
	{
		if (d == wDay)
		{
			candidate = 0;
			return true;
		}
		int delta = d - wDay;
		if (candidate > delta)
		{
			candidate = delta;
		}
	}
	return false;
}


static void Add(int value, std::vector<int>& dst)
{
	for (std::vector<int>::iterator iter = dst.begin(); iter != dst.end(); iter++)
	{
		int next = *iter;
		if (value < next)
		{
			dst.insert(iter, value);
			return;
		}
		else if (value == next)
		{
			return;
		}
	}
	dst.push_back(value);
}


std::vector<int> CronValue::Enumerate(int offset) const
{
	std::vector<int> ret;
	for (RefPtr<CronValue> pValue = Self; pValue; pValue = pValue->Next)
	{
		if (pValue->Type == CRON_ALL || pValue->Type == CRON_ANY || pValue->Type == CRON_EMPTY)
		{
			ret.clear();
			if (m_Element == CRON_YEAR)
			{
				SYSTEMTIME st = { 0 };
				GetSystemTime(&st);
				FileTime ft(st);
				ft.AddMinutes(offset);
				ft.ToSystemTime(st);
				int from = st.wYear;
				int to = Max(m_Element);
				for (int value = from; value <= to; value++)
				{
					Add(value, ret);
				}
			}
			else
			{
				int from = Min(m_Element);
				int to = Max(m_Element);
				for (int value = from; value <= to; value++)
				{
					Add(value, ret);
				}
			}
			break;
		}
		else if (pValue->Type == CRON_SINGLE)
		{
			int delta =
				m_Element == CRON_HOUR ? (offset / 60 + 24) :
				m_Element == CRON_MINUTE ? (offset % 60 + 60) :
				0;
			if (pValue->Step > 0)
			{
				int from = CRON_NUMBER(pValue->Value);
				int to = Max(m_Element);
				for (int value = from; value <= to; value += pValue->Step)
				{
					Add(Normalize(value + delta), ret);
				}
			}
			else
			{
				Add(Normalize(CRON_NUMBER(pValue->Value) + delta), ret);
			}
		}
		else if (pValue->Type == CRON_RANGE)
		{
			int delta =
				m_Element == CRON_HOUR ? (offset / 60 + 24) :
				m_Element == CRON_MINUTE ? (offset % 60 + 60) :
				0;
			int from = CRON_NUMBER(pValue->From);
			int to = CRON_NUMBER(pValue->To);
			if (to < from)
			{
				if (m_Element == CRON_YEAR)
				{
					to = from;
				}
				else
				{
					to += Num(m_Element);
				}
			}
			for (int value = from; value <= to; value += pValue->Step)
			{
				Add(Normalize(value), ret);
			}
		}
	}
	return ret;
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
