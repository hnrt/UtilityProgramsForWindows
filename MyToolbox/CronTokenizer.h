#pragma once


#include <Windows.h>
#include "CronElement.h"


#define CRON_TOKEN_EOF           0x000000
#define CRON_TOKEN_INTEGER       0x110001
#define CRON_TOKEN_INTEGER_W     0x110002
#define CRON_TOKEN_INTEGER_L     0x110003
#define CRON_TOKEN_MONTH_MIN     0x111001
#define CRON_TOKEN_JAN           0x111001
#define CRON_TOKEN_FEB           0x111002
#define CRON_TOKEN_MAR           0x111003
#define CRON_TOKEN_APR           0x111004
#define CRON_TOKEN_MAY           0x111005
#define CRON_TOKEN_JUN           0x111006
#define CRON_TOKEN_JUL           0x111007
#define CRON_TOKEN_AUG           0x111008
#define CRON_TOKEN_SEP           0x111009
#define CRON_TOKEN_OCT           0x11100A
#define CRON_TOKEN_NOV           0x11100B
#define CRON_TOKEN_DEC           0x11100C
#define CRON_TOKEN_MONTH_MAX     0x11100C
#define CRON_TOKEN_DAYOFWEEK_MIN 0x111011
#define CRON_TOKEN_SUN           0x111011
#define CRON_TOKEN_MON           0x111012
#define CRON_TOKEN_TUE           0x111013
#define CRON_TOKEN_WED           0x111014
#define CRON_TOKEN_THU           0x111015
#define CRON_TOKEN_FRI           0x111016
#define CRON_TOKEN_SAT           0x111017
#define CRON_TOKEN_DAYOFWEEK_MAX 0x111017


namespace hnrt
{
	extern const PCWSTR CronMonthWords[];
	extern const PCWSTR CronDayOfWeekWords[];

	class CronTokenizer
	{
	public:

		CronTokenizer(PCWSTR, CronElement = CRON_ELEMENT_UNSPECIFIED);
		CronTokenizer(const CronTokenizer&) = delete;
		~CronTokenizer() = default;
		void operator =(const CronTokenizer&) = delete;
		void SetElement(CronElement);
		int GetNext();
		int GetValue() const;
		int GetOffset() const;

	private:

		int Find(const PCWSTR*, PCWSTR);

		const WCHAR* m_o;
		const WCHAR* m_p;
		const WCHAR* m_q;
		WCHAR m_c;
		int m_v;
		CronElement m_e;
	};

	inline void CronTokenizer::SetElement(CronElement element)
	{
		m_e = element;
	}

	inline int CronTokenizer::GetValue() const
	{
		return m_v;
	}

	inline int CronTokenizer::GetOffset() const
	{
		return static_cast<int>(m_p - m_o);
	}
}
