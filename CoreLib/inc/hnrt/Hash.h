#pragma once

#include "hnrt/BCryptAlgHandle.h"
#include "hnrt/BCryptHashHandle.h"
#include "hnrt/DataFeeder.h"

namespace hnrt
{
	class Hash
	{
	public:

		Hash();
		Hash(PCWSTR pszAlgId);
		Hash(const Hash& src);
		~Hash();
		void operator =(const Hash& src);
		void Close();
		const BYTE* get_Value() const;
		DWORD get_ValueLength() const;
		PCWSTR get_Text() const;

		__declspec(property(get = get_Value)) const BYTE* Value;
		__declspec(property(get = get_ValueLength)) DWORD ValueLength;
		__declspec(property(get = get_Text)) PCWSTR Text;

	protected:

		void Compute(DataFeeder& rDataFeeder);

		PCWSTR m_pszAlgId;
		BCryptAlgHandle m_hAlg;
		BCryptHashHandle m_hHash;
		PBYTE m_pValue;
		DWORD m_dwValueLength;
		PWSTR m_psz;
	};

	inline const BYTE* Hash::get_Value() const
	{
		return m_pValue;
	}

	inline DWORD Hash::get_ValueLength() const
	{
		return m_dwValueLength;
	}

	class MD5Hash
		: public Hash
	{
	public:

		MD5Hash(DataFeeder& rDataFeeder);
		MD5Hash(const MD5Hash& src);
		~MD5Hash() = default;
		void operator =(const MD5Hash& src);
	};

	class SHA1Hash
		: public Hash
	{
	public:

		SHA1Hash(DataFeeder& rDataFeeder);
		SHA1Hash(const SHA1Hash& src);
		~SHA1Hash() = default;
		void operator =(const SHA1Hash& src);
	};

	class SHA256Hash
		: public Hash
	{
	public:

		SHA256Hash(DataFeeder& rDataFeeder);
		SHA256Hash(const SHA256Hash& src);
		~SHA256Hash() = default;
		void operator =(const SHA256Hash& src);
	};

	class SHA384Hash
		: public Hash
	{
	public:

		SHA384Hash(DataFeeder& rDataFeeder);
		SHA384Hash(const SHA384Hash& src);
		~SHA384Hash() = default;
		void operator =(const SHA384Hash& src);
	};

	class SHA512Hash
		: public Hash
	{
	public:

		SHA512Hash(DataFeeder& rDataFeeder);
		SHA512Hash(const SHA512Hash& src);
		~SHA512Hash() = default;
		void operator =(const SHA512Hash& src);
	};
}