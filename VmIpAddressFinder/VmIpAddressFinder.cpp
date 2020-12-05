#include "VmIpAddressFinder.h"
#include "GuestIntrinsicExchangeItems.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <stdexcept>
#include <list>
#include "hnrt/Path.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/StringCollection.h"
#include "hnrt/FileMapper.h"
#include "hnrt/FileWriter.h"
#include "hnrt/ComException.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/Debug.h"


#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "Core.lib")


using namespace hnrt;


int wmain(int argc, wchar_t* argv[])
{
    DBGFNC(L"wmain");

    _wsetlocale(LC_ALL, L"");

    try
    {
        VmIpAddressFinder app(argc, argv);
        app.ParseCommandLine();
        app.Run();
        return EXIT_SUCCESS;
    }
    catch (int exitCode)
    {
        return exitCode;
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
    }
    catch (std::exception e)
    {
        VmIpAddressFinder::PutError(L"ERROR: %hs", e.what());
    }
    catch (...)
    {
        VmIpAddressFinder::PutError(L"ERROR: Unhandled exception caught.");
    }
    return EXIT_FAILURE;
}


static VmIpAddressFinder* s_pSingleton;


VmIpAddressFinder& VmIpAddressFinder::Instance()
{
    return *s_pSingleton;
}


VmIpAddressFinder::VmIpAddressFinder(int argc, wchar_t* argv[])
    : ComLibrary(COINIT_MULTITHREADED)
    , m_args(argc, argv)
    , m_pszName(Path::GetFileNameWithoutExtension(m_args.CommandPath))
    , m_debugLevel(0)
    , m_command(VmIpAddressFinderCommandType::None)
    , m_bSecurityInitialized(false)
    , m_pszServiceName(L"VmIpAddressUpdater")
    , m_hServiceStatus(nullptr)
    , m_dwCurrentState(SERVICE_STOPPED)
    , m_hEvent(nullptr)
{
    s_pSingleton = this;
}


VmIpAddressFinder::~VmIpAddressFinder()
{
    if (m_hEvent)
    {
        CloseHandle(m_hEvent);
    }
}


void VmIpAddressFinder::ParseCommandLine()
{
    DBGFNC(L"VmIpAddressFinder::ParseCommandLine");

    while (m_args.Exists())
    {
        PCWSTR psz = *m_args;
        if (!String::Compare(psz, L"-help") || !String::Compare(psz, L"-h") || !String::Compare(psz, L"-?"))
        {
            Help();
            throw EXIT_SUCCESS;
        }
        else if (!String::Compare(psz, L"-debug") || !String::Compare(psz, L"-D"))
        {
            m_debugLevel++;
            m_args++;
        }
        else if (!String::Compare(psz, L"-print") || !String::Compare(psz, L"-p"))
        {
            if (m_command != VmIpAddressFinderCommandType::None)
            {
                goto failure;
            }
            m_command = VmIpAddressFinderCommandType::HostsPrint;
            m_args++;
        }
        else if (!String::Compare(psz, L"-update") || !String::Compare(psz, L"-u"))
        {
            if (m_command != VmIpAddressFinderCommandType::None)
            {
                goto failure;
            }
            m_command = VmIpAddressFinderCommandType::HostsUpdate;
            m_args++;
        }
        else if (!String::Compare(psz, L"-install"))
        {
            if (m_command != VmIpAddressFinderCommandType::None)
            {
                goto failure;
            }
            m_command = VmIpAddressFinderCommandType::ServiceInstall;
            m_args++;
        }
        else if (!String::Compare(psz, L"-uninstall"))
        {
            if (m_command != VmIpAddressFinderCommandType::None)
            {
                goto failure;
            }
            m_command = VmIpAddressFinderCommandType::ServiceUninstall;
            m_args++;
        }
        else if (!String::Compare(psz, L"-start"))
        {
            if (m_command != VmIpAddressFinderCommandType::None)
            {
                goto failure;
            }
            m_command = VmIpAddressFinderCommandType::ServiceStart;
            m_args++;
        }
        else if (!String::Compare(psz, L"-stop"))
        {
            if (m_command != VmIpAddressFinderCommandType::None)
            {
                goto failure;
            }
            m_command = VmIpAddressFinderCommandType::ServiceStop;
            m_args++;
        }
        else if (!String::Compare(psz, L"-run"))
        {
            if (m_command != VmIpAddressFinderCommandType::None)
            {
                goto failure;
            }
            m_command = VmIpAddressFinderCommandType::ServiceRun;
            m_args++;
        }
        else
        {
        failure:
            throw Exception(L"Bad command line syntax. Please try -help for usage. Thank you.");
        }
    }

    if (m_command == VmIpAddressFinderCommandType::None)
    {
        Help();
        throw EXIT_SUCCESS;
    }
}


