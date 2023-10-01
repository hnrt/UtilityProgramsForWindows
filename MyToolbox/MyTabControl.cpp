#include "pch.h"
#include "MyTabControl.h"
#include "MyToolbox.h"
#include "hnrt/LogicalFont.h"
#include "hnrt/WindowHandle.h"
#include "hnrt/RegistryKey.h"
#include "hnrt/RegistryValue.h"


#define FACENAME L"Segoe UI"
#define POINTSIZE 8


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
    SetFont(*this,
        LogicalFont()
        .SetFaceName(FACENAME)
        .SetHeight(POINTSIZE, *this)
        .SetJapaneseCharSet()
        .Create());
}


void MyTabControl::OnTabSelectionChanging()
{
	ShowWindow(m_pApp->GetChild(CurrentItem), SW_HIDE);
}


void MyTabControl::OnTabSelectionChanged()
{
    SaveToRegistry();
    HWND hwndChild = m_pApp->GetChild(CurrentItem);
    ShowWindow(hwndChild, SW_SHOW);
    SetFocus(hwndChild);
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
