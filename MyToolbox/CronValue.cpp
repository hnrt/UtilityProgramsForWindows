#include "pch.h"
#include "CronValue.h"
#include "CronTokenizer.h"
#include "hnrt/Buffer.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/String.h"
#include "hnrt/Exception.h"


using namespace hnrt;


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
			buf += CronMonthWords[value - min];
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
			buf += CronDayOfWeekWords[value - min];
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
	static RefPtr<CronValue> pValue = RefPtr<CronValue>(new CronValue(CRON_INVALID_VALUE, CRON_ELEMENT_UNSPECIFIED));
	return pValue;
}


CronValue::CronValue(CronValueType type, CronElement element)
	: RefObj()
	, m_type(type)
	, m_next()
	, m_element(element)
{
}


CronValue::~CronValue()
{
	m_next = nullptr;
}


void CronValue::Append(RefPtr<CronValue> pValue)
{
	if (m_next)
	{
		m_next->Append(pValue);
	}
	else
	{
		m_next = pValue;
	}
}


String CronValue::ToString() const
{
	StringBuffer buf(260);
	for (const CronValue* pCur = this; pCur; pCur = pCur->next.Ptr)
	{
		if (buf.Len)
		{
			buf.AppendFormat(L",");
		}
		switch (pCur->type)
		{
		case CRON_EMPTY:
			break;
		case CRON_ALL:
			buf.AppendFormat(L"*");
			break;
		case CRON_ANY:
			buf.AppendFormat(L"?");
			break;
		case CRON_SINGLE:
		{
			const CronValueSingle* pThis = dynamic_cast<const CronValueSingle*>(pCur);
			AppendIntegerOrWord(pThis->value, pThis->element, buf);
			if (pThis->step > 1)
			{
				buf.AppendFormat(L"/%d", pThis->step);
			}
			break;
		}
		case CRON_RANGE:
		{
			const CronValueRange* pThis = dynamic_cast<const CronValueRange*>(pCur);
			AppendIntegerOrWord(pThis->from, pThis->element, buf);
			buf.AppendFormat(L"-");
			AppendIntegerOrWord(pThis->to, pThis->element, buf);
			if (pThis->step > 1)
			{
				buf.AppendFormat(L"/%d", pThis->step);
			}
			break;
		}
		case CRON_LASTDAY:
			buf.AppendFormat(L"L");
			break;
		case CRON_WEEKDAY:
			buf.AppendFormat(L"W");
			break;
		case CRON_CLOSEST_WEEKDAY:
			buf.AppendFormat(L"%dW", dynamic_cast<const CronValueClosestWeekDay*>(pCur)->dom);
			break;
		case CRON_NTH_DAYOFWEEK:
		{
			const CronValueNthDayOfWeek* pThis = dynamic_cast<const CronValueNthDayOfWeek*>(pCur);
			AppendIntegerOrWord(pThis->dow, pThis->element, buf);
			buf.AppendFormat(L"#%d", pThis->nth);
			break;
		}
		case CRON_LAST_DAYOFWEEK:
			buf.AppendFormat(L"%dL", dynamic_cast<const CronValueLastDayOfWeek*>(pCur)->dow);
			break;
		default:
			break;
		}
	}
	return String(buf);
}


static int Adjust(int value, CronElement element, int offset)
{
	if (element == CRON_DAYOFMONTH)
	{
		return (((value - 1) + (offset / (24 * 60)) + 31) % 31) + 1;
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
		Buffer<int> tmp(buf.Len);
		memcpy_s(tmp.Ptr, tmp.Len * sizeof(int), buf.Ptr, buf.Len * sizeof(int));
		buf.Resize(buf.Len * 2);
		memcpy_s(buf.Ptr, buf.Len * sizeof(int), tmp.Ptr, tmp.Len * sizeof(int));
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


RefPtr<CronValueEvaluation> CronValue::Evaluate(int offset) const
{
	int min = Min(element);
	int max = Max(element);
	Buffer<int> samples(static_cast<size_t>(max) - static_cast<size_t>(min) + 1);
	size_t count = 0;
	for (const CronValue* pCur = this; pCur; pCur = pCur->next.Ptr)
	{
		switch (pCur->type)
		{
		case CRON_EMPTY:
			if (pCur->element == CRON_YEAR)
			{
				for (int next = min; next <= max; next++)
				{
					Add(samples, count, next);
				}
			}
			else if (pCur->element == CRON_SECOND)
			{
				Add(samples, count, 0);
			}
			break;
		case CRON_ALL:
			for (int next = min; next <= max; next++)
			{
				Add(samples, count, next);
			}
			break;
		case CRON_ANY:
			break;
		case CRON_SINGLE:
		{
			const CronValueSingle* pThis = dynamic_cast<const CronValueSingle*>(pCur);
			int value = CRON_NUMBER(pThis->value);
			if (pThis->step > 0)
			{
				for (int next = value; next <= max; next += pThis->step)
				{
					Add(samples, count, Adjust(next, element, offset));
				}
			}
			else
			{
				Add(samples, count, Adjust(value, element, offset));
			}
			break;
		}
		case CRON_RANGE:
		{
			const CronValueRange* pThis = dynamic_cast<const CronValueRange*>(pCur);
			int from = CRON_NUMBER(pThis->from);
			int to = CRON_NUMBER(pThis->to);
			for (int next = from; next <= to; next += pThis->step)
			{
				Add(samples, count, Adjust(next, element, offset));
			}
			break;
		}
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
	return RefPtr<CronValueEvaluation>(new CronValueEvaluation(samples.Detach(), static_cast<int>(count)));
}


int CronValue::Count(CronValueType type) const
{
	int count = 0;
	for (const CronValue* pCur = this; pCur; pCur = pCur->next.Ptr)
	{
		if (pCur->type == type)
		{
			count++;
		}
	}
	return count;
}


RefPtr<CronValue> CronValueEmpty::Create(CronElement element)
{
	return RefPtr<CronValue>(new CronValueEmpty(element));
}


RefPtr<CronValue> CronValueAll::Create(CronElement element)
{
	return RefPtr<CronValue>(new CronValueAll(element));
}


RefPtr<CronValue> CronValueAny::Create(CronElement element)
{
	return RefPtr<CronValue>(new CronValueAny(element));
}


RefPtr<CronValue> CronValueSingle::Create(CronElement element, int value, int step)
{
	return RefPtr<CronValue>(new CronValueSingle(element, value, step));
}


RefPtr<CronValue> CronValueRange::Create(CronElement element, int from, int to, int step)
{
	return RefPtr<CronValue>(new CronValueRange(element, from, to, step));
}


RefPtr<CronValue> CronValueLastDay::Create(CronElement element)
{
	return RefPtr<CronValue>(new CronValueLastDay(element));
}


RefPtr<CronValue> CronValueWeekDay::Create(CronElement element)
{
	return RefPtr<CronValue>(new CronValueWeekDay(element));
}


RefPtr<CronValue> CronValueClosestWeekDay::Create(CronElement element, int dom)
{
	return RefPtr<CronValue>(new CronValueClosestWeekDay(element, dom));
}


RefPtr<CronValue> CronValueNthDayOfWeek::Create(CronElement element, int dow, int nth)
{
	return RefPtr<CronValue>(new CronValueNthDayOfWeek(element, dow, nth));
}


RefPtr<CronValue> CronValueLastDayOfWeek::Create(CronElement element, int dow)
{
	return RefPtr<CronValue>(new CronValueLastDayOfWeek(element, dow));
}
