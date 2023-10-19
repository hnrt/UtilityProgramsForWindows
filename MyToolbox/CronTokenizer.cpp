#include "pch.h"
#include "CronTokenizer.h"
#include "hnrt/Exception.h"


#define FORMAT_BADSEQUENCE L"Error: Bad sequence at %u."
#define FORMAT_BADCHARACTOR L"Error: Bad character %c at %u."


using namespace hnrt;


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
		m_v = m_c - L'0';
		m_c = *m_p++;
		while (iswdigit(m_c))
		{
			m_v = m_v * 10 + m_c - L'0';
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
		psz[0] = L'\0';
		psz[1] = L'\0';
		m_c = *m_p++;
		if (iswalpha(m_c))
		{
			*psz++ = m_c;
			m_c = *m_p++;
			if (iswalpha(m_c))
			{
				*psz++ = m_c;
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
