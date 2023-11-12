#include "pch.h"
#include "hnrt/Clipboard.h"
#include "hnrt/Buffer.h"
#include <exception>
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


bool Clipboard::Paste(HWND hwnd, RefPtr<ClipboardText>& pText)
{
	bool bRet = false;
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
					pText = RefPtr<ClipboardText>(new ClipboardText(pSrc));
					bRet = true;
					GlobalUnlock(hMem);
					try
					{
						NotifyOfPaste(hwnd, pText->Ptr);
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
		if (!bRet)
		{
			SetLastError(dwError);
		}
	}
	return bRet;
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


ClipboardText::ClipboardText(PCWSTR psz)
	: RefObj()
	, m_psz(_wcsdup(psz))
{
	if (!m_psz)
	{
		throw std::bad_alloc();
	}
}


ClipboardText::~ClipboardText()
{
	free(m_psz);
}
