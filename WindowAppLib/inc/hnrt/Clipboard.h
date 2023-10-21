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

	class Clipboard
	{
	public:

		static bool Copy(HWND hwnd, const WCHAR* pText, size_t cch = static_cast<size_t>(-1));
		static bool Copy(HWND hwnd, HWND hwndText);
		static bool Copy(HWND hwnd, HWND hwndDialog, int idControl);
		static bool Paste(HWND hwnd, RefPtr<ClipboardText>& pText);
	};
}
