#include "HostsUpdateService.h"
#include "HostsFile.h"
#include "AddressResolution.h"
#include "hnrt/ResourceString.h"
#include "hnrt/Service.h"
#include "hnrt/Path.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/Buffer.h"
#include "hnrt/MultibyteString.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/Debug.h"
#include "resource.h"
#include <stdio.h>
#include <stdarg.h>
#include <list>
#include <map>


using namespace hnrt;


constexpr auto REGKEY = L"SOFTWARE\\hnrt\\HostsUpdater";
constexpr auto REGVAL_VERSION = L"Version";
constexpr auto REGVAL_LOGFILE = L"LogFile";
constexpr auto LOGFILE_DEFAULT = L"%ALLUSERSPROFILE%\\hnrt\\HostsUpdater.log";
constexpr auto REGVAL_HOSTSFILE = L"HostsFile";
constexpr auto HOSTSFILE_DEFAULT = L"%windir%\\System32\\drivers\\etc\\hosts";
constexpr auto REGKEY_MAPPINGS = L"SOFTWARE\\hnrt\\HostsUpdater\\Mappings";


HostsUpdateService* HostsUpdateService::m_pSingleton = nullptr;
SpinLock::Type HostsUpdateService::m_lockPointer = SPIN_LOCK_INITIALIZER;


RefPtr<HostsUpdateService> HostsUpdateService::GetInstance()
{
	SpinLock lock(m_lockPointer);
	if (m_pSingleton) {
		m_pSingleton->AddRef();
	}
	else
	{
		m_pSingleton = new HostsUpdateService();
	}
	return RefPtr<HostsUpdateService>(m_pSingleton);
}


HostsUpdateService::HostsUpdateService()
	: m_lockSelf(SPIN_LOCK_INITIALIZER)
	, m_szServiceName(ResourceString(IDS_SERVICE_NAME))
	, m_dwError(ERROR_SUCCESS)
	, m_hServiceStatus(NULL)
	, m_dwCurrentState(0)
	, m_dwCheckPoint(0)
	, m_ExclusiveOperation(0)
	, m_hEventMain(CreateEventW(NULL, TRUE, FALSE, NULL))
	, m_szAppDir(Path::Combine(Path::GetKnownFolder(FOLDERID_ProgramData), L"hnrt"))
	, m_szLogFile()
	, m_hLogFile()
	, m_szHostsFile()
	, m_Mappings()
	, m_bReadOnly(false)
{
}


HostsUpdateService::~HostsUpdateService()
{
	SpinLock lock(m_lockPointer);
	m_pSingleton = nullptr;
}


void HostsUpdateService::Install(PCWSTR pszCommand)
{
	ServiceControlManager scm;
	scm.Open();
	ServiceConfiguration cfg;
	cfg.SetName(m_szServiceName)
		.SetDisplayName(ResourceString(IDS_SERVICE_DISPLAY_NAME))
		.SetDescription(IDS_SERVICE_DESCRIPTION)
		.SetBinaryPathName(NULL, pszCommand);
	Service svc;
	svc.Create(scm, cfg);
	fwprintf(stdout, ResourceString(IDS_INFO_CREATED_SERVICE), m_szServiceName.Ptr);
	CreateRegistry();
	CreateAppDirIfNotExist();
}


void HostsUpdateService::Uninstall()
{
	ServiceControlManager scm;
	scm.Open();
	Service svc;
	svc.Open(scm, m_szServiceName, SERVICE_ALL_ACCESS);
	svc.QueryStatus();
	if (svc.CurrentState != SERVICE_STOPPED)
	{
		fwprintf(stdout,
			svc.CurrentState == SERVICE_RUNNING ? ResourceString(IDS_INFO_SERVICE_IS_RUNNING).Ptr :
			svc.CurrentState == SERVICE_PAUSED ? ResourceString(IDS_INFO_SERVICE_IS_SUSPENDED).Ptr :
			ResourceString(IDS_INFO_SERVICE_IS_NOT_STOPPED).Ptr, m_szServiceName.Ptr);
		svc.Stop();
		fwprintf(stdout, ResourceString(IDS_INFO_STOPPING_SERVICE), m_szServiceName.Ptr);
		for (int count = 0; count < 30; count++)
		{
			Sleep(100);
			svc.QueryStatus();
			if (svc.CurrentState == SERVICE_STOPPED)
			{
				fwprintf(stdout, ResourceString(IDS_INFO_STOPPED_SERVICE), m_szServiceName.Ptr);
				break;
			}
		}
	}
	svc.Delete();
	fwprintf(stdout, ResourceString(IDS_INFO_DELETED_SERVICE), m_szServiceName.Ptr);
}


