#include "HostsFile.h"
#include "HostsReader.h"
#include "hnrt/Heap.h"
#include "hnrt/Debug.h"
#include "hnrt/ErrorMessage.h"
#include <stdio.h>
#include <ctype.h>


using namespace hnrt;


HostsFile::HostsFile(PCWSTR pszFileName)
	: m_pszFileName(pszFileName)
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
}


void HostsFile::Open()
{
	m_hFile = CreateFileW(m_pszFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!m_hFile.isValid)
	{
		m_dwError = GetLastError();
		throw Exception(L"Failed to open \"%s\". (%lu: %s)", m_pszFileName, m_dwError, ErrorMessage::Get(m_dwError));
	}
}


void HostsFile::Close()
{
	DBGFNC(L"HostsFile::Close");
	m_hFile.Close();
	m_buf.Resize(0);
	m_dwError = ERROR_SUCCESS;
	m_Entries.Clear();
}


void HostsFile::Read()
{
	DBGFNC(L"HostsFile::Read");
	int cb = GetFileSize(m_hFile, NULL);
	if (cb == INVALID_FILE_SIZE)
	{
		m_dwError = GetLastError();
		DBGPUT(L"GetFileSize: %lu %s", m_dwError, ErrorMessage::Get(m_dwError));
		throw Exception(L"Failed to get size of \"%s\". (%lu: %s)", m_pszFileName, m_dwError, ErrorMessage::Get(m_dwError));
	}
	Buffer<CHAR> buf(cb);
	DWORD dwRead = 0;
	if (!ReadFile(m_hFile, buf, cb, &dwRead, NULL))
	{
		m_dwError = GetLastError();
		DBGPUT(L"ReadFile: %lu %s", m_dwError, ErrorMessage::Get(m_dwError));
		throw Exception(L"Failed to read from \"%s\". (%lu: %s)", m_pszFileName, m_dwError, ErrorMessage::Get(m_dwError));
	}
	if (cb != static_cast<int>(dwRead))
	{
		m_dwError = ERROR_INCORRECT_SIZE;
		DBGPUT(L"ReadFile: expected=%d actual=%d", cb, static_cast<int>(dwRead));
		throw Exception(L"Failed to read from \"%s\". expected=%d actual=%d", m_pszFileName, cb, static_cast<int>(dwRead));
	}
	int cch = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, buf, cb, NULL, 0);
	m_buf.Resize(cch);
	MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, buf, cb, m_buf, cch);
	DBGPUT(L"bytes=%d characters=%d", cb, cch);
}


