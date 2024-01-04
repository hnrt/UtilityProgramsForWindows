#pragma once

#include "hnrt/WindowSize.h"
#include "hnrt/FontHandle.h"
#include "hnrt/String.h"

#ifdef DialogBox
#undef DialogBox
#endif
#ifdef SendMessage
#undef SendMessage
#endif

namespace hnrt
{
	class DialogBox
		: public WindowSize
		, public WindowLayout
	{
	public:

		DialogBox(UINT idTemplate);
		DialogBox(const DialogBox&) = delete;
		virtual ~DialogBox();
		void operator =(const DialogBox&) = delete;
		operator HWND() const;
		virtual void Open(HINSTANCE hInstance);
		virtual void Open(HWND hwndParent);
		virtual void Open(HWND hwndParent, int cx, int cy);
		virtual void Open(HWND hwndParent, int x, int y, int cx, int cy);
		virtual void Close();
		void SetFont(HFONT hFont);
		void SetFont(int id, HFONT hFont);
		HWND GetChild(int id) const;
		BOOL EnableWindow(int id, BOOL bEnabled = TRUE) const;
		BOOL DisableWindow(int id) const;
		void SetFocus(int id) const;
		LRESULT SendMessage(int id, UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const;
		UINT GetTextLength(int id) const;
		String GetText(int id) const;
		void SetText(int id, PCWSTR psz = L"") const;
		void CopyAllText(int id) const;
		void EditGetSelection(int id, int& start, int& end) const;
		void EditSetSelection(int id, int start, int end) const;
		void EditCut(int id) const;
		void EditCopy(int id) const;
		void EditPaste(int id) const;
		void EditDelete(int id) const;
		void EditSelectAll(int id) const;
		void EditSetReadOnly(int id, BOOL bReadOnly = TRUE) const;
		void EditClear(int id) const;
		void ButtonCheck(int id, BOOL bCheck = TRUE) const;
		void ButtonUncheck(int id) const;
		BOOL ButtonIsChecked(int id) const;
		int ComboBoxGetCount(int id) const;
		void ComboBoxAdd(int id, const String& szText, int value) const;
		void ComboBoxRemove(int id, int value) const;
		int ComboBoxGetSelection(int id, int defaultValue = 0) const;
		void ComboBoxSetSelection(int id, int value) const;
		void ComboBoxSetSelection(int id, const String& szText) const;
		void ComboBoxClearSelection(int id) const;
		String ComboBoxGetText(int id, int index, const String& szDefault = String::Empty) const;
		int ListBoxGetCount(int id) const;
		int ListBoxGetSelection(int id, int defaultValue = 0) const;
		bool ListBoxSetSelection(int id, int index) const;
		bool ListBoxIsSelected(int id, int index) const;
		int ListBoxAddText(int id, const String& szText = String::Empty) const;
		String ListBoxGetText(int id, int index, const String& szDefault = String::Empty) const;
		int ListBoxInsertText(int id, int index, const String& szText = String::Empty) const;
		int ListBoxSetText(int id, int index, const String& szText = String::Empty) const;
		int ListBoxFind(int id, const String& szText, int fromIndex = -1) const;
		int ListBoxDelete(int id, int index) const;

		HWND get_hwnd() const;
		HFONT get_hFont() const;

		__declspec(property(get = get_hwnd)) HWND hwnd;
		__declspec(property(get = get_hFont)) HFONT hFont;

	protected:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void OnClose();
		virtual void OnSize(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnNotify(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnTimer(WPARAM wParam, LPARAM lParam); // NEEDS TO RETURN ZERO
		virtual INT_PTR OnControlColorStatic(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnControlColorEdit(WPARAM wParam, LPARAM lParam);

	private:

		static INT_PTR CALLBACK MessageCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

		void OnCreate(HWND hwnd);
		void OnDestroy(HWND hwnd);

		HWND m_hwnd;
		UINT m_idTemplate;
		FontHandle m_hFont;
	};

	inline DialogBox::operator HWND() const
	{
		return m_hwnd;
	}

	inline HWND DialogBox::get_hwnd() const
	{
		return m_hwnd;
	}

	inline HFONT DialogBox::get_hFont() const
	{
		return m_hFont;
	}
}
