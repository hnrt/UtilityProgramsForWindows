#include "pch.h"
#include "hnrt/SemanticVersion.h"


using namespace hnrt;


SemanticVersion::SemanticVersion()
	: m_Level(0)
	, m_Major(-1)
	, m_Minor(-1)
	, m_Patch(-1)
	, m_PreRelease()
	, m_Build()
{
}


SemanticVersion::SemanticVersion(PCWSTR psz)
	: m_Level(-1)
	, m_Major(-1)
	, m_Minor(-1)
	, m_Patch(-1)
	, m_PreRelease()
	, m_Build()
{
	Parse(psz);
}


SemanticVersion::SemanticVersion(const SemanticVersion& src)
	: m_Level(src.m_Level)
	, m_Major(src.m_Major)
	, m_Minor(src.m_Minor)
	, m_Patch(src.m_Patch)
	, m_PreRelease(src.m_PreRelease)
	, m_Build(src.m_Build)
{
}


SemanticVersion& SemanticVersion::operator = (const SemanticVersion& src)
{
	m_Level = src.m_Level;
	m_Major = src.m_Major;
	m_Minor = src.m_Minor;
	m_Patch = src.m_Patch;
	m_PreRelease = src.m_PreRelease;
	m_Build = src.m_Build;
	return *this;
}


String SemanticVersion::ToString() const
{
	switch (m_Level)
	{
	case 0:
		return String::Format(L"");
	case 1:
		return String::Format(L"%d", m_Major);
	case 2:
		return String::Format(L"%d.%d", m_Major, m_Minor);
	case 3:
		if (m_PreRelease.Len > 0)
		{
			if (m_Build.Len > 0)
			{
				return String::Format(L"%d.%d.%d-%s+%s", m_Major, m_Minor, m_Patch, m_PreRelease, m_Build);
			}
			else
			{
				return String::Format(L"%d.%d.%d-%s", m_Major, m_Minor, m_Patch, m_PreRelease);
			}
		}
		else if (m_Build.Len > 0)
		{
			return String::Format(L"%d.%d.%d+%s", m_Major, m_Minor, m_Patch, m_Build);
		}
		else
		{
			return String::Format(L"%d.%d.%d", m_Major, m_Minor, m_Patch);
		}
	default:
		return String::Format(L"");
	}
}


int SemanticVersion::CompareTo(const SemanticVersion& other) const
{
	int major1 = -1, minor1 = -1, patch1 = -1, major2 = -1, minor2 = -1, patch2 = -1;
	switch (m_Level)
	{
	case 0:
		major1 = INT_MAX - 1;
		minor1 = INT_MAX - 1;
		patch1 = INT_MAX - 1;
		break;
	case 3:
		patch1 = m_Patch;
		//FALLTHROUGH
	case 2:
		minor1 = m_Minor;
		//FALLTHROUGH
	case 1:
		major1 = m_Major;
		//FALLTHROUGH
	default:
		break;
	}
	switch (other.m_Level)
	{
	case 0:
		major2 = INT_MAX - 1;
		minor2 = INT_MAX - 1;
		patch2 = INT_MAX - 1;
		break;
	case 3:
		patch2 = other.m_Patch;
		//FALLTHROUGH
	case 2:
		minor2 = other.m_Minor;
		//FALLTHROUGH
	case 1:
		major2 = other.m_Major;
		//FALLTHROUGH
	default:
		break;
	}
	int d = major1 - major2;
	if (d != 0)
	{
		return d;
	}
	d = minor1 - minor2;
	if (d != 0)
	{
		return d;
	}
	d = patch1 - patch2;
	if (d != 0)
	{
		return d;
	}
	if (m_PreRelease.Len == 0)
	{
		if (other.m_PreRelease.Len > 0)
		{
			return 1;
		}
	}
	else if (other.m_PreRelease.Len == 0)
	{
		return -1;
	}
	d = Compare(m_PreRelease, other.m_PreRelease);
	if (d != 0)
	{
		return d;
	}
	if (m_Build.Len == 0)
	{
		if (other.m_Build.Len > 0)
		{
			return 1;
		}
	}
	else if (other.m_Build.Len == 0)
	{
		return -1;
	}
	return Compare(m_Build, other.m_Build);
}


