#include "pch.h"
#include "CronParser.h"
#include "hnrt/Exception.h"


#define CRON_WC_STEP		1U
#define CRON_WC_ANY			2U
#define CRON_WC_LASTDAY		4U
#define CRON_WC_WEEKDAY		8U
#define CRON_WC_NTH			16U


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
		throw Exception(L"One or more extra characters follow.");
	}
}


static UINT GetFlags(CronElement element)
{
	switch (element)
	{
	case CRON_SECOND: return CRON_WC_STEP;
	case CRON_MINUTE: return CRON_WC_STEP;
	case CRON_HOUR: return CRON_WC_STEP;
	case CRON_DAYOFMONTH: return CRON_WC_STEP | CRON_WC_ANY | CRON_WC_LASTDAY | CRON_WC_WEEKDAY;
	case CRON_MONTH: return CRON_WC_STEP;
	case CRON_DAYOFWEEK: return CRON_WC_ANY | CRON_WC_LASTDAY | CRON_WC_NTH;
	case CRON_YEAR: return CRON_WC_STEP;
	default: throw Exception(L"CronParser::Run: Bad element: %d", element);
	}
}


static const PCWSTR* GetWords(CronElement element)
{
	switch (element)
	{
	case CRON_MONTH: return MonthWords;
	case CRON_DAYOFWEEK: return DayOfWeekWords;
	default: return nullptr;
	}
}


CronValue* CronParser::Run(CronElement element)
{
	return Run(element, GetFlags(element), GetWords(element));
}


CronValue* CronParser::Run(CronElement element, UINT flags, const PCWSTR* ppsz)
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

	while (true)
	{
		CronValue* pValue = nullptr;
		int value1 = ~0;
		int value2 = ~0;
		if ((flags & CRON_WC_LASTDAY) != 0 && m_next == L'L')
		{
			m_next = m_tokenizer.GetNext();
			pValue = CronValueLastDay::Create(element);
		}
		else if ((flags & CRON_WC_WEEKDAY) != 0 && m_next == L'W')
		{
			m_next = m_tokenizer.GetNext();
			pValue = CronValueWeekDay::Create(element);
		}
		else if (ParseInteger(element, CronValue::Min(element), CronValue::Max(element), ppsz, value1))
		{
			if (ParseRange(element, value1, ppsz, value2))
			{
				pValue = CronValueRange::Create(element, value1, value2);
				int value3 = ~0;
				if ((flags & CRON_WC_STEP) != 0 && ParseStep(element, value3))
				{
					pValue->range.step = value3;
				}
			}
			else if ((flags & CRON_WC_LASTDAY) != 0 && m_next == L'L')
			{
				if (value1 >= CRON_WORD_DISPLACEMENT)
				{
					throw Exception(L"Bad sequence for %s.", CronValue::Name(element));
				}
				m_next = m_tokenizer.GetNext();
				pValue = CronValueLastDayOfWeek::Create(element, value1);
			}
			else if ((flags & CRON_WC_WEEKDAY) != 0 && m_next == L'W')
			{
				if (value1 >= CRON_WORD_DISPLACEMENT)
				{
					throw Exception(L"Bad sequence for %s.", CronValue::Name(element));
				}
				m_next = m_tokenizer.GetNext();
				pValue = CronValueClosestWeekDay::Create(element, value1);
			}
			else if ((flags & CRON_WC_NTH) != 0 && m_next == L'#')
			{
				m_next = m_tokenizer.GetNext();
				if (ParseInteger(element, 1, 5, nullptr, value2))
				{
					pValue = CronValueNthDayOfWeek::Create(element, value1, value2);
				}
				else
				{
					throw Exception(L"Missing nth for %s.", CronValue::Name(element));
				}
			}
			else
			{
				pValue = CronValueSingle::Create(element, value1);
				int value3 = ~0;
				if ((flags & CRON_WC_STEP) != 0 && ParseStep(element, value3))
				{
					pValue->single.step = value3;
				}
			}
		}
		else
		{
			throw Exception(L"Missing %s.", CronValue::Name(element));
		}
		*ppNext = pValue;
		ppNext = &pValue->any.pNext;
		if (m_next == L',')
		{
			m_next = m_tokenizer.GetNext();
		}
		else
		{
			break;
		}
	}

	return pReturnValue;
}


bool CronParser::ParseRange(CronElement element, int min, const PCWSTR* ppsz, int& value)
{
	if (m_next == L'-')
	{
		m_next = m_tokenizer.GetNext();
		if (ParseInteger(element, (min >= CRON_WORD_DISPLACEMENT) ? (min - CRON_WORD_DISPLACEMENT) : min, CronValue::Max(element), ppsz, value))
		{
			return true;
		}
		else
		{
			throw Exception(L"Missing range to-value for %s.", CronValue::Name(element));
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
		if (ParseInteger(element, 1, CronValue::Max(element) - 1, nullptr, value))
		{
			return true;
		}
		else
		{
			throw Exception(L"Missing step for %s.", CronValue::Name(element));
		}
	}
	else
	{
		return false;
	}
}


bool CronParser::ParseInteger(CronElement element, int min, int max, const PCWSTR* ppsz, int& value)
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
			throw Exception(L"Illegal value for %s.", CronValue::Name(element));
		}
		return true;
	}
	else if (ppsz && m_next == CRON_TOKEN_WORD)
	{
		m_tokenizer.FindValue(ppsz, CronValue::Min(element));
		value = m_tokenizer.GetValue();
		if (min <= value && value <= max)
		{
			value += CRON_WORD_DISPLACEMENT;
			m_next = m_tokenizer.GetNext();
		}
		else
		{
			throw Exception(L"Illegal value for %s.", CronValue::Name(element));
		}
		return true;
	}
	else
	{
		return false;
	}
}
