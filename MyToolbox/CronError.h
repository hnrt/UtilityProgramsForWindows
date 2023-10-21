#pragma once


#include "CronElement.h"


namespace hnrt
{
	enum CronErrorType
	{
		CRON_ERROR_OUTOFRANGE,
		CRON_ERROR_NOSTEP,
		CRON_ERROR_NOUPPERBOUND,
		CRON_ERROR_NOORDINALNUMBER,
		CRON_ERROR_BADSEQUENCE
	};

	struct CronError
	{
		CronErrorType type;
		CronElement element;
		int offset;
		CronError(CronErrorType type_, CronElement element_, int offset_);
	};

	inline CronError::CronError(CronErrorType type_, CronElement element_, int offset_)
		: type(type_)
		, element(element_)
		, offset(offset_)
	{
	}
}
