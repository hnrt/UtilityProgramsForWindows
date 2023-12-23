#include "pch.h"
#include "hnrt/CronValue.h"
#include "hnrt/CronTokenizer.h"
#include "hnrt/Buffer.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/String.h"
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


#if 0
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
#endif


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
