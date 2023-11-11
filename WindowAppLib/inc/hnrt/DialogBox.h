#pragma once


#include "hnrt/WindowSize.h"
#include "hnrt/FontHandle.h"


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
		HWND GetChild(int id) const;
		BOOL EnableWindow(int id, BOOL bEnabled = TRUE) const;
		BOOL DisableWindow(int id) const;
		LRESULT SendMessage(int id, UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const;
		UINT GetTextLength(int id) const;
		PWCHAR GetText(int id, PWCHAR pBuf, size_t cch) const;
		void SetText(int id, PCWSTR psz = L"") const;
		void GetTextSelection(int id, int& start, int& end) const;
		void SetTextSelection(int id, int start, int end) const;
		void SetReadOnlyEdit(int id, BOOL bReadOnly = TRUE) const;
		void CutText(int id) const;
		void CopyText(int id) const;
		void PasteText(int id) const;
		void DeleteText(int id) const;
		void SelectAllText(int id) const;
		void CopyAllText(int id) const;
		void ClearEdit(int id) const;
		void CheckButton(int id, BOOL bCheck = TRUE) const;
		void UncheckButton(int id) const;
		int GetButtonState(int id) const;
		void AddStringToComboBox(int id, PCWSTR psz, int value) const;
		void RemoveValueFromComboBox(int id, int value) const;
		int GetComboBoxSelection(int id, int defaultValue = 0) const;
		void SetComboBoxSelection(int id, int value) const;
		void SetComboBoxSelection(int id, PCWSTR psz) const;
		void ClearComboBoxSelection(int id) const;
		int GetListBoxTextLength(int id, int index, size_t defaultValue = 0) const;
		PWCHAR GetListBoxText(int id, int index, PWCHAR pBuf, PCWSTR pszDefault = L"") const;
		void SetFocus(int id) const;

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

	inline void CheckButton(int id)
	{

	}
}
