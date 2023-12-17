#include "pch.h"
#include "hnrt/CronTokenizer.h"
#include "hnrt/CronError.h"


const PCWSTR hnrt::CronMonthWords[] = {
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


const PCWSTR hnrt::CronDayOfWeekWords[] = {
	L"SUN",
	L"MON",
	L"TUE",
	L"WED",
	L"THU",
	L"FRI",
	L"SAT",
	nullptr
};


using namespace hnrt;


CronTokenizer::CronTokenizer(PCWSTR psz, CronElement element)
	: m_o(psz)
	, m_p(psz)
	, m_q(psz + 1)
	, m_c(*psz)
	, m_v(0)
	, m_e(element)
{
}


int CronTokenizer::GetNext()
{
	int sym;
	switch (m_c)
	{
	case L'-':
	case L',':
	case L'/':
	case L'#':
		m_p = m_q - 1;
		sym = m_c;
		m_c = *m_q++;
		return sym;
	default:
		break;
	}
	while (iswspace(m_c))
	{
		m_c = *m_q++;
	}
	m_p = m_q - 1;
	switch (m_c)
	{
	case L'\0':
		return CRON_TOKEN_EOF;
	case L'*':
	case L'?':
		sym = m_c;
		m_c = *m_q++;
		if (m_c == L'\0' || m_c == L' ')
		{
			return sym;
		}
		break;
	default:
		if (iswdigit(m_c))
		{
			m_v = m_c - L'0';
			m_c = *m_q++;
			while (iswdigit(m_c))
			{
				m_v = m_v * 10 + m_c - L'0';
				m_c = *m_q++;
			}
			if (m_c == L'W')
			{
				sym = CRON_TOKEN_INTEGER_W;
				m_c = *m_q++;
			}
			else if (m_c == L'L')
			{
				sym = CRON_TOKEN_INTEGER_L;
				m_c = *m_q++;
			}
			else
			{
				sym = CRON_TOKEN_INTEGER;
			}
			if (m_c == L'\0' || m_c == L' ' || m_c == L',' || m_c == L'-' || m_c == L'/' || m_c == L'#')
			{
				return sym;
			}
		}
		else if (iswalpha(m_c))
		{
			WCHAR sz[4] = { 0 };
			sz[0] = m_c;
			m_c = *m_q++;
			if (iswalpha(m_c))
			{
				sz[1] = m_c;
				m_c = *m_q++;
				if (iswalpha(m_c))
				{
					sz[2] = m_c;
					m_c = *m_q++;
					if (m_c == L'\0' || m_c == L' ' || m_c == L',' || m_c == L'-' || m_c == L'/' || m_c == L'#')
					{
						int index;
						if ((index = Find(CronMonthWords, sz)) >= 0)
						{
							return CRON_TOKEN_MONTH_MIN + index;
						}
						else if ((index = Find(CronDayOfWeekWords, sz)) >= 0)
						{
							return CRON_TOKEN_DAYOFWEEK_MIN + index;
						}
					}
				}
			}
			else if (m_c == L'\0' || m_c == L' ' || m_c == L',')
			{
				if (sz[0] == L'L' || sz[0] == L'W')
				{
					return sz[0];
				}
			}
		}
		break;
	}
	throw CronError(CRON_ERROR_BADSEQUENCE, m_e, GetOffset());
}


int CronTokenizer::Find(const PCWSTR* ppsz, PCWSTR psz)
{
	int index = 0;
	while (ppsz[index])
	{
		if (!wcscmp(ppsz[index], psz))
		{
			return index;
		}
		index++;
	}
	return -1;
}
