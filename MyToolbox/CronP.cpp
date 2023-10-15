#include "pch.h"
#include "CronP.h"
#include "CronValue.h"
#include "hnrt/Exception.h"


using namespace hnrt;


#define FORMAT_BADSEQUENCE L"Error: Bad sequence at %u."
#define FORMAT_BADCHARACTOR L"Error: Bad character %c at %u."


CronTokenizer::CronTokenizer(PCWSTR psz)
	: m_o(psz)
	, m_p(psz + 1)
	, m_c(*psz)
	, m_v(0)
	, m_s()
{
}


int CronTokenizer::GetNext()
{
	int sym;
	while (iswspace(m_c))
	{
		m_c = *m_p++;
	}
	if (m_c == 0)
	{
		sym = CRON_TOKEN_EOF;
	}
	else if (iswdigit(m_c))
	{
		WCHAR sz[32] = { 0 };
		WCHAR* psz = sz;
		*psz++ = m_c;
		*psz = 0;
		m_v = wcstoul(sz, nullptr, 10);
		m_c = *m_p++;
		while (iswdigit(m_c))
		{
			*psz++ = m_c;
			*psz = 0;
			m_v = wcstoul(sz, nullptr, 10);
			psz = sz + swprintf_s(sz, L"%d", m_v);
			m_c = *m_p++;
		}
		if (m_c == L'\0' || m_c == L' ' || m_c == L',' || m_c == L'-' || m_c == L'/' || m_c == L'L' || m_c == L'W' || m_c == L'#')
		{
			sym = CRON_TOKEN_INTEGER;
		}
		else
		{
			throw Exception(FORMAT_BADSEQUENCE, static_cast<UINT>(m_p - 1 - m_o));
		}
	}
	else if (iswalpha(m_c))
	{
		WCHAR* psz = m_s;
		*psz++ = m_c;
		*psz = 0;
		m_c = *m_p++;
		if (iswalpha(m_c))
		{
			*psz++ = m_c;
			*psz = 0;
			m_c = *m_p++;
			if (iswalpha(m_c))
			{
				*psz++ = m_c;
				*psz = 0;
				m_c = *m_p++;
			}
			else
			{
				throw Exception(FORMAT_BADSEQUENCE, static_cast<UINT>(m_p - 1 - m_o));
			}
			if (m_c == L'\0' || m_c == L' ' || m_c == L',' || m_c == L'-' || m_c == L'/' || m_c == L'#')
			{
				sym = CRON_TOKEN_WORD;
			}
			else
			{
				throw Exception(FORMAT_BADSEQUENCE, static_cast<UINT>(m_p - 1 - m_o));
			}
		}
		else if (m_s[0] == L'L' || m_s[0] == L'W')
		{
			if (m_c == L'\0' || m_c == L' ' || m_c == L',')
			{
				sym = m_s[0];
			}
			else
			{
				throw Exception(FORMAT_BADSEQUENCE, static_cast<UINT>(m_p - 1 - m_o));
			}
		}
		else
		{
			throw Exception(FORMAT_BADSEQUENCE, static_cast<UINT>(m_p - 1 - m_o));
		}
	}
	else
	{
		switch (m_c)
		{
		case L'*':
		case L'?':
			sym = m_c;
			m_c = *m_p++;
			if (!(m_c == L'\0' || m_c == L' '))
			{
				throw Exception(FORMAT_BADSEQUENCE, static_cast<UINT>(m_p - 1 - m_o));
			}
			break;
		case L'-':
		case L',':
		case L'/':
		case L'#':
			sym = m_c;
			m_c = *m_p++;
			break;
		default:
			throw Exception(FORMAT_BADCHARACTOR, m_c, static_cast<UINT>(m_p - 1 - m_o));
		}
	}
	return sym;
}


void CronTokenizer::FindValue(const PCWSTR* ppsz, int base)
{
	unsigned int index = 0;
	while (ppsz[index])
	{
		if (!wcscmp(ppsz[index], m_s))
		{
			m_v = base + index;
			return;
		}
		index++;
	}
	throw Exception(FORMAT_BADSEQUENCE, static_cast<UINT>(m_p - 4 - m_o));
}


CronParser::CronParser(PCWSTR psz)
	: m_tokenizer(psz)
	, m_next(m_tokenizer.GetNext())
{
}


bool CronParser::isEOF() const
{
	return m_next == CRON_TOKEN_EOF;
}


union CronValue* CronParser::Run(CronElement element, UINT flags, const PCWSTR* ppsz)
{
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
				if ((flags & CRON_WC_STEP) != 0 && ParseStep(element, value1, ppsz, value3))
				{
					pValue->range.step = value3;
				}
			}
			else if ((flags & CRON_WC_LASTDAY) != 0 && m_next == L'L')
			{
				m_next = m_tokenizer.GetNext();
				pValue = CronValueLastDayOfWeek::Create(element, value1);
			}
			else if ((flags & CRON_WC_WEEKDAY) != 0 && m_next == L'W')
			{
				m_next = m_tokenizer.GetNext();
				pValue = CronValueClosestWeekDay::Create(element, value1);
			}
			else if ((flags & CRON_WC_NTH) != 0 && m_next == L'#')
			{
				m_next = m_tokenizer.GetNext();
				if (ParseInteger(element, 1, 5, ppsz, value2))
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
				if ((flags & CRON_WC_STEP) != 0 && ParseStep(element, value1, ppsz, value3))
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
		if (ParseInteger(element, min, CronValue::Max(element), ppsz, value))
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


bool CronParser::ParseStep(CronElement element, int min, const PCWSTR* ppsz, int& value)
{
	if (m_next == L'/')
	{
		m_next = m_tokenizer.GetNext();
		if (ParseInteger(element, 1, CronValue::Max(element) - min, ppsz, value))
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
		m_tokenizer.FindValue(ppsz, min);
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
	else
	{
		return false;
	}
}
