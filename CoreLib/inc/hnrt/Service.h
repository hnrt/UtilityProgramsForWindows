#pragma once


#include "hnrt/DoubleNullTerminatedStringArray.h"


namespace hnrt
{
    class ServiceConfiguration
    {
    public:

        ServiceConfiguration();
        ServiceConfiguration(const ServiceConfiguration&);
        ~ServiceConfiguration() = default;
        ServiceConfiguration& operator =(const ServiceConfiguration&);
        ServiceConfiguration& SetName(PCWSTR);
        ServiceConfiguration& SetDisplayName(PCWSTR);
        ServiceConfiguration& SetDescription(PCWSTR);
        ServiceConfiguration& SetDesiredAccess(DWORD);
        ServiceConfiguration& SetServiceType(DWORD);
        ServiceConfiguration& SetStartType(DWORD);
        ServiceConfiguration& SetErrorControl(DWORD);
        ServiceConfiguration& SetBinaryPathName(PCWSTR);
        ServiceConfiguration& SetLoadOrderGroup(PCWSTR);
        ServiceConfiguration& AddDependency(PCWSTR);
        ServiceConfiguration& SetServiceStartName(PCWSTR);
        ServiceConfiguration& SetPassword(PCWSTR);

    private:

        friend class Service;

        PCWSTR m_pszName;
        PCWSTR m_pszDisplayName;
        PCWSTR m_pszDescription;
        DWORD m_dwDesiredAccess;
        DWORD m_dwServiceType;
        DWORD m_dwStartType;
        DWORD m_dwErrorControl;
        PCWSTR m_pszBinaryPathName;
        PCWSTR m_pszLoadOrderGroup;
        DoubleNullTerminatedStringArray m_dependencies;
        PCWSTR m_pszServiceStartName;
        PCWSTR m_pszPassword;
    };

    class Service
    {
    public:

        Service();
        Service(const Service&) = delete;
        ~Service();
        Service& operator =(const Service&) = delete;
        operator SC_HANDLE() const;
        void Create(SC_HANDLE hSCM, ServiceConfiguration& config);
        void Open(SC_HANDLE hSCM, PCWSTR pszName, DWORD dwDesiredAccess = SERVICE_ALL_ACCESS);
        void Close();
        void Start(DWORD dwNumServiceArgs = 0UL, PCWSTR* pServiceArgVectors = NULL);
        void Stop();
        void Pause();
        void Resume();
        void Interrogate();
        void Delete();
        DWORD get_ServiceType() const;
        DWORD get_CurrentState() const;
        DWORD get_ControlsAccepted() const;
        DWORD get_Win32ExitCode() const;
        DWORD get_ServiceSpecificExitCode() const;
        DWORD get_CheckPoint() const;
        DWORD get_WaitHint() const;

        __declspec(property(get = get_ServiceType)) DWORD ServiceType;
        __declspec(property(get = get_CurrentState)) DWORD CurrentState;
        __declspec(property(get = get_ControlsAccepted)) DWORD ControlsAccepted;
        __declspec(property(get = get_Win32ExitCode)) DWORD Win32ExitCode;
        __declspec(property(get = get_ServiceSpecificExitCode)) DWORD ServiceSpecificExitCode;
        __declspec(property(get = get_CheckPoint)) DWORD CheckPoint;
        __declspec(property(get = get_WaitHint)) DWORD WaitHint;

    private:

        SC_HANDLE m_h;
        PCWSTR m_pszName;
        SERVICE_STATUS m_status;
    };

    class ServiceControlManager
    {
    public:

        ServiceControlManager();
        ServiceControlManager(const ServiceControlManager&) = delete;
        ~ServiceControlManager();
        ServiceControlManager& operator =(const ServiceControlManager&) = delete;
        void Open(PCWSTR pszMachineName = NULL, PCWSTR pszDatabaseName = NULL, DWORD dwDesiredAccess = SC_MANAGER_ALL_ACCESS);
        void Close();
        operator SC_HANDLE() const;

    private:

        SC_HANDLE m_h;
    };

    inline Service::operator SC_HANDLE() const
    {
        return m_h;
    }

    inline DWORD Service::get_ServiceType() const
    {
        return m_status.dwServiceType;
    }

    inline DWORD Service::get_CurrentState() const
    {
        return m_status.dwCurrentState;
    }

    inline DWORD Service::get_ControlsAccepted() const
    {
        return m_status.dwControlsAccepted;
    }

    inline DWORD Service::get_Win32ExitCode() const
    {
        return m_status.dwWin32ExitCode;
    }

    inline DWORD Service::get_ServiceSpecificExitCode() const
    {
        return m_status.dwServiceSpecificExitCode;
    }

    inline DWORD Service::get_CheckPoint() const
    {
        return m_status.dwCheckPoint;
    }

    inline DWORD Service::get_WaitHint() const
    {
        return m_status.dwWaitHint;
    }

    inline ServiceControlManager::operator SC_HANDLE() const
    {
        return m_h;
    }
}
