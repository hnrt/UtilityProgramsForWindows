#pragma once


#include "hnrt/AnyApp.h"
#include "hnrt/WindowSize.h"


namespace hnrt
{
	class DialogApp
		: protected AnyApp
		, protected WindowSize
		, protected WindowLayout
	{
	public:

		DialogApp(UINT idTemplate);
		DialogApp(const DialogApp&) = delete;
		virtual ~DialogApp() = default;
		void operator =(const DialogApp&) = delete;
		virtual void Open(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow);
		virtual void Close(HINSTANCE hInstance);

	protected:

		static INT_PTR CALLBACK MessageCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
		static DialogApp* GetInstance(HWND hDlg);

		virtual void ProcessMessage(MSG* pMsg);
		virtual void OnCreate(HWND hDlg);
		virtual void OnDestroy(HWND hDlg);
		virtual void OnClose(HWND hDlg);
		virtual void OnSize(HWND hDlg, WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnTimer(HWND hDlg, WPARAM wParam, LPARAM lParam);

		UINT m_idTemplate;
	};
}
