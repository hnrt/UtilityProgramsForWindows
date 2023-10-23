#pragma once


#include "hnrt/RefObj.h"
#include "hnrt/RefPtr.h"


namespace hnrt
{
	class CronValueEvaluation
		: public RefObj
	{
	public:

		CronValueEvaluation(int*, int);
		CronValueEvaluation(const CronValueEvaluation&) = delete;
		virtual ~CronValueEvaluation();
		void operator =(const CronValueEvaluation&) = delete;
		int operator [](int) const;
		operator PCWSTR() const;
		PCWSTR ToString() const;
		bool EqualToOrGreaterThan(int, WORD&);
		bool Next(WORD&);
		int get_Len() const;

		__declspec(property(get = get_Len)) int Len;

	private:

		int* m_ptr;
		int m_len;
		PWSTR m_psz;
		int m_cur;
	};

	inline CronValueEvaluation::operator PCWSTR() const
	{
		return ToString();
	}

	inline int CronValueEvaluation::get_Len() const
	{
		return m_len;
	}
}
