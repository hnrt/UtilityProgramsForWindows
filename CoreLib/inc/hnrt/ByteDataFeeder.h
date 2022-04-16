#pragma once

#include "hnrt/DataFeeder.h"

#define BYTEDATAFEEDER_DEFAULT_CAPACITY 65536UL

namespace hnrt
{
	class ByteDataFeeder
		: public DataFeeder
	{
	public:

		ByteDataFeeder(const void* pSource, size_t cbSourceLength, DWORD dwCapacity = BYTEDATAFEEDER_DEFAULT_CAPACITY);
		ByteDataFeeder(const ByteDataFeeder&) = delete;
		virtual ~ByteDataFeeder();
		void operator =(const ByteDataFeeder&) = delete;
		virtual bool HasNext();

	protected:

		const BYTE* m_pCur;
		const BYTE* m_pEnd;
	};

	inline ByteDataFeeder::ByteDataFeeder(const void* pSource, size_t cbSourceLength, DWORD dwCapacity)
		: DataFeeder(dwCapacity)
		, m_pCur(reinterpret_cast<const BYTE*>(pSource))
		, m_pEnd(reinterpret_cast<const BYTE*>(pSource) + cbSourceLength)
	{
	}

	inline ByteDataFeeder::~ByteDataFeeder()
	{
	}

	inline bool ByteDataFeeder::HasNext()
	{
		if (m_pCur < m_pEnd)
		{
			ptrdiff_t cbRemaining = m_pEnd - m_pCur;
			m_dwRead = (cbRemaining < m_dwCapacity) ? static_cast<DWORD>(cbRemaining) : m_dwCapacity;
			memcpy_s(m_pBuffer, m_dwRead, m_pCur, m_dwRead);
			m_pCur += m_dwRead;
			return true;
		}
		else
		{
			m_dwRead = 0UL;
			return false;
		}
	}
}