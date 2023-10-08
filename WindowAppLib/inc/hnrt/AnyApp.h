#pragma once


#include <Windows.h>


namespace hnrt
{
	class CommandLine;

	class AnyApp
	{
	public:

		static AnyApp* GetAppPtr();

		AnyApp();
		AnyApp(const AnyApp&) = delete;
		virtual ~AnyApp();
		void operator =(const AnyApp&) = delete;
		void SetAccelerators(HINSTANCE hInstance, UINT id);
		virtual void Open(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow);
		virtual void Run();
		int TryProcessMessage();

		inline int get_ExitCode();

		__declspec(property(get = get_ExitCode)) int ExitCode;

	private:

		static AnyApp* m_pThis;

		bool m_bExit;

	protected:

		virtual void ProcessMessage(MSG*) = 0;

		int m_iExitCode;
		CommandLine* m_pCommandLine;
		HACCEL m_hAccelTable;
	};

	inline int AnyApp::get_ExitCode()
	{
		return m_iExitCode;
	}

	template<class T>
	T& GetApp()
	{
		return *reinterpret_cast<T*>(AnyApp::GetAppPtr());
	}
}
