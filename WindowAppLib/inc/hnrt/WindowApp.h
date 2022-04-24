#pragma once


#include "hnrt/AnyApp.h"
#include "hnrt/WindowSize.h"
#include "hnrt/WindowClass.h"
#include "hnrt/WindowPreferences.h"


namespace hnrt
{
	class WindowApp
		: public AnyApp
		, protected WindowSize
		, protected WindowLayout
	{
	public:

		WindowApp(PCWSTR pszClassName);
		WindowApp(const WindowApp&) = delete;
		virtual ~WindowApp() = default;
		void operator =(const WindowApp&) = delete;
		virtual void Open(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow);
		virtual void Close();

	protected:

		static LRESULT CALLBACK MessageCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static WindowApp* GetInstance(HWND hwnd);

		virtual void ProcessMessage(MSG* pMsg);
		virtual void OnCreate(HWND hwnd);
		virtual void OnDestroy(HWND hwnd);
		virtual void OnClose(HWND hwnd);
		virtual void OnSize(HWND hwnd, WPARAM wParam, LPARAM lParam);
		virtual LRESULT OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam);
		virtual LRESULT OnTimer(HWND hDlg, WPARAM wParam, LPARAM lParam);

		WindowClass& get_Class();
		WindowPreferences& get_Preferences();

		__declspec(property(get = get_Class)) WindowClass& C;
		__declspec(property(get = get_Preferences)) WindowPreferences& P;

	private:

		WindowClass m_class;
		WindowPreferences m_preferences;
	};

	inline WindowClass& WindowApp::get_Class()
	{
		return m_class;
	}

	inline WindowPreferences& WindowApp::get_Preferences()
	{
		return m_preferences;
	}
}
