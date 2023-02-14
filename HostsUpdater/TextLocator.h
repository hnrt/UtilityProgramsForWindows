#pragma once

#include <Windows.h>

namespace hnrt
{
	struct TextLocator
	{
		int Start;
		int End;

		int Compare(const WCHAR* pContent, PCWSTR pszValue);

		int get_Len() const;

		__declspec(property(get = get_Len)) int Len;
	};

	inline int TextLocator::Compare(const WCHAR* pContent, PCWSTR pszValue)
	{
		return wcsncmp(pContent + Start, pszValue, Len);
	}

	inline int TextLocator::get_Len() const
	{
		return End - Start;
	}
}
