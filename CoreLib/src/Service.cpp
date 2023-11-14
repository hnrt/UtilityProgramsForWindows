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
    : m_Name()
    , m_DisplayName()
    , m_Description()
    , m_dwDesiredAccess(GENERIC_ALL)
    , m_dwServiceType(SERVICE_WIN32_OWN_PROCESS)
    , m_dwStartType(SERVICE_AUTO_START)
    , m_dwErrorControl(SERVICE_ERROR_CRITICAL)
    , m_BinaryPathName()
    , m_LoadOrderGroup()
    , m_dependencies()
    , m_ServiceStartName()
    , m_Password(L"")
{
}


ServiceConfiguration::ServiceConfiguration(const ServiceConfiguration& src)
    : m_Name()
    , m_DisplayName()
    , m_Description()
    , m_dwDesiredAccess(GENERIC_ALL)
    , m_dwServiceType(SERVICE_WIN32_OWN_PROCESS)
    , m_dwStartType(SERVICE_AUTO_START)
    , m_dwErrorControl(SERVICE_ERROR_CRITICAL)
    , m_BinaryPathName()
    , m_LoadOrderGroup()
    , m_dependencies()
    , m_ServiceStartName()
    , m_Password(L"")
{
    operator =(src);
}


ServiceConfiguration& ServiceConfiguration::operator =(const ServiceConfiguration& src)
{
    m_Name = src.m_Name;
    m_DisplayName = src.m_DisplayName;
    m_Description = src.m_Description;
    m_dwDesiredAccess = src.m_dwDesiredAccess;
    m_dwServiceType = src.m_dwServiceType;
    m_dwStartType = src.m_dwStartType;
    m_dwErrorControl = src.m_dwErrorControl;
    m_BinaryPathName = src.m_BinaryPathName;
    m_LoadOrderGroup = src.m_LoadOrderGroup;
    m_dependencies = src.m_dependencies;
    m_ServiceStartName = src.m_ServiceStartName;
    m_Password = src.m_Password;
    return *this;
}


ServiceConfiguration& ServiceConfiguration::SetName(PCWSTR psz)
{
    m_Name = psz;
    return *this;
}


ServiceConfiguration& ServiceConfiguration::SetDisplayName(PCWSTR psz)
{
    m_DisplayName = psz;
    return *this;
}


ServiceConfiguration& ServiceConfiguration::SetDescription(PCWSTR psz)
{
    m_Description = psz;
    return *this;
}


ServiceConfiguration& ServiceConfiguration::SetDescription(UINT uId)
{
    WCHAR szPath[MAX_PATH];
    if (GetModuleFileNameW(NULL, szPath, MAX_PATH))
    {
        m_Description = String::Format2(L"@%s,-%u", szPath, uId);
    }
    else
    {
        m_Description = ResourceString(uId).Ptr;
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
        m_BinaryPathName = String::Format2(L"\"%s\"", pszPath);
    }
    else
    {
        m_BinaryPathName = pszPath;
    }
    return *this;
}


ServiceConfiguration& ServiceConfiguration::SetBinaryPathName(PCWSTR pszPath, PCWSTR pszArg1)
{
    SetBinaryPathName(pszPath);
    if (wcschr(pszArg1, L' '))
    {
        m_BinaryPathName = String::Format2(L"%s \"%s\"", m_BinaryPathName.Str, pszArg1);
    }
    else
    {
        m_BinaryPathName = String::Format2(L"%s %s", m_BinaryPathName.Str, pszArg1);
    }
    return *this;
}


ServiceConfiguration& ServiceConfiguration::SetBinaryPathName(PCWSTR pszPath, PCWSTR pszArg1, PCWSTR pszArg2)
{
    SetBinaryPathName(pszPath, pszArg1);
    if (wcschr(pszArg2, L' '))
    {
        m_BinaryPathName = String::Format2(L"%s \"%s\"", m_BinaryPathName.Str, pszArg2);
    }
    else
    {
        m_BinaryPathName = String::Format2(L"%s %s", m_BinaryPathName.Str, pszArg2);
    }
    return *this;
}


