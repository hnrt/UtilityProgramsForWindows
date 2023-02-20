#include "HostsFile.h"
#include "HostsReader.h"
#include "hnrt/Heap.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/ResourceString.h"
#include "hnrt/Debug.h"
#include "resource.h"
#include <stdio.h>


using namespace hnrt;


HostsFile::HostsFile(PCWSTR pszFileName, bool bReadOnly)
	: m_pszFileName(Clone(pszFileName))
	, m_bReadOnly(bReadOnly)
	, m_hFile(INVALID_HANDLE_VALUE)
	, m_dwError(ERROR_SUCCESS)
	, m_buf()
{
	DBGFNC(L"HostsFile::ctor");
	DBGPUT(L"FileName=%s", pszFileName);
}


HostsFile::~HostsFile()
{
	DBGFNC(L"HostsFile::dtor");
	Close();
	free(m_pszFileName);
}


void HostsFile::Open()
{
	m_hFile = CreateFileW(m_pszFileName, m_bReadOnly ? GENERIC_READ : GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!m_hFile.isValid)
	{
		m_dwError = GetLastError();
		throw Exception(ResourceString(IDS_FAILED_OPEN), m_pszFileName, m_dwError, ErrorMessage::Get(m_dwError));
	}
}


void HostsFile::Close()
{
	DBGFNC(L"HostsFile::Close");
	m_hFile.Close();
	m_buf.Resize(0);
	m_Entries.Resize(0);
	m_dwError = ERROR_SUCCESS;
}


void HostsFile::Read()
{
	DBGFNC(L"HostsFile::Read");
	int cb = GetFileSize(m_hFile, NULL);
	if (cb == INVALID_FILE_SIZE)
	{
		m_dwError = GetLastError();
		DBGPUT(L"GetFileSize: %lu %s", m_dwError, ErrorMessage::Get(m_dwError));
		throw Exception(ResourceString(IDS_FAILED_GETSIZE), m_pszFileName, m_dwError, ErrorMessage::Get(m_dwError));
	}
	Buffer<CHAR> buf(cb);
	DWORD dwRead = 0;
	if (!ReadFile(m_hFile, buf, cb, &dwRead, NULL))
	{
		m_dwError = GetLastError();
		DBGPUT(L"ReadFile: %lu %s", m_dwError, ErrorMessage::Get(m_dwError));
		throw Exception(ResourceString(IDS_FAILED_READ), m_pszFileName, m_dwError, ErrorMessage::Get(m_dwError));
	}
	if (cb != static_cast<int>(dwRead))
	{
		m_dwError = ERROR_INCORRECT_SIZE;
		DBGPUT(L"ReadFile: expected=%d actual=%d", cb, static_cast<int>(dwRead));
		throw Exception(ResourceString(IDS_FAILED_READ_SIZE_MISMATCH), m_pszFileName, cb, static_cast<int>(dwRead));
	}
	int cch = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, buf, cb, NULL, 0);
	m_buf.Resize(cch);
	MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, buf, cb, m_buf, cch);
	DBGPUT(L"bytes=%d characters=%d", cb, cch);
}


