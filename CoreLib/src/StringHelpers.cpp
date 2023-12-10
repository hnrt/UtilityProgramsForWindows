#include "pch.h"
#include "hnrt/StringHelpers.h"


using namespace hnrt;


String StringHelpers::JoinBy(const std::vector<DWORD>& values, PCWSTR pszSeparator, PCWSTR pszFormat)
{
	String result;
	switch (values.size())
	{
	case 0:
		break;
	case 1:
		result.Format(pszFormat, values[0]);
		break;
	default:
	{
		std::vector<DWORD>::const_iterator iter = values.cbegin();
		result.Format(pszFormat, *iter);
		String szCombinedFormat(CONCAT2, L"%s", pszFormat);
		for (iter++; iter != values.cend(); iter++)
		{
			result.AppendFormat(szCombinedFormat, pszSeparator, *iter);
		}
	}
	}
	return result;
}