void VmIpAddressFinder::Help()
{
    Put(L"Usage:");
    Put(L"  %s -print", m_pszName);
    Put(L"  %s -update", m_pszName);
    Put(L"  %s -install", m_pszName);
    Put(L"  %s -uninstall", m_pszName);
    Put(L"  %s -start", m_pszName);
    Put(L"  %s -stop", m_pszName);
    Put(L"  %s -debug", m_pszName);
    Put(L"  %s -help", m_pszName);
}


void VmIpAddressFinder::Run()
{
    DBGFNC(L"VmIpAddressFinder::Run");

    switch (m_command)
    {
    case VmIpAddressFinderCommandType::HostsPrint:
        PrintAddresses();
        break;
    case VmIpAddressFinderCommandType::HostsUpdate:
        UpdateHosts();
        break;
    case VmIpAddressFinderCommandType::ServiceInstall:
        InstallService();
        break;
    case VmIpAddressFinderCommandType::ServiceUninstall:
        UninstallService();
        break;
    case VmIpAddressFinderCommandType::ServiceStart:
        StartService();
        break;
    case VmIpAddressFinderCommandType::ServiceStop:
        StopService();
        break;
    case VmIpAddressFinderCommandType::ServiceRun:
        RunService();
        break;
    default:
        break;
    }
}


void VmIpAddressFinder::PrintAddresses()
{
    DBGFNC(L"VmIpAddressFinder::PrintAddresses");

    KeyValueMap nameAddressMap;
    FindAddresses(nameAddressMap);

    for (KeyValueMap::ConstIterator iter = nameAddressMap.Begin(); iter != nameAddressMap.End(); iter++)
    {
        Put(L"%s\t%s", iter->second, iter->first);
    }
}


