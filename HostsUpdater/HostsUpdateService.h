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
		void Run();
		void CreateRegistry();
		void ReadRegistry();
		void ProcessHostsFile();

		void SetReadOnly();

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
		void OpenLogFile(const String& szFileName = String::Empty);
		void CloseLogFile();
		void Log(PCWSTR pszFormat, ...);
		void CreateRegistryV1(RegistryKey&);
		void CreateAppDirIfNotExist();

		static HostsUpdateService* m_pSingleton;
		static SpinLock::Type m_lockPointer;

		SpinLock::Type m_lockSelf;
		String m_szServiceName;
		DWORD m_dwError;
		SERVICE_STATUS_HANDLE m_hServiceStatus;
		DWORD m_dwCurrentState;
		DWORD m_dwCheckPoint;
		LONG m_ExclusiveOperation;
		WindowsHandle m_hEventMain;
		String m_szAppDir;
		String m_szLogFile;
		WindowsHandle m_hLogFile;
		String m_szHostsFile;
		KeyValueMap m_Mappings;
		bool m_bReadOnly;
	};

	inline void HostsUpdateService::SetReadOnly()
	{
		m_bReadOnly = true;
	}
}
