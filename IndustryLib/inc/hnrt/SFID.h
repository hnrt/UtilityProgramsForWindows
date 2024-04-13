#pragma once

#include "hnrt/String.h"

#define SFID_LENGTH 18
#define SFID_LENGTH_EXCLUDING_CHECKSUM 15

#define SFID_KEYPREFIX_NAME L"Key Prefix"
#define SFID_KEYPREFIX_OFFSET 0
#define SFID_KEYPREFIX_LENGTH 3

#define SFID_INSTANCE_NAME L"Instance"
#define SFID_INSTANCE_OFFSET (SFID_KEYPREFIX_OFFSET + SFID_KEYPREFIX_LENGTH)
#define SFID_INSTANCE_LENGTH 2

#define SFID_RESERVED_NAME L"Reserved"
#define SFID_RESERVED_VALUE L"0"
#define SFID_RESERVED_OFFSET (SFID_INSTANCE_OFFSET + SFID_INSTANCE_LENGTH)
#define SFID_RESERVED_LENGTH 1

#define SFID_UNIQUEID_NAME L"Unique Indentifier"
#define SFID_UNIQUEID_OFFSET (SFID_RESERVED_OFFSET + SFID_RESERVED_LENGTH)
#define SFID_UNIQUEID_LENGTH 9

#define SFID_CHECKSUM_NAME L"Checksum"
#define SFID_CHECKSUM_OFFSET (SFID_UNIQUEID_OFFSET + SFID_UNIQUEID_LENGTH)
#define SFID_CHECKSUM_LENGTH 3

namespace hnrt
{
	class SFID
	{
	public:

		static SFID Parse(PCWSTR psz);
		static SFID Parse(PCWSTR pszKeyPrefix, PCWSTR pszInstance, PCWSTR pszUniqueId);
		static String ComputeChecksum(const WCHAR s[]);
		static LONGLONG ComputeSerialNumber(String sz);
		static BOOL IsValid(WCHAR c);

		SFID();
		SFID(const SFID& other) = default;
		~SFID() = default;
		SFID& operator =(const SFID& other);
		SFID& operator =(PCWSTR psz);
		SFID& operator +=(int delta);
		SFID& operator -=(int delta);
		operator PCWSTR() const;
		String get_KeyPrefix() const;
		String get_Instance() const;
		String get_Reserved() const;
		String get_UniqueId() const;
		String get_Checksum() const;
		LONGLONG get_UniqueIdValue() const;
		void put_UniqueIdValue(LONGLONG);
		LONGLONG get_UniqueIdMaxValue() const;

		__declspec(property(get = get_KeyPrefix)) String KeyPrefix;
		__declspec(property(get = get_Instance)) String Instance;
		__declspec(property(get = get_Reserved)) String Reserved;
		__declspec(property(get = get_UniqueId)) String UniqueId;
		__declspec(property(get = get_Checksum)) String Checksum;
		__declspec(property(get = get_UniqueIdValue, put = put_UniqueIdValue)) LONGLONG UniqueIdValue;
		__declspec(property(get = get_UniqueIdMaxValue)) LONGLONG UniqueIdMaxValue;

	protected:

		static int ParseBase62(PCWSTR psz, int length, int offset, bool bCheckEnd);
		static String Base62Encode(ULONGLONG value, int length);

		SFID(PCWSTR psz);
		SFID(PCWSTR pszKeyPrefix, PCWSTR pszInstance, PCWSTR pszUniqueId);

		WCHAR m_d[SFID_LENGTH + 1];
	};

	inline SFID::operator PCWSTR() const
	{
		return m_d;
	}

	inline String SFID::get_KeyPrefix() const
	{
		return String(&m_d[SFID_KEYPREFIX_OFFSET], SFID_KEYPREFIX_LENGTH);
	}

	inline String SFID::get_Instance() const
	{
		return String(&m_d[SFID_INSTANCE_OFFSET], SFID_INSTANCE_LENGTH);
	}

	inline String SFID::get_Reserved() const
	{
		return String(&m_d[SFID_RESERVED_OFFSET], SFID_RESERVED_LENGTH);
	}

	inline String SFID::get_UniqueId() const
	{
		return String(&m_d[SFID_UNIQUEID_OFFSET], SFID_UNIQUEID_LENGTH);
	}

	inline String SFID::get_Checksum() const
	{
		return String(&m_d[SFID_CHECKSUM_OFFSET], SFID_CHECKSUM_LENGTH);
	}
}
