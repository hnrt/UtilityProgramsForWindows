#include "pch.h"
#include "hnrt/StringUtils.h"
#include "hnrt/StringCommons.h"
//#include "hnrt/Heap.h"
#include "hnrt/Exception.h"


using namespace hnrt;


String hnrt::JoinBy(const Array<DWORD>& values, PCWSTR pszSeparator, PCWSTR pszFormat)
{
	switch (values.Length)
	{
	case 0:
		return String::Empty;
	case 1:
		return String::Format(pszFormat, values[0]);
	default:
	{
		String result = String::Format(pszFormat, values[0]);
		String szCombinedFormat(CONCAT2, L"%s", pszFormat);
		for (DWORD index = 1; index < values.Length; index++)
		{
			result.AppendFormat(szCombinedFormat, pszSeparator, values[index]);
		}
		return result;
	}
	}
}


static void DoSplitBy(PCWCHAR pCur, PCWCHAR pEnd, WCHAR separator, Array<String>& list)
{
	int index;
	while ((index = IndexOf(pCur, separator, pEnd - pCur)) >= 0)
	{
		list.PushBack(String(pCur, index));
		pCur += index + 1;
	}
	list.PushBack(String(pCur));
}


Array<String> hnrt::SplitBy(String str, WCHAR separator)
{
	Array<String> list;
	DoSplitBy(str.Ptr, str.Ptr + str.Len, separator, list);
	return list;
}


Array<String> hnrt::SplitBy(PCWSTR psz, WCHAR separator)
{
	Array<String> list;
	DoSplitBy(psz, psz + StrLen(psz), separator, list);
	return list;
}
