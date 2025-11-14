#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include "hnrt/CoreLibrary.h"
#include "hnrt/Path.h"
#include "hnrt/Environment.h"
#include "hnrt/StringUtils.h"
#include "hnrt/SemanticVersion.h"
#include "hnrt/Buffer.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/Debug.h"


#pragma comment(lib, "Core")


using namespace hnrt;


static CoreLibrary theCore;


static String FindJavaExe()
{
	static const WCHAR JAVA_EXE[] = L"java.exe";

	String JAVA_HOME = GetEnvironmentVariable(L"JAVA_HOME");
	if (JAVA_HOME.Len)
	{
		String javaExe = Path::Combine(JAVA_HOME, L"bin", JAVA_EXE);
		if (Path::Exists(javaExe))
		{
			Debug::Put(L"%s ...FOUND", javaExe);
			return javaExe;
		}
		Debug::Put(L"%s ...NOT FOUND", javaExe);
	}

	String PATH = GetEnvironmentVariable(L"PATH");
	if (PATH.Len)
	{
		Array<String> paths = SplitBy(PATH, L';');
		for (DWORD dwIndex = 0; dwIndex < paths.Length; dwIndex++)
		{
			String next = paths[dwIndex];
			if (next.Len)
			{
				String javaExe = Path::Combine(next, JAVA_EXE);
				if (Path::Exists(javaExe))
				{
					Debug::Put(L"%s ...FOUND", javaExe);
					return javaExe;
				}
				Debug::Put(L"%s ...NOT FOUND", javaExe);
			}
		}
	}

	return String::Empty;
}


static String FindJar(const wchar_t* pszExe)
{
	String fileName = Path::GetFileName(pszExe);
	String dirPath = Path::GetDirectoryName(pszExe);
	int nExtension = 4; // .exe or .jar
	int nBaseName = static_cast<int>(fileName.Len) - nExtension;
	String pattern = String::Format(L"%.*s*.jar", nBaseName, fileName);
	Debug::Put(L"dir=%s", dirPath);
	Debug::Put(L"file=%s", fileName);
	Debug::Put(L"pattern=%s", pattern);
	Array<DirectoryEntry> entries;
	Path::ListFiles(entries, dirPath, pattern);
	String jarFile;
	SemanticVersion version;
	for (DWORD dwIndex = 0; dwIndex < entries.Length; dwIndex++)
	{
		DirectoryEntry& entry = entries[dwIndex];
		if (entry.szFileName.Len == fileName.Len)
		{
			jarFile = entry.szFileName;
			version = SemanticVersion(L"");
			break;
		}
		if (entry.szFileName.Ptr[nBaseName] == L'-')
		{
			int nVersion = static_cast<int>(entry.szFileName.Len) - nBaseName - 1 - nExtension;
			String s = String::Format(L"%.*s", nVersion, entry.szFileName.Ptr + nBaseName + 1);
			SemanticVersion v(s);
			Debug::Put(L"candidate=%s major=%d minor=%d patch=%d prerelease=%s build=%s", entry.szFileName, v.Major, v.Minor, v.Patch, v.PreRelease, v.Build);
			if (jarFile.Len == 0 || version < v)
			{
				jarFile = entry.szFileName;
				version = v;
			}
		}
	}
	if (jarFile.Len == 0)
	{
		return String::Empty;
	}
	String jarPath = Path::Combine(dirPath, jarFile);
	Debug::Put(L"jar=%s", jarPath);
	return jarPath;
}


static int ExecuteJava(String javaExe, String targetJar, int argc, wchar_t* argv[])
{
	StringBuffer command;
	if (wcschr(javaExe, L' '))
	{
		command.AppendFormat(L"\"%s\"", javaExe);
	}
	else
	{
		command.Append(javaExe);
	}
	if (wcschr(targetJar, L' '))
	{
		command.AppendFormat(L" -jar \"%s\"", targetJar);
	}
	else
	{
		command.AppendFormat(L" -jar %s", targetJar);
	}
	for (int i = 1; argv[i]; i++)
	{
		if (wcschr(argv[i], L' '))
		{
			command.AppendFormat(L" \"%s\"", argv[i]);
		}
		else
		{
			command.AppendFormat(L" %s", argv[i]);
		}
	}
	Debug::Put(L"command=%s", command.Ptr);
	DWORD dwExitCode = EXIT_FAILURE;
	STARTUPINFOW si = { 0 };
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi = { 0 };
	if (CreateProcessW(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		if (GetExitCodeProcess(pi.hProcess, &dwExitCode))
		{
			Debug::Put(L"ExitCode=%lu", dwExitCode);
		}
		else
		{
			fwprintf(stderr, L"ERROR: Failed to get exit code: %s\n", ErrorMessage::Get(GetLastError()));
		}
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else
	{
		fwprintf(stderr, L"ERROR: Failed to execute java: %s\n", ErrorMessage::Get(GetLastError()));
	}
	return static_cast<int>(dwExitCode);
}


int wmain(int argc, wchar_t* argv[])
{
	String javaExe = FindJavaExe();
	if (javaExe.Len == 0)
	{
		fwprintf(stderr, L"ERROR: java.exe not found.\n");
		return EXIT_FAILURE;
	}
	String targetJar = FindJar(argv[0]);
	if (targetJar.Len == 0)
	{
		fwprintf(stderr, L"ERROR: jar file not found.\n");
		return EXIT_FAILURE;
	}
	return ExecuteJava(javaExe, targetJar, argc, argv);
}
