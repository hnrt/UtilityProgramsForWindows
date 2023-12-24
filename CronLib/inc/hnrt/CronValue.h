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
		int Count(CronValueType) const;
		/// <summary>
		/// Checks if the corresponding element of the given SYSTEMTIME matches with one of the scheduled timings this value defines.
		/// If there is no exact match, the given SYSTEMTIME will be altered by using the closest scheduled timing (called as 'candidate').
		/// Note that the definition of the closest scheduled timing is the earliest scheduled time that is later than the given time.
		/// </summary>
		/// <param name="st">[IN/OUT] Time to check</param>
		/// <returns>
		/// 1 if the given SYSTEMTIME has the exact match; no changes are made.
		/// -1 if a candidate was found; the given SYSTEMTIME was altered accordingly.
		/// 0 if no candidate was found.
		/// </returns>
		int GetNext(SYSTEMTIME&) const;

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
		static bool CheckRange(int from, int to, int step, int target, int& candidate);
		static bool CheckWeekDay(WORD wYear, WORD wMonth, WORD wDay, int target, int& candidate);
		static bool CheckClosestWeekDay(WORD wYear, WORD wMonth, int day, int target, int& candidate);
		static bool CheckLastDay(WORD wYear, WORD wMonth, int target, int& candidate);
		static bool CheckDayOfWeekRange(int from, int to, WORD wDay, WORD wLastDay, int target, int& candidate);
		static bool CheckLastDayOfWeek(WORD wYear, WORD wMonth, WORD wDay, int target, int& candidate);
		static bool CheckNthDayOfWeek(WORD wYear, WORD wMonth, WORD wDay, int dow, int nth, int target, int& candidate);
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
