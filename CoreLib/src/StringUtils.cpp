#include "pch.h"
#include "hnrt/StringUtils.h"
#include "hnrt/Heap.h"
#include "hnrt/Exception.h"


using namespace hnrt;


String hnrt::JoinBy(const std::vector<DWORD>& values, PCWSTR pszSeparator, PCWSTR pszFormat)
{
	switch (values.size())
	{
	case 0:
		return String();
	case 1:
		return String::Format(pszFormat, values[0]);
	default:
	{
		String result;
		std::vector<DWORD>::const_iterator iter = values.cbegin();
		result = String::Format(pszFormat, *iter);
		String szCombinedFormat(CONCAT2, L"%s", pszFormat);
		for (iter++; iter != values.cend(); iter++)
		{
			result.AppendFormat(szCombinedFormat, pszSeparator, *iter);
		}
		return result;
	}
	}
}
