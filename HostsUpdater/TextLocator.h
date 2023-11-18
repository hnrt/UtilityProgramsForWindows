#pragma once

#include "hnrt/StringCaseInsensitive.h"
#include "hnrt/Debug.h"

namespace hnrt
{
	struct TextLocator
	{
		int Start;
		int End;

		TextLocator() = default;
		TextLocator(const TextLocator&) = default;
		~TextLocator() = default;
		void* operator new(size_t, void*);
		int Compare(const WCHAR* pContent, PCWSTR pszValue);

		int get_Len() const;

		__declspec(property(get = get_Len)) int Len;
	};

	inline void* TextLocator::operator new(size_t unused, void* ptr)
	{
		return ptr;
	}

	inline int TextLocator::Compare(const WCHAR* pContent, PCWSTR pszValue)
	{
		DBGPUT(L"StringCase::Compare(\"%.*s\",\"%s\")=%d", Len, pContent + Start, pszValue, StringCase::Compare(pContent + Start, Len, pszValue, -1));
		return StringCase::Compare(pContent + Start, Len, pszValue, -1);
	}

	inline int TextLocator::get_Len() const
	{
		return End - Start;
	}
}
