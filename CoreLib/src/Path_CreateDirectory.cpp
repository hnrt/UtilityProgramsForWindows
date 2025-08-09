#include "pch.h"
#include "hnrt/Path.h"


using namespace hnrt;


BOOL Path::CreateDirectory(PCWSTR pszPath)
{
	if (!Exists(pszPath))
	{
		return CreateDirectory(GetDirectoryName(pszPath)) && CreateDirectoryW(pszPath, NULL);
	}
	else
	{
		return IsDirectory(pszPath);
	}
}
