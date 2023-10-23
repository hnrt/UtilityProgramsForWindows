#include "pch.h"
#include "CronParser.h"
#include "CronError.h"
#include "hnrt/Exception.h"


#define CRON_WC_STEP             1U
#define CRON_WC_ANY              2U
#define CRON_WC_LASTDAY          4U
#define CRON_WC_WEEKDAY          8U
#define CRON_WC_CLOSEST_WEEKDAY 16U
#define CRON_WC_NTH_DAYOFWEEK   32U
#define CRON_WC_LAST_DAYOFWEEK  64U


using namespace hnrt;


CronParser::CronParser(PCWSTR psz)
	: m_tokenizer(psz)
	, m_next(m_tokenizer.GetNext())
{
}


void CronParser::Run(CronValue*& pYear, CronValue*& pMonth, CronValue*& pDayOfMonth, CronValue*& pDayOfWeek, CronValue*& pHour, CronValue*& pMinute)
{
	pMinute = Run(CRON_MINUTE);
	pHour = Run(CRON_HOUR);
	pDayOfMonth = Run(CRON_DAYOFMONTH);
	pMonth = Run(CRON_MONTH);
	pDayOfWeek = Run(CRON_DAYOFWEEK);
	pYear = Run(CRON_YEAR);

	if (!isEnd())
	{
		throw CronError(CRON_ERROR_EXTRACHARACTER, CRON_ELEMENT_UNSPECIFIED, m_tokenizer.GetOffset());
	}

	if (pDayOfMonth->type == CRON_ALL)
	{
		if (pDayOfWeek->type == CRON_ALL)
		{
			throw CronError(CRON_ERROR_DUPLICATEALL, CRON_ELEMENT_UNSPECIFIED, m_tokenizer.GetOffset());
		}
		else if (pDayOfWeek->type != CRON_ANY)
		{
			throw CronError(CRON_ERROR_ANYDAYOFWEEKREQUIRED, CRON_ELEMENT_UNSPECIFIED, m_tokenizer.GetOffset());
		}
	}
	else if (pDayOfMonth->type == CRON_ANY)
	{
		if (pDayOfWeek->type == CRON_ANY)
		{
			throw CronError(CRON_ERROR_DUPLICATEANY, CRON_ELEMENT_UNSPECIFIED, m_tokenizer.GetOffset());
		}
	}
	else if (pDayOfWeek->type == CRON_ALL)
	{
		if (pDayOfMonth->type != CRON_ANY)
		{
			throw CronError(CRON_ERROR_ANYDAYOFMONTHREQUIRED, CRON_ELEMENT_UNSPECIFIED, m_tokenizer.GetOffset());
		}
	}
	else if (pDayOfWeek->type != CRON_ANY)
	{
		throw CronError(CRON_ERROR_ANYDAYREQUIRED, CRON_ELEMENT_UNSPECIFIED, m_tokenizer.GetOffset());
	}

	if (pDayOfWeek->Count(CRON_NTH_DAYOFWEEK) > 1)
	{
		throw CronError(CRON_ERROR_MULTIPLEORDINAL, CRON_ELEMENT_UNSPECIFIED, m_tokenizer.GetOffset());
	}
}


void CronParser::Run(CronValue*& pYear, CronValue*& pMonth, CronValue*& pDayOfMonth, CronValue*& pDayOfWeek, CronValue*& pHour, CronValue*& pMinute, CronValue*& pSecond)
{
	pSecond = Run(CRON_SECOND);
	Run(pYear, pMonth, pDayOfMonth, pDayOfWeek, pHour, pMinute);
}


CronValue* CronParser::RunOnlyFor(CronElement element)
{
	CronValue* pValue = Run(element);
	if (isEnd())
	{
		return pValue;
	}
	else
	{
		CronValue::Free(pValue);
		throw CronError(CRON_ERROR_EXTRACHARACTER, element, m_tokenizer.GetOffset());
	}
}


