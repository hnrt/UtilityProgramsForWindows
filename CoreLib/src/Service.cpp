#include "pch.h"
#include "hnrt/Service.h"
#include "hnrt/String.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/ResourceString.h"


using namespace hnrt;


////////////////////////////////////////////////////////////////////////////
////
////  S E R V I C E   C O N F I G U R A T I O N
////
////////////////////////////////////////////////////////////////////////////


ServiceConfiguration::ServiceConfiguration()
    : m_pszName(NULL)
    , m_pszDisplayName(NULL)
    , m_pszDescription(NULL)
    , m_dwDesiredAccess(GENERIC_ALL)
    , m_dwServiceType(SERVICE_WIN32_OWN_PROCESS)
    , m_dwStartType(SERVICE_AUTO_START)
    , m_dwErrorControl(SERVICE_ERROR_CRITICAL)
    , m_pszBinaryPathName(NULL)
    , m_pszLoadOrderGroup(NULL)
    , m_dependencies()
    , m_pszServiceStartName(NULL)
    , m_pszPassword(L"")
{
}


ServiceConfiguration::ServiceConfiguration(const ServiceConfiguration& src)
    : m_pszName(NULL)
    , m_pszDisplayName(NULL)
    , m_pszDescription(NULL)
    , m_dwDesiredAccess(GENERIC_ALL)
    , m_dwServiceType(SERVICE_WIN32_OWN_PROCESS)
    , m_dwStartType(SERVICE_AUTO_START)
    , m_dwErrorControl(SERVICE_ERROR_CRITICAL)
    , m_pszBinaryPathName(NULL)
    , m_pszLoadOrderGroup(NULL)
    , m_dependencies()
    , m_pszServiceStartName(NULL)
    , m_pszPassword(L"")
{
    operator =(src);
}


ServiceConfiguration& ServiceConfiguration::operator =(const ServiceConfiguration& src)
{
    m_pszName = src.m_pszName;
    m_pszDisplayName = src.m_pszDisplayName;
    m_pszDescription = src.m_pszDescription;
    m_dwDesiredAccess = src.m_dwDesiredAccess;
    m_dwServiceType = src.m_dwServiceType;
    m_dwStartType = src.m_dwStartType;
    m_dwErrorControl = src.m_dwErrorControl;
    m_pszBinaryPathName = src.m_pszBinaryPathName;
    m_pszLoadOrderGroup = src.m_pszLoadOrderGroup;
    m_dependencies = src.m_dependencies;
    m_pszServiceStartName = src.m_pszServiceStartName;
    m_pszPassword = src.m_pszPassword;
    return *this;
}


ServiceConfiguration& ServiceConfiguration::SetName(PCWSTR psz)
{
    m_pszName = String::Copy(psz);
    return *this;
}


ServiceConfiguration& ServiceConfiguration::SetDisplayName(PCWSTR psz)
{
    m_pszDisplayName = String::Copy(psz);
    return *this;
}


ServiceConfiguration& ServiceConfiguration::SetDescription(PCWSTR psz)
{
    m_pszDescription = String::Copy(psz);
    return *this;
}


ServiceConfiguration& ServiceConfiguration::SetDescription(UINT uId)
{
    WCHAR szPath[MAX_PATH];
    if (GetModuleFileNameW(NULL, szPath, MAX_PATH))
    {
        m_pszDescription = String::Format(L"@%s,-%u", szPath, uId);
    }
    else
    {
        m_pszDescription = String::Copy(ResourceString(uId));
    }
    return *this;
}


ServiceConfiguration& ServiceConfiguration::SetDesiredAccess(DWORD dw)
{
    m_dwDesiredAccess = dw;
    return *this;
}


ServiceConfiguration& ServiceConfiguration::SetServiceType(DWORD dw)
{
    m_dwServiceType = dw;
    return *this;
}


ServiceConfiguration& ServiceConfiguration::SetStartType(DWORD dw)
{
    m_dwStartType = dw;
    return *this;
}


ServiceConfiguration& ServiceConfiguration::SetErrorControl(DWORD dw)
{
    m_dwErrorControl = dw;
    return *this;
}


