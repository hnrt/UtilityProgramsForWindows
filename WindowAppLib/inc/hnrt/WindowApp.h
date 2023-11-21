#pragma once


#include "hnrt/AnyApp.h"
#include "hnrt/WindowSize.h"
#include "hnrt/WindowClass.h"
#include "hnrt/WindowPreferences.h"


namespace hnrt
{
	class WindowApp
		: public AnyApp
		, public WindowSize
		, public WindowLayout
	{
	public:

		WindowApp(PCWSTR pszClassName);
		WindowApp(const WindowApp&) = delete;
		virtual ~WindowApp() = default;
		void operator =(const WindowApp&) = delete;
		virtual void Open(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow);
		virtual void Close();
		void SetText(PCWSTR) const;

		HWND get_hwnd() const;
		WindowClass& get_class();
		WindowPreferences& get_preferences();

		__declspec(property(get = get_hwnd)) HWND hwnd;
		__declspec(property(get = get_class)) WindowClass& C;
		__declspec(property(get = get_preferences)) WindowPreferences& P;

	protected:

		static LRESULT CALLBACK MessageCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		virtual void ProcessMessage(MSG* pMsg);
		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void OnClose();
		virtual void OnSize(WPARAM wParam, LPARAM lParam);
		virtual LRESULT OnCommand(WPARAM wParam, LPARAM lParam);
		virtual LRESULT OnNotify(WPARAM wParam, LPARAM lParam);
		virtual LRESULT OnTimer(WPARAM wParam, LPARAM lParam); // NEEDS TO RETURN ZERO

	private:

		void OnCreate(HWND hwnd);
		void OnDestroy(HWND hwnd);

		HWND m_hwnd;
		WindowClass m_class;
		WindowPreferences m_preferences;
	};

	inline HWND WindowApp::get_hwnd() const
	{
		return m_hwnd;
	}

	inline WindowClass& WindowApp::get_class()
	{
		return m_class;
	}

	inline WindowPreferences& WindowApp::get_preferences()
	{
		return m_preferences;
	}
}
