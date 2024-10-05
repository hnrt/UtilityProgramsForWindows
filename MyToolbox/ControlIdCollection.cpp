#include "pch.h"
#include "ControlIdCollection.h"
#include "hnrt/Interlocked.h"


using namespace hnrt;


ControlIdCollection::ControlIdCollection()
	: m_count(0)
	, m_values(nullptr)
{
}


ControlIdCollection::~ControlIdCollection()
{
	delete[] m_values;
}


ControlIdCollection& ControlIdCollection::Add(int id)
{
	int indexL = 0;
	int indexH = static_cast<int>(m_count) - 1;
	while (indexL <= indexH)
	{
		int indexM = (indexL + indexH) / 2;
		WORD next = m_values[indexM];
		if (next < static_cast<WORD>(id))
		{
			indexL = indexM + 1;
		}
		else if (static_cast<WORD>(id) < next)
		{
			indexH = indexM - 1;
		}
		else
		{
			return *this;
		}
	}
	WORD index = static_cast<WORD>(indexL);
	WORD* values = new WORD[m_count + 1];
	if (index > 0)
	{
		memcpy_s(&values[0], index * sizeof(WORD), &m_values[0], index * sizeof(WORD));
	}
	values[index] = static_cast<WORD>(id);
	if (index < m_count)
	{
		memcpy_s(&values[index + 1], (m_count - index) * sizeof(WORD), &m_values[index], (m_count - index) * sizeof(WORD));
	}
	delete[] Interlocked<WORD*>::ExchangePointer(&m_values, values);
	m_count++;
	return *this;
}


ControlIdCollection& ControlIdCollection::RemoveAll()
{
	delete[] Interlocked<WORD*>::ExchangePointer(&m_values, 0);
	m_count = 0;
	return *this;
}


bool ControlIdCollection::Contains(int id)
{
	int indexL = 0;
	int indexH = static_cast<int>(m_count) - 1;
	while (indexL <= indexH)
	{
		int indexM = (indexL + indexH) / 2;
		WORD next = m_values[indexM];
		if (next < static_cast<WORD>(id))
		{
			indexL = indexM + 1;
		}
		else if (static_cast<WORD>(id) < next)
		{
			indexH = indexM - 1;
		}
		else
		{
			return true;
		}
	}
	return false;
}
