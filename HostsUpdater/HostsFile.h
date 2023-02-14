#pragma once

#include "HostEntryCollection.h"
#include "hnrt/WindowsHandle.h"
#include "hnrt/Buffer.h"
#include "hnrt/Exception.h"
#include <list>
#include <map>

namespace hnrt
{
	class HostsFile
	{
	public:

		typedef std::pair<HostEntry*, PCWSTR> HostEntryAddressPair;
		typedef std::map<HostEntry*, PCWSTR> UpdateMap;
		typedef std::pair<PCWSTR, PCWSTR> AddressAliasPair;
		typedef std::list<AddressAliasPair> AppendList;

		HostsFile(PCWSTR pszFileName);
		HostsFile(const HostsFile&) = delete;
		~HostsFile();
		void operator =(const HostsFile&) = delete;
		void Open();
		void Close();
		void Read();
		void CreateBackup();
		void Write();
		void Parse();
		void Rebuild(const UpdateMap& updateEntries, const AppendList& appendEntries);

		PCWSTR get_FileName() const;
		DWORD get_Error() const;
		DWORD get_Length() const;
		HostEntryCollection& get_Entries();
		const Buffer<WCHAR>& get_Buffer() const;

		__declspec(property(get = get_FileName)) PCWSTR Name;
		__declspec(property(get = get_Error)) DWORD Error;
		__declspec(property(get = get_Length)) DWORD Length;
		__declspec(property(get = get_Entries)) HostEntryCollection& Entries;
		__declspec(property(get = get_Buffer)) const Buffer<WCHAR>& UTF16;

	private:

		size_t Rebuild(const UpdateMap& updateEntries, const AppendList& appendEntries, WCHAR buf[], size_t bufsz);

		PCWSTR m_pszFileName;
		WindowsHandle m_hFile;
		DWORD m_dwError;
		Buffer<WCHAR> m_buf;
		HostEntryCollection m_Entries;
	};

	inline PCWSTR HostsFile::get_FileName() const
	{
		return m_pszFileName;
	}

	inline DWORD HostsFile::get_Error() const
	{
		return m_dwError;
	}

	inline DWORD HostsFile::get_Length() const
	{
		return static_cast<DWORD>(WideCharToMultiByte(CP_UTF8, 0, m_buf, static_cast<int>(m_buf.Len), NULL, 0, NULL, NULL));
	}

	inline HostEntryCollection& HostsFile::get_Entries()
	{
		return m_Entries;
	}

	inline const Buffer<WCHAR>& HostsFile::get_Buffer() const
	{
		return m_buf;
	}
}
