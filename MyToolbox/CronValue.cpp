#include "pch.h"
#include "CronValue.h"
#include "hnrt/Buffer.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/String.h"
#include "hnrt/Exception.h"


using namespace hnrt;


const PCWSTR hnrt::MonthWords[] = {
	L"JAN",
	L"FEB",
	L"MAR",
	L"APR",
	L"MAY",
	L"JUN",
	L"JUL",
	L"AUG",
	L"SEP",
	L"OCT",
	L"NOV",
	L"DEC",
	nullptr
};


const PCWSTR hnrt::DayOfWeekWords[] = {
	L"SUN",
	L"MON",
	L"TUE",
	L"WED",
	L"THU",
	L"FRI",
	L"SAT",
	nullptr
};


CronValue* CronValueAll::Create(CronElement element)
{
	CronValueAll* pThis = new CronValueAll();
	pThis->type = CRON_ALL;
	pThis->pNext = nullptr;
	pThis->element = element;
	return reinterpret_cast<CronValue*>(pThis);
}


CronValue* CronValueAny::Create(CronElement element)
{
	CronValueAny* pThis = new CronValueAny();
	pThis->type = CRON_ANY;
	pThis->pNext = nullptr;
	pThis->element = element;
	return reinterpret_cast<CronValue*>(pThis);
}


CronValue* CronValueSingle::Create(CronElement element, int value)
{
	CronValueSingle* pThis = new CronValueSingle();
	pThis->type = CRON_SINGLE;
	pThis->pNext = nullptr;
	pThis->element = element;
	pThis->step = 0;
	pThis->value = value;
	return reinterpret_cast<CronValue*>(pThis);
}


CronValue* CronValueRange::Create(CronElement element, int from, int to)
{
	CronValueRange* pThis = new CronValueRange();
	pThis->type = CRON_RANGE;
	pThis->pNext = nullptr;
	pThis->element = element;
	pThis->step = 1;
	pThis->from = from;
	pThis->to = to;
	return reinterpret_cast<CronValue*>(pThis);
}


CronValue* CronValueLastDay::Create(CronElement element)
{
	CronValueLastDay* pThis = new CronValueLastDay();
	pThis->type = CRON_LASTDAY;
	pThis->pNext = nullptr;
	pThis->element = element;
	return reinterpret_cast<CronValue*>(pThis);
}


CronValue* CronValueWeekDay::Create(CronElement element)
{
	CronValueWeekDay* pThis = new CronValueWeekDay();
	pThis->type = CRON_WEEKDAY;
	pThis->pNext = nullptr;
	pThis->element = element;
	return reinterpret_cast<CronValue*>(pThis);
}


CronValue* CronValueClosestWeekDay::Create(CronElement element, int dom)
{
	CronValueClosestWeekDay* pThis = new CronValueClosestWeekDay();
	pThis->type = CRON_CLOSEST_WEEKDAY;
	pThis->pNext = nullptr;
	pThis->element = element;
	pThis->unused = 0;
	pThis->dom = dom;
	return reinterpret_cast<CronValue*>(pThis);
}


CronValue* CronValueNthDayOfWeek::Create(CronElement element, int dow, int nth)
{
	CronValueNthDayOfWeek* pThis = new CronValueNthDayOfWeek();
	pThis->type = CRON_NTH_DAYOFWEEK;
	pThis->pNext = nullptr;
	pThis->element = element;
	pThis->unused = 0;
	pThis->dow = dow;
	pThis->nth = nth;
	return reinterpret_cast<CronValue*>(pThis);
}


CronValue* CronValueLastDayOfWeek::Create(CronElement element, int dow)
{
	CronValueLastDayOfWeek* pThis = new CronValueLastDayOfWeek();
	pThis->type = CRON_LAST_DAYOFWEEK;
	pThis->pNext = nullptr;
	pThis->element = element;
	pThis->unused = 0;
	pThis->dow = dow;
	return reinterpret_cast<CronValue*>(pThis);
}


