#include "pch.h"
#include "MyTabControl.h"
#include "MyToolbox.h"
#include "hnrt/RegistryKey.h"
#include "hnrt/RegistryValue.h"


#define REG_SUBKEY L"SOFTWARE\\hnrt\\MyToolbox"
#define REG_NAME_CURRENTTAB L"CurrentTab"


using namespace hnrt;


MyTabControl::MyTabControl()
	: TabControl()
	, m_pApp(nullptr)
{
}


void MyTabControl::Open(MyToolbox* pApp)
{
	m_pApp = pApp;
	TabControl::Open(pApp->hwnd);
}


void MyTabControl::OnTabSelectionChanging()
{
	ShowWindow(m_pApp->GetChild(CurrentItem), SW_HIDE);
}


void MyTabControl::OnTabSelectionChanged()
{
    SaveToRegistry();
    ShowWindow(m_pApp->GetChild(CurrentItem), SW_SHOW);
}


void MyTabControl::LoadFromRegistry()
{
    RegistryKey hKey;
    LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, REG_SUBKEY);
    if (rc == ERROR_SUCCESS)
    {
        CurrentItem = static_cast<INT>(RegistryValue().GetDWORD(hKey, REG_NAME_CURRENTTAB));
    }
    else
    {
        CurrentItem = 0;
    }
}


void MyTabControl::SaveToRegistry()
{
    RegistryKey hKey;
    LSTATUS rc = hKey.Create(HKEY_CURRENT_USER, REG_SUBKEY);
    if (rc == ERROR_SUCCESS)
    {
        RegistryValue::SetDWORD(hKey, REG_NAME_CURRENTTAB, CurrentItem);
    }
}
