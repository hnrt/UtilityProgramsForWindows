#include "pch.h"
#include "hnrt/LastModified.h"
#include "hnrt/Time.h"


#define DEFAULT_GRACE_PERIOD 3000 // milliseconds


using namespace hnrt;


LONGLONG LastModified::m_DefaultGracePeriod = DEFAULT_GRACE_PERIOD;


void LastModified::SetDefaultGracePeriod(LONG nGracePeriodInMilliseconds)
{
	m_DefaultGracePeriod = static_cast<LONGLONG>(nGracePeriodInMilliseconds);
}


LastModified::LastModified(LONG nGracePeriodInMilliseconds)
	: m_GracePeriod(nGracePeriodInMilliseconds < 0 ? m_DefaultGracePeriod : static_cast<LONGLONG>(nGracePeriodInMilliseconds))
	, m_At(0)
	, m_By(0)
{
}


void LastModified::Clear()
{
	m_At = 0;
	m_By = 0;
	SetCursor(LoadCursor(NULL, IDC_ARROW));
}


void LastModified::put_By(int id)
{
	m_At = FileTime().Intervals;
	m_By = id;
	SetCursor(LoadCursor(NULL, IDC_WAIT));
}


bool LastModified::get_IsUpdateRequired() const
{
	if (m_At > 0)
	{
		return m_At < FileTime().AddMilliseconds(-m_GracePeriod).Intervals;
	}
	return false;
}
