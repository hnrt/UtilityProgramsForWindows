#include "TextLocatorCollection.h"
#include "hnrt/Heap.h"
#include "hnrt/Exception.h"


using namespace hnrt;


TextLocatorCollection::TextLocatorCollection()
	: m_pBase(nullptr)
	, m_Size(0)
	, m_Count(0)
{
}


TextLocatorCollection::TextLocatorCollection(const TextLocatorCollection& src)
	: m_pBase(nullptr)
	, m_Size(0)
	, m_Count(0)
{
	Copy(src);
}


TextLocatorCollection::~TextLocatorCollection()
{
	Clear();
}


TextLocatorCollection& TextLocatorCollection::operator =(const TextLocatorCollection& src)
{
	Clear();
	Copy(src);
	return *this;
}


const TextLocator& TextLocatorCollection::operator[](int index) const
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
		throw Exception(L"TextLocatorCollection: Index out of range.");
	}
}


void TextLocatorCollection::Clear()
{
	free(m_pBase);
	m_pBase = nullptr;
	m_Size = 0;
	m_Count = 0;
}


void TextLocatorCollection::Add(const TextLocator& src)
{
	if (m_Count + 1 > m_Size)
	{
		m_Size += m_Size == 0 ? 16 : m_Size < 65536 ? m_Size : 65536;
		m_pBase = Allocate<TextLocator>(m_pBase, m_Size);
		memset(&m_pBase[m_Count], 0, (m_Size - m_Count) * sizeof(TextLocator));
	}
	m_pBase[m_Count++] = src;
}


bool TextLocatorCollection::Contains(const WCHAR* pContent, PCWSTR pszValue)
{
	for (DWORD dwIndex = 0; dwIndex < m_Count; dwIndex++)
	{
		if (!m_pBase[dwIndex].Compare(pContent, pszValue))
		{
			return true;
		}
	}
	return false;
}


void TextLocatorCollection::Copy(const TextLocatorCollection& src)
{
	if (src.m_Count)
	{
		m_pBase = Allocate<TextLocator>(src.m_Size);
		m_Size = src.m_Size;
		memcpy_s(m_pBase, m_Size * sizeof(TextLocator), src.m_pBase, src.m_Size * sizeof(TextLocator));
		m_Count = src.m_Count;
	}
}
