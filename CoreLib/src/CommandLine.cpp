#include "pch.h"
#include <shellapi.h>
#include "hnrt/Win32Exception.h"
#include "hnrt/CommandLine.h"


#pragma comment(lib,"Shell32")


using namespace hnrt;


CommandLine::CommandLine(PCWSTR pszCmdLine)
	: m_ppsz(nullptr)
	, m_count(0)
{
	m_ppsz = CommandLineToArgvW(pszCmdLine, &m_count);
	if (!m_ppsz)
	{
		DWORD dwError = GetLastError();
		throw Win32Exception(dwError, L"CommandLineToArgv failed.");
	}
}


CommandLine::~CommandLine()
{
	LocalFree(m_ppsz);
}


PCWSTR CommandLine::operator [](int index) const
{
	if (index < 0)
	{
		index += m_count;
	}
	if (0 <= index && index < m_count)
	{
		return m_ppsz[index];
	}
	else
	{
		return nullptr;
	}
}


CommandLineIterator::CommandLineIterator(const CommandLine& cmdLine)
	: m_CommandLine(cmdLine)
	, m_index(-1)
{
}


bool CommandLineIterator::get_HasNext()
{
	return ++m_index < m_CommandLine.Count;
}


PCWSTR CommandLineIterator::get_Next() const
{
	return m_CommandLine[m_index];
}
