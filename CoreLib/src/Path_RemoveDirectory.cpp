#include "pch.h"
#include "hnrt/Path.h"
#include "hnrt/Debug.h"
#ifdef _DEBUG
#include "hnrt/ErrorMessage.h"
#endif


using namespace hnrt;


BOOL Path::RemoveDirectory(PCWSTR pszPath)
{
	DBGFNC(L"Path::RemoveDirectory(%s)", pszPath);
	if (!Exists(pszPath))
	{
		DBGPUT(L"The specified path does not exist.");
		return TRUE;
	}
	if (!IsDirectory(pszPath))
	{
		DBGPUT(L"The specified path is not of a directory.");
		return FALSE;
	}
	Array<DirectoryEntry> directories;
	ListDirectories(directories, pszPath);
	for (DWORD dwIndex = 0; dwIndex < directories.Length; dwIndex++)
	{
		DirectoryEntry& entry = directories[dwIndex];
		if (!RemoveDirectory(Combine(pszPath, entry.szFileName)))
		{
			return FALSE;
		}
	}
	Array<DirectoryEntry> files;
	ListFiles(files, pszPath);
	for (DWORD dwIndex = 0; dwIndex < files.Length; dwIndex++)
	{
		DirectoryEntry& entry = files[dwIndex];
		if (::DeleteFileW(Combine(pszPath, entry.szFileName)))
		{
			DBGPUT(L"Deleted %s", entry.szFileName);
		}
		else
		{
#ifdef _DEBUG
			DWORD dwError = GetLastError();
			Debug::Put(L"Failed to delete %s (%ld %s)", entry.szFileName, dwError, ErrorMessage::Get(dwError));
#endif
			return FALSE;
		}
	}
	if (::RemoveDirectoryW(pszPath))
	{
		DBGPUT(L"Deleted.");
		return TRUE;
	}
	else
	{
#ifdef _DEBUG
		DWORD dwError = GetLastError();
		Debug::Put(L"Failed to delete (%ld %s)", dwError, ErrorMessage::Get(dwError));
#endif
		return FALSE;
	}
}
