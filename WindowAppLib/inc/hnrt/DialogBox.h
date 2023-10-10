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
		BOOL EnableWindow(int id, BOOL bEnabled = TRUE);
		BOOL DisableWindow(int id);
		LRESULT SendMessage(int id, UINT msg, WPARAM wParam = 0, LPARAM lParam = 0);
		UINT GetTextLength(int id);
		PWCHAR GetText(int id, PWCHAR pBuf, size_t cch);
		void SetText(int id, PCWSTR psz = L"");
		void CheckButton(int id, BOOL bCheck = TRUE);
		void UncheckButton(int id);
		int GetButtonState(int id);
		void AddStringToComboBox(int id, PCWSTR psz);
		int GetComboBoxSelection(int id, int defaultValue = 0);
		void SetComboBoxSelection(int id, int index);
		void SetComboBoxSelection(int id, PCWSTR psz);
		void ClearComboBoxSelection(int id);
		UINT GetListBoxTextLength(int id, int index, size_t defaultValue = 0);
		PWCHAR GetListBoxText(int id, int index, PWCHAR pBuf, PCWSTR pszDefault = L"");

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
		virtual INT_PTR OnTimer(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnNotify(WPARAM wParam, LPARAM lParam);
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
