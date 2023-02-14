#pragma once

#include "hnrt/RefObj.h"
#include "hnrt/RefPtr.h"
#include "hnrt/String.h"
#include "hnrt/WindowsHandle.h"
#include "hnrt/SpinLock.h"
#include "hnrt/KeyValueMap.h"
#include "hnrt/RegistryKey.h"

namespace hnrt
{
	struct Mapping
	{
		PWSTR pszName;
		PWSTR pszValue;
	};

	class HostsUpdateService
		: public RefObj
	{
	public:

		static RefPtr<HostsUpdateService> GetInstance();

		HostsUpdateService();
		HostsUpdateService(const HostsUpdateService&) = delete;
		~HostsUpdateService();
		void operator =(const HostsUpdateService&) = delete;
		void Install(PCWSTR pszCommand);
		void Uninstall();
		bool Run();
		void CreateRegistry();
		void ReadRegistry();
		void ProcessHostsFile();

	private:

		static VOID WINAPI ServiceMain(DWORD dwArgc, PWSTR* pszArgv);
		static DWORD WINAPI HandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);

		bool OnStart();
		DWORD OnStop();
		DWORD OnPause();
		DWORD OnContinue();
		DWORD OnInterrogate();
		DWORD OnShutdown();
		DWORD OnSessionChange(DWORD, DWORD);
		void MainLoop();
		bool SetStatus(DWORD dwState, DWORD dwPreviousStatus = 0, DWORD dwWaitHint = 0);
		void OpenLogFile(PCWSTR pszFileName = nullptr);
		void CloseLogFile();
		void Log(PCWSTR pszFormat, ...);
		void CreateRegistryV1(RegistryKey&);
		void CreateAppDirIfNotExist();

		static HostsUpdateService* m_pSingleton;
		static SpinLock::Type m_lockPointer;

		SpinLock::Type m_lockSelf;
		PCWSTR m_pszServiceName;
		DWORD m_dwError;
		SERVICE_STATUS_HANDLE m_hServiceStatus;
		DWORD m_dwCurrentState;
		DWORD m_dwCheckPoint;
		LONG m_ExclusiveOperation;
		WindowsHandle m_hEventMain;
		PCWSTR m_pszAppDir;
		PCWSTR m_pszLogFile;
		WindowsHandle m_hLogFile;
		PCWSTR m_pszHostsFile;
		KeyValueMap m_Mappings;
	};
}
