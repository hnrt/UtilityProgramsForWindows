#pragma once


namespace hnrt
{
	enum CronElement
	{
		CRON_YEAR,
		CRON_MONTH,
		CRON_DAYOFMONTH,
		CRON_DAYOFWEEK,
		CRON_HOUR,
		CRON_MINUTE,
		CRON_SECOND
	};

	enum CronValueType
	{
		CRON_ALL, // *
		CRON_ANY, // ?
		CRON_SINGLE, // 1
		CRON_RANGE, // 2-5
		CRON_LASTDAY, // L
		CRON_WEEKDAY, // W
		CRON_CLOSEST_WEEKDAY, // 3W (Weekday closest to 3rd day in the month)
		CRON_NTH_DAYOFWEEK, // 3#2 (2nd Tuesday in the month)
		CRON_LAST_DAYOFWEEK, // 6L (Last Friday in the month)
		CRON_INVALID
	};

	struct CronValueAll
	{
		CronValueType type; // CRON_ALL
		union CronValue* pNext; // null
		CronElement element;
		static CronValue* Create(CronElement element);
	};

	struct CronValueAny
	{
		CronValueType type; // CRON_ANY
		union CronValue* pNext; // null
		CronElement element;
		static CronValue* Create(CronElement element);
	};

	struct CronValueSingle
	{
		CronValueType type; // CRON_SINGLE
		union CronValue* pNext;
		CronElement element;
		int step;
		int value;
		int max;
		static CronValue* Create(CronElement element, int value);
	};

	struct CronValueRange
	{
		CronValueType type; // CRON_RANGE
		union CronValue* pNext;
		CronElement element;
		int step;
		int from;
		int to;
		static CronValue* Create(CronElement element, int from, int to);
	};

	struct CronValueLastDay
	{
		CronValueType type; // CRON_LASTDAY
		union CronValue* pNext;
		CronElement element;
		static CronValue* Create(CronElement element);
	};

	struct CronValueWeekDay
	{
		CronValueType type; // CRON_WEEKDAY
		union CronValue* pNext;
		CronElement element;
		static CronValue* Create(CronElement element);
	};

	struct CronValueClosestWeekDay
	{
		CronValueType type; // CRON_CLOSEST_WEEKDAY
		union CronValue* pNext;
		CronElement element;
		int unused;
		int dom;
		static CronValue* Create(CronElement element, int dom);
	};

	struct CronValueNthDayOfWeek
	{
		CronValueType type; // CRON_NTH_DAYOFWEEK
		union CronValue* pNext;
		CronElement element;
		int unused;
		int dow;
		int nth;
		static CronValue* Create(CronElement element, int dow, int nth);
	};

	struct CronValueLastDayOfWeek
	{
		CronValueType type; // CRON_LAST_DAYOFWEEK
		union CronValue* pNext;
		CronElement element;
		int unused;
		int dow;
		static CronValue* Create(CronElement element, int dow);
	};

	union CronValue
	{
		struct {
			CronValueType type;
			union CronValue* pNext;
			CronElement element;
		};
		CronValueAll all;
		CronValueAny any;
		CronValueSingle single;
		CronValueRange range;
		CronValueLastDay lastday;
		CronValueWeekDay weekday;
		CronValueClosestWeekDay closestwd;
		CronValueNthDayOfWeek nthdow;
		CronValueLastDayOfWeek lastdow;

		operator PCWSTR() const;
		PCWSTR ToString() const;
		PCWSTR Evaluate(int offset = 0) const;
		int Count() const;
		int Count(CronValueType type) const;

		static void Free(CronValue*& pValue);
		static PCWSTR Name(CronElement element);
		static int Min(CronElement element);
		static int Max(CronElement element);
		static const CronValue* Invalid();
	};

	extern const PCWSTR MonthWords[];

	extern const PCWSTR DayOfWeekWords[];
}
