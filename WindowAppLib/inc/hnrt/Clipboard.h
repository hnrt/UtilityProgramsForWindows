#pragma once


#include "hnrt/RefObj.h"
#include "hnrt/RefPtr.h"


namespace hnrt
{
	class ClipboardText
		: public RefObj
	{
	public:

		ClipboardText(PCWSTR);
		ClipboardText(const ClipboardText&) = delete;
		~ClipboardText();
		void operator =(const ClipboardText&) = delete;
		operator PCWSTR() const;
		PCWSTR get_Ptr() const;

		__declspec(property(get = get_Ptr)) PCWSTR Ptr;

	private:

		PWSTR m_psz;
	};

	inline ClipboardText::operator PCWSTR() const
	{
		return m_psz;
	}

	inline PCWSTR ClipboardText::get_Ptr() const
	{
		return m_psz;
	}

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

		static bool Copy(HWND hwnd, const WCHAR* pText, size_t cch = static_cast<size_t>(-1));
		static bool Copy(HWND hwnd, HWND hwndText);
		static bool Copy(HWND hwnd, HWND hwndDialog, int idControl);
		static bool Paste(HWND hwnd, RefPtr<ClipboardText>& pText);
		static void Register(RefPtr<ClipboardObserver>& pObserver);
		static bool Unregister(RefPtr<ClipboardObserver>& pObserver);
	};
}