void HostsUpdateService::Run()
{
	SERVICE_TABLE_ENTRYW ServiceTable[] =
	{
		{ const_cast<PWSTR>(m_szServiceName.Ptr), ServiceMain },
		{ NULL, NULL }
	};
	if (!StartServiceCtrlDispatcherW(ServiceTable))
	{
		m_dwError = GetLastError();
		throw Win32Exception(m_dwError, ResourceString(IDS_FAILED_START_SVC_CTRL_DISPATCHER));
	}
}


VOID WINAPI HostsUpdateService::ServiceMain(DWORD dwArgc, PWSTR* pszArgv)
{
	auto pInstance = GetInstance();
	if (pInstance->OnStart())
	{
		pInstance->MainLoop();
	}
}


DWORD WINAPI HostsUpdateService::HandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
	auto pInstance = GetInstance();
	switch (dwControl) {
	case SERVICE_CONTROL_INTERROGATE:
		return pInstance->OnInterrogate();
	case SERVICE_CONTROL_STOP:
		return pInstance->OnStop();
	case SERVICE_CONTROL_PAUSE:
		return pInstance->OnPause();
	case SERVICE_CONTROL_CONTINUE:
		return pInstance->OnContinue();
	case SERVICE_CONTROL_SHUTDOWN:
		return pInstance->OnShutdown();
	case SERVICE_CONTROL_SESSIONCHANGE:
		return pInstance->OnSessionChange(dwEventType, reinterpret_cast<WTSSESSION_NOTIFICATION*>(lpEventData)->dwSessionId);
	default:
		return ERROR_CALL_NOT_IMPLEMENTED;
	}
}


bool HostsUpdateService::OnStart()
{
	m_hServiceStatus = RegisterServiceCtrlHandlerExW(m_szServiceName, HandlerEx, this);
	if (m_hServiceStatus)
	{
		SetStatus(SERVICE_START_PENDING);
		ReadRegistry();
		return true;
	}
	else
	{
		_InterlockedExchange(&m_dwCurrentState, SERVICE_STOPPED);
		m_dwCheckPoint = 0;
		return false;
	}
}


DWORD HostsUpdateService::OnStop()
{
	if (SetStatus(SERVICE_STOP_PENDING))
	{
		SetEvent(m_hEventMain);
		return NO_ERROR;
	}
	else
	{
		return m_dwError;
	}
}


DWORD HostsUpdateService::OnPause()
{
	if (SetStatus(SERVICE_PAUSE_PENDING))
	{
		SetEvent(m_hEventMain);
		return NO_ERROR;
	}
	else
	{
		return m_dwError;
	}
}


DWORD HostsUpdateService::OnContinue()
{
	if (SetStatus(SERVICE_CONTINUE_PENDING))
	{
		SetEvent(m_hEventMain);
		return NO_ERROR;
	}
	else
	{
		return m_dwError;
	}
	return 0;
}


DWORD HostsUpdateService::OnInterrogate()
{
	SetStatus(m_dwCurrentState);
	return NO_ERROR;
}


DWORD HostsUpdateService::OnShutdown()
{
	SetStatus(SERVICE_STOP_PENDING);
	SetEvent(m_hEventMain);
	return NO_ERROR;
}


DWORD HostsUpdateService::OnSessionChange(DWORD dwEventType, DWORD dwSessionId)
{
	switch (dwEventType)
	{
	case WTS_CONSOLE_CONNECT: // The session identified by lParam was connected to the console terminal or RemoteFX session.
		break;
	case WTS_CONSOLE_DISCONNECT: // The session identified by lParam was disconnected from the console terminal or RemoteFX session.
		break;
	case WTS_REMOTE_CONNECT: // The session identified by lParam was connected to the remote terminal.
		break;
	case WTS_REMOTE_DISCONNECT:// The session identified by lParam was disconnected from the remote terminal.
		break;
	case WTS_SESSION_LOGON: // A user has logged on to the session identified by lParam.
		SetEvent(m_hEventMain);
		break;
	case WTS_SESSION_LOGOFF: // A user has logged off the session identified by lParam.
		break;
	case WTS_SESSION_LOCK: // The session identified by lParam has been locked.
		break;
	case WTS_SESSION_UNLOCK: // The session identified by lParam has been unlocked.
		SetEvent(m_hEventMain);
		break;
	case WTS_SESSION_REMOTE_CONTROL: // The session identified by lParam has changed its remote controlled status.To determine the status, call GetSystemMetrics and check the SM_REMOTECONTROL metric.
		break;
	case WTS_SESSION_CREATE: // Reserved for future use.
		break;
	case WTS_SESSION_TERMINATE: // Reserved for future use.
		break;
	default:
		return NO_ERROR;
	}
	return NO_ERROR;
}


