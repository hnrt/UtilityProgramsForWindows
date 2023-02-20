#include "HostsUpdateService.h"
#include "HostsFile.h"
#include "AddressResolution.h"
#include "hnrt/ResourceString.h"
#include "hnrt/Service.h"
#include "hnrt/Path.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/Buffer.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/Debug.h"
#include "resource.h"
#include <stdio.h>
#include <stdarg.h>
#include <list>
#include <map>


using namespace hnrt;


#define REGKEY L"SOFTWARE\\hnrt\\HostsUpdater"
#define REGVAL_VERSION L"Version"
#define REGVAL_LOGFILE L"LogFile"
#define LOGFILE_DEFAULT L"%ALLUSERSPROFILE%\\hnrt\\HostsUpdater.log"
#define REGVAL_HOSTSFILE L"HostsFile"
#define HOSTSFILE_DEFAULT L"%windir%\\System32\\drivers\\etc\\hosts"
#define REGKEY_MAPPINGS REGKEY L"\\Mappings"


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
	, m_pszServiceName(Clone(ResourceString(IDS_SERVICE_NAME)))
	, m_dwError(ERROR_SUCCESS)
	, m_hServiceStatus(NULL)
	, m_dwCurrentState(0)
	, m_dwCheckPoint(0)
	, m_ExclusiveOperation(0)
	, m_hEventMain(CreateEventW(NULL, TRUE, FALSE, NULL))
	, m_pszAppDir(Path::Combine(Path::GetKnownFolder(FOLDERID_ProgramData), L"hnrt"))
	, m_pszLogFile(nullptr)
	, m_hLogFile()
	, m_pszHostsFile(nullptr)
	, m_Mappings()
	, m_bReadOnly(false)
{
}


HostsUpdateService::~HostsUpdateService()
{
	SpinLock lock(m_lockPointer);
	m_pSingleton = nullptr;
	free(m_pszServiceName);
	free(m_pszLogFile);
	free(m_pszHostsFile);
}


void HostsUpdateService::Install(PCWSTR pszCommand)
{
	ServiceControlManager scm;
	scm.Open();
	ServiceConfiguration cfg;
	cfg.SetName(m_pszServiceName)
		.SetDisplayName(ResourceString(IDS_SERVICE_DISPLAY_NAME))
		.SetDescription(IDS_SERVICE_DESCRIPTION)
		.SetBinaryPathName(NULL, pszCommand);
	Service svc;
	svc.Create(scm, cfg);
	fwprintf(stdout, ResourceString(IDS_INFO_CREATED_SERVICE), m_pszServiceName);
	CreateRegistry();
	CreateAppDirIfNotExist();
}


void HostsUpdateService::Uninstall()
{
	ServiceControlManager scm;
	scm.Open();
	Service svc;
	svc.Open(scm, m_pszServiceName, SERVICE_ALL_ACCESS);
	svc.QueryStatus();
	if (svc.CurrentState != SERVICE_STOPPED)
	{
		fwprintf(stdout,
			svc.CurrentState == SERVICE_RUNNING ? ResourceString(IDS_INFO_SERVICE_IS_RUNNING).Ptr :
			svc.CurrentState == SERVICE_PAUSED ? ResourceString(IDS_INFO_SERVICE_IS_SUSPENDED).Ptr :
			ResourceString(IDS_INFO_SERVICE_IS_NOT_STOPPED).Ptr, m_pszServiceName);
		svc.Stop();
		fwprintf(stdout, ResourceString(IDS_INFO_STOPPING_SERVICE), m_pszServiceName);
		for (int count = 0; count < 30; count++)
		{
			Sleep(100);
			svc.QueryStatus();
			if (svc.CurrentState == SERVICE_STOPPED)
			{
				fwprintf(stdout, ResourceString(IDS_INFO_STOPPED_SERVICE), m_pszServiceName);
				break;
			}
		}
	}
	svc.Delete();
	fwprintf(stdout, ResourceString(IDS_INFO_DELETED_SERVICE), m_pszServiceName);
}