static UINT GetFlags(CronElement element)
{
	switch (element)
	{
	case CRON_SECOND: return CRON_WC_STEP;
	case CRON_MINUTE: return CRON_WC_STEP;
	case CRON_HOUR: return CRON_WC_STEP;
	case CRON_DAYOFMONTH: return CRON_WC_STEP | CRON_WC_ANY | CRON_WC_LASTDAY | CRON_WC_WEEKDAY | CRON_WC_CLOSEST_WEEKDAY;
	case CRON_MONTH: return CRON_WC_STEP;
	case CRON_DAYOFWEEK: return CRON_WC_ANY | CRON_WC_NTH_DAYOFWEEK | CRON_WC_LAST_DAYOFWEEK;
	case CRON_YEAR: return CRON_WC_STEP;
	default: throw Exception(L"CronParser::Run: Bad element: %d", element);
	}
}


CronValue* CronParser::Run(CronElement element)
{
	return Run(element, GetFlags(element));
}


CronValue* CronParser::Run(CronElement element, UINT flags)
{
	m_tokenizer.SetElement(element);

	if (element == CRON_YEAR)
	{
		if (m_next == CRON_TOKEN_EOF)
		{
			return CronValueEmpty::Create(CRON_YEAR);
		}
	}

	if (m_next == L'*')
	{
		m_next = m_tokenizer.GetNext();
		return CronValueAll::Create(element);
	}
	else if ((flags & CRON_WC_ANY) != 0 && m_next == L'?')
	{
		m_next = m_tokenizer.GetNext();
		return CronValueAny::Create(element);
	}

	CronValue* pReturnValue = nullptr;
	CronValue** ppNext = &pReturnValue;

	try
	{
		while (true)
		{
			CronValue* pValue = nullptr;
			int value1 = ~0;
			int value2 = ~0;
			int value3 = ~0;
			if ((flags & CRON_WC_WEEKDAY) != 0 && m_next == L'W')
			{
				m_next = m_tokenizer.GetNext();
				pValue = CronValueWeekDay::Create(element);
			}
			else if ((flags & CRON_WC_LASTDAY) != 0 && m_next == L'L')
			{
				m_next = m_tokenizer.GetNext();
				pValue = CronValueLastDay::Create(element);
			}
			else if (ParseIntegerOrLabel(element, CronValue::Min(element), CronValue::Max(element), value1))
			{
				if (ParseRange(element, value1, value2))
				{
					if ((flags & CRON_WC_STEP) != 0 && ParseStep(element, value3))
					{
						pValue = CronValueRange::Create(element, value1, value2, value3);
					}
					else
					{
						pValue = CronValueRange::Create(element, value1, value2);
					}
				}
				else if ((flags & CRON_WC_NTH_DAYOFWEEK) != 0 && ParseOrdinal(value2))
				{
					pValue = CronValueNthDayOfWeek::Create(element, value1, value2);
				}
				else if ((flags & CRON_WC_STEP) != 0 && ParseStep(element, value3))
				{
					pValue = CronValueSingle::Create(element, value1, value3);
				}
				else
				{
					pValue = CronValueSingle::Create(element, value1);
				}
			}
			else if ((flags & CRON_WC_CLOSEST_WEEKDAY) != 0 && ParseIntegerW(value1))
			{
				pValue = CronValueClosestWeekDay::Create(element, value1);
			}
			else if ((flags & CRON_WC_LAST_DAYOFWEEK) != 0 && ParseIntegerL(value1))
			{
				pValue = CronValueLastDayOfWeek::Create(element, value1);
			}
			else
			{
				throw CronError(CRON_ERROR_BADSEQUENCE, element, m_tokenizer.GetOffset());
			}
			*ppNext = pValue;
			ppNext = &pValue->any.pNext;
			if (m_next == L',')
			{
				m_next = m_tokenizer.GetNext();
			}
			else
			{
				return pReturnValue;
			}
		}
	}
	catch (CronError e)
	{
		CronValue::Free(pReturnValue);
		throw e;
	}
}


bool CronParser::ParseRange(CronElement element, int min, int& value)
{
	if (m_next == L'-')
	{
		m_next = m_tokenizer.GetNext();
		if (ParseIntegerOrLabel(element, CRON_NUMBER(min), CronValue::Max(element), value))
		{
			return true;
		}
		else
		{
			throw CronError(CRON_ERROR_NOUPPERBOUND, element, m_tokenizer.GetOffset());
		}
	}
	else
	{
		return false;
	}
}