void HostsUpdateService::MainLoop()
{
	if (!SetStatus(SERVICE_RUNNING, SERVICE_START_PENDING))
	{
		return;
	}
	Log(L"INFO: Started.");
	while (_InterlockedCompareExchange(&m_dwCurrentState, SERVICE_STOP_PENDING, SERVICE_STOP_PENDING) != SERVICE_STOP_PENDING)
	{
		ProcessHostsFile();
		switch (m_dwCurrentState)
		{
		case SERVICE_PAUSE_PENDING:
			SetStatus(SERVICE_PAUSED, SERVICE_PAUSE_PENDING);
			Log(L"INFO: Suspended.");
			CloseLogFile();
			WaitForSingleObject(m_hEventMain, INFINITE);
			ResetEvent(m_hEventMain);
			break;
		case SERVICE_CONTINUE_PENDING:
			SetStatus(SERVICE_RUNNING, SERVICE_CONTINUE_PENDING);
			OpenLogFile();
			Log(L"INFO: Resumed.");
			break;
		default:
			WaitForSingleObject(m_hEventMain, 1000 * 600);
			ResetEvent(m_hEventMain);
			break;
		}
	}
	OpenLogFile();
	Log(L"INFO: Ended.");
	CloseLogFile();
	SetStatus(SERVICE_STOPPED);
}


bool HostsUpdateService::SetStatus(DWORD dwState, DWORD dwPreviousStatus, DWORD dwWaitHint)
{
	if (dwPreviousStatus)
	{
		if (_InterlockedCompareExchange(&m_dwCurrentState, dwState, dwPreviousStatus) != dwPreviousStatus)
		{
			return false;
		}
	}
	else
	{
		dwPreviousStatus = _InterlockedExchange(&m_dwCurrentState, dwState);
	}

	if (dwPreviousStatus != dwState)
	{
		m_dwCheckPoint = 0;
	}

	SERVICE_STATUS ss = { 0 };

	ss.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	ss.dwCurrentState = dwState;
	ss.dwCheckPoint = ++m_dwCheckPoint;
	ss.dwWaitHint = dwWaitHint;
	switch (dwState)
	{
	case SERVICE_STOPPED:
		break;
	case SERVICE_START_PENDING:
		ss.dwControlsAccepted = SERVICE_ACCEPT_STOP;
		break;
	case SERVICE_STOP_PENDING:
		break;
	case SERVICE_RUNNING:
		ss.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SESSIONCHANGE | SERVICE_ACCEPT_PAUSE_CONTINUE;
		break;
	case SERVICE_CONTINUE_PENDING:
		ss.dwControlsAccepted = SERVICE_ACCEPT_STOP;
		break;
	case SERVICE_PAUSE_PENDING:
		ss.dwControlsAccepted = SERVICE_ACCEPT_STOP;
		break;
	case SERVICE_PAUSED:
		ss.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;
		break;
	default:
		break;
	}

	if (SetServiceStatus(m_hServiceStatus, &ss))
	{
		return true;
	}
	else
	{
		m_dwError = GetLastError();
		return false;
	}
}


