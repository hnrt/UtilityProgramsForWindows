#include "pch.h"
#include "hnrt/AnyCodeException.h"


using namespace hnrt;


AnyCodeException::AnyCodeException(int nReason, int nOffset)
	: Exception()
	, m_nReason(nReason)
	, m_nOffset(nOffset)
{
}


AnyCodeException::AnyCodeException(const AnyCodeException& other)
	: Exception(other)
	, m_nReason(other.m_nReason)
	, m_nOffset(other.m_nOffset)
{
}


AnyCodeException& AnyCodeException::operator =(const AnyCodeException& other)
{
	(void)Exception::operator =(other);
	m_nReason = other.m_nReason;
	m_nOffset = other.m_nOffset;
	return *this;
}
