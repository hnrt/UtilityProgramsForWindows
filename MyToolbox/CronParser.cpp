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


static Exception TranslateError(const CronError& e)
{
	switch (e.type)
	{
	case CRON_ERROR_OUTOFRANGE:
		return Exception(L"Out of range at %d.", e.offset);
	case CRON_ERROR_NOSTEP:
		return Exception(L"Missing step number at %d.", e.offset);
	case CRON_ERROR_NOUPPERBOUND:
		return Exception(L"Missing upper bound at %d.", e.offset);
	case CRON_ERROR_NOORDINALNUMBER:
		return Exception(L"Missing ordinal number at %d.", e.offset);
	case CRON_ERROR_BADSEQUENCE:
		return Exception(L"Bad sequence at %d.", e.offset);
	default:
		return Exception(L"Unknown error: type=%d offset=%d", e.type, e.offset);
	}
}


static Exception TranslateErrorWithName(const CronError& e)
{
	switch (e.type)
	{
	case CRON_ERROR_OUTOFRANGE:
		return Exception(L"%s: Out of range at %d.", CronValue::Name(e.element), e.offset);
	case CRON_ERROR_NOSTEP:
		return Exception(L"%s: Missing step number at %d.", CronValue::Name(e.element), e.offset);
	case CRON_ERROR_NOUPPERBOUND:
		return Exception(L"%s: Missing upper bound at %d.", CronValue::Name(e.element), e.offset);
	case CRON_ERROR_NOORDINALNUMBER:
		return Exception(L"%s: Missing ordinal number at %d.", CronValue::Name(e.element), e.offset);
	case CRON_ERROR_BADSEQUENCE:
		return Exception(L"%s: Bad sequence at %d.", CronValue::Name(e.element), e.offset);
	default:
		return Exception(L"Unknown error: type=%d element=%s offset=%d", e.type, CronValue::Name(e.element), e.offset);
	}
}


CronParser::CronParser(PCWSTR psz)
	: m_tokenizer(psz)
	, m_next(0)
{
	try
	{
		m_next = m_tokenizer.GetNext();
	}
	catch (CronError e)
	{
		throw TranslateError(e);
	}
}


void CronParser::Run(CronValue*& pYear, CronValue*& pMonth, CronValue*& pDayOfMonth, CronValue*& pDayOfWeek, CronValue*& pHour, CronValue*& pMinute)
{
	try
	{
		pMinute = Run(CRON_MINUTE);
		pHour = Run(CRON_HOUR);
		pDayOfMonth = Run(CRON_DAYOFMONTH);
		pMonth = Run(CRON_MONTH);
		pDayOfWeek = Run(CRON_DAYOFWEEK);
		pYear = Run(CRON_YEAR);
	}
	catch (CronError e)
	{
		throw TranslateErrorWithName(e);
	}

	if (!isEnd())
	{
		throw Exception(L"One or more extra characters follow.");
	}

	if (pDayOfMonth->type == CRON_ALL)
	{
		if (pDayOfWeek->type == CRON_ALL)
		{
			throw Exception(L"Both day of the month and day of the week cannot be *.");
		}
		else if (pDayOfWeek->type != CRON_ANY)
		{
			throw Exception(L"Day of the week must be ? when day of the month is *.");
		}
	}
	else if (pDayOfMonth->type == CRON_ANY)
	{
		if (pDayOfWeek->type == CRON_ANY)
		{
			throw Exception(L"Both day of the month and day of the week cannot be ?.");
		}
	}
	else if (pDayOfWeek->type == CRON_ALL)
	{
		if (pDayOfMonth->type != CRON_ANY)
		{
			throw Exception(L"Day of the month must be ? when day of the week is *.");
		}
	}
	else if (pDayOfWeek->type != CRON_ANY)
	{
		throw Exception(L"Either day of the month or day of the week must be ?.");
	}

	if (pDayOfWeek->Count(CRON_NTH_DAYOFWEEK) > 1)
	{
		throw Exception(L"# cannot be specified two or more times.");
	}
}


void CronParser::Run(CronValue*& pYear, CronValue*& pMonth, CronValue*& pDayOfMonth, CronValue*& pDayOfWeek, CronValue*& pHour, CronValue*& pMinute, CronValue*& pSecond)
{
	try
	{
		pSecond = Run(CRON_SECOND);
	}
	catch (CronError e)
	{
		throw TranslateErrorWithName(e);
	}
	Run(pYear, pMonth, pDayOfMonth, pDayOfWeek, pHour, pMinute);
}


CronValue* CronParser::RunOnlyFor(CronElement element)
{
	try
	{
		CronValue* pValue = Run(element);
		if (isEnd())
		{
			return pValue;
		}
		else
		{
			CronValue::Free(pValue);
			throw Exception(L"Extra character at %d.", m_tokenizer.GetOffset());
		}
	}
	catch (CronError e)
	{
		throw TranslateError(e);
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
	if (element == CRON_YEAR)
	{
		if (m_next == CRON_TOKEN_EOF)
		{
			return nullptr;
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
					int value3 = ~0;
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
				else
				{
					int value3 = ~0;
					if ((flags & CRON_WC_STEP) != 0 && ParseStep(element, value3))
					{
						pValue = CronValueSingle::Create(element, value1, value3);
					}
					else
					{
						pValue = CronValueSingle::Create(element, value1);
					}
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
		if (ParseIntegerOrLabel(element, (min >= CRON_WORD_DISPLACEMENT) ? (min - CRON_WORD_DISPLACEMENT) : min, CronValue::Max(element), value))
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
