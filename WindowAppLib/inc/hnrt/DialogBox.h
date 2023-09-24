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

		HWND get_hwnd() const;
		HFONT get_hFont() const;

		__declspec(property(get = get_hwnd)) HWND hwnd;
		__declspec(property(get = get_hFont)) HFONT hFont;

		static DialogBox* GetInstance(HWND hwnd);

	protected:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void OnClose();
		virtual void OnSize(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnTimer(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnNotify(WPARAM wParam, LPARAM lParam);

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