void VmIpAddressFinder::UpdateHosts()
{
    DBGFNC(L"VmIpAddressFinder::UpdateHosts");

    KeyValueMap nameAddressMap;
    FindAddresses(nameAddressMap);

    PCWSTR pszHostsPath = Path::Combine(Path::GetKnownFolder(FOLDERID_System), L"drivers", L"etc", L"hosts");
    FileMapper hostsMapper(pszHostsPath);
    Hosts hosts(hostsMapper.Ptr, static_cast<size_t>(hostsMapper.Len));
    hostsMapper.Close();

    bool bChanged = ExamineHosts(hosts, nameAddressMap);

    if (bChanged || nameAddressMap.Size())
    {
        Buffer<char> hostsData;
        RecreateHostsData(hosts, nameAddressMap, hostsData);

        PCWSTR pszHostsPath2 = String::Format(L"%s.new", pszHostsPath);
        FileWriter hostsFile2(pszHostsPath2);
        hostsFile2.Write(hostsData.Ptr, hostsData.Len);
        hostsFile2.Close();

        SYSTEMTIME t;
        GetLocalTime(&t);
        PCWSTR pszHostsPath3 = String::Format(L"%s.%u%02u%02uT%02u%02u%02u", pszHostsPath, t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

        if (!MoveFileW(pszHostsPath, pszHostsPath3))
        {
            DWORD dwError = GetLastError();
            throw Win32Exception(dwError, L"Failed to rename \"%s\" to \"%s\".", Path::GetFileName(pszHostsPath), Path::GetFileName(pszHostsPath3));
        }
        if (!MoveFileW(pszHostsPath2, pszHostsPath))
        {
            DWORD dwError = GetLastError();
            throw Win32Exception(dwError, L"Failed to rename \"%s\" to \"%s\".", Path::GetFileName(pszHostsPath2), Path::GetFileName(pszHostsPath));
        }

        Put(L"%s: Updated. (Backup: %s)", pszHostsPath, Path::GetFileName(pszHostsPath3));
    }
    else
    {
        Put(L"%s: No need to update.", pszHostsPath);
    }
}


void VmIpAddressFinder::FindAddresses(KeyValueMap& nameAddressMap)
{
    HRESULT hr;

    if (!m_bSecurityInitialized)
    {
        hr = CoInitializeSecurity(
            NULL,                        // Security descriptor    
            -1,                          // COM negotiates authentication service
            NULL,                        // Authentication services
            NULL,                        // Reserved
            RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication level for proxies
            RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation level for proxies
            NULL,                        // Authentication info
            EOAC_NONE,                   // Additional capabilities of the client or server
            NULL);                       // Reserved
        if (FAILED(hr))
        {
            throw ComException(hr, L"Failed to initialize security.");
        }
        m_bSecurityInitialized = true;
    }

    RefPtr<IWbemLocator> pLocator;
    hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLocator);
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to create IWbemLocator object.");
    }

    AdapterAddressCollection adapterAddresses(AF_INET);
    if (m_debugLevel)
    {
        DebugPut(adapterAddresses);
    }

    RefPtr<IWbemServices> pServicesVirtualization;
    ConnectServer(pLocator.Ptr, L"root\\virtualization\\v2", &pServicesVirtualization);

    KeyValueMapList computerSystems;
    ExecQuery(pServicesVirtualization.Ptr, L"Select * From Msvm_ComputerSystem", computerSystems);

    if (m_debugLevel)
    {
        DebugPut(computerSystems, L"ComputerSystem");
    }

    KeyValueMapList exchangeComponents;
    ExecQuery(pServicesVirtualization.Ptr, L"Select * From Msvm_KvpExchangeComponent", exchangeComponents);

    if (m_debugLevel)
    {
        DebugPut(exchangeComponents, L"ExchangeComponent");
    }

    for (KeyValueMapList::ConstIterator iter = computerSystems.Begin(); iter != computerSystems.End(); iter++)
    {
        KeyValueMap& computerSystemMap = **iter;
        PCWSTR pszName = computerSystemMap.GetValue(L"Name");
        if (!pszName)
        {
            continue;
        }
        KeyValueMap* pExchangeComponentMap = exchangeComponents.GetMap(L"SystemName", pszName);
        if (!pExchangeComponentMap)
        {
            continue;
        }
        PCWSTR pszElementName = computerSystemMap.GetValue(L"ElementName");
        GuestIntrinsicExchangeItems guestIntrinsicExchangeItems(*pExchangeComponentMap);
        if (m_debugLevel)
        {
            DebugPut(guestIntrinsicExchangeItems, L"GuestIntrinsicExchangeItems");
        }
        StringCollection addresses;
        addresses.Split(guestIntrinsicExchangeItems.NetworkAddressIPv4, L';');
        for (size_t index = 0; index < addresses.Count; index++)
        {
            PCWSTR pszAddr = addresses[index];
            if (adapterAddresses.FindSameSubnetIPv4(pszAddr))
            {
                nameAddressMap.Add(guestIntrinsicExchangeItems.FullyQualifiedDomainName, pszAddr);
                break;
            }
        }
    }
}


void VmIpAddressFinder::ConnectServer(IWbemLocator* pLocator, PCWSTR pszNamespace, IWbemServices** ppServices)
{
    HRESULT hr = pLocator->ConnectServer(
        const_cast<BSTR>(pszNamespace),  //namespace
        NULL,       // User name 
        NULL,       // User password
        0,         // Locale 
        NULL,     // Security flags
        0,         // Authority 
        0,        // Context object 
        ppServices);   // IWbemServices proxy
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to create IWbemServices object for namespace \"%s\".", pszNamespace);
    }

    // Set the proxy so that impersonation of the client occurs.
    hr = CoSetProxyBlanket(*ppServices,
        RPC_C_AUTHN_WINNT,
        RPC_C_AUTHZ_NONE,
        NULL,
        RPC_C_AUTHN_LEVEL_CALL,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE);
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to set proxy blanket.");
    }
}


void VmIpAddressFinder::ExecQuery(IWbemServices* pServices, PCWSTR pszQuery, KeyValueMapList& list)
{
    RefPtr<IEnumWbemClassObject> pEnum;
    HRESULT hr = pServices->ExecQuery(
        const_cast<PWSTR>(L"WQL"),
        const_cast<PWSTR>(pszQuery),
        WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnum);
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to execute query \"%s\".", pszQuery);
    }
    while (true)
    {
        RefPtr<IWbemClassObject> pClass;
        ULONG uReturned = 0;
        hr = pEnum->Next(WBEM_INFINITE, 1, &pClass, &uReturned);
        if (hr == WBEM_S_NO_ERROR)
        {
            EnumClassObject(pClass.Ptr, 0L, list);
        }
        else if (hr == WBEM_S_FALSE)
        {
            break;
        }
        else
        {
            throw ComException(hr, L"Failed to fetch an item from query \"%s\".", pszQuery);
        }
    }
}


