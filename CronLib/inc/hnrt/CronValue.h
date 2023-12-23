#pragma once


#include "hnrt/RefObj.h"
#include "hnrt/RefPtr.h"
#include "hnrt/String.h"
#include "hnrt/CronElement.h"
#include "hnrt/CronValueType.h"


#define CRON_WORD_DISPLACEMENT 10000

#define CRON_NUMBER(x) (((x) < CRON_WORD_DISPLACEMENT) ? (x) : ((x) - CRON_WORD_DISPLACEMENT))


namespace hnrt
{
	class CronValue
		: public RefObj
	{
	public:

		CronValue(const CronValue&) = delete;
		virtual ~CronValue();
		void operator =(const CronValue&) = delete;
		void Append(RefPtr<CronValue>);
		String ToString() const;
		int Count(CronValueType type) const;

		CronElement get_Element() const;
		CronValueType get_Type() const;
		int get_Value() const;
		int get_Step() const;
		int get_From() const;
		int get_To() const;
		int get_Nth() const;
		RefPtr<CronValue> get_Next() const;
		RefPtr<CronValue> get_Self() const;

		__declspec(property(get = get_Element)) CronElement Element;
		__declspec(property(get = get_Type)) CronValueType Type;
		__declspec(property(get = get_Value)) int Value;
		__declspec(property(get = get_Step)) int Step;
		__declspec(property(get = get_From)) int From;
		__declspec(property(get = get_To)) int To;
		__declspec(property(get = get_Nth)) int Nth;
		__declspec(property(get = get_Next)) RefPtr<CronValue> Next;
		__declspec(property(get = get_Self)) RefPtr<CronValue> Self;

	protected:

		CronValue(CronElement element, CronValueType type);

		CronElement m_Element;
		CronValueType m_Type;
		union CronValueComplex {
			struct CronValueSingle {
				int value;
				int step;
			} single;
			struct CronValueRange {
				int from;
				int to;
				int step;
			} range;
			struct CronValueDayOfMonth {
				int value;
			} dom;
			struct CronValueDayOfWeek {
				int value;
				int nth;
			} dow;
		} m_Complex;
		RefPtr<CronValue> m_pNext;

	public:

		static PCWSTR Name(CronElement element);
		static int Min(CronElement element);
		static int Max(CronElement element);
		static RefPtr<CronValue> InvalidValue();
		static RefPtr<CronValue> CreateEmpty(CronElement element);
		static RefPtr<CronValue> CreateAll(CronElement element);
		static RefPtr<CronValue> CreateAny(CronElement element);
		static RefPtr<CronValue> CreateLastDay(CronElement element);
		static RefPtr<CronValue> CreateWeekDay(CronElement element);
		static RefPtr<CronValue> CreateSingle(CronElement element, int value, int step = 0);
		static RefPtr<CronValue> CreateRange(CronElement element, int from, int to, int step = 1);
		static RefPtr<CronValue> CreateClosestWeekDay(CronElement element, int dom);
		static RefPtr<CronValue> CreateNthDayOfWeek(CronElement element, int dow, int nth);
		static RefPtr<CronValue> CreateLastDayOfWeek(CronElement element, int dow);
	};

	inline CronElement CronValue::get_Element() const
	{
		return m_Element;
	}

	inline CronValueType CronValue::get_Type() const
	{
		return m_Type;
	}

	inline RefPtr<CronValue> CronValue::get_Next() const
	{
		return m_pNext;
	}

	inline RefPtr<CronValue> CronValue::get_Self() const
	{
		const_cast<CronValue*>(this)->AddRef();
		return RefPtr<CronValue>(const_cast<CronValue*>(this));
	}
}
