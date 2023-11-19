#pragma once


#define _WIN32_DCOM
#include <winsock2.h>
#include <Windows.h>
#include <wbemidl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hnrt/ComLibrary.h"
#include "hnrt/Args.h"
#include "hnrt/RefPtr.h"
#include "hnrt/String.h"
#include "hnrt/AdapterAddressCollection.h"
#include "hnrt/Buffer.h"
#include "hnrt/KeyValueMap.h"
#include "Hosts.h"


namespace hnrt
{
    enum class VmIpAddressFinderCommandType
    {
        None,
        HostsPrint,
        HostsUpdate,
        ServiceInstall,
        ServiceUninstall,
        ServiceStart,
        ServiceStop,
        ServiceRun
    };

    class VmIpAddressFinder
        : public ComLibrary
    {
    public:

        static VmIpAddressFinder& Instance();
        static void EnableServiceMode();
        static void OpenStream();
        static void __cdecl CloseStream();
        static void Put(PCWSTR pszFormat, ...);
        static void PutError(PCWSTR pszFormat, ...);
        static void VaPut(FILE* fp, PCWSTR pszFormat, va_list argList);

        VmIpAddressFinder(int argc, wchar_t* argv[]);
        VmIpAddressFinder(const VmIpAddressFinder&) = delete;
        ~VmIpAddressFinder();
        void ParseCommandLine();
        void Run();
        void PrintAddresses();
        void UpdateHosts();
        void InstallService() const;
        void UninstallService() const;
        #undef StartService
        void StartService() const;
        void StopService() const;
        void RunService();

    private:

        void Help();

        void FindAddresses(KeyValueMap& nameAddressMap);
        void ConnectServer(IWbemLocator* pLocator, PCWSTR pszNamespace, IWbemServices** ppServices);
        void ExecQuery(IWbemServices* pServices, PCWSTR pszQuery, KeyValueMapList& list);
        void EnumClassObject(IWbemClassObject* pClass, long lEnumFlags, KeyValueMapList& list);

        static void DebugPut(const AdapterAddressCollection& aa);
        static void DebugPut(const KeyValueMapList& list, PCWSTR pszHeader);
        static void DebugPut(const KeyValueMap& map, PCWSTR pszHeader);

        static bool ExamineHosts(Hosts& hosts, KeyValueMap& nameAddressMap);
        static void RecreateHostsData(const Hosts& hosts, const KeyValueMap& nameAddressMap, Buffer<char>& hostsData);

        static String GetServiceCommandPath();
        static VOID WINAPI ServiceMain(DWORD dwArgc, PWSTR* pszArgv);
        static DWORD WINAPI HandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);
        void ServiceMain(Args& args);
        DWORD ControlService(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData);
        void SetServiceStatus(DWORD dwState);

        Args m_args;
        String m_szName;
        long m_debugLevel;
        VmIpAddressFinderCommandType m_command;
        bool m_bSecurityInitialized;
        String m_szServiceName;
        SERVICE_STATUS_HANDLE m_hServiceStatus;
        DWORD m_dwCurrentState;
        HANDLE m_hEvent;
    };
}