ServiceConfiguration& ServiceConfiguration::SetBinaryPathName(PCWSTR pszPath)
{
    WCHAR szFileName[MAX_PATH] = { 0 };
    if (!pszPath)
    {
        if (!GetModuleFileNameW(NULL, szFileName, MAX_PATH))
        {
            throw Win32Exception(GetLastError(), L"Failed to get the module file name.");
        }
        pszPath = szFileName;
    }
    if (wcschr(pszPath, L' '))
    {
        m_pszBinaryPathName = String::Format(L"\"%s\"", pszPath);
    }
    else
    {
        m_pszBinaryPathName = String::Copy(pszPath);
    }
    return *this;
}


ServiceConfiguration& ServiceConfiguration::SetBinaryPathName(PCWSTR pszPath, PCWSTR pszArg1)
{
    SetBinaryPathName(pszPath);
    if (wcschr(pszArg1, L' '))
    {
        m_pszBinaryPathName = String::Format(L"%s \"%s\"", m_pszBinaryPathName, pszArg1);
    }
    else
    {
        m_pszBinaryPathName = String::Format(L"%s %s", m_pszBinaryPathName, pszArg1);
    }
    return *this;
}


ServiceConfiguration& ServiceConfiguration::SetBinaryPathName(PCWSTR pszPath, PCWSTR pszArg1, PCWSTR pszArg2)
{
    SetBinaryPathName(pszPath, pszArg1);
    if (wcschr(pszArg2, L' '))
    {
        m_pszBinaryPathName = String::Format(L"%s \"%s\"", m_pszBinaryPathName, pszArg2);
    }
    else
    {
        m_pszBinaryPathName = String::Format(L"%s %s", m_pszBinaryPathName, pszArg2);
    }
    return *this;
}


ServiceConfiguration& ServiceConfiguration::SetBinaryPathName(PCWSTR pszPath, PCWSTR pszArg1, PCWSTR pszArg2, PCWSTR pszArg3)
{
    SetBinaryPathName(pszPath, pszArg1, pszArg2);
    if (wcschr(pszArg3, L' '))
    {
        m_pszBinaryPathName = String::Format(L"%s \"%s\"", m_pszBinaryPathName, pszArg3);
    }
    else
    {
        m_pszBinaryPathName = String::Format(L"%s %s", m_pszBinaryPathName, pszArg3);
    }
    return *this;
}


ServiceConfiguration& ServiceConfiguration::SetLoadOrderGroup(PCWSTR psz)
{
    m_pszLoadOrderGroup = String::Copy(psz);
    return *this;
}


ServiceConfiguration& ServiceConfiguration::AddDependency(PCWSTR psz)
{
    m_dependencies.Add(psz);
    return *this;
}


ServiceConfiguration& ServiceConfiguration::SetServiceStartName(PCWSTR psz)
{
    m_pszServiceStartName = String::Copy(psz);
    return *this;
}


ServiceConfiguration& ServiceConfiguration::SetPassword(PCWSTR psz)
{
    m_pszPassword = String::Copy(psz);
    return *this;
}


////////////////////////////////////////////////////////////////////////////
////
////  S E R V I C E
////
////////////////////////////////////////////////////////////////////////////


Service::Service()
    : m_h(NULL)
    , m_pszName(NULL)
    , m_status()
{
    memset(&m_status, 0, sizeof(m_status));
}


Service::~Service()
{
    Close();
}


void Service::Create(SC_HANDLE hSCM, ServiceConfiguration& config)
{
    Close();

    PCWSTR pszBinaryPathName;
    WCHAR szFileName[MAX_PATH + 2] = { 0 };
    if (config.m_pszBinaryPathName)
    {
        pszBinaryPathName = config.m_pszBinaryPathName;
    }
    else
    {
        if (!GetModuleFileNameW(NULL, &szFileName[1], MAX_PATH))
        {
            throw Win32Exception(GetLastError(), L"Failed to get the module file name.");
        }
        if (wcschr(&szFileName[1], L' '))
        {
            size_t cch = wcslen(szFileName);
            szFileName[0] = L'\"';
            szFileName[cch + 0] = L'\"';
            szFileName[cch + 1] = L'\0';
            pszBinaryPathName = &szFileName[0];
        }
        else
        {
            pszBinaryPathName = &szFileName[1];
        }
    }

    m_h = CreateServiceW(hSCM,
        config.m_pszName,
        config.m_pszDisplayName,
        config.m_dwDesiredAccess,
        config.m_dwServiceType,
        config.m_dwStartType,
        config.m_dwErrorControl,
        pszBinaryPathName,
        config.m_pszLoadOrderGroup,
        NULL,
        config.m_dependencies,
        config.m_pszServiceStartName,
        config.m_pszPassword);
    if (!m_h)
    {
        throw Win32Exception(GetLastError(), L"Failed to create service \"%s\".", config.m_pszName);
    }

    m_pszName = config.m_pszName;

    if (config.m_pszDescription)
    {
        SERVICE_DESCRIPTIONW sd = { 0 };
        sd.lpDescription = const_cast<PWSTR>(config.m_pszDescription);
        if (!ChangeServiceConfig2W(m_h, SERVICE_CONFIG_DESCRIPTION, &sd))
        {
            throw Win32Exception(GetLastError(), L"Failed to set description of service \"%s\".", config.m_pszName);
        }
    }
}


