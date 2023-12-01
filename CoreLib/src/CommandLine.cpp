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


PCWSTR CommandLine::operator [](INT_PTR index) const
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


void CommandLine::Remove(INT_PTR index)
{
	if (index < 0)
	{
		index += m_count;
	}
	if (0 <= index && index < m_count)
	{
		INT_PTR n = static_cast<INT_PTR>(m_count) - (index + 1);
		if (n > 0)
		{
			memmove(&m_ppsz[index], &m_ppsz[index + 1], n * sizeof(m_ppsz[0]));
		}
		m_ppsz[--m_count] = nullptr;
	}
}


CommandLineIterator::CommandLineIterator(CommandLine& cmdLine)
	: m_CommandLine(cmdLine)
	, m_index(-1)
{
}


void CommandLineIterator::RemoveNext()
{
	if (m_index < m_CommandLine.Count)
	{
		m_CommandLine.Remove(m_index--);
	}
}


bool CommandLineIterator::get_HasNext()
{
	return ++m_index < m_CommandLine.Count;
}


PCWSTR CommandLineIterator::get_Next() const
{
	return m_CommandLine[m_index];
}
