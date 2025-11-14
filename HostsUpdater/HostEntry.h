#pragma once

#include "TextLocator.h"
#include "hnrt/Array.h"

namespace hnrt
{
	struct HostEntry
	{
		TextLocator Address;
		Array<TextLocator> Names;

		HostEntry();
		HostEntry(const HostEntry&) = default;
		~HostEntry() = default;
		void* operator new(size_t, void*);
		bool Contains(const WCHAR* pContent, PCWSTR pszValue);
	};

	inline HostEntry::HostEntry()
		: Address()
		, Names()
	{
	}

	inline void* HostEntry::operator new(size_t unused, void* ptr)
	{
		return ptr;
	}

	inline bool HostEntry::Contains(const WCHAR* pContent, PCWSTR pszValue)
	{
		for (DWORD dwIndex = 0; dwIndex < Names.Length; dwIndex++)
		{
			if (!Names[dwIndex].Compare(pContent, pszValue))
			{
				return true;
			}
		}
		return false;
	}
}