void Service::Open(SC_HANDLE hSCM, PCWSTR pszName, DWORD dwDesiredAccess)
{
    Close();

    m_h = OpenServiceW(hSCM, pszName, dwDesiredAccess);
    if (!m_h)
    {
        throw Win32Exception(GetLastError(), L"Failed to open service \"%s\".", pszName);
    }

    m_pszName = String::Copy(pszName);
}


void Service::Close()
{
    if (m_h)
    {
        CloseServiceHandle(m_h);
        m_h = NULL;
        m_pszName = NULL;
        memset(&m_status, 0, sizeof(m_status));
    }
}


DWORD Service::QueryStatus()
{
    if (!QueryServiceStatus(m_h, &m_status))
    {
        throw Win32Exception(GetLastError(), L"Failed to query status of service \"%s\".", m_pszName);
    }
    return m_status.dwCurrentState;
}


void Service::Start(DWORD dwNumServiceArgs, PCWSTR* pServiceArgVectors)
{
    if (!StartServiceW(m_h, dwNumServiceArgs, pServiceArgVectors))
    {
        throw Win32Exception(GetLastError(), L"Failed to start service \"%s\".", m_pszName);
    }
}


void Service::Stop()
{
    if (!ControlService(m_h, SERVICE_CONTROL_STOP, &m_status))
    {
        throw Win32Exception(GetLastError(), L"Failed to stop service \"%s\".", m_pszName);
    }
}


void Service::Pause()
{
    if (!ControlService(m_h, SERVICE_CONTROL_PAUSE, &m_status))
    {
        throw Win32Exception(GetLastError(), L"Failed to pause service \"%s\".", m_pszName);
    }
}


void Service::Resume()
{
    if (!ControlService(m_h, SERVICE_CONTROL_CONTINUE, &m_status))
    {
        throw Win32Exception(GetLastError(), L"Failed to resume service \"%s\".", m_pszName);
    }
}


void Service::Interrogate()
{
    if (!ControlService(m_h, SERVICE_CONTROL_INTERROGATE, &m_status))
    {
        throw Win32Exception(GetLastError(), L"Failed to interrogate service \"%s\".", m_pszName);
    }
}


void Service::Delete()
{
    if (!DeleteService(m_h))
    {
        throw Win32Exception(GetLastError(), L"Failed to delete service \"%s\".", m_pszName);
    }
}


////////////////////////////////////////////////////////////////////////////
////
////  S E R V I C E   C O N T R O L   M A N A G E R
////
////////////////////////////////////////////////////////////////////////////


ServiceControlManager::ServiceControlManager()
    : m_h(NULL)
{
}


ServiceControlManager::~ServiceControlManager()
{
    Close();
}


void ServiceControlManager::Open(PCWSTR pszMachineName, PCWSTR pszDatabaseName, DWORD dwDesiredAccess)
{
    Close();

    m_h = OpenSCManagerW(pszMachineName, pszDatabaseName, dwDesiredAccess);
    if (!m_h)
    {
        throw Win32Exception(GetLastError(),
            L"Failed to open Service Control Manager. machine=%s database=%s access=%08lX",
            pszMachineName ? pszMachineName : L"NULL", pszDatabaseName ? pszDatabaseName : L"NULL", dwDesiredAccess);
    }
}


void ServiceControlManager::Close()
{
    if (m_h)
    {
        CloseServiceHandle(m_h);
        m_h = NULL;
    }
}
