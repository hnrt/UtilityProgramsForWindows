#pragma once

#include <Windows.h>

namespace hnrt
{
	class DataFeeder
	{
	public:

		DataFeeder(DWORD dwCapacity);
		DataFeeder(const DataFeeder&) = delete;
		virtual ~DataFeeder();
		void operator =(const DataFeeder&) = delete;
		virtual bool HasNext() = 0;
		virtual PBYTE get_Data();
		virtual DWORD get_DataLength();

		__declspec(property(get = get_Data)) PBYTE Data;
		__declspec(property(get = get_DataLength)) DWORD DataLength;

	protected:

		PBYTE m_pBuffer;
		DWORD m_dwCapacity;
		DWORD m_dwRead;
	};

	inline DataFeeder::DataFeeder(DWORD dwCapacity)
		: m_pBuffer(new BYTE[dwCapacity])
		, m_dwCapacity(dwCapacity)
		, m_dwRead(0)
	{
	}

	inline DataFeeder::~DataFeeder()
	{
		delete[] m_pBuffer;
	}

	inline PBYTE DataFeeder::get_Data()
	{
		return m_pBuffer;
	}

	inline DWORD DataFeeder::get_DataLength()
	{
		return m_dwRead;
	}
}