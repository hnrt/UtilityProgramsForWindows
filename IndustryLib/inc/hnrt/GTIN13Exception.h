#pragma once

#include "hnrt/Exception.h"

namespace hnrt
{
	class GTIN13Exception
		: public Exception
	{
	public:

		GTIN13Exception(int nReason, int nOffset, PCWSTR pszFormat, ...);
		GTIN13Exception(const GTIN13Exception& other);
		virtual ~GTIN13Exception() = default;
		GTIN13Exception& operator =(const GTIN13Exception& other);
		int get_Reason() const;
		int get_Offset() const;

		__declspec(property(get = get_Reason)) int Reason;
		__declspec(property(get = get_Offset)) int Offset;

	protected:

		int m_nReason;
		int m_nOffset;
	};

	inline int GTIN13Exception::get_Reason() const
	{
		return m_nReason;
	}

	inline int GTIN13Exception::get_Offset() const
	{
		return m_nOffset;
	}
}

#define GTIN13_NOERROR 0
#define GTIN13_INVALID_GS1COMPANYPREFIXLENGTH 1
#define GTIN13_EMPTY 2
#define GTIN13_MALFORMED_GS1COMPANYPREFIX 3
#define GTIN13_MALFORMED_ITEMREFERENCE 4
#define GTIN13_INCORRECT_CHECKDIGIT 5
#define GTIN13_EXTRA_CHARACTERS 6
