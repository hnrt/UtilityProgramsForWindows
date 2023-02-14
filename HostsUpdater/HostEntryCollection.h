#pragma once

#include "HostEntry.h"

namespace hnrt
{
	class HostEntryCollection
	{
	public:

		HostEntryCollection();
		HostEntryCollection(const HostEntryCollection&);
		~HostEntryCollection();
		HostEntryCollection& operator =(const HostEntryCollection&);
		void Clear();
		void Add(const HostEntry& entry);
		HostEntry* FindByName(const WCHAR* pContent, PCWSTR pszName);
		const HostEntry& operator[](int index) const;
		HostEntry& operator[](int index);
		DWORD get_Count() const;

		__declspec(property(get = get_Count)) DWORD Count;

	private:

		void Copy(const HostEntryCollection&);

		HostEntry* m_pBase;
		DWORD m_Size;
		DWORD m_Count;
	};

	inline DWORD HostEntryCollection::get_Count() const
	{
		return m_Count;
	}
}