void HostsFile::CreateBackup()
{
	DBGFNC(L"HostsFile::CreateBackup");
	WCHAR szFileName[MAX_PATH] = { 0 };
	_snwprintf_s(szFileName, _TRUNCATE, L"%s.backup", m_pszFileName);
	DBGPUT(L"FileName=%s", szFileName);
	WindowsHandle hFile = CreateFileW(szFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!m_hFile.isValid)
	{
		m_dwError = GetLastError();
		DBGPUT(L"CreateFile: %lu %s", m_dwError, ErrorMessage::Get(m_dwError));
		throw Exception(L"Failed to open \"%s\". (%lu: %s)", m_pszFileName, m_dwError, ErrorMessage::Get(m_dwError));
	}
	DWORD dwError = GetLastError();
	if (dwError == ERROR_ALREADY_EXISTS)
	{
		int cb = GetFileSize(hFile, NULL);
		if (cb == INVALID_FILE_SIZE)
		{
			m_dwError = GetLastError();
			DBGPUT(L"GetFileSize: %lu %s", m_dwError, ErrorMessage::Get(m_dwError));
			throw Exception(L"Failed to get size of \"%s\". (%lu: %s)", m_pszFileName, m_dwError, ErrorMessage::Get(m_dwError));
		}
		Buffer<CHAR> buf1(cb);
		DWORD dwRead = 0;
		if (!ReadFile(hFile, buf1, cb, &dwRead, NULL))
		{
			m_dwError = GetLastError();
			DBGPUT(L"ReadFile: %lu %s", m_dwError, ErrorMessage::Get(m_dwError));
			throw Exception(L"Failed to read from \"%s\". (%lu: %s)", m_pszFileName, m_dwError, ErrorMessage::Get(m_dwError));
		}
		if (cb != static_cast<int>(dwRead))
		{
			m_dwError = ERROR_INCORRECT_SIZE;
			DBGPUT(L"ReadFile: expected=%d actual=%d", cb, static_cast<int>(dwRead));
			throw Exception(L"Failed to read from \"%s\". expected=%d actual=%d", m_pszFileName, cb, static_cast<int>(dwRead));
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
		throw Exception(L"Failed to rewind pointer of \"%s\". (%lu: %s)", m_pszFileName, m_dwError, ErrorMessage::Get(m_dwError));
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
		throw Exception(L"Failed to write to \"%s\". (%lu: %s)", m_pszFileName, m_dwError, ErrorMessage::Get(m_dwError));
	}
	if (cb != static_cast<int>(dwWritten))
	{
		m_dwError = ERROR_INCORRECT_SIZE;
		DBGPUT(L"WriteFile: expected=%d actual=%d", cb, static_cast<int>(dwWritten));
		throw Exception(L"Failed to write to \"%s\". expected=%d actual=%d", m_pszFileName, cb, static_cast<int>(dwWritten));
	}
	if (!SetEndOfFile(hFile))
	{
		m_dwError = GetLastError();
		DBGPUT(L"SetEndOfFile: %lu %s", m_dwError, ErrorMessage::Get(m_dwError));
		throw Exception(L"Failed to set size of \"%s\". (%lu: %s)", m_pszFileName, m_dwError, ErrorMessage::Get(m_dwError));
	}
	DBGPUT(L"characters=%d bytes=%d", cch, cb);
}


void HostsFile::Write()
{
	DBGFNC(L"HostsFile::Write");
	if (SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		m_dwError = GetLastError();
		DBGPUT(L"SetFilePointer: %lu %s", m_dwError, ErrorMessage::Get(m_dwError));
		throw Exception(L"Failed to rewind pointer of \"%s\". (%lu: %s)", m_pszFileName, m_dwError, ErrorMessage::Get(m_dwError));
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
		throw Exception(L"Failed to write to \"%s\". (%lu: %s)", m_pszFileName, m_dwError, ErrorMessage::Get(m_dwError));
	}
	if (cb != static_cast<int>(dwWritten))
	{
		m_dwError = ERROR_INCORRECT_SIZE;
		DBGPUT(L"WriteFile: expected=%d actual=%d", cb, static_cast<int>(dwWritten));
		throw Exception(L"Failed to write to \"%s\". expected=%d actual=%d", m_pszFileName, cb, static_cast<int>(dwWritten));
	}
	if (!SetEndOfFile(m_hFile))
	{
		m_dwError = GetLastError();
		DBGPUT(L"SetEndOfFile: %lu %s", m_dwError, ErrorMessage::Get(m_dwError));
		throw Exception(L"Failed to set size of \"%s\". (%lu: %s)", m_pszFileName, m_dwError, ErrorMessage::Get(m_dwError));
	}
	DBGPUT(L"characters=%d bytes=%d", cch, cb);
}


void HostsFile::Parse()
{
	DBGFNC(L"HostsFile::Parse");
	m_dwError = ERROR_SUCCESS;
	m_Entries.Clear();
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
				entry.Names.Add(reader.Token);
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
					entry.Names.Add(reader.Token);
					t = reader.NextToken();
				}
				else
				{
					break;
				}
			}
			m_Entries.Add(entry);
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
				throw Exception(L"Failed to parse \"%s\".", m_pszFileName);
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
	int offset = 0;
	for (DWORD dwIndex = 0; dwIndex < m_Entries.Count; dwIndex++)
	{
		UpdateMap::const_iterator iter = updateEntries.find(&m_Entries[dwIndex]);
		if (iter != updateEntries.end())
		{
			len = iter->first->Address.Start - offset;
			if (bufsz) wmemcpy_s(&buf[size], bufsz - size, &m_buf[offset], len);
			size += len;
			len = wcslen(iter->second);
			if (bufsz) wmemcpy_s(&buf[size], bufsz - size, iter->second, len);
			size += len;
			offset = iter->first->Address.End;
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
		len = wcslen(iter->first);
		if (bufsz) wmemcpy_s(&buf[size], bufsz - size, iter->first, len);
		size += len;
		if (bufsz) wmemcpy_s(&buf[size], bufsz - size, L"\t", 1);
		size += 1;
		len = wcslen(iter->second);
		if (bufsz) wmemcpy_s(&buf[size], bufsz - size, iter->second, len);
		size += len;
		if (bufsz) wmemcpy_s(&buf[size], bufsz - size, L"\r\n", 2);
		size += 2;
	}
	return size;
}
