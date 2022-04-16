#pragma once

#include "hnrt/DataFeeder.h"
#include "hnrt/WindowsHandle.h"
#include "hnrt/Win32Exception.h"

#define FILEDATAFEEDER_DEFAULT_CAPACITY 65536UL

namespace hnrt
{
	class FileDataFeeder
		: public DataFeeder
	{
	public:

		FileDataFeeder(PCWSTR pszFileName, DWORD dwCapacity = FILEDATAFEEDER_DEFAULT_CAPACITY);
		FileDataFeeder(const FileDataFeeder&) = delete;
		virtual ~FileDataFeeder() = default;
		void operator =(const FileDataFeeder&) = delete;
		virtual bool HasNext();
		ULONGLONG get_TotalLength() const;

		__declspec(property(get = get_TotalLength)) ULONGLONG TotalLength;

	protected:

		WindowsHandle m_h;
		ULONGLONG m_cbTotalLength;
	};

	inline FileDataFeeder::FileDataFeeder(PCWSTR pszFileName, DWORD dwCapacity)
		: DataFeeder(dwCapacity)
		, m_h(CreateFileW(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))
		, m_cbTotalLength(0ULL)
	{
		if (m_h == INVALID_HANDLE_VALUE)
		{
			DWORD dwError = GetLastError();
			delete[] m_pBuffer;
			m_pBuffer = nullptr;
			throw Win32Exception(dwError, L"CreateFile(OPEN_EXISTING) failed.");
		}
	}

	inline bool FileDataFeeder::HasNext()
	{
		if (!ReadFile(m_h, m_pBuffer, m_dwCapacity, &m_dwRead, NULL))
		{
			DWORD dwError = GetLastError();
			throw Win32Exception(dwError, L"ReadFile failed.");
		}
		m_cbTotalLength += m_dwRead;
		return m_dwRead > 0UL ? true : false;
	}

	inline size_t FileDataFeeder::get_TotalLength() const
	{
		return m_cbTotalLength;
	}
}