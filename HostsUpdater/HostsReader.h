#pragma once

#include "TextLocator.h"

#define HOSTSREADER_EOF			-1
#define HOSTSREADER_TOKEN_BASE	0x110000
#define HOSTSREADER_ERROR		(HOSTSREADER_TOKEN_BASE+0)
#define HOSTSREADER_WS			(HOSTSREADER_TOKEN_BASE+1)
#define HOSTSREADER_COMMENT		(HOSTSREADER_TOKEN_BASE+2)
#define HOSTSREADER_IPV4ADDR	(HOSTSREADER_TOKEN_BASE+3)
#define HOSTSREADER_IPV6ADDR	(HOSTSREADER_TOKEN_BASE+4)
#define HOSTSREADER_HOSTNAME	(HOSTSREADER_TOKEN_BASE+5)

namespace hnrt
{
	class HostsReader
	{
	public:

		HostsReader(const WCHAR* pBase, size_t cch);
		HostsReader(const HostsReader&) = delete;
		~HostsReader() = default;
		void operator =(const HostsReader&) = delete;
		int NextToken();

		int get_Offset() const;
		const TextLocator& get_Token() const;
		const WCHAR* get_TokenPtr() const;
		DWORD get_TokenLen() const;

		__declspec(property(get = get_Offset)) int Offset;
		__declspec(property(get = get_Token)) const TextLocator& Token;
		__declspec(property(get = get_TokenPtr)) const WCHAR* TokenPtr;
		__declspec(property(get = get_TokenLen)) DWORD TokenLen;

	private:

		int NextChar();
		bool ParseIPv4();
		bool ParseIPv6();
		bool ParseHostName();

		const WCHAR* m_pBase;
		const WCHAR* m_pCur;
		const WCHAR* m_pEnd;
		int m_c;
		TextLocator m_Token;
		bool m_bAddress;
	};

	inline int HostsReader::get_Offset() const
	{
		return static_cast<int>(m_pCur - 1 - m_pBase);
	}

	inline const TextLocator& HostsReader::get_Token() const
	{
		return m_Token;
	}

	inline const WCHAR* HostsReader::get_TokenPtr() const
	{
		return m_pBase + m_Token.Start;
	}

	inline DWORD HostsReader::get_TokenLen() const
	{
		return m_Token.Len;
	}
}
