#include "VmIpAddressFinder.h"
#include "hnrt/Service.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/ComException.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/Debug.h"


using namespace hnrt;


void VmIpAddressFinder::InstallService() const
{
    DBGFNC(L"VmIpAddressFinder::InstallService");

    ServiceControlManager hScm;
    Service hSvc;

    hScm.Open();
    hSvc.Create(hScm,
        ServiceConfiguration()
        .SetName(m_szServiceName)
        .SetDisplayName(L"VM IP Address Updater")
        .SetDescription(L"Collect VM address information and updates hosts file accordingly.")
        .SetDesiredAccess(GENERIC_ALL)
        .SetServiceType(SERVICE_WIN32_OWN_PROCESS)
        .SetStartType(SERVICE_AUTO_START)
        .SetErrorControl(SERVICE_ERROR_CRITICAL)
        .SetBinaryPathName(GetServiceCommandPath()));
}


void VmIpAddressFinder::UninstallService() const
{
    DBGFNC(L"VmIpAddressFinder::UninstallService");

    ServiceControlManager hScm;
    Service hSvc;

    hScm.Open();
    hSvc.Open(hScm, m_szServiceName, SERVICE_ALL_ACCESS);
    hSvc.Delete();
}


void VmIpAddressFinder::StartService() const
{
    DBGFNC(L"VmIpAddressFinder::StartService");

    ServiceControlManager hScm;
    Service hSvc;

    hScm.Open();
    hSvc.Open(hScm, m_szServiceName, SERVICE_ALL_ACCESS);
    hSvc.Start();
}


void VmIpAddressFinder::StopService() const
{
    DBGFNC(L"VmIpAddressFinder::StopService");

    ServiceControlManager hScm;
    Service hSvc;

    hScm.Open();
    hSvc.Open(hScm, m_szServiceName, SERVICE_ALL_ACCESS);
    hSvc.Stop();
}


void VmIpAddressFinder::RunService()
{
    DebugFunc func(L"VmIpAddressFinder::RunService");

    EnableServiceMode();

    SERVICE_TABLE_ENTRYW table[] =
    {
        { const_cast<LPWSTR>(m_szServiceName.Ptr), ServiceMain },
        { NULL, NULL }
    };

    if (!StartServiceCtrlDispatcherW(table))
    {
        throw Win32Exception(GetLastError(), L"Failed to dispatch a service.");
    }
}


String VmIpAddressFinder::GetServiceCommandPath()
{
    WCHAR szFileName[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, szFileName, MAX_PATH);
    return String(PRINTF, L"\"%s\" -run", szFileName);
}


VOID WINAPI VmIpAddressFinder::ServiceMain(DWORD dwArgc, PWSTR* pszArgv)
{
    DebugFunc func(L"VmIpAddressFinder::ServiceMain");

    try
    {
        Args args(dwArgc, pszArgv);
        Instance().ServiceMain(args);
    }
    catch (ComException e)
    {
        VmIpAddressFinder::PutError(L"ERROR: %s", e.Message);
        VmIpAddressFinder::PutError(L"  Code: %08lX", e.Result);
    }
    catch (Win32Exception e)
    {
        VmIpAddressFinder::PutError(L"ERROR: %s", e.Message);
        VmIpAddressFinder::PutError(L"  Reason: %s (%lu)", ErrorMessage::Get(e.Error), e.Error);
    }
    catch (Exception e)
    {
        VmIpAddressFinder::PutError(L"ERROR: %s", e.Message);
    }
    catch (std::exception e)
    {
        VmIpAddressFinder::PutError(L"ERROR: %hs", e.what());
    }
    catch (...)
    {
        VmIpAddressFinder::PutError(L"ERROR: Unhandled exception caught.");
    }
}


DWORD WINAPI VmIpAddressFinder::HandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
    VmIpAddressFinder* pThis = reinterpret_cast<VmIpAddressFinder*>(lpContext);
    return pThis->ControlService(dwControl, dwEventType, lpEventData);
}