bool CronParser::ParseStep(CronElement element, int& value)
{
	if (m_next == L'/')
	{
		m_next = m_tokenizer.GetNext();
		if (ParseInteger(element, 1, CronValue::Max(element) - 1, value))
		{
			return true;
		}
		else
		{
			throw CronError(CRON_ERROR_NOSTEP, element, m_tokenizer.GetOffset());
		}
	}
	else
	{
		return false;
	}
}


bool CronParser::ParseOrdinal(int& value)
{
	if (m_next == L'#')
	{
		m_next = m_tokenizer.GetNext();
		if (ParseInteger(CRON_DAYOFWEEK, 1, (31 + 7 - 1) / 7, value))
		{
			return true;
		}
		else
		{
			throw CronError(CRON_ERROR_NOORDINALNUMBER, CRON_DAYOFWEEK, m_tokenizer.GetOffset());
		}
	}
	else
	{
		return false;
	}
}


bool CronParser::ParseIntegerOrLabel(CronElement element, int min, int max, int& value)
{
	if (m_next == CRON_TOKEN_INTEGER)
	{
		value = m_tokenizer.GetValue();
		if (min <= value && value <= max)
		{
			m_next = m_tokenizer.GetNext();
		}
		else
		{
			throw CronError(CRON_ERROR_OUTOFRANGE, element, m_tokenizer.GetOffset());
		}
		return true;
	}
	else if (element == CRON_MONTH && CRON_TOKEN_MONTH_MIN <= m_next && m_next <= CRON_TOKEN_MONTH_MAX)
	{
		value = m_next - CRON_TOKEN_MONTH_MIN + CronValue::Min(CRON_MONTH) + CRON_WORD_DISPLACEMENT;
		m_next = m_tokenizer.GetNext();
		return true;
	}
	else if (element == CRON_DAYOFWEEK && CRON_TOKEN_DAYOFWEEK_MIN <= m_next && m_next <= CRON_TOKEN_DAYOFWEEK_MAX)
	{
		value = m_next - CRON_TOKEN_DAYOFWEEK_MIN + CronValue::Min(CRON_DAYOFWEEK) + CRON_WORD_DISPLACEMENT;
		m_next = m_tokenizer.GetNext();
		return true;
	}
	else
	{
		return false;
	}
}


bool CronParser::ParseInteger(CronElement element, int min, int max, int& value)
{
	if (m_next == CRON_TOKEN_INTEGER)
	{
		value = m_tokenizer.GetValue();
		if (min <= value && value <= max)
		{
			m_next = m_tokenizer.GetNext();
		}
		else
		{
			throw CronError(CRON_ERROR_OUTOFRANGE, element, m_tokenizer.GetOffset());
		}
		return true;
	}
	else
	{
		return false;
	}
}


bool CronParser::ParseIntegerW(int& value)
{
	if (m_next == CRON_TOKEN_INTEGER_W)
	{
		value = m_tokenizer.GetValue();
		if (CronValue::Min(CRON_DAYOFMONTH) <= value && value <= CronValue::Max(CRON_DAYOFMONTH))
		{
			m_next = m_tokenizer.GetNext();
		}
		else
		{
			throw CronError(CRON_ERROR_OUTOFRANGE, CRON_DAYOFMONTH, m_tokenizer.GetOffset());
		}
		return true;
	}
	else
	{
		return false;
	}
}


bool CronParser::ParseIntegerL(int& value)
{
	if (m_next == CRON_TOKEN_INTEGER_L)
	{
		value = m_tokenizer.GetValue();
		if (CronValue::Min(CRON_DAYOFWEEK) <= value && value <= CronValue::Max(CRON_DAYOFWEEK))
		{
			m_next = m_tokenizer.GetNext();
		}
		else
		{
			throw CronError(CRON_ERROR_OUTOFRANGE, CRON_DAYOFWEEK, m_tokenizer.GetOffset());
		}
		return true;
	}
	else
	{
		return false;
	}
}
