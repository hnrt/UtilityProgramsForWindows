#pragma once


#include <Windows.h>


#define CRON_TOKEN_EOF     0x000000
#define CRON_TOKEN_INTEGER 0x110001
#define CRON_TOKEN_WORD    0x110002


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
		int GetValue() const;

	private:

		const WCHAR* m_o;
		const WCHAR* m_p;
		WCHAR m_c;
		int m_v;
		WCHAR m_s[4];
	};

	inline int CronTokenizer::GetValue() const
	{
		return m_v;
	}
}