void VmIpAddressFinder::EnumClassObject(IWbemClassObject* pClass, long lEnumFlags, KeyValueMapList& list)
{
    KeyValueMap* pMap = list.Add();
    HRESULT hr = pClass->BeginEnumeration(lEnumFlags);
    while (hr == S_OK)
    {
        BSTR strName = nullptr;
        VARIANT varItem;
        VariantInit(&varItem);
        CIMTYPE cimType = -1L;
        hr = pClass->Next(0, &strName, &varItem, &cimType, nullptr);
        if (hr == S_OK)
        {
            switch (varItem.vt)
            {
            case VT_NULL:
                pMap->Add(String::Copy(strName), nullptr);
                break;

            case VT_I4:
                pMap->Add(String::Copy(strName), String::Format(L"%ld", varItem.lVal));
                break;

            case VT_BOOL:
                if (varItem.boolVal == -1)
                {
                    pMap->Add(String::Copy(strName), L"TRUE");
                }
                else if (varItem.boolVal == 0)
                {
                    pMap->Add(String::Copy(strName), L"FALSE");
                }
                else
                {
                    pMap->Add(String::Copy(strName), String::Format(L"%d", varItem.boolVal));
                }
                break;

            case VT_BSTR:
                pMap->Add(String::Copy(strName), String::Format(L"%s", varItem.bstrVal));
                break;

            case (VT_ARRAY | VT_I4):
            case (VT_ARRAY | VT_BSTR):
            {
                SAFEARRAY* parray = varItem.parray;
                if (parray->cDims == 1 && parray->rgsabound[0].lLbound == 0L)
                {
                    pMap->Add(String::Format(L"%s.Count", strName), String::Format(L"%lu", parray->rgsabound[0].cElements));
                    for (ULONG i = 0; i < parray->rgsabound[0].cElements; i++)
                    {
                        PCWSTR key = String::Format(L"%s[%lu]", strName, i);
                        PCWSTR value;
                        switch ((varItem.vt & ~VT_ARRAY))
                        {
                        case VT_I4:
                            value = String::Format(L"%ld", reinterpret_cast<LONG*>(parray->pvData)[i]);
                            break;
                        case VT_BSTR:
                            value = String::Format(L"%s", reinterpret_cast<BSTR*>(parray->pvData)[i]);
                            break;
                        default:
                            throw Exception(L"VmIpAddressFinder::EnumClassObject: Unexpected.");
                        }
                        pMap->Add(key, value);
                    }
                }
                else if (parray->cDims == 2 && parray->rgsabound[0].lLbound == 0L)
                {
                    pMap->Add(
                        String::Format(L"%s.Count", strName),
                        String::Format(L"%lu,%lu",
                            parray->rgsabound[0].cElements,
                            parray->rgsabound[1].cElements));
                    for (ULONG i = 0; i < parray->rgsabound[0].cElements; i++)
                    {
                        for (ULONG j = 0; i < parray->rgsabound[1].cElements; i++)
                        {
                            PCWSTR key = String::Format(L"%s[%lu][%lu]", strName, i, j);
                            PCWSTR value;
                            switch ((varItem.vt & ~VT_ARRAY))
                            {
                            case VT_I4:
                                value = String::Format(L"%ld", reinterpret_cast<LONG**>(parray->pvData)[i][j]);
                                break;
                            case VT_BSTR:
                                value = String::Format(L"%s", reinterpret_cast<BSTR**>(parray->pvData)[i][j]);
                                break;
                            default:
                                throw Exception(L"VmIpAddressFinder::EnumClassObject: Unexpected.");
                            }
                            pMap->Add(key, value);
                        }
                    }
                }
                else
                {
                    Put(L"#***NOT_SUPPORTED***");
                    Put(L"#%s:", strName);
                    Put(L"#|dims=%u", parray->cDims);
                    Put(L"#|features=%u", parray->fFeatures);
                    Put(L"#|elements=%lu", parray->cbElements);
                    for (USHORT i = 0; i < parray->cDims; i++)
                    {
                        Put(L"#|[%u]Lbound=%ld,elements=%lu", i, parray->rgsabound[i].lLbound, parray->rgsabound[i].cElements);
                    }
                }
                break;
            }

            default:
                Put(L"#%s (type=%u)", strName, varItem.vt);
                pMap->Add(String::Copy(strName), String::Format(L"?(type=%u)", varItem.vt));
                break;
            }
        }
        SysFreeString(strName);
        VariantClear(&varItem);
    }
    hr = pClass->EndEnumeration();
}