void VmIpAddressFinder::ServiceMain(Args& args)
{
    m_hEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!m_hEvent)
    {
        throw Win32Exception(GetLastError(), L"Failed to create an event.");
    }

    m_hServiceStatus = ::RegisterServiceCtrlHandlerExW(m_szServiceName, HandlerEx, this);
    if (!m_hServiceStatus)
    {
        throw Win32Exception(GetLastError(), L"Failed to register service control handler.");
    }

    SetServiceStatus(SERVICE_START_PENDING);

    SetServiceStatus(SERVICE_RUNNING);

    while (true)
    {
        DWORD dwRet = WAIT_TIMEOUT;
        DWORD dwInterval;

        switch (m_dwCurrentState)
        {
        case SERVICE_RUNNING:
            DBGPUT(L"SERVICE_RUNNING");
            try
            {
                dwInterval = 1000 * 60;
                UpdateHosts();
                dwInterval = 1000 * 60 * 5;
            }
            catch (ComException e)
            {
                VmIpAddressFinder::PutError(L"ERROR: %s", e.Message);
                VmIpAddressFinder::PutError(L"  Code: %08lX", e.Result);
            }
            catch (Win32Exception e)
            {
                VmIpAddressFinder::PutError(L"ERROR: %s", e.Message);
                VmIpAddressFinder::PutError(L"  Reason: %s (%lu)", ErrorMessage::Get(e.Error), e.Error);
            }
            catch (Exception e)
            {
                VmIpAddressFinder::PutError(L"ERROR: %s", e.Message);
            }
            catch (std::bad_alloc)
            {
                VmIpAddressFinder::PutError(L"ERROR: Out of memory.");
                goto done;
            }
            catch (std::exception e)
            {
                VmIpAddressFinder::PutError(L"ERROR: %hs", e.what());
            }
            catch (...)
            {
                VmIpAddressFinder::PutError(L"ERROR: Unhandled exception caught.");
                goto done;
            }
            ResetEvent(m_hEvent);
            dwRet = WaitForSingleObject(m_hEvent, dwInterval);
            break;

        case SERVICE_STOP_PENDING:
            DBGPUT(L"SERVICE_STOP_PENDING");
            goto done;

        case SERVICE_PAUSE_PENDING:
            DBGPUT(L"SERVICE_PAUSE_PENDING");
            SetServiceStatus(SERVICE_PAUSED);
            ResetEvent(m_hEvent);
            dwRet = WaitForSingleObject(m_hEvent, INFINITE);
            break;

        case SERVICE_CONTINUE_PENDING:
            DBGPUT(L"SERVICE_CONTINUE_PENDING");
            SetServiceStatus(SERVICE_RUNNING);
            break;

        default:
            throw Exception(L"VmIpAddressFinder::ServiceMain: Unhandled state: %lu", m_dwCurrentState);
        }

        if (dwRet != WAIT_OBJECT_0 && dwRet != WAIT_TIMEOUT)
        {
            throw Win32Exception(GetLastError(), L"Failed to wait for an event.");
        }
    }

done:

    SetServiceStatus(SERVICE_STOPPED);
}


DWORD VmIpAddressFinder::ControlService(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData)
{
    DBGFNC(L"VmIpAddressFinder::ControlService");

    switch (dwControl)
    {
    case SERVICE_CONTROL_STOP:
        DBGPUT(L"SERVICE_CONTROL_STOP");
        SetServiceStatus(SERVICE_STOP_PENDING);
        SetEvent(m_hEvent);
        return NO_ERROR;

    case SERVICE_CONTROL_INTERROGATE:
        DBGPUT(L"SERVICE_CONTROL_INTERROGATE");
        SetServiceStatus(m_dwCurrentState);
        return NO_ERROR;

    case SERVICE_CONTROL_PAUSE:
        DBGPUT(L"SERVICE_CONTROL_PAUSE");
        SetServiceStatus(SERVICE_PAUSE_PENDING);
        SetEvent(m_hEvent);
        return NO_ERROR;

    case SERVICE_CONTROL_CONTINUE:
        DBGPUT(L"SERVICE_CONTROL_CONTINUE");
        SetServiceStatus(SERVICE_CONTINUE_PENDING);
        SetEvent(m_hEvent);
        return NO_ERROR;

    default:
        DBGPUT(L"SERVICE_CONTROL_%lu", dwControl);
        return ERROR_CALL_NOT_IMPLEMENTED;
    }
}


static PCWSTR GetStateText(DWORD dwState)
{
    return dwState == SERVICE_START_PENDING ? L"SERVICE_START_PENDING" :
        dwState == SERVICE_RUNNING ? L"SERVICE_RUNNING" :
        dwState == SERVICE_PAUSED ? L"SERVICE_PAUSED" :
        dwState == SERVICE_STOPPED ? L"SERVICE_STOPPED" :
        dwState == SERVICE_STOP_PENDING ? L"SERVICE_STOP_PENDING" :
        dwState == SERVICE_PAUSE_PENDING ? L"SERVICE_PAUSE_PENDING" :
        dwState == SERVICE_CONTINUE_PENDING ? L"SERVICE_CONTINUE_PENDING" :
        String::Copy(String(PRINTF, L"%lu", dwState));
}


#define MAX_ATTEMPTS 10
#define RETRY_INTERVAL 1000


void VmIpAddressFinder::SetServiceStatus(DWORD dwState)
{
    DBGFNC(L"VmIpAddressFinder::SetServiceStatus");
    DBGPUT(L"State=%s", GetStateText(dwState));

    DWORD dwError = ERROR_SUCCESS;

    for (LONG attempts = 0; attempts < MAX_ATTEMPTS; attempts++)
    {
        if (attempts)
        {
            Sleep(RETRY_INTERVAL);
        }

        SERVICE_STATUS ss = { 0 };
        ss.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
        ss.dwCurrentState = dwState;
        ss.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;
        if (::SetServiceStatus(m_hServiceStatus, &ss))
        {
            m_dwCurrentState = dwState;
            return;
        }
        else
        {
            dwError = GetLastError();
            DBGPUT(L"SetServiceStatus failed: %s (%lu)", ErrorMessage::Get(dwError), dwError);
        }
    }

    throw Win32Exception(dwError, L"SetServiceStatus(%s) failed.", GetStateText(dwState));
}
