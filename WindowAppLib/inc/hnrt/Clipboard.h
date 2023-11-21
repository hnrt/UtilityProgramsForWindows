#pragma once


#include "hnrt/RefObj.h"
#include "hnrt/RefPtr.h"
#include "hnrt/String.h"


namespace hnrt
{
	class ClipboardObserver
		: public RefObj
	{
	public:

		ClipboardObserver();
		ClipboardObserver(const ClipboardObserver&) = delete;
		virtual ~ClipboardObserver() = default;
		void operator =(const ClipboardObserver&) = delete;
		virtual void Copy(HWND hwnd, PCWSTR psz);
		virtual void Paste(HWND hwnd, PCWSTR psz);
	};

	inline ClipboardObserver::ClipboardObserver()
		: RefObj()
	{
	}

	inline void ClipboardObserver::Copy(HWND hwnd, PCWSTR psz)
	{
		UNREFERENCED_PARAMETER(hwnd);
		UNREFERENCED_PARAMETER(psz);
	}

	inline void ClipboardObserver::Paste(HWND hwnd, PCWSTR psz)
	{
		UNREFERENCED_PARAMETER(hwnd);
		UNREFERENCED_PARAMETER(psz);
	}

	class Clipboard
	{
	public:

		static bool Write(HWND hwnd, const WCHAR* pText, INT_PTR cch = -1);
		static bool Write(HWND hwnd, HWND hwndText);
		static bool Write(HWND hwnd, HWND hwndDialog, int idControl);
		static String Read(HWND hwnd);
		static void Register(RefPtr<ClipboardObserver>& pObserver);
		static bool Unregister(RefPtr<ClipboardObserver>& pObserver);
	};
}
