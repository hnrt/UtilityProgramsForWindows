#include "pch.h"
#include "hnrt/Path.h"
#include "hnrt/Debug.h"
#ifdef _DEBUG
#include "hnrt/ErrorMessage.h"
#endif


using namespace hnrt;
using namespace std;


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
	vector<DirectoryEntry> directories;
	ListDirectories(directories, pszPath);
	for (vector<DirectoryEntry>::const_iterator iter = directories.begin(); iter != directories.end(); iter++)
	{
		if (!RemoveDirectory(Combine(pszPath, iter->szFileName)))
		{
			return FALSE;
		}
	}
	vector<DirectoryEntry> files;
	ListFiles(files, pszPath);
	for (vector<DirectoryEntry>::const_iterator iter = files.begin(); iter != files.end(); iter++)
	{
		if (::DeleteFileW(Combine(pszPath, iter->szFileName)))
		{
			DBGPUT(L"Deleted %s", iter->szFileName);
		}
		else
		{
#ifdef _DEBUG
			DWORD dwError = GetLastError();
			Debug::Put(L"Failed to delete %s (%ld %s)", iter->szFileName, dwError, ErrorMessage::Get(dwError));
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