PCWSTR CronValue::Name(CronElement element)
{
	switch (element)
	{
	case CRON_YEAR: return L"year";
	case CRON_MONTH: return L"month";
	case CRON_DAYOFMONTH: return L"day of the month";
	case CRON_DAYOFWEEK: return L"day of the week";
	case CRON_HOUR: return L"hour";
	case CRON_MINUTE: return L"minute";
	case CRON_SECOND: return L"second";
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


void CronValue::Free(CronValue*& pValue)
{
	if (pValue)
	{
		CronValue* pCur = pValue;
		while (pCur)
		{
			CronValue* pNext = pCur->pNext;
			delete pCur;
			pCur = pNext;
		}
		pValue = nullptr;
	}
}


int hnrt::CountCronValues(CronValue*& pValue)
{
	int count = 0;
	for (const CronValue* pCur = pValue; pCur; pCur = pCur->pNext)
	{
		count++;
	}
	return count;
}


int hnrt::CountCronValues(CronValue*& pValue, CronValueType type)
{
	int count = 0;
	for (const CronValue* pCur = pValue; pCur; pCur = pCur->pNext)
	{
		if (pCur->type == type)
		{
			count++;
		}
	}
	return count;
}


CronValue::operator PCWSTR() const
{
	return ToString();
}


PCWSTR CronValue::ToString() const
{
	StringBuffer buf(260);
	for (const CronValue* pCur = this; pCur; pCur = pCur->pNext)
	{
		if (buf.Len)
		{
			buf.AppendFormat(L",");
		}
		switch (pCur->type)
		{
		case CRON_ALL:
			buf.AppendFormat(L"*");
			break;
		case CRON_ANY:
			buf.AppendFormat(L"?");
			break;
		case CRON_SINGLE:
			if (pCur->single.step > 1)
			{
				buf.AppendFormat(L"%d/%d", pCur->single.value, pCur->single.step);
			}
			else
			{
				buf.AppendFormat(L"%d", pCur->single.value);
			}
			break;
		case CRON_RANGE:
			if (pCur->range.step > 1)
			{
				buf.AppendFormat(L"%d-%d/%d", pCur->range.from, pCur->range.to, pCur->range.step);
			}
			else
			{
				buf.AppendFormat(L"%d-%d", pCur->range.from, pCur->range.to);
			}
			break;
		case CRON_LASTDAY:
			buf.AppendFormat(L"L");
			break;
		case CRON_WEEKDAY:
			buf.AppendFormat(L"W");
			break;
		case CRON_CLOSEST_WEEKDAY:
			buf.AppendFormat(L"%dW", pCur->closestwd.dom);
			break;
		case CRON_NTH_DAYOFWEEK:
			buf.AppendFormat(L"%d#%d", pCur->nthdow.dow, pCur->nthdow.nth);
			break;
		case CRON_LAST_DAYOFWEEK:
			buf.AppendFormat(L"%dL", pCur->lastdow.dow);
			break;
		default:
			break;
		}
	}
	return String::Copy(buf);
}


static void Add(Buffer<int>& buf, size_t& count, int value)
{
	if (count + 1 > buf.Len)
	{
		buf.Resize(buf.Len * 2);
	}
	for (size_t index = 0; index < count; index++)
	{
		if (value < buf[index])
		{
			memmove_s(&buf[index + 1], (buf.Len - (index + 1)) * sizeof(int), &buf[index], (count - index) * sizeof(int));
			count++;
			buf[index] = value;
			return;
		}
		else if (value == buf[index])
		{
			return;
		}
	}
	buf[count++] = value;
}


PCWSTR CronValue::Evaluate() const
{
	StringBuffer buf(260);
	int min = Min(element);
	int max = Max(element);
	Buffer<int> samples(max - min + 1);
	if (element == CRON_YEAR)
	{
		SYSTEMTIME st = { 0 };
		GetLocalTime(&st);
		min = st.wYear;
	}
	size_t count = 0;
	for (const CronValue* pCur = this; pCur; pCur = pCur->pNext)
	{
		switch (pCur->type)
		{
		case CRON_ALL:
			for (int next = min; next <= max; next++)
			{
				Add(samples, count, next);
			}
			break;
		case CRON_ANY:
			break;
		case CRON_SINGLE:
			if (pCur->single.step > 0)
			{
				for (int next = pCur->single.value; next <= max; next += pCur->single.step)
				{
					Add(samples, count, next);
				}
			}
			else
			{
				Add(samples, count, pCur->single.value);
			}
			break;
		case CRON_RANGE:
			for (int next = pCur->range.from; next <= pCur->range.to; next += pCur->range.step)
			{
				Add(samples, count, next);
			}
			break;
		case CRON_LASTDAY:
			break;
		case CRON_WEEKDAY:
			break;
		case CRON_CLOSEST_WEEKDAY:
			break;
		case CRON_NTH_DAYOFWEEK:
			break;
		case CRON_LAST_DAYOFWEEK:
			break;
		default:
			break;
		}
	}
	if (count > 0)
	{
		buf.AppendFormat(L"%d", samples[0]);
		for (size_t index = 1; index < count; index++)
		{
			buf.AppendFormat(L" %d", samples[index]);
		}
	}
	return String::Copy(buf);
}


const CronValue* CronValue::Invalid()
{
	static CronValueAny value;
	value.type = CRON_INVALID;
	return const_cast<const CronValue*>(reinterpret_cast<CronValue*>(&value));
}
