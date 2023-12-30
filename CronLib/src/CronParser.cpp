#include "pch.h"
#include "hnrt/CronParser.h"
#include "hnrt/Cron.h"
#include "hnrt/CronError.h"
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


void CronParser::Run(Cron& cron)
{
	if (cron.m_bSecond)
	{
		cron.m_pSecond = Run(CRON_SECOND);
	}
	else if (!cron.m_pSecond)
	{
		cron.m_pSecond = CronValue::CreateAll(CRON_SECOND);
	}
	cron.m_pMinute = Run(CRON_MINUTE);
	cron.m_pHour = Run(CRON_HOUR);
	cron.m_pDayOfMonth = Run(CRON_DAYOFMONTH);
	cron.m_pMonth = Run(CRON_MONTH);
	cron.m_pDayOfWeek = Run(CRON_DAYOFWEEK);
	cron.m_pYear = Run(CRON_YEAR);

	if (!isEnd())
	{
		throw CronError(CRON_ERROR_EXTRACHARACTER, CRON_ELEMENT_UNSPECIFIED, m_tokenizer.GetOffset());
	}

	if (cron.m_pDayOfMonth->Type == CRON_ALL)
	{
		if (cron.m_pDayOfWeek->Type == CRON_ALL)
		{
			throw CronError(CRON_ERROR_DUPLICATEALL, CRON_ELEMENT_UNSPECIFIED, m_tokenizer.GetOffset());
		}
		else if (cron.m_pDayOfWeek->Type != CRON_ANY)
		{
			throw CronError(CRON_ERROR_ANYDAYOFWEEKREQUIRED, CRON_ELEMENT_UNSPECIFIED, m_tokenizer.GetOffset());
		}
	}
	else if (cron.m_pDayOfMonth->Type == CRON_ANY)
	{
		if (cron.m_pDayOfWeek->Type == CRON_ANY)
		{
			throw CronError(CRON_ERROR_DUPLICATEANY, CRON_ELEMENT_UNSPECIFIED, m_tokenizer.GetOffset());
		}
	}
	else if (cron.m_pDayOfWeek->Type == CRON_ALL)
	{
		if (cron.m_pDayOfMonth->Type != CRON_ANY)
		{
			throw CronError(CRON_ERROR_ANYDAYOFMONTHREQUIRED, CRON_ELEMENT_UNSPECIFIED, m_tokenizer.GetOffset());
		}
	}
	else if (cron.m_pDayOfWeek->Type != CRON_ANY)
	{
		throw CronError(CRON_ERROR_ANYDAYREQUIRED, CRON_ELEMENT_UNSPECIFIED, m_tokenizer.GetOffset());
	}

	if (cron.m_pDayOfWeek->Count(CRON_NTH_DAYOFWEEK) > 1)
	{
		throw CronError(CRON_ERROR_MULTIPLEORDINAL, CRON_ELEMENT_UNSPECIFIED, m_tokenizer.GetOffset());
	}
}


RefPtr<CronValue> CronParser::RunOnlyFor(CronElement element)
{
	RefPtr<CronValue> pValue = Run(element);
	if (isEnd())
	{
		return pValue;
	}
	else
	{
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


RefPtr<CronValue> CronParser::Run(CronElement element)
{
	return Run(element, GetFlags(element));
}


RefPtr<CronValue> CronParser::Run(CronElement element, UINT flags)
{
	m_tokenizer.SetElement(element);

	if (element == CRON_YEAR)
	{
		if (m_next == CRON_TOKEN_EOF)
		{
			return CronValue::CreateEmpty(CRON_YEAR);
		}
	}

	if (m_next == L'*')
	{
		m_next = m_tokenizer.GetNext();
		return CronValue::CreateAll(element);
	}
	else if ((flags & CRON_WC_ANY) != 0 && m_next == L'?')
	{
		m_next = m_tokenizer.GetNext();
		return CronValue::CreateAny(element);
	}

	RefPtr<CronValue> pReturnValue;

	while (true)
	{
		RefPtr<CronValue> pValue;
		int value1 = ~0;
		int value2 = ~0;
		int value3 = ~0;
		if ((flags & CRON_WC_WEEKDAY) != 0 && m_next == L'W')
		{
			m_next = m_tokenizer.GetNext();
			pValue = CronValue::CreateWeekDay(element);
		}
		else if ((flags & CRON_WC_LASTDAY) != 0 && m_next == L'L')
		{
			m_next = m_tokenizer.GetNext();
			pValue = CronValue::CreateLastDay(element);
		}
		else if (ParseIntegerOrLabel(element, value1))
		{
			if (ParseRange(element, value2))
			{
				if ((flags & CRON_WC_STEP) != 0 && ParseStep(element, value3))
				{
					pValue = CronValue::CreateRange(element, value1, value2, value3);
				}
				else
				{
					pValue = CronValue::CreateRange(element, value1, value2);
				}
			}
			else if ((flags & CRON_WC_NTH_DAYOFWEEK) != 0 && ParseOrdinal(value2))
			{
				pValue = CronValue::CreateNthDayOfWeek(element, value1, value2);
			}
			else if ((flags & CRON_WC_STEP) != 0 && ParseStep(element, value3))
			{
				pValue = CronValue::CreateSingle(element, value1, value3);
			}
			else
			{
				pValue = CronValue::CreateSingle(element, value1);
			}
		}
		else if ((flags & CRON_WC_CLOSEST_WEEKDAY) != 0 && ParseIntegerW(value1))
		{
			pValue = CronValue::CreateClosestWeekDay(element, value1);
		}
		else if ((flags & CRON_WC_LAST_DAYOFWEEK) != 0 && ParseIntegerL(value1))
		{
			pValue = CronValue::CreateLastDayOfWeek(element, value1);
		}
		else
		{
			throw CronError(CRON_ERROR_BADSEQUENCE, element, m_tokenizer.GetOffset());
		}
		if (pReturnValue)
		{
			pReturnValue->Append(pValue);
		}
		else
		{
			pReturnValue = pValue;
		}
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


bool CronParser::ParseRange(CronElement element, int& value)
{
	if (m_next == L'-')
	{
		m_next = m_tokenizer.GetNext();
		if (ParseIntegerOrLabel(element, value))
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
		if (ParseInteger(element, 1, CronValue::Max(element) - CronValue::Min(element) + 1, value))
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


inline int MinOrdinal() {
	return 1;
}


inline int MaxOrdinal() {
	int dom = CronValue::Max(CRON_DAYOFMONTH) - CronValue::Min(CRON_DAYOFMONTH) + 1; // 31
	int dow = CronValue::Max(CRON_DAYOFWEEK) - CronValue::Min(CRON_DAYOFWEEK) + 1; // 7
	return (dom + dow - 1) / dow;
}


bool CronParser::ParseOrdinal(int& value)
{
	if (m_next == L'#')
	{
		m_next = m_tokenizer.GetNext();
		if (ParseInteger(CRON_DAYOFWEEK, MinOrdinal(), MaxOrdinal(), value))
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


bool CronParser::ParseIntegerOrLabel(CronElement element, int& value)
{
	if (m_next == CRON_TOKEN_INTEGER)
	{
		value = m_tokenizer.GetValue();
		if (CronValue::Min(element) <= value && value <= CronValue::Max(element))
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