void HostsUpdateService::Run()
{
	SERVICE_TABLE_ENTRYW ServiceTable[] =
	{
		{ m_pszServiceName, ServiceMain },
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
	m_hServiceStatus = RegisterServiceCtrlHandlerExW(m_pszServiceName, HandlerEx, this);
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


void HostsUpdateService::OpenLogFile(PCWSTR pszFileName)
{
	if (pszFileName)
	{
		m_pszLogFile = Clone(pszFileName);
	}
	else if (m_hLogFile.isValid || !m_pszLogFile)
	{
		return;
	}
	m_hLogFile = CreateFileW(m_pszLogFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
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
	va_list ap1, ap2;
	va_start(ap1, pszFormat);
	va_copy(ap2, ap1);
	int cch = _vscwprintf(pszFormat, ap1);
	Buffer<WCHAR> wbuf(cch + 1);
	_vsnwprintf_s(wbuf, wbuf.Len, _TRUNCATE, pszFormat, ap2);
	va_end(ap2);
	va_end(ap1);
	int cb2 = WideCharToMultiByte(CP_UTF8, 0, wbuf, cch, NULL, 0, NULL, NULL);
	Buffer<CHAR> buf(32 + cb2);
	int cb1 = _snprintf_s(buf, buf.Len, _TRUNCATE, "%04d-%02d-%02dT%02d:%02d:%02d.%03d ", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
	WideCharToMultiByte(CP_UTF8, 0, wbuf, cch, buf + cb1, cb2, NULL, NULL);
	int cb = cb1 + cb2;
	buf[cb++] = '\r';
	buf[cb++] = '\n';
	SpinLock lock(m_lockSelf);
	WriteFile(m_hLogFile, buf.Ptr, cb, NULL, NULL);
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
			DWORD dwError;
			RegistryValue value;
			switch (value.GetDWORD(key, REGVAL_VERSION, 0))
			{
			case 0:
				dwError = value.Query(key, REGVAL_VERSION);
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
			case 1:
				fwprintf(stdout, ResourceString(IDS_INFO_FOUND_REGISTRY_VERSION_1_UNDER_HKLM), REGKEY);
				break;
			default:
				fwprintf(stdout, ResourceString(IDS_WARNING_REGISTRY_UNEXPECTED_VALUE_LU), REGVAL_VERSION, value.UInt32);
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
	DWORD dwAttr = GetFileAttributesW(m_pszAppDir);
	if ((dwAttr & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
	{
		fwprintf(stdout, ResourceString(IDS_INFO_APPDIR_IS), m_pszAppDir);
		return;
	}
	if (CreateDirectoryW(m_pszAppDir, NULL))
	{
		fwprintf(stdout, ResourceString(IDS_INFO_CREATED_DIRECTORY), m_pszAppDir);
	}
	else
	{
		DWORD dwError = GetLastError();
		fwprintf(stdout, ResourceString(IDS_WARNING_FAILED_CREATE_DIRECTORY), m_pszAppDir, dwError, ErrorMessage::Get(dwError));
	}
}


void HostsUpdateService::ReadRegistry()
{
	DBGFNC(L"HostsUpdateService::ReadRegistry");
	RegistryKey key;
	if (key.Open(HKEY_LOCAL_MACHINE, REGKEY, 0, KEY_READ) == ERROR_SUCCESS)
	{
		RegistryValue value;
		Buffer<WCHAR> tmp(MAX_PATH);
		PCWSTR pszLogFileName = value.GetString(key, REGVAL_LOGFILE);
		if (pszLogFileName)
		{
			DBGPUT(L"%s=%s", REGVAL_LOGFILE, pszLogFileName);
			OpenLogFile(pszLogFileName);
		}
		PCWSTR pszHostsFileName = value.GetString(key, REGVAL_HOSTSFILE);
		if (pszHostsFileName)
		{
			DBGPUT(L"%s=%s", REGVAL_HOSTSFILE, pszHostsFileName);
			m_pszHostsFile = Clone(pszHostsFileName);
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
				m_Mappings.Add(String::Copy(szName), String::Copy(szValue));
			}
		}
	}
	if (!m_pszHostsFile)
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
	if (!m_pszHostsFile)
	{
		DBGPUT(L"m_pszHostsFile=null");
		return;
	}
	DBGPUT(L"m_pszHostsFile=%s", m_pszHostsFile);
	HostsFile hosts(m_pszHostsFile, m_bReadOnly);
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
				DBGPUT(L"alias=%s hostname=%s: Resolution failed.", iter->first, iter->second);
				continue;
			}
			DBGPUT(L"alias=%s hostname=%s: Resolution succeeded.", iter->first, iter->second);
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
						updateEntries.insert(HostsFile::HostEntryAddressPair(pEntry, std::wstring(ar[0])));
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
				appendEntries.push_back(HostsFile::AddressAliasPair(std::wstring(ar[0]), std::wstring(ar.Alias)));
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