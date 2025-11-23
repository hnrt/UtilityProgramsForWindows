#include "pch.h"
#include <Windows.h>
#include "hnrt/WindowsPlatform.h"
#include "hnrt/String.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/Exception.h"


using namespace hnrt;


String hnrt::GetModuleFileName(HMODULE hModule)
{
	String fileName(MAX_PATH - 1);
	DWORD dwLength = ::GetModuleFileNameW(hModule, fileName.Buf, MAX_PATH);
	if (!dwLength)
	{
		DWORD dwError = GetLastError();
		throw Exception(L"GetModuleFileName failed. (Error %lu %s)", dwError, ErrorMessage::Get(dwError));
	}
	fileName.Len = dwLength;
	return fileName;
}
