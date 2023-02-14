#pragma once

#include "TextLocator.h"
#include <Windows.h>

namespace hnrt
{
	struct TextLocatorCollection
	{
	public:

		TextLocatorCollection();
		TextLocatorCollection(const TextLocatorCollection&);
		~TextLocatorCollection();
		TextLocatorCollection& operator =(const TextLocatorCollection&);
		const TextLocator& operator[](int index) const;
		void Clear();
		void Add(const TextLocator&);
		bool Contains(const WCHAR* pContent, PCWSTR pszValue);

		DWORD get_Count() const;

		__declspec(property(get = get_Count)) DWORD Count;

	private:

		void Copy(const TextLocatorCollection&);

		TextLocator* m_pBase;
		DWORD m_Size;
		DWORD m_Count;
	};

	inline DWORD TextLocatorCollection::get_Count() const
	{
		return m_Count;
	}
}
