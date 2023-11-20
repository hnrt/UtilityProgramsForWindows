#include "pch.h"
#include "CronValueEvaluation.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/Exception.h"


using namespace hnrt;


RefPtr<CronValueEvaluation> CronValueEvaluation::CreateZero()
{
	return RefPtr<CronValueEvaluation>(new CronValueEvaluation(new int[1] { 0 }, 1));
}



CronValueEvaluation::CronValueEvaluation(int* ptr, int len)
	: RefObj()
	, m_ptr(ptr)
	, m_len(len)
	, m_cur(0)
	, m_sz()
{
}


CronValueEvaluation::~CronValueEvaluation()
{
	delete[] m_ptr;
}


int CronValueEvaluation::operator [](int index) const
{
	if (0 <= index && index < m_len)
	{
		return m_ptr[index];
	}
	else if (0 <= index + m_len && index + m_len < m_len)
	{
		return m_ptr[index + m_len];
	}
	else
	{
		throw Exception(L"CronValueEvaluation::operator []: Index out of range: %d", index);
	}
}


PCWSTR CronValueEvaluation::ToString() const
{
	if (m_len <= 0)
	{
		return L"";
	}
	if (!m_sz)
	{
		StringBuffer buf(260);
		buf.AppendFormat(L"%d", m_ptr[0]);
		for (int index = 1; index < m_len; index++)
		{
			buf.AppendFormat(L" %d", m_ptr[index]);
		}
		m_sz = buf;
	}
	return m_sz;
}


bool CronValueEvaluation::EqualToOrGreaterThan(int value, WORD& target)
{
	if (m_len > 0)
	{
		while (m_cur < m_len)
		{
			int next = m_ptr[m_cur++];
			if (next >= value)
			{
				target = static_cast<WORD>(next);
				return true;
			}
		}
		m_cur = 0;
	}
	return false;
}


bool CronValueEvaluation::Next(WORD& target)
{
	if (m_cur < m_len)
	{
		target = static_cast<WORD>(m_ptr[m_cur++]);
		return true;
	}
	m_cur = 0;
	return false;
}
