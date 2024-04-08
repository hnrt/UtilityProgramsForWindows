#pragma once

#include "hnrt/AnyCodeException.h"

namespace hnrt
{
	class SFIDException
		: public AnyCodeException
	{
	public:

		SFIDException(int nReason, int nOffset, PCWSTR pszFormat, ...);
		SFIDException(const SFIDException& other);
		virtual ~SFIDException() = default;
		SFIDException& operator =(const SFIDException& other);
	};
}

#define SFID_SUCCESS                 0
#define SFID_EMPTY                   1
#define SFID_TOO_SHORT               2
#define SFID_TOO_LONG                3
#define SFID_KEYPREFIX_TOO_SHORT     4
#define SFID_KEYPREFIX_TOO_LONG      5
#define SFID_MALFORMED_KEYPREFIX     6
#define SFID_INSTANCE_TOO_SHORT      7
#define SFID_INSTANCE_TOO_LONG       8
#define SFID_MALFORMED_INSTANCE      9
#define SFID_MALFORMED_RESERVED     10
#define SFID_UNIQUEID_TOO_SHORT     11
#define SFID_UNIQUEID_TOO_LONG      12
#define SFID_MALFORMED_UNIQUEID     13
#define SFID_CHECKSUM_TOO_SHORT     14
#define SFID_CHECKSUM_TOO_LONG      15
#define SFID_MALFORMED_CHECKSUM     16
#define SFID_INCORRECT_CHECKSUM     17
