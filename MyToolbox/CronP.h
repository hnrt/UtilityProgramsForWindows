#pragma once


#include <Windows.h>
#include "CronValue.h"


#define CRON_TOKEN_EOF     0x000000
#define CRON_TOKEN_INTEGER 0x110001
#define CRON_TOKEN_WORD    0x110002


#define CRON_WC_STEP		1U
#define CRON_WC_ANY			2U
#define CRON_WC_LASTDAY		4U
#define CRON_WC_WEEKDAY		8U
#define CRON_WC_NTH			16U


namespace hnrt
{
	class CronTokenizer
	{
	public:

		CronTokenizer(PCWSTR);
		CronTokenizer(const CronTokenizer&) = delete;
		~CronTokenizer() = default;
		void operator =(const CronTokenizer&) = delete;
		int GetNext();
		void FindValue(const PCWSTR*, int);
		unsigned int GetValue() const;

	private:

		const WCHAR* m_o;
		const WCHAR* m_p;
		WCHAR m_c;
		int m_v;
		WCHAR m_s[4];
	};

	inline unsigned int CronTokenizer::GetValue() const
	{
		return m_v;
	}

	class CronParser
	{
	public:

		CronParser(PCWSTR);
		CronParser(const CronParser&) = delete;
		~CronParser() = default;
		void operator =(const CronParser&) = delete;
		union CronValue* Run(CronElement element, UINT flags = 0, const PCWSTR* ppsz = nullptr);
		bool isEOF() const;

	private:

		bool ParseRange(CronElement element, int min, const PCWSTR* ppsz, int& value);
		bool ParseStep(CronElement element, int min, const PCWSTR* ppsz, int& value);
		bool ParseInteger(CronElement element, int min, int max, const PCWSTR* ppsz, int& value);

		CronTokenizer m_tokenizer;
		int m_next;
	};
	}
