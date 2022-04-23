#pragma once


#include "hnrt/AnyApp.h"
#include "hnrt/WindowSize.h"
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
		virtual void Close(HINSTANCE hInstance);

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

		WindowPreferences m_preferences;
	};
}
