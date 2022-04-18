#pragma once


#include <Windows.h>


namespace hnrt
{
	class DialogApp
	{
	public:

		DialogApp(UINT idTemplate);
		DialogApp(const DialogApp&) = delete;
		virtual ~DialogApp() = default;
		void operator =(const DialogApp&) = delete;
		virtual void Open(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow);
		virtual void Run();
		virtual void Close();

		inline int get_ExitCode();

		__declspec(property(get = get_ExitCode)) int ExitCode;


	protected:

		int TryProcessMessage();

		static INT_PTR CALLBACK ProcessMessage(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
		static DialogApp* GetInstance(HWND hDlg);
		static void UpdateLayout(HWND hDlg, UINT id, LONG dx, LONG dy, LONG dcx, LONG dcy, BOOL bInvalidate = FALSE);

		virtual void OnCreate(HWND hDlg);
		virtual void OnDestory(HWND hDlg);
		virtual void OnClose(HWND hDlg);
		virtual void OnSize(HWND hDlg, WPARAM wParam, LPARAM lParam);
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual void OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam);

		int m_iExitCode;
		UINT m_idTemplate;
		HACCEL m_hAccelTable;
		HWND m_hwnd;
		LONG m_cxInitial;
		LONG m_cyInitial;
		LONG m_cxMinimum;
		LONG m_cyMinimum;
		LONG m_cxClientInitial;
		LONG m_cyClientInitial;
		LONG m_cxClient;
		LONG m_cyClient;
	};

	inline int DialogApp::get_ExitCode()
	{
		return m_iExitCode;
	}
}
