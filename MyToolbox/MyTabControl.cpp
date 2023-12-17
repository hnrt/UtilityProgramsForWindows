#include "pch.h"
#include "MyTabControl.h"
#include "MyDialogBox.h"
#include "hnrt/TabControlItem.h"
#include "hnrt/LogicalFont.h"
#include "hnrt/WindowHandle.h"
#include "hnrt/RegistryKey.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/Exception.h"


using namespace hnrt;


#define FACENAME L"Segoe UI"
#define POINTSIZE 10


#define REG_NAME_CURRENTTAB L"CurrentTab"


MyTabControl::MyTabControl()
	: TabControl()
    , m_pDialogBoxes()
{
}


void MyTabControl::Open(HWND hwndParent)
{
	TabControl::Open(hwndParent);
    SetFont(*this,
        LogicalFont()
        .SetFaceName(FACENAME)
        .SetHeight(POINTSIZE, *this)
        .SetJapaneseCharSet()
        .Create());
}


void MyTabControl::Add(PCWSTR pszLabel, MyDialogBox* pDialogBox)
{
    int count = ItemCount;
    pDialogBox->SetId(count);
    m_pDialogBoxes[count] = pDialogBox;
    TabControlItem().SetText(pszLabel).InsertInto(*this);
}


const MyDialogBox& MyTabControl::operator [](int index) const
{
    int count = ItemCount;
    if (0 <= index && index < count)
    {
        return *m_pDialogBoxes[index];
    }
    else if (0 <= index + count && index < 0)
    {
        return *m_pDialogBoxes[index + count];
    }
    else
    {
        throw Exception(L"MyTabControl::operator[]: Index out of range: %d", index);
    }
}


MyDialogBox& MyTabControl::operator [](int index)
{
    int count = ItemCount;
    if (0 <= index && index < count)
    {
        return *m_pDialogBoxes[index];
    }
    else if (0 <= index + count && index < 0)
    {
        return *m_pDialogBoxes[index + count];
    }
    else
    {
        throw Exception(L"MyTabControl::operator[]: Index out of range: %d", index);
    }
}


void MyTabControl::OnTabSelectionChanging()
{
    MyDialogBox& dialogBox = *m_pDialogBoxes[CurrentItem];
	ShowWindow(dialogBox, SW_HIDE);
    dialogBox.OnTabSelectionChanging();
}


void MyTabControl::OnTabSelectionChanged()
{
    MyDialogBox& dialogBox = *m_pDialogBoxes[CurrentItem];
    SaveToRegistry();
    HWND hwndChild = dialogBox;
    ShowWindow(hwndChild, SW_SHOW);
    SetFocus(hwndChild);
    dialogBox.OnTabSelectionChanged();
}


void MyTabControl::LoadFromRegistry()
{
    RegistryKey hKey;
    LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, REG_SUBKEY);
    if (rc == ERROR_SUCCESS)
    {
        CurrentItem = static_cast<INT>(RegistryValue::GetDWORD(hKey, REG_NAME_CURRENTTAB));
    }
    else
    {
        CurrentItem = 0;
    }
}


void MyTabControl::SaveToRegistry() const
{
    RegistryKey hKey;
    LSTATUS rc = hKey.Create(HKEY_CURRENT_USER, REG_SUBKEY);
    if (rc == ERROR_SUCCESS)
    {
        RegistryValue::SetDWORD(hKey, REG_NAME_CURRENTTAB, CurrentItem);
    }
}
