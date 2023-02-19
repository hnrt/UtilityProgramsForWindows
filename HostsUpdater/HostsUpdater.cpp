#include "HostsUpdateService.h"
#include "hnrt/String.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/ResourceString.h"
#include "resource.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include <map>

#pragma comment(lib, "Core")
#pragma comment(lib, "ws2_32")

using namespace hnrt;

#define COMMAND_NONE 0
#define COMMAND_INSTALL 1
#define COMMAND_UNINSTALL 2
#define COMMAND_RUN 3
#define COMMAND_CREATE_REGISTRY 4
#define COMMAND_READ_REGISTRY 5
#define COMMAND_PROCESS 6

#define COMMAND_STRING_INSTALL L"install"
#define COMMAND_STRING_UNINSTALL L"uninstall"
#define COMMAND_STRING_RUN L"run"
#define COMMAND_STRING_CREATE_REGISTRY L"create_registry"
#define COMMAND_STRING_READ_REGISTRY L"read_registry"
#define COMMAND_STRING_PROCESS L"process"

typedef std::map<const wchar_t*, int, StringCaseLessThan> CommandMap;
#define INSERT(x) insert(std::pair<const wchar_t*, int>(COMMAND_STRING_##x,COMMAND_##x))

static void ParseCommandLine(wchar_t** ppCur, wchar_t** ppEnd, int& command)
{
	CommandMap cmdmap;
	cmdmap.INSERT(INSTALL);
	cmdmap.INSERT(UNINSTALL);
	cmdmap.INSERT(RUN);
	cmdmap.INSERT(CREATE_REGISTRY);
	cmdmap.INSERT(READ_REGISTRY);
	cmdmap.INSERT(PROCESS);
	if (ppCur < ppEnd)
	{
		wchar_t* psz = *ppCur++;
		CommandMap::const_iterator found = cmdmap.find(psz);
		if (found != cmdmap.end())
		{
			command = found->second;
			switch (command)
			{
			case COMMAND_PROCESS:
				while (ppCur < ppEnd)
				{
					wchar_t* psz = *ppCur++;
					if (!String::CaseCompare(psz, L"-readonly"))
					{
						HostsUpdateService::GetInstance()->SetReadOnly();
					}
					else
					{
						throw Exception(ResourceString(IDS_INVALID_PARAMETER), psz);
					}
				}
				break;
			default:
				break;
			}
		}
		else
		{
			throw Exception(ResourceString(IDS_INVALID_COMMAND), psz);
		}
	}
	if (ppCur < ppEnd)
	{
		throw Exception(ResourceString(IDS_TOO_MANY_ARGS));
	}
}

static void Help()
{
	WCHAR szFileName[MAX_PATH] = { 0 };
	if (GetModuleFileNameW(NULL, szFileName, MAX_PATH))
	{
		PWCHAR p = wcsrchr(szFileName, L'\\');
		if (p)
		{
			p++;
			wmemmove(szFileName, p, wcslen(p) + 1);
		}
	}
	else
	{
		wcscpy_s(szFileName, L"hostsupdater.exe");
	}
	fwprintf(stderr, ResourceString(IDS_USAGE), szFileName,
		COMMAND_STRING_INSTALL, COMMAND_STRING_UNINSTALL,
		COMMAND_STRING_CREATE_REGISTRY, COMMAND_STRING_READ_REGISTRY, COMMAND_STRING_PROCESS);
}

static void RunCommand(int command)
{
	if (command == COMMAND_NONE)
	{
		Help();
		return;
	}

	auto pInstance = HostsUpdateService::GetInstance();
	switch (command)
	{
	case COMMAND_INSTALL:
		pInstance->Install(COMMAND_STRING_RUN);
		break;
	case COMMAND_UNINSTALL:
		pInstance->Uninstall();
		break;
	case COMMAND_RUN:
		pInstance->Run();
		break;
	case COMMAND_CREATE_REGISTRY:
		pInstance->CreateRegistry();
		break;
	case COMMAND_READ_REGISTRY:
		pInstance->ReadRegistry();
		break;
	case COMMAND_PROCESS:
		pInstance->ReadRegistry();
		pInstance->ProcessHostsFile();
		break;
	default:
		throw Exception(ResourceString(IDS_UNEXPECTED_COMMAND), command);
	}
}

int wmain(int argc, wchar_t* argv[])
{
	int exitCode = EXIT_FAILURE;

	_wsetlocale(LC_ALL, L"");

	WSADATA ws = { 0 };
	int rc = WSAStartup(MAKEWORD(2, 2), &ws);
	if (rc)
	{
		fwprintf(stderr, ResourceString(IDS_WARNING_WSINIT_FAILURE), rc, ErrorMessage::Get(rc));
	}

	try
	{
		auto pInstance = HostsUpdateService::GetInstance(); // increments reference counter
		int command = COMMAND_NONE;
		ParseCommandLine(&argv[1], &argv[argc], command);
		RunCommand(command);
		exitCode = EXIT_SUCCESS;
	}
	catch (Win32Exception ex)
	{
		fwprintf(stderr, ResourceString(IDS_ERROR_MESSAGE_CODE_REASON), ex.Message, ex.Error, ErrorMessage::Get(ex.Error));
	}
	catch (Exception ex)
	{
		fwprintf(stderr, ResourceString(IDS_ERROR_MESSAGE), ex.Message);
	}
	catch (...)
	{
		fwprintf(stderr, ResourceString(IDS_ERROR_UNEXPECTED_EXCEPTION));
	}

	WSACleanup();

	return exitCode;
}
