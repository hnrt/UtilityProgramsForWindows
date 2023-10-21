#pragma once


#include "CronValue.h"
#include "CronTokenizer.h"


namespace hnrt
{
	class CronParser
	{
	public:

		CronParser(PCWSTR);
		CronParser(const CronParser&) = delete;
		~CronParser() = default;
		void operator =(const CronParser&) = delete;
		void Run(CronValue*& pYear, CronValue*& pMonth, CronValue*& pDayOfMonth, CronValue*& pDayOfWeek, CronValue*& pHour, CronValue*& pMinute);
		void Run(CronValue*& pYear, CronValue*& pMonth, CronValue*& pDayOfMonth, CronValue*& pDayOfWeek, CronValue*& pHour, CronValue*& pMinute, CronValue*& pSecond);
		CronValue* RunOnlyFor(CronElement element);
		bool isEnd() const;

	private:

		CronValue* Run(CronElement element);
		CronValue* Run(CronElement element, UINT flags);
		bool ParseRange(CronElement element, int min, int& value);
		bool ParseStep(CronElement element, int& value);
		bool ParseOrdinal(int& value);
		bool ParseIntegerOrLabel(CronElement element, int min, int max, int& value);
		bool ParseInteger(CronElement element, int min, int max, int& value);
		bool ParseIntegerW(int& value);
		bool ParseIntegerL(int& value);

		CronTokenizer m_tokenizer;
		int m_next;
	};

	inline bool CronParser::isEnd() const
	{
		return m_next == CRON_TOKEN_EOF;
	}
}