void HostsUpdateService::OpenLogFile(const String& szFileName)
{
	if (szFileName)
	{
		m_szLogFile = szFileName;
	}
	else if (m_hLogFile.isValid || !m_szLogFile)
	{
		return;
	}
	m_hLogFile = CreateFileW(m_szLogFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hLogFile.isValid)
	{
		if (SetFilePointer(m_hLogFile, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER)
		{
			CloseLogFile();
		}
	}
}


void HostsUpdateService::CloseLogFile()
{
	m_hLogFile.Close();
}


void HostsUpdateService::Log(PCWSTR pszFormat, ...)
{
	if (!m_hLogFile.isValid)
	{
		return;
	}
	SYSTEMTIME t = { 0 };
	GetLocalTime(&t);
	String sz(PRINTF, L"%04d-%02d-%02dT%02d:%02d:%02d.%03d ", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
	va_list argList;
	va_start(argList, pszFormat);
	sz.VaAppendFormat(pszFormat, argList);
	va_end(argList);
	sz.Append(L"\r\n");
	MultibyteString szUTF8 = ToUTF8(sz);
	SpinLock lock(m_lockSelf);
	WriteFile(m_hLogFile, szUTF8.Ptr, static_cast<DWORD>(szUTF8.Len), NULL, NULL);
}


void HostsUpdateService::CreateRegistry()
{
	RegistryKey key;
	DWORD dwDisposition = 0;
	if (key.Create(HKEY_LOCAL_MACHINE, REGKEY, 0, KEY_ALL_ACCESS, NULL, &dwDisposition) == ERROR_SUCCESS)
	{
		if (dwDisposition == REG_CREATED_NEW_KEY)
		{
			CreateRegistryV1(key);
		}
		else if (dwDisposition == REG_OPENED_EXISTING_KEY)
		{
			DWORD dwValue = RegistryValue::GetDWORD(key, REGVAL_VERSION, 0);
			switch (dwValue)
			{
			case 0:
			{
				RegistryValue value;
				DWORD dwError = value.Query(key, REGVAL_VERSION);
				if (dwError == ERROR_FILE_NOT_FOUND)
				{
					fwprintf(stdout, ResourceString(IDS_WARNING_REGISTRY_MISSING_CREATING), REGVAL_VERSION);
					CreateRegistryV1(key);
				}
				else
				{
					fwprintf(stdout, ResourceString(IDS_WARNING_REGISTRY_UNAVAILABLE), REGVAL_VERSION, dwError, ErrorMessage::Get(dwError));
				}
				break;
			}
			case 1:
				fwprintf(stdout, ResourceString(IDS_INFO_FOUND_REGISTRY_VERSION_1_UNDER_HKLM), REGKEY);
				break;
			default:
				fwprintf(stdout, ResourceString(IDS_WARNING_REGISTRY_UNEXPECTED_VALUE_LU), REGVAL_VERSION, dwValue);
				break;
			}
		}
	}
}


void HostsUpdateService::CreateRegistryV1(RegistryKey& key)
{
	RegistryValue::SetDWORD(key, REGVAL_VERSION, 1);
	RegistryValue::SetEXPANDSZ(key, REGVAL_LOGFILE, LOGFILE_DEFAULT);
	RegistryValue::SetEXPANDSZ(key, REGVAL_HOSTSFILE, HOSTSFILE_DEFAULT);
	RegistryKey keyMappings;
	keyMappings.Create(HKEY_LOCAL_MACHINE, REGKEY_MAPPINGS);
	fwprintf(stdout, ResourceString(IDS_INFO_CREATED_REGISTRY_VERSION_1_UNDER_HKLM), REGKEY);
}


void HostsUpdateService::CreateAppDirIfNotExist()
{
	if (Path::IsDirectory(m_szAppDir))
	{
		fwprintf(stdout, ResourceString(IDS_INFO_APPDIR_IS), m_szAppDir.Ptr);
		return;
	}
	if (CreateDirectoryW(m_szAppDir, NULL))
	{
		fwprintf(stdout, ResourceString(IDS_INFO_CREATED_DIRECTORY), m_szAppDir.Ptr);
	}
	else
	{
		DWORD dwError = GetLastError();
		fwprintf(stdout, ResourceString(IDS_WARNING_FAILED_CREATE_DIRECTORY), m_szAppDir.Ptr, dwError, ErrorMessage::Get(dwError));
	}
}


void HostsUpdateService::ReadRegistry()
{
	DBGFNC(L"HostsUpdateService::ReadRegistry");
	RegistryKey key;
	if (key.Open(HKEY_LOCAL_MACHINE, REGKEY, 0, KEY_READ) == ERROR_SUCCESS)
	{
		String szLogFileName = RegistryValue::GetString(key, REGVAL_LOGFILE);
		if (szLogFileName.Len)
		{
			DBGPUT(L"%s=%s", REGVAL_LOGFILE, szLogFileName);
			OpenLogFile(szLogFileName);
		}
		String szHostsFileName = RegistryValue::GetString(key, REGVAL_HOSTSFILE);
		if (szHostsFileName.Len)
		{
			DBGPUT(L"%s=%s", REGVAL_HOSTSFILE, szHostsFileName);
			m_szHostsFile = szHostsFileName;
		}
	}
	if (key.Open(HKEY_LOCAL_MACHINE, REGKEY_MAPPINGS, 0, KEY_READ) == ERROR_SUCCESS)
	{
		m_Mappings.Clear();
		for (DWORD dwIndex = 0; ; dwIndex++)
		{
			DWORD dwType = 0;
			WCHAR szName[260] = { 0 };
			DWORD dwNameLen = _countof(szName);
			WCHAR szValue[260] = { 0 };
			DWORD dwValueLen = sizeof(szValue) - sizeof(WCHAR);
			LSTATUS status = RegEnumValueW(key, dwIndex, szName, &dwNameLen, NULL, &dwType, reinterpret_cast<LPBYTE>(szValue), &dwValueLen);
			if (status == ERROR_NO_MORE_ITEMS)
			{
				break;
			}
			else if (status != ERROR_SUCCESS)
			{
				Log(L"WARNING: Mappings[%lu] %lu %s", dwIndex, status, ErrorMessage::Get(status));
				continue;
			}
			if (dwType == REG_SZ)
			{
				reinterpret_cast<LPBYTE>(szValue)[dwValueLen + 0] = 0;
				reinterpret_cast<LPBYTE>(szValue)[dwValueLen + 1] = 0;
				DBGPUT(L"Mappings[%lu] \"%s\"=\"%s\"", dwIndex, szName, szValue);
				m_Mappings.Add(String(szName), String(szValue));
			}
		}
	}
	if (!m_szHostsFile.IsSet)
	{
		Log(L"WARNING: No hosts file name is set.");
	}
	if (!m_Mappings.Size())
	{
		Log(L"WARNING: No mapping is set.");
	}
}


void HostsUpdateService::ProcessHostsFile()
{
	DBGFNC(L"HostsUpdateService::ProcessHostsFile");
	if (!m_szHostsFile.Len)
	{
		DBGPUT(L"m_szHostsFile=null");
		return;
	}
	DBGPUT(L"m_szHostsFile=%s", m_szHostsFile);
	HostsFile hosts(m_szHostsFile, m_bReadOnly);
	try
	{
		hosts.Open();
		hosts.Read();
		hosts.Parse();
		HostsFile::UpdateMap updateEntries;
		HostsFile::AppendList appendEntries;
		for (KeyValueMap::ConstIterator iter = m_Mappings.Begin(); iter != m_Mappings.End(); iter++)
		{
			AddressResolution ar(iter->first, iter->second);
			if (!ar.Resolve())
			{
				DBGPUT(L"alias=%s hostname=%s: Resolution failed.", iter->first.Ptr, iter->second.Ptr);
				continue;
			}
			DBGPUT(L"alias=%s hostname=%s: Resolution succeeded.", iter->first.Ptr, iter->second.Ptr);
			HostEntry* pEntry = hosts.FindByName(hosts.UTF16, ar.Alias);
			if (pEntry)
			{
				DBGPUT(L"%s is found in hosts. Checking if address(%.*s) has been updated...", ar.Alias, pEntry->Address.Len, hosts.UTF16 + pEntry->Address.Start);
				bool bMatch = false;
				for (DWORD dwIndex = 0; dwIndex < ar.Count; dwIndex++)
				{
					DBGPUT(L"[%lu]=%s", dwIndex, ar[dwIndex]);
					if (!pEntry->Address.Compare(hosts.UTF16, ar[dwIndex]))
					{
						DBGPUT(L"Address matches with the resolved one.");
						bMatch = true;
						break;
					}
				}
				if (!bMatch)
				{
					if (updateEntries.find(pEntry) == updateEntries.end())
					{
						DBGPUT(L"Address doesn't match with the resolved one; it is going to be updated.");
						updateEntries.insert(HostsFile::HostEntryAddressPair(pEntry, String(ar[0])));
					}
					else
					{
						DBGPUT(L"Address doesn't match with the resolved one; it is going to be updated. (Already handled.)");
					}
				}
			}
			else
			{
				DBGPUT(L"%s is not found in hosts; Address/hostname pair is going to be appended.", ar.Alias);
				appendEntries.push_back(HostsFile::AddressAliasPair(String(ar[0]), String(ar.Alias)));
			}
		}
		if (updateEntries.size() + appendEntries.size())
		{
			hosts.CreateBackup();
			hosts.Rebuild(updateEntries, appendEntries);
			hosts.Write();
			if (m_bReadOnly)
			{
				wprintf(L"%.*s", static_cast<int>(hosts.UTF16.Len), hosts.UTF16.Ptr);
			}
			else
			{
				Log(L"INFO: Wrote \"%s\" %lu bytes.", hosts.Name, hosts.Length);
			}
		}
	}
	catch (Exception ex)
	{
		Log(L"ERROR: %s", ex.Message);
	}
}