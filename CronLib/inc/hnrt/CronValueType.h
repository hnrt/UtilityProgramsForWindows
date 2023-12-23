#pragma once

namespace hnrt
{
	enum CronValueType
	{
		CRON_EMPTY,
		CRON_ALL, // *
		CRON_ANY, // ?
		CRON_LASTDAY, // L
		CRON_WEEKDAY, // W
		CRON_SINGLE, // 1
		CRON_RANGE, // 2-5
		CRON_CLOSEST_WEEKDAY, // 3W (Weekday closest to 3rd day in the month)
		CRON_NTH_DAYOFWEEK, // 3#2 (2nd Tuesday in the month)
		CRON_LAST_DAYOFWEEK, // 6L (Last Friday in the month)
		CRON_INVALID_VALUE
	};
}
