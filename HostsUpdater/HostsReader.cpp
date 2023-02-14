#include "HostsReader.h"


#define CR L'\r'
#define LF L'\n'
#define HT L'\t'
#define SP L' '


using namespace hnrt;


static bool IsHexDigit(int c)
{
	return iswdigit(c) || (L'A' <= c && c <= L'F') || (L'a' <= c && c <= L'f');
}


static bool IsDelimiter(int c)
{
	return c == SP || c == HT || c == L'#' || c == CR || c == LF || c == HOSTSREADER_EOF;
}


static bool IsEOL(int c)
{
	return c == CR || c == LF || c == HOSTSREADER_EOF;
}


HostsReader::HostsReader(const WCHAR* pBase, size_t cch)
	: m_pBase(pBase)
	, m_pCur(pBase)
	, m_pEnd(pBase + cch)
	, m_c(HOSTSREADER_EOF)
	, m_Token()
	, m_bAddress(false)
{
	m_c = NextChar();
}


int HostsReader::NextToken()
{
	if (m_c == HOSTSREADER_EOF)
	{
		return HOSTSREADER_EOF;
	}
	m_Token.Start = Offset;
	if (m_c == CR)
	{
		m_c = NextChar();
		if (m_c == LF)
		{
			m_c = NextChar();
		}
		m_Token.End = Offset;
		m_bAddress = false;
		return LF;
	}
	else if (m_c == LF)
	{
		m_c = NextChar();
		m_Token.End = Offset;
		m_bAddress = false;
		return LF;
	}
	else if (m_c == SP || m_c == HT)
	{
		m_c = NextChar();
		while (m_c == SP || m_c == HT)
		{
			m_c = NextChar();
		}
		m_Token.End = Offset;
		return HOSTSREADER_WS;
	}
	else if (m_c == L'#')
	{
		m_c = NextChar();
		while (!IsEOL(m_c))
		{
			m_c = NextChar();
		}
		m_Token.End = Offset;
		return HOSTSREADER_COMMENT;
	}
	else if (!m_bAddress)
	{
		if (iswdigit(m_c))
		{
			if (ParseIPv4())
			{
				m_Token.End = Offset;
				m_bAddress = true;
				return HOSTSREADER_IPV4ADDR;
			}
			m_pCur = m_pBase + m_Token.Start;
			m_c = NextChar();
		}
		if (ParseIPv6())
		{
			m_Token.End = Offset;
			m_bAddress = true;
			return HOSTSREADER_IPV6ADDR;
		}
		m_pCur = m_pBase + m_Token.Start;
		m_c = NextChar();
	}
	else if (iswalpha(m_c))
	{
		if (ParseHostName())
		{
			m_Token.End = Offset;
			return HOSTSREADER_HOSTNAME;
		}
		m_pCur = m_pBase + m_Token.Start;
		m_c = NextChar();
	}
	while (!IsEOL(m_c))
	{
		m_c = NextChar();
	}
	m_Token.End = Offset;
	return HOSTSREADER_ERROR;
}


int HostsReader::NextChar()
{
	return m_pCur < m_pEnd ? *m_pCur++ : -1;
}


bool HostsReader::ParseIPv4()
{
	int y = 0;
	while (true)
	{
		int a = 0;
		int x = 0;
		while (true)
		{
			if (iswdigit(m_c))
			{
				a = a * 10 + m_c - L'0';
				if (a > 255)
				{
					return false;
				}
				m_c = NextChar();
				if (++x == 3)
				{
					break;
				}
			}
			else if (x == 0)
			{
				return false;
			}
			else
			{
				break;
			}
		}
		if (++y == 4)
		{
			return IsDelimiter(m_c);
		}
		else if (m_c == L'.')
		{
			m_c = NextChar();
		}
		else
		{
			return false;
		}
	}
}


bool HostsReader::ParseIPv6()
{
	int z = 0;
	if (m_c == L':')
	{
		m_c = NextChar();
		if (m_c == L':')
		{
			m_c = NextChar();
			if (IsDelimiter(m_c))
			{
				return true;
			}
			z = 1;
		}
		else
		{
			return false;
		}
	}
	int y = 0;
	while (true)
	{
		int x = 0;
		while (true)
		{
			if (IsHexDigit(m_c))
			{
				m_c = NextChar();
				if (++x == 4)
				{
					break;
				}
			}
			else if (x == 0)
			{
				return false;
			}
			else
			{
				break;
			}
		}
		if (z == 0)
		{
			if (++y == 8)
			{
				return IsDelimiter(m_c);
			}
			else if (m_c == L':')
			{
				m_c = NextChar();
				if (m_c == L':')
				{
					m_c = NextChar();
					if (IsDelimiter(m_c))
					{
						return true;
					}
					z = 1;
				}
			}
			else
			{
				return false;
			}
		}
		else if (++y == 7)
		{
			return IsDelimiter(m_c);
		}
		else if (m_c == L':')
		{
			m_c = NextChar();
		}
		else
		{
			return IsDelimiter(m_c);
		}
	}
}


bool HostsReader::ParseHostName()
{
	if (iswalpha(m_c))
	{
		m_c = NextChar();
	}
	else
	{
		return false;
	}
	while (true)
	{
		if (IsDelimiter(m_c))
		{
			return true;
		}
		else if (iswalnum(m_c))
		{
			m_c = NextChar();
		}
		else if (m_c == L'-' || m_c == L'.')
		{
			m_c = NextChar();
			if (iswalnum(m_c))
			{
				m_c = NextChar();
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
}