void VmIpAddressFinder::DebugPut(const AdapterAddressCollection& aa)
{
    Put(L"#AdapterAddressCollection:");
    for (ULONG i = 0; i < aa.Count; i++)
    {
        const IP_ADAPTER_ADDRESSES* p = aa[i];
        Put(L"#  %hs", p->AdapterName);
        PIP_ADAPTER_UNICAST_ADDRESS_LH pUA = p->FirstUnicastAddress;
        while (pUA)
        {
            if (pUA->Address.lpSockaddr->sa_family == AF_INET)
            {
                struct sockaddr_in* pv4 = reinterpret_cast<struct sockaddr_in*>(pUA->Address.lpSockaddr);
                Put(L"#    %u.%u.%u.%u OnLinkPrefixLength=%u mask=%08lX",
                    pv4->sin_addr.S_un.S_un_b.s_b1, pv4->sin_addr.S_un.S_un_b.s_b2, pv4->sin_addr.S_un.S_un_b.s_b3, pv4->sin_addr.S_un.S_un_b.s_b4,
                    pUA->OnLinkPrefixLength,
                    AdapterUnicastAddress::GetSubnetMask(pUA));
            }
            pUA = pUA->Next;
        }
    }
}


void VmIpAddressFinder::DebugPut(const KeyValueMapList& list, PCWSTR pszHeader)
{
    for (KeyValueMapList::ConstIterator iter = list.Begin(); iter != list.End(); iter++)
    {
        KeyValueMap* pMap = *iter;
        DebugPut(*pMap, pszHeader);
    }
}


void VmIpAddressFinder::DebugPut(const KeyValueMap& map, PCWSTR pszHeader)
{
    Put(L"#%s:", pszHeader);
    for (KeyValueMap::ConstIterator iter = map.Begin(); iter != map.End(); iter++)
    {
        PCWSTR pszKey = iter->first;
        PCWSTR pszValue = iter->second;
        Put(L"#  %s=%s", pszKey, pszValue ? pszValue : L"(null)");
    }
}


bool VmIpAddressFinder::ExamineHosts(Hosts& hosts, KeyValueMap& nameAddressMap)
{
    bool bChanged = false;
    for (std::list<HostsNode*>::iterator iter = hosts.List.begin(); iter != hosts.List.end(); iter++)
    {
        HostsNode* pAddrNode = *iter;
        HostsNode* pHostNode = pAddrNode->Host;
        while (pHostNode)
        {
            PCWSTR pszKey = String::ToUcs(pHostNode->Text);
            PCWSTR pszValue = nameAddressMap.GetValue(pszKey);
            if (pszValue)
            {
                if (!String::Compare(pszValue, String::ToUcs(pAddrNode->Text)))
                {
                    // Address not changed -- Do nothing
                }
                else
                {
                    // Address changed -- Overwrite it with the new one.
                    pAddrNode->Text = String::ToAcp(pszValue);
                    bChanged = true;
                }
                nameAddressMap.Remove(pszKey);
            }
            pHostNode = pHostNode->Host;
        }
    }
    return bChanged;
}


void VmIpAddressFinder::RecreateHostsData(const Hosts& hosts, const KeyValueMap& nameAddressMap, Buffer<char>& hostsData)
{
    StringBuffer buf(8192);
    HostsNode* pNode = hosts.Root;
    while (pNode->Type != HostsNodeType::End)
    {
        buf.AppendFormat(L"%hs", pNode->Text);
        pNode = pNode->Next;
    }
    if (buf.Len && buf[buf.Len - 1] != L'\n')
    {
        buf.AppendFormat(L"\r\n");
    }
    for (KeyValueMap::ConstIterator iter = nameAddressMap.Begin(); iter != nameAddressMap.End(); iter++)
    {
        buf.AppendFormat(L"%s\t%s\r\n", iter->second, iter->first);
    }

    int cb = WideCharToMultiByte(CP_ACP, 0, &buf, static_cast<int>(buf.Len), NULL, 0, NULL, NULL);
    if (cb < 0)
    {
        throw Exception(L"Failed to convert to Acp.");
    }
    hostsData.Resize(cb);
    int cb2 = WideCharToMultiByte(CP_ACP, 0, &buf, static_cast<int>(buf.Len), hostsData, cb, NULL, NULL);
    if (cb2 != cb)
    {
        throw Exception(L"Failed to convert to Acp. Expected=%d Actual=%d", cb, cb2);
    }
}
