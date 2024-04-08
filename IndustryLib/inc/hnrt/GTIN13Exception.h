#pragma once

#include "hnrt/AnyCodeException.h"

namespace hnrt
{
	class GTIN13Exception
		: public AnyCodeException
	{
	public:

		GTIN13Exception(int nReason, int nOffset, PCWSTR pszFormat, ...);
		GTIN13Exception(const GTIN13Exception& other);
		virtual ~GTIN13Exception() = default;
		GTIN13Exception& operator =(const GTIN13Exception& other);
	};
}

#define GTIN13_SUCCESS 0
#define GTIN13_INVALID_GS1COMPANYPREFIXLENGTH 1
#define GTIN13_EMPTY 2
#define GTIN13_MALFORMED_GS1COMPANYPREFIX 3
#define GTIN13_MALFORMED_ITEMREFERENCE 4
#define GTIN13_INCORRECT_CHECKDIGIT 5
#define GTIN13_EXTRA_CHARACTERS 6
