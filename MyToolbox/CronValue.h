#pragma once


#include "hnrt/RefObj.h"
#include "hnrt/RefPtr.h"
#include "CronElement.h"
#include "CronValueEvaluation.h"


#define CRON_WORD_DISPLACEMENT 10000

#define CRON_NUMBER(x) (((x) < CRON_WORD_DISPLACEMENT) ? (x) : ((x) - CRON_WORD_DISPLACEMENT))


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

	class CronValue
		: public RefObj
	{
	public:

		static PCWSTR Name(CronElement element);
		static int Min(CronElement element);
		static int Max(CronElement element);
		static RefPtr<CronValue> InvalidValue();

		CronValue(const CronValue&) = delete;
		virtual ~CronValue();
		void operator =(const CronValue&) = delete;
		void Append(RefPtr<CronValue>);
		operator PCWSTR() const;
		PCWSTR ToString() const;
		RefPtr<CronValueEvaluation> Evaluate(int offset = 0) const;
		int Count(CronValueType type) const;

		CronValueType get_type() const;
		const RefPtr<CronValue> get_next() const;
		CronElement get_element() const;

		__declspec(property(get = get_type)) CronValueType type;
		__declspec(property(get = get_next)) const RefPtr<CronValue> next;
		__declspec(property(get = get_element)) CronElement element;

	protected:

		CronValue(CronValueType type_, CronElement element_);

		CronValueType m_type;
		RefPtr<CronValue> m_next;
		CronElement m_element;
	};

	inline CronValueType CronValue::get_type() const
	{
		return m_type;
	}

	inline const RefPtr<CronValue> CronValue::get_next() const
	{
		return m_next;
	}

	inline CronElement CronValue::get_element() const
	{
		return m_element;
	}

	inline CronValue::operator PCWSTR() const
	{
		return ToString();
	}

	class CronValueEmpty
		: public CronValue
	{
	public:

		static RefPtr<CronValue> Create(CronElement element);

		CronValueEmpty(const CronValueEmpty&) = delete;
		virtual ~CronValueEmpty() = default;
		void operator =(const CronValueEmpty&) = delete;

	protected:

		CronValueEmpty(CronElement element);
	};

	inline CronValueEmpty::CronValueEmpty(CronElement element)
		: CronValue(CRON_EMPTY, element)
	{
	}

	class CronValueAll
		: public CronValue
	{
	public:

		static RefPtr <CronValue> Create(CronElement element);

		CronValueAll(const CronValueAll&) = delete;
		virtual ~CronValueAll() = default;
		void operator =(const CronValueAll&) = delete;

	protected:

		CronValueAll(CronElement element);
	};

	inline CronValueAll::CronValueAll(CronElement element)
		: CronValue(CRON_ALL, element)
	{
	}

	class CronValueAny
		: public CronValue
	{
	public:

		static RefPtr<CronValue> Create(CronElement element);

		CronValueAny(const CronValueAny&) = delete;
		virtual ~CronValueAny() = default;
		void operator =(const CronValueAny&) = delete;

	protected:

		CronValueAny(CronElement element);
	};

	inline CronValueAny::CronValueAny(CronElement element)
		: CronValue(CRON_ANY, element)
	{
	}

	class CronValueSingle
		: public CronValue
	{
	public:

		static RefPtr<CronValue> Create(CronElement element, int value, int step = 0);

		CronValueSingle(const CronValueSingle&) = delete;
		virtual ~CronValueSingle() = default;
		void operator =(const CronValueSingle&) = delete;

		int get_step() const;
		int get_value() const;

		__declspec(property(get = get_step)) int step;
		__declspec(property(get = get_value)) int value;

	protected:

		CronValueSingle(CronElement element, int value, int step);

		int m_step;
		int m_value;
	};

	inline CronValueSingle::CronValueSingle(CronElement element, int value, int step)
		: CronValue(CRON_SINGLE, element)
		, m_step(step)
		, m_value(value)
	{
	}

	inline int CronValueSingle::get_step() const
	{
		return m_step;
	}

	inline int CronValueSingle::get_value() const
	{
		return m_value;
	}

	class CronValueRange
		: public CronValue
	{
	public:

		static RefPtr<CronValue> Create(CronElement element, int from, int to, int step = 1);

		CronValueRange(const CronValueRange&) = delete;
		virtual ~CronValueRange() = default;
		void operator =(const CronValueRange&) = delete;

		int get_step() const;
		int get_from() const;
		int get_to() const;

		__declspec(property(get = get_step)) int step;
		__declspec(property(get = get_from)) int from;
		__declspec(property(get = get_to)) int to;

	protected:

		CronValueRange(CronElement element, int from, int to, int step);

		int m_step;
		int m_from;
		int m_to;
	};

	inline CronValueRange::CronValueRange(CronElement element, int from, int to, int step)
		: CronValue(CRON_RANGE, element)
		, m_step(step)
		, m_from(from)
		, m_to(to)
	{
	}

	inline int CronValueRange::get_step() const
	{
		return m_step;
	}

	inline int CronValueRange::get_from() const
	{
		return m_from;
	}

	inline int CronValueRange::get_to() const
	{
		return m_to;
	}

	class CronValueLastDay
		: public CronValue
	{
	public:

		static RefPtr<CronValue> Create(CronElement element);

		CronValueLastDay(const CronValueLastDay&) = delete;
		virtual ~CronValueLastDay() = default;
		void operator =(const CronValueLastDay&) = delete;

	protected:

		CronValueLastDay(CronElement element);
	};

	inline CronValueLastDay::CronValueLastDay(CronElement element)
		: CronValue(CRON_LASTDAY, element)
	{
	}

	class CronValueWeekDay
		: public CronValue
	{
	public:

		static RefPtr<CronValue> Create(CronElement element);

		CronValueWeekDay(const CronValueWeekDay&) = delete;
		virtual ~CronValueWeekDay() = default;
		void operator =(const CronValueWeekDay&) = delete;

	protected:

		CronValueWeekDay(CronElement element);
	};

	inline CronValueWeekDay::CronValueWeekDay(CronElement element)
		: CronValue(CRON_WEEKDAY, element)
	{
	}

	class CronValueClosestWeekDay
		: public CronValue
	{
	public:

		static RefPtr<CronValue> Create(CronElement element, int dom);

		CronValueClosestWeekDay(const CronValueClosestWeekDay&) = delete;
		virtual ~CronValueClosestWeekDay() = default;
		void operator =(const CronValueClosestWeekDay&) = delete;

		int get_dom() const;

		__declspec(property(get = get_dom)) int dom;

	protected:

		CronValueClosestWeekDay(CronElement element, int dom);

		int m_dom;
	};

	inline CronValueClosestWeekDay::CronValueClosestWeekDay(CronElement element, int dom)
		: CronValue(CRON_CLOSEST_WEEKDAY, element)
		, m_dom(dom)
	{
	}

	inline int CronValueClosestWeekDay::get_dom() const
	{
		return m_dom;
	}

	class CronValueNthDayOfWeek
		: public CronValue
	{
	public:

		static RefPtr<CronValue> Create(CronElement element, int dow, int nth);

		CronValueNthDayOfWeek(const CronValueNthDayOfWeek&) = delete;
		virtual ~CronValueNthDayOfWeek() = default;
		void operator =(const CronValueNthDayOfWeek&) = delete;

		int get_dow() const;
		int get_nth() const;

		__declspec(property(get = get_dow)) int dow;
		__declspec(property(get = get_nth)) int nth;

	protected:

		CronValueNthDayOfWeek(CronElement element, int dow, int nth);

		int m_dow;
		int m_nth;
	};

	inline CronValueNthDayOfWeek::CronValueNthDayOfWeek(CronElement element, int dow, int nth)
		: CronValue(CRON_NTH_DAYOFWEEK, element)
		, m_dow(dow)
		, m_nth(nth)
	{
	}

	inline int CronValueNthDayOfWeek::get_dow() const
	{
		return m_dow;
	}

	inline int CronValueNthDayOfWeek::get_nth() const
	{
		return m_nth;
	}

	class CronValueLastDayOfWeek
		: public CronValue
	{
	public:

		static RefPtr<CronValue> Create(CronElement element, int dow);

		CronValueLastDayOfWeek(const CronValueLastDayOfWeek&) = delete;
		virtual ~CronValueLastDayOfWeek() = default;
		void operator =(const CronValueLastDayOfWeek&) = delete;

		int get_dow() const;

		__declspec(property(get = get_dow)) int dow;

	protected:

		CronValueLastDayOfWeek(CronElement element, int dow);

		int m_dow;
	};

	inline CronValueLastDayOfWeek::CronValueLastDayOfWeek(CronElement element, int dow)
		: CronValue(CRON_LAST_DAYOFWEEK, element)
		, m_dow(dow)
	{
	}

	inline int CronValueLastDayOfWeek::get_dow() const
	{
		return m_dow;
	}
}
