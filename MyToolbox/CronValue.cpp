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


CronValue* CronValueSingle::Create(CronElement element, int value, int step)
{
	CronValueSingle* pThis = new CronValueSingle();
	pThis->type = CRON_SINGLE;
	pThis->pNext = nullptr;
	pThis->element = element;
	pThis->step = step;
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


CronValue* CronValueRange::Create(CronElement element, int from, int to, int step)
{
	CronValueRange* pThis = new CronValueRange();
	pThis->type = CRON_RANGE;
	pThis->pNext = nullptr;
	pThis->element = element;
	pThis->step = step;
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


CronValue::operator PCWSTR() const
{
	return ToString();
}


static void AppendIntegerOrWord(int value, CronElement element, StringBuffer& buf)
{
	int min = CronValue::Min(element);
	int max = CronValue::Max(element);
	if (min <= value && value <= max)
	{
		buf.AppendFormat(L"%d", value);
	}
	else if (element == CRON_MONTH)
	{
		value -= CRON_WORD_DISPLACEMENT;
		if (min <= value && value <= max)
		{
			buf.AppendFormat(L"%s", MonthWords[value - min]);
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
			buf.AppendFormat(L"%s", DayOfWeekWords[value - min]);
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
			AppendIntegerOrWord(pCur->single.value, pCur->single.element, buf);
			if (pCur->single.step > 1)
			{
				buf.AppendFormat(L"/%d", pCur->single.step);
			}
			break;
		case CRON_RANGE:
			AppendIntegerOrWord(pCur->range.from, pCur->range.element, buf);
			buf.AppendFormat(L"-");
			AppendIntegerOrWord(pCur->range.to, pCur->range.element, buf);
			if (pCur->range.step > 1)
			{
				buf.AppendFormat(L"/%d", pCur->range.step);
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
			AppendIntegerOrWord(pCur->nthdow.dow, pCur->nthdow.element, buf);
			buf.AppendFormat(L"#%d", pCur->nthdow.nth);
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


static int Adjust(int value, CronElement element, int offset)
{
	if (element == CRON_DAYOFMONTH)
	{
		return (value + (offset / (24 * 60)) + 32) % 32;
	}
	else if (element == CRON_HOUR)
	{
		return (value + (offset / 60) + 24) % 24;
	}
	else if (element == CRON_MINUTE)
	{
		return (value + (offset % 60) + 60) % 60;
	}
	else
	{
		return value;
	}
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


PCWSTR CronValue::Evaluate(int offset) const
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
		int value1;
		int value2;
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
			value1 = pCur->single.value >= CRON_WORD_DISPLACEMENT ? pCur->single.value - CRON_WORD_DISPLACEMENT : pCur->single.value;
			if (pCur->single.step > 0)
			{
				for (int next = value1; next <= max; next += pCur->single.step)
				{
					Add(samples, count, Adjust(next, element, offset));
				}
			}
			else
			{
				Add(samples, count, Adjust(value1, element, offset));
			}
			break;
		case CRON_RANGE:
			value1 = pCur->range.from >= CRON_WORD_DISPLACEMENT ? pCur->range.from - CRON_WORD_DISPLACEMENT : pCur->range.from;
			value2 = pCur->range.to >= CRON_WORD_DISPLACEMENT ? pCur->range.to - CRON_WORD_DISPLACEMENT : pCur->range.to;
			for (int next = value1; next <= value2; next += pCur->range.step)
			{
				Add(samples, count, Adjust(next, element, offset));
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


int CronValue::Count() const
{
	int count = 0;
	for (const CronValue* pCur = this; pCur; pCur = pCur->pNext)
	{
		count++;
	}
	return count;
}


int CronValue::Count(CronValueType type) const
{
	int count = 0;
	for (const CronValue* pCur = this; pCur; pCur = pCur->pNext)
	{
		if (pCur->type == type)
		{
			count++;
		}
	}
	return count;
}


const CronValue* CronValue::Invalid()
{
	static CronValueAny value;
	value.type = CRON_INVALID_VALUE;
	return const_cast<const CronValue*>(reinterpret_cast<CronValue*>(&value));
}
