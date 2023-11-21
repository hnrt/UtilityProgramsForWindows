#include "pch.h"
#include "hnrt/Clipboard.h"
#include "hnrt/Buffer.h"
#include <list>


using namespace hnrt;


typedef std::list<RefPtr<ClipboardObserver>> ClipboardObserverList;


static ClipboardObserverList observers;


static void NotifyOfCopy(HWND hwnd, PCWSTR psz)
{
	for (ClipboardObserverList::iterator iter = observers.begin(); iter != observers.end(); iter++)
	{
		RefPtr<ClipboardObserver>& pObserver = *iter;
		pObserver->Copy(hwnd, psz);
	}
}


static void NotifyOfPaste(HWND hwnd, PCWSTR psz)
{
	for (ClipboardObserverList::iterator iter = observers.begin(); iter != observers.end(); iter++)
	{
		RefPtr<ClipboardObserver>& pObserver = *iter;
		pObserver->Paste(hwnd, psz);
	}
}


bool Clipboard::Write(HWND hwnd, const WCHAR* pText, INT_PTR cch)
{
	if (cch < 0)
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
	try
	{
		NotifyOfCopy(hwnd, pDst);
	}
	catch (...)
	{
	}
	GlobalUnlock(hMem);
	bool bRet = false;
	DWORD dwError = ERROR_SUCCESS;
	if (OpenClipboard(hwnd))
	{
		if (EmptyClipboard())
		{
			if (SetClipboardData(CF_UNICODETEXT, hMem))
			{
				bRet = true;
			}
			else
			{
				dwError = GetLastError();
			}
		}
		else
		{
			dwError = GetLastError();
		}
		CloseClipboard();
		if (!bRet)
		{
			SetLastError(dwError);
		}
	}
	return bRet;
}


bool Clipboard::Write(HWND hwnd, HWND hwndText)
{
	int cch = GetWindowTextLengthW(hwndText) + 1;
	Buffer<WCHAR> buf(cch);
	cch = GetWindowTextW(hwndText, buf, cch);
	return Write(hwnd, buf, cch);
}


bool Clipboard::Write(HWND hwnd, HWND hwndDialog, int idControl)
{
	return Write(hwnd, GetDlgItem(hwndDialog, idControl));
}


String Clipboard::Read(HWND hwnd)
{
	String szReturn;
	DWORD dwError = ERROR_SUCCESS;
	if (OpenClipboard(hwnd))
	{
		if (IsClipboardFormatAvailable(CF_UNICODETEXT))
		{
			HANDLE hMem = GetClipboardData(CF_UNICODETEXT);
			if (hMem)
			{
				PWCHAR pSrc = reinterpret_cast<PWCHAR>(GlobalLock(hMem));
				if (pSrc)
				{
					szReturn = pSrc;
					GlobalUnlock(hMem);
					try
					{
						NotifyOfPaste(hwnd, szReturn);
					}
					catch (...)
					{
					}
				}
				else
				{
					dwError = GetLastError();
				}
			}
			else
			{
				dwError = GetLastError();
			}
		}
		else
		{
			dwError = GetLastError();
		}
		CloseClipboard();
		if (!szReturn)
		{
			SetLastError(dwError);
		}
	}
	return szReturn;
}


void Clipboard::Register(RefPtr<ClipboardObserver>& pObserver)
{
	observers.push_back(pObserver);
}


bool Clipboard::Unregister(RefPtr<ClipboardObserver>& pObserver)
{
	for (ClipboardObserverList::iterator iter = observers.begin(); iter != observers.end(); iter++)
	{
		RefPtr<ClipboardObserver>& pNext = *iter;
		if (pNext.Ptr == pObserver.Ptr)
		{
			observers.erase(iter);
			return true;
		}
	}
	return false;
}
