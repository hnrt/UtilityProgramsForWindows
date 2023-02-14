#include "HostEntryCollection.h"
#include "hnrt/Heap.h"
#include "hnrt/Exception.h"


#pragma warning(disable:4291)


using namespace hnrt;


HostEntryCollection::HostEntryCollection()
	: m_pBase(nullptr)
	, m_Size(0)
	, m_Count(0)
{
}


HostEntryCollection::HostEntryCollection(const HostEntryCollection& src)
	: m_pBase(nullptr)
	, m_Size(0)
	, m_Count(0)
{
	Copy(src);
}


HostEntryCollection::~HostEntryCollection()
{
	Clear();
}


HostEntryCollection& HostEntryCollection::operator =(const HostEntryCollection& src)
{
	Clear();
	Copy(src);
	return *this;
}


void HostEntryCollection::Clear()
{
	for (int index = static_cast<int>(m_Count) - 1; index >= 0; index--)
	{
		m_pBase[index].~HostEntry();
	}
	free(m_pBase);
	m_pBase = nullptr;
	m_Size = 0;
	m_Count = 0;
}


void HostEntryCollection::Copy(const HostEntryCollection& src)
{
	if (src.m_Count)
	{
		m_pBase = Allocate<HostEntry>(src.m_Size);
		m_Size = src.m_Size;
		for (DWORD dwIndex = 0; dwIndex < src.m_Count; dwIndex++)
		{
			new (&m_pBase[dwIndex]) HostEntry(src.m_pBase[dwIndex]);
		}
		m_Count = src.m_Count;
	}
}


void HostEntryCollection::Add(const HostEntry& entry)
{
	if (m_Count + 1 > m_Size)
	{
		m_Size += m_Size == 0 ? 16 : m_Size < 65536 ? m_Size : 65536;
		m_pBase = Allocate<HostEntry>(m_pBase, m_Size);
	}
	new (&m_pBase[m_Count++]) HostEntry(entry);
}


HostEntry* HostEntryCollection::FindByName(const WCHAR* pContent, PCWSTR pszName)
{
	for (DWORD dwIndex = 0; dwIndex < m_Count; dwIndex++)
	{
		HostEntry* pEntry = &m_pBase[dwIndex];
		if (pEntry->Names.Contains(pContent, pszName))
		{
			return pEntry;
		}
	}
	return nullptr;
}


const HostEntry& HostEntryCollection::operator[](int index) const
{
	if (0 <= index && index < static_cast<int>(m_Count))
	{
		return m_pBase[index];
	}
	else if (0 <= static_cast<int>(m_Count) + index && index < 0)
	{
		return m_pBase[static_cast<int>(m_Count) + index];
	}
	else
	{
		throw Exception(L"HostEntryCollection: Index out of range.");
	}
}


HostEntry& HostEntryCollection::operator[](int index)
{
	if (0 <= index && index < static_cast<int>(m_Count))
	{
		return m_pBase[index];
	}
	else if (0 <= static_cast<int>(m_Count) + index && index < 0)
	{
		return m_pBase[static_cast<int>(m_Count) + index];
	}
	else
	{
		throw Exception(L"HostEntryCollection: Index out of range.");
	}
}
