#pragma once


#include <Windows.h>


namespace hnrt
{
	class AnyApp
	{
	public:

		AnyApp();
		AnyApp(const AnyApp&) = delete;
		virtual ~AnyApp() = default;
		void operator =(const AnyApp&) = delete;
		void SetAccelerators(HINSTANCE hInstance, UINT id);
		virtual void Open(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow);
		virtual void Run();
		virtual void Close() = 0;

		inline int get_ExitCode();

		__declspec(property(get = get_ExitCode)) int ExitCode;

	protected:

		int TryProcessMessage();
		virtual void ProcessMessage(MSG* pMsg) = 0;

		int m_iExitCode;
		HACCEL m_hAccelTable;
		HWND m_hwnd;
	};

	inline int AnyApp::get_ExitCode()
	{
		return m_iExitCode;
	}
}
