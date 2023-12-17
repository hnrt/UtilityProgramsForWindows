#pragma once


#include "CronElement.h"


namespace hnrt
{
	enum CronErrorType
	{
		CRON_ERROR_BADSEQUENCE,
		CRON_ERROR_OUTOFRANGE,
		CRON_ERROR_NOSTEP,
		CRON_ERROR_NOUPPERBOUND,
		CRON_ERROR_NOORDINALNUMBER,
		CRON_ERROR_EXTRACHARACTER,
		CRON_ERROR_DUPLICATEALL,
		CRON_ERROR_DUPLICATEANY,
		CRON_ERROR_ANYDAYOFMONTHREQUIRED,
		CRON_ERROR_ANYDAYOFWEEKREQUIRED,
		CRON_ERROR_ANYDAYREQUIRED,
		CRON_ERROR_MULTIPLEORDINAL
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