ServiceConfiguration& ServiceConfiguration::SetBinaryPathName(PCWSTR pszPath, PCWSTR pszArg1, PCWSTR pszArg2, PCWSTR pszArg3)
{
    SetBinaryPathName(pszPath, pszArg1, pszArg2);
    if (wcschr(pszArg3, L' '))
    {
        m_BinaryPathName = String::Format2(L"%s \"%s\"", m_BinaryPathName.Str, pszArg3);
    }
    else
    {
        m_BinaryPathName = String::Format2(L"%s %s", m_BinaryPathName.Str, pszArg3);
    }
    return *this;
}


ServiceConfiguration& ServiceConfiguration::SetLoadOrderGroup(PCWSTR psz)
{
    m_LoadOrderGroup = psz;
    return *this;
}


ServiceConfiguration& ServiceConfiguration::AddDependency(PCWSTR psz)
{
    m_dependencies.Add(psz);
    return *this;
}


ServiceConfiguration& ServiceConfiguration::SetServiceStartName(PCWSTR psz)
{
    m_ServiceStartName = psz;
    return *this;
}


ServiceConfiguration& ServiceConfiguration::SetPassword(PCWSTR psz)
{
    m_Password = psz;
    return *this;
}


////////////////////////////////////////////////////////////////////////////
////
////  S E R V I C E
////
////////////////////////////////////////////////////////////////////////////


Service::Service()
    : m_h(NULL)
    , m_Name()
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
    if (config.m_BinaryPathName)
    {
        pszBinaryPathName = config.m_BinaryPathName;
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
        config.m_Name,
        config.m_DisplayName,
        config.m_dwDesiredAccess,
        config.m_dwServiceType,
        config.m_dwStartType,
        config.m_dwErrorControl,
        pszBinaryPathName,
        config.m_LoadOrderGroup,
        NULL,
        config.m_dependencies,
        config.m_ServiceStartName,
        config.m_Password);
    if (!m_h)
    {
        throw Win32Exception(GetLastError(), L"Failed to create service \"%s\".", config.m_Name.Str);
    }

    m_Name = config.m_Name;

    if (config.m_Description)
    {
        SERVICE_DESCRIPTIONW sd = { 0 };
        sd.lpDescription = const_cast<PWSTR>(config.m_Description.Str);
        if (!ChangeServiceConfig2W(m_h, SERVICE_CONFIG_DESCRIPTION, &sd))
        {
            throw Win32Exception(GetLastError(), L"Failed to set description of service \"%s\".", config.m_Name.Str);
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

    m_Name = pszName;
}


void Service::Close()
{
    if (m_h)
    {
        CloseServiceHandle(m_h);
        m_h = NULL;
        m_Name = NULL;
        memset(&m_status, 0, sizeof(m_status));
    }
}


DWORD Service::QueryStatus()
{
    if (!QueryServiceStatus(m_h, &m_status))
    {
        throw Win32Exception(GetLastError(), L"Failed to query status of service \"%s\".", m_Name.Str);
    }
    return m_status.dwCurrentState;
}


void Service::Start(DWORD dwNumServiceArgs, PCWSTR* pServiceArgVectors)
{
    if (!StartServiceW(m_h, dwNumServiceArgs, pServiceArgVectors))
    {
        throw Win32Exception(GetLastError(), L"Failed to start service \"%s\".", m_Name.Str);
    }
}


void Service::Stop()
{
    if (!ControlService(m_h, SERVICE_CONTROL_STOP, &m_status))
    {
        throw Win32Exception(GetLastError(), L"Failed to stop service \"%s\".", m_Name.Str);
    }
}


void Service::Pause()
{
    if (!ControlService(m_h, SERVICE_CONTROL_PAUSE, &m_status))
    {
        throw Win32Exception(GetLastError(), L"Failed to pause service \"%s\".", m_Name.Str);
    }
}


void Service::Resume()
{
    if (!ControlService(m_h, SERVICE_CONTROL_CONTINUE, &m_status))
    {
        throw Win32Exception(GetLastError(), L"Failed to resume service \"%s\".", m_Name.Str);
    }
}


void Service::Interrogate()
{
    if (!ControlService(m_h, SERVICE_CONTROL_INTERROGATE, &m_status))
    {
        throw Win32Exception(GetLastError(), L"Failed to interrogate service \"%s\".", m_Name.Str);
    }
}


void Service::Delete()
{
    if (!DeleteService(m_h))
    {
        throw Win32Exception(GetLastError(), L"Failed to delete service \"%s\".", m_Name.Str);
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
