#pragma once

#include "hnrt/String.h"
#include "hnrt/DataFeeder.h"
#include "hnrt/RefPtr.h"

namespace hnrt
{
	class Hash
	{
	public:

		Hash();
		Hash(const Hash&);
		virtual ~Hash();
		void operator =(const Hash&);
		void Close();
		Hash& Set(RefPtr<DataFeeder>);
		Hash& Set(const void*, size_t);
		const BYTE* get_Value() const;
		DWORD get_ValueLength() const;

		__declspec(property(get = get_Value)) const BYTE* Value;
		__declspec(property(get = get_ValueLength)) DWORD ValueLength;

	protected:

		Hash(PCWSTR);
		Hash(PCWSTR, RefPtr<DataFeeder>);
		void Compute() const;

		PCWSTR m_pszAlgorithm;
		mutable RefPtr<DataFeeder> m_pDataFeeder;
		mutable DWORD m_dwValueLength;
		mutable PBYTE m_pValue;
	};

	class MD5Hash
		: public Hash
	{
	public:

		MD5Hash(RefPtr<DataFeeder>);
		MD5Hash(const void*, size_t);
		MD5Hash(const MD5Hash&);
		~MD5Hash() = default;
		void operator =(const MD5Hash&);
	};

	class SHA1Hash
		: public Hash
	{
	public:

		SHA1Hash(RefPtr<DataFeeder>);
		SHA1Hash(const void*, size_t);
		SHA1Hash(const SHA1Hash&);
		~SHA1Hash() = default;
		void operator =(const SHA1Hash&);
	};

	class SHA256Hash
		: public Hash
	{
	public:

		SHA256Hash(RefPtr<DataFeeder>);
		SHA256Hash(const void*, size_t);
		SHA256Hash(const SHA256Hash&);
		~SHA256Hash() = default;
		void operator =(const SHA256Hash&);
	};

	class SHA384Hash
		: public Hash
	{
	public:

		SHA384Hash(RefPtr<DataFeeder>);
		SHA384Hash(const void*, size_t);
		SHA384Hash(const SHA384Hash&);
		~SHA384Hash() = default;
		void operator =(const SHA384Hash&);
	};

	class SHA512Hash
		: public Hash
	{
	public:

		SHA512Hash(RefPtr<DataFeeder>);
		SHA512Hash(const void*, size_t);
		SHA512Hash(const SHA512Hash&);
		~SHA512Hash() = default;
		void operator =(const SHA512Hash&);
	};
}