void HostsFile::CreateBackup()
{
	if (m_bReadOnly)
	{
		return;
	}
	DBGFNC(L"HostsFile::CreateBackup");
	WCHAR szFileName[MAX_PATH] = { 0 };
	_snwprintf_s(szFileName, _TRUNCATE, L"%s.backup", m_pszFileName);
	DBGPUT(L"FileName=%s", szFileName);
	WindowsHandle hFile = CreateFileW(szFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!m_hFile.isValid)
	{
		m_dwError = GetLastError();
		DBGPUT(L"CreateFile: %lu %s", m_dwError, ErrorMessage::Get(m_dwError));
		throw Exception(ResourceString(IDS_FAILED_OPEN), m_pszFileName, m_dwError, ErrorMessage::Get(m_dwError));
	}
	DWORD dwError = GetLastError();
	if (dwError == ERROR_ALREADY_EXISTS)
	{
		int cb = GetFileSize(hFile, NULL);
		if (cb == INVALID_FILE_SIZE)
		{
			m_dwError = GetLastError();
			DBGPUT(L"GetFileSize: %lu %s", m_dwError, ErrorMessage::Get(m_dwError));
			throw Exception(ResourceString(IDS_FAILED_GETSIZE), m_pszFileName, m_dwError, ErrorMessage::Get(m_dwError));
		}
		Buffer<CHAR> buf1(cb);
		DWORD dwRead = 0;
		if (!ReadFile(hFile, buf1, cb, &dwRead, NULL))
		{
			m_dwError = GetLastError();
			DBGPUT(L"ReadFile: %lu %s", m_dwError, ErrorMessage::Get(m_dwError));
			throw Exception(ResourceString(IDS_FAILED_READ), m_pszFileName, m_dwError, ErrorMessage::Get(m_dwError));
		}
		if (cb != static_cast<int>(dwRead))
		{
			m_dwError = ERROR_INCORRECT_SIZE;
			DBGPUT(L"ReadFile: expected=%d actual=%d", cb, static_cast<int>(dwRead));
			throw Exception(ResourceString(IDS_FAILED_READ_SIZE_MISMATCH), m_pszFileName, cb, static_cast<int>(dwRead));
		}
		int cch = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, buf1, cb, NULL, 0);
		Buffer<WCHAR> buf2(cch);
		MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, buf1, cb, buf2, cch);
		if (buf2.Len == m_buf.Len && !wmemcmp(buf2, m_buf, m_buf.Len))
		{
			DBGPUT(L"No difference.");
			return;
		}
	}
	if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		m_dwError = GetLastError();
		DBGPUT(L"SetFilePointer: %lu %s", m_dwError, ErrorMessage::Get(m_dwError));
		throw Exception(ResourceString(IDS_FAILED_REWIND), m_pszFileName, m_dwError, ErrorMessage::Get(m_dwError));
	}
	int cch = static_cast<int>(m_buf.Len);
	int cb = WideCharToMultiByte(CP_UTF8, 0, m_buf, cch, NULL, 0, NULL, NULL);
	Buffer<CHAR> buf(cb);
	WideCharToMultiByte(CP_UTF8, 0, m_buf, cch, buf, cb, NULL, NULL);
	DWORD dwWritten = 0;
	if (!WriteFile(hFile, buf, cb, &dwWritten, NULL))
	{
		m_dwError = GetLastError();
		DBGPUT(L"WriteFile: %lu %s", m_dwError, ErrorMessage::Get(m_dwError));
		throw Exception(ResourceString(IDS_FAILED_WRITE), m_pszFileName, m_dwError, ErrorMessage::Get(m_dwError));
	}
	if (cb != static_cast<int>(dwWritten))
	{
		m_dwError = ERROR_INCORRECT_SIZE;
		DBGPUT(L"WriteFile: expected=%d actual=%d", cb, static_cast<int>(dwWritten));
		throw Exception(ResourceString(IDS_FAILED_WRITE_SIZE_MISMATCH), m_pszFileName, cb, static_cast<int>(dwWritten));
	}
	if (!SetEndOfFile(hFile))
	{
		m_dwError = GetLastError();
		DBGPUT(L"SetEndOfFile: %lu %s", m_dwError, ErrorMessage::Get(m_dwError));
		throw Exception(ResourceString(IDS_FAILED_SETSIZE), m_pszFileName, m_dwError, ErrorMessage::Get(m_dwError));
	}
	DBGPUT(L"characters=%d bytes=%d", cch, cb);
}


void HostsFile::Write()
{
	if (m_bReadOnly)
	{
		return;
	}
	DBGFNC(L"HostsFile::Write");
	if (SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		m_dwError = GetLastError();
		DBGPUT(L"SetFilePointer: %lu %s", m_dwError, ErrorMessage::Get(m_dwError));
		throw Exception(ResourceString(IDS_FAILED_REWIND), m_pszFileName, m_dwError, ErrorMessage::Get(m_dwError));
	}
	int cch = static_cast<int>(m_buf.Len);
	int cb = WideCharToMultiByte(CP_UTF8, 0, m_buf, cch, NULL, 0, NULL, NULL);
	Buffer<CHAR> buf(cb);
	WideCharToMultiByte(CP_UTF8, 0, m_buf, cch, buf, cb, NULL, NULL);
	DWORD dwWritten = 0;
	if (!WriteFile(m_hFile, buf, cb, &dwWritten, NULL))
	{
		m_dwError = GetLastError();
		DBGPUT(L"WriteFile: %lu %s", m_dwError, ErrorMessage::Get(m_dwError));
		throw Exception(ResourceString(IDS_FAILED_WRITE), m_pszFileName, m_dwError, ErrorMessage::Get(m_dwError));
	}
	if (cb != static_cast<int>(dwWritten))
	{
		m_dwError = ERROR_INCORRECT_SIZE;
		DBGPUT(L"WriteFile: expected=%d actual=%d", cb, static_cast<int>(dwWritten));
		throw Exception(ResourceString(IDS_FAILED_WRITE_SIZE_MISMATCH), m_pszFileName, cb, static_cast<int>(dwWritten));
	}
	if (!SetEndOfFile(m_hFile))
	{
		m_dwError = GetLastError();
		DBGPUT(L"SetEndOfFile: %lu %s", m_dwError, ErrorMessage::Get(m_dwError));
		throw Exception(ResourceString(IDS_FAILED_SETSIZE), m_pszFileName, m_dwError, ErrorMessage::Get(m_dwError));
	}
	DBGPUT(L"characters=%d bytes=%d", cch, cb);
}


void HostsFile::Parse()
{
	DBGFNC(L"HostsFile::Parse");
	m_dwError = ERROR_SUCCESS;
	m_Entries.Resize(0);
	HostsReader reader(m_buf, m_buf.Len);
	int t = reader.NextToken();
	while (true)
	{
		if (t == HOSTSREADER_WS)
		{
			DBGPUT(L"WS");
			t = reader.NextToken();
		}
		if (t == HOSTSREADER_IPV4ADDR || t == HOSTSREADER_IPV6ADDR)
		{
			DBGPUT(L"ADDR %.*s", reader.TokenLen, reader.TokenPtr);
			HostEntry entry;
			entry.Address = reader.Token;
			t = reader.NextToken();
			if (t == HOSTSREADER_WS)
			{
				DBGPUT(L"WS");
				t = reader.NextToken();
			}
			else
			{
				DBGPUT(L"ERROR");
				m_dwError = ERROR_INVALID_DATA;
				t = reader.NextToken();
				continue;
			}
			if (t == HOSTSREADER_HOSTNAME)
			{
				DBGPUT(L"NAME %.*s", reader.TokenLen, reader.TokenPtr);
				entry.Names += reader.Token;
				t = reader.NextToken();
			}
			else
			{
				DBGPUT(L"ERROR");
				m_dwError = ERROR_INVALID_DATA;
				t = reader.NextToken();
				continue;
			}
			while (t == HOSTSREADER_WS)
			{
				DBGPUT(L"WS");
				t = reader.NextToken();
				if (t == HOSTSREADER_HOSTNAME)
				{
					DBGPUT(L"NAME %.*s", reader.TokenLen, reader.TokenPtr);
					entry.Names += reader.Token;
					t = reader.NextToken();
				}
				else
				{
					break;
				}
			}
			m_Entries += entry;
		}
		if (t == HOSTSREADER_COMMENT)
		{
			DBGPUT(L"COMMENT");
			t = reader.NextToken();
		}
		if (t == L'\n')
		{
			DBGPUT(L"NL");
			t = reader.NextToken();
		}
		else if (t == HOSTSREADER_EOF)
		{
			DBGPUT(L"EOF(%lu)", m_dwError);
			if (m_dwError == ERROR_SUCCESS)
			{
				return;
			}
			else
			{
				throw Exception(ResourceString(IDS_FAILED_PARSE), m_pszFileName);
			}
		}
		else
		{
			DBGPUT(L"ERROR");
			m_dwError = ERROR_INVALID_DATA;
			t = reader.NextToken();
			continue;
		}
	}
}


void HostsFile::Rebuild(const UpdateMap& updateEntries, const AppendList& appendEntries)
{
	size_t size = Rebuild(updateEntries, appendEntries, nullptr, 0);
	Buffer<WCHAR> buf(size);
	Rebuild(updateEntries, appendEntries, buf, buf.Len);
	m_buf.Resize(buf.Len);
	wmemcpy_s(m_buf, m_buf.Len, buf, buf.Len);
}


size_t HostsFile::Rebuild(const UpdateMap& updateEntries, const AppendList& appendEntries, WCHAR buf[], size_t bufsz)
{
	size_t size = 0, len;
	LONGLONG offset = 0;
	for (DWORD dwIndex = 0; dwIndex < m_Entries.Count; dwIndex++)
	{
		UpdateMap::const_iterator iter = updateEntries.find(&m_Entries[dwIndex]);
		if (iter != updateEntries.end())
		{
			len = static_cast<LONGLONG>(iter->first->Address.Start) - offset;
			if (bufsz) wmemcpy_s(&buf[size], bufsz - size, &m_buf[offset], len);
			size += len;
			len = wcslen(iter->second.c_str());
			if (bufsz) wmemcpy_s(&buf[size], bufsz - size, iter->second.c_str(), len);
			size += len;
			offset = static_cast<LONGLONG>(iter->first->Address.End);
		}
	}
	len = m_buf.Len - offset;
	if (bufsz) wmemcpy_s(&buf[size], bufsz - size, &m_buf[offset], len);
	size += len;
	if (m_buf[m_buf.Len - 1] != L'\n')
	{
		if (bufsz) wmemcpy_s(&buf[size], bufsz - size, L"\r\n", 2);
		size += 2;
	}
	for (AppendList::const_iterator iter = appendEntries.begin(); iter != appendEntries.end(); iter++)
	{
		len = wcslen(iter->first.c_str());
		if (bufsz) wmemcpy_s(&buf[size], bufsz - size, iter->first.c_str(), len);
		size += len;
		if (bufsz) wmemcpy_s(&buf[size], bufsz - size, L"\t", 1);
		size += 1;
		len = wcslen(iter->second.c_str());
		if (bufsz) wmemcpy_s(&buf[size], bufsz - size, iter->second.c_str(), len);
		size += len;
		if (bufsz) wmemcpy_s(&buf[size], bufsz - size, L"\r\n", 2);
		size += 2;
	}
	return size;
}

HostEntry* HostsFile::FindByName(const WCHAR* pContent, PCWSTR pszName)
{
	for (DWORD dwIndex = 0; dwIndex < m_Entries.Count; dwIndex++)
	{
		HostEntry* pEntry = &m_Entries[dwIndex];
		if (pEntry->Contains(pContent, pszName))
		{
			return pEntry;
		}
	}
	return nullptr;

}
