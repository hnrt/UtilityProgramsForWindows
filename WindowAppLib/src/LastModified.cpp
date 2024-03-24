#include "pch.h"
#include "hnrt/LastModified.h"
#include "hnrt/Time.h"


#define DEFAULT_GRACE_PERIOD 3000 // milliseconds


using namespace hnrt;


static HCURSOR hCursorActive = LoadCursor(NULL, IDC_WAIT);
static HCURSOR hCursorInactive = LoadCursor(NULL, IDC_ARROW);


LONGLONG LastModified::m_DefaultGracePeriod = DEFAULT_GRACE_PERIOD;


void LastModified::SetDefaultGracePeriod(LONG nGracePeriodInMilliseconds)
{
	m_DefaultGracePeriod = static_cast<LONGLONG>(nGracePeriodInMilliseconds);
}


LastModified::LastModified(LONG nGracePeriodInMilliseconds)
	: m_GracePeriod(nGracePeriodInMilliseconds < 0 ? m_DefaultGracePeriod : static_cast<LONGLONG>(nGracePeriodInMilliseconds))
	, m_At(0)
	, m_By(0)
	, m_bCursorChange(false)
{
}


void LastModified::Clear()
{
	put_By(0);
}


void LastModified::Forget()
{
	if (m_bCursorChange && m_At > 0)
	{
		SetCursor(hCursorActive);
	}
}


void LastModified::Recall()
{
	if (m_bCursorChange && m_At > 0)
	{
		SetCursor(hCursorActive);
	}
}


void LastModified::put_By(int id)
{
	if (id)
	{
		m_At = FileTime().Intervals;
		m_By = id;
		if (m_bCursorChange)
		{
			SetCursor(hCursorActive);
		}
	}
	else
	{
		m_At = 0;
		m_By = 0;
		if (m_bCursorChange)
		{
			SetCursor(hCursorInactive);
		}
	}
}


bool LastModified::get_IsUpdateRequired() const
{
	if (m_At > 0)
	{
		return m_At < FileTime().AddMilliseconds(-m_GracePeriod).Intervals;
	}
	return false;
}


void LastModified::put_CursorChange(bool value)
{
	m_bCursorChange = value;
	SetCursor(m_bCursorChange && m_At > 0 ? hCursorActive : hCursorInactive);
}