bool SemanticVersion::Parse(PCWSTR ptr)
{
	int level = 0;
	if (*ptr == L'\0')
	{
		m_Level = level;
		return true;
	}
	if (ParseNumericIdentifier(ptr, m_Major))
	{
		level = 1;
	}
	else
	{
		return false;
	}
	if (*ptr == L'\0')
	{
		m_Level = level;
		return true;
	}
	if (*ptr == L'.')
	{
		ptr++;
		if (ParseNumericIdentifier(ptr, m_Minor))
		{
			level = 2;
		}
		else
		{
			return false;
		}
	}
	if (*ptr == L'\0')
	{
		m_Level = level;
		return true;
	}
	if (*ptr == L'.')
	{
		ptr++;
		if (ParseNumericIdentifier(ptr, m_Patch))
		{
			level = 3;
		}
		else
		{
			return false;
		}
	}
	if (*ptr == L'-')
	{
		ptr++;
		if (!ParseSuffix(ptr, m_PreRelease))
		{
			return false;
		}
	}
	if (*ptr == L'+')
	{
		ptr++;
		if (!ParseSuffix(ptr, m_Build))
		{
			return false;
		}
	}
	if (*ptr == L'\0')
	{
		m_Level = level;
		return true;
	}
	else
	{
		return false;
	}
}


bool SemanticVersion::ParseNumericIdentifier(PCWSTR& p, int& v)
{
	if (*p == L'0')
	{
		if (!iswdigit(p[1]))
		{
			v = 0;
			p++;
			return true;
		}
		else
		{
			return false;
		}
	}
	int n;
	if (iswdigit(*p))
	{
		n = *p++ - L'0';
	}
	else
	{
		return false;
	}
	while (iswdigit(*p))
	{
		n = n * 10 + *p++ - L'0';
	}
	v = n;
	return true;
}


bool SemanticVersion::ParseSuffix(PCWSTR& p, String& prv)
{
	PCWSTR p0 = p;
	if (!ParseAlphaNumericIdentifier(p))
	{
		return false;
	}
	while (*p == L'.')
	{
		p++;
		if (!ParseAlphaNumericIdentifier(p))
		{
			return false;
		}
	}
	prv = String(p0, p - p0);
	return true;
}


bool SemanticVersion::ParseAlphaNumericIdentifier(PCWSTR& p)
{
	if (iswalnum(*p) || *p == L'-')
	{
		p++;
	}
	else
	{
		return false;
	}
	while (iswalnum(*p) || *p == L'-')
	{
		p++;
	}
	return true;
}


int SemanticVersion::Compare(PCWSTR p1, PCWSTR p2)
{
	int d;
	WCHAR c1 = *p1++;
	WCHAR c2 = *p2++;
	while (c1 && c2)
	{
		if (iswdigit(c1))
		{
			if (iswdigit(c2))
			{
				int n1 = 0;
				int n2 = 0;
				do {
					n1 = n1 * 10 + c1 - L'0';
					c1 = *p1++;
				} while (iswdigit(c1));
				do {
					n2 = n2 * 10 + c2 - L'0';
					c2 = *p2++;
				} while (iswdigit(c2));
				d = n1 - n2;
				if (d != 0)
				{
					return d;
				}
			}
			else
			{
				return -1;
			}
		}
		else if (iswdigit(c2))
		{
			return 1;
		}
		else
		{
			d = c1 - c2;
			if (d != 0)
			{
				return d;
			}
			c1 = *p1++;
			c2 = *p2++;
		}
	}
	return c1 - c2;
}
