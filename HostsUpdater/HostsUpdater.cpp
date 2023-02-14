#include "HostsUpdateService.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/ErrorMessage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>

#pragma comment(lib, "Core")
#pragma comment(lib, "ws2_32")

using namespace hnrt;

#define COMMAND_NONE 0
#define COMMAND_INSTALL 1
#define COMMAND_UNINSTALL 2
#define COMMAND_RUN 3
#define COMMAND_CREATE_REGISTRY 4
#define COMMAND_PROCESS 5

#define COMMAND_STRING_INSTALL L"install"
#define COMMAND_STRING_UNINSTALL L"uninstall"
#define COMMAND_STRING_RUN L"run"
#define COMMAND_STRING_CREATE_REGISTRY L"create_registry"
#define COMMAND_STRING_PROCESS L"process"

static bool ParseCommandLine(wchar_t** ppCur, wchar_t** ppEnd, int& command)
{
	if (ppCur < ppEnd)
	{
		wchar_t* psz = *ppCur++;
		if (!wcscmp(psz, COMMAND_STRING_INSTALL))
		{
			command = COMMAND_INSTALL;
		}
		else if (!wcscmp(psz, COMMAND_STRING_UNINSTALL))
		{
			command = COMMAND_UNINSTALL;
		}
		else if (!wcscmp(psz, COMMAND_STRING_RUN))
		{
			command = COMMAND_RUN;
		}
		else if (!wcscmp(psz, COMMAND_STRING_CREATE_REGISTRY))
		{
			command = COMMAND_CREATE_REGISTRY;
		}
		else if (!wcscmp(psz, COMMAND_STRING_PROCESS))
		{
			command = COMMAND_PROCESS;
		}
		else
		{
			fwprintf(stderr, L"ERROR: Invalid command: %s\n", psz);
			return false;
		}
	}
	if (ppCur < ppEnd)
	{
		fwprintf(stderr, L"ERROR: Too many arguments.\n");
		return false;
	}
	return true;
}

static void help()
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
		wcscpy_s(szFileName, L"hostsupd.exe");
	}
	fwprintf(stderr, L"Usage: %s %s|%s|%s|%s\n", szFileName,
		COMMAND_STRING_INSTALL, COMMAND_STRING_UNINSTALL, COMMAND_STRING_CREATE_REGISTRY, COMMAND_STRING_PROCESS);
}

int wmain(int argc, wchar_t* argv[])
{
	try
	{
		_wsetlocale(LC_ALL, L"");
		int command = COMMAND_NONE;
		if (!ParseCommandLine(&argv[1], &argv[argc], command))
		{
			return EXIT_FAILURE;
		}
		if (command == COMMAND_NONE)
		{
			help();
			return EXIT_SUCCESS;
		}
		WSADATA ws = { 0 };
		int rc = WSAStartup(MAKEWORD(2, 2), &ws);
		if (rc)
		{
			fwprintf(stderr, L"WARNING: Winsock initialization failed. %d %s\n", rc,
				rc == WSAVERNOTSUPPORTED ? L"(Version is not supported.)" :
				rc == WSASYSNOTREADY ? L"(Network subsystem is not yet ready.)" :
				L"");
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
		case COMMAND_PROCESS:
			pInstance->ReadRegistry();
			pInstance->ProcessHostsFile();
			break;
		default:
			break;
		}
		WSACleanup();
		return EXIT_SUCCESS;
	}
	catch (Win32Exception ex)
	{
		fwprintf(stderr, L"ERROR: %s (%lu: %s)\n", ex.Message, ex.Error, ErrorMessage::Get(ex.Error));
	}
	catch (...)
	{
		fwprintf(stderr, L"ERROR: Unexpected exception caught.\n");
	}
	WSACleanup();
	return EXIT_FAILURE;
}
