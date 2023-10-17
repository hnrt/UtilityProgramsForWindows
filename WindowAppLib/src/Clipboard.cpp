#include "pch.h"
#include "hnrt/Clipboard.h"
#include "hnrt/Buffer.h"


using namespace hnrt;


bool Clipboard::Copy(HWND hwnd, const WCHAR* pText, size_t cch)
{
	if (cch == static_cast<size_t>(-1))
	{
		cch = wcslen(pText);
	}
	size_t cb = (cch + 1) * sizeof(WCHAR);
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, cb);
	if (hMem == NULL)
	{
		return false;
	}
	PWCHAR pDst = reinterpret_cast<PWCHAR>(GlobalLock(hMem));
	if (pDst == NULL)
	{
		return false;
	}
	memcpy_s(pDst, cb, pText, cch * sizeof(WCHAR));
	pDst[cch] = L'\0';
	GlobalUnlock(hMem);
	OpenClipboard(hwnd);
	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, hMem);
	CloseClipboard();
	return true;
}


bool Clipboard::Copy(HWND hwnd, HWND hwndText)
{
	int cch = GetWindowTextLengthW(hwndText);
	Buffer<WCHAR> buf(cch + 1);
	cch = GetWindowTextW(hwndText, buf, cch + 1);
	return Copy(hwnd, buf, cch);
}


bool Clipboard::Copy(HWND hwnd, HWND hwndDialog, int idControl)
{
	return Copy(hwnd, GetDlgItem(hwndDialog, idControl));
}
