#include "pch.h"
#include "NativeToAsciiDialogBox.h"
#include "MyToolbox.h"
#include "resource.h"
#include "hnrt/RegistryKey.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/ResourceString.h"
#include "hnrt/WindowDesign.h"
#include "hnrt/WindowLayoutSnapshot.h"
#include "hnrt/WhileInScope.h"
#include "hnrt/Menu.h"
#include "hnrt/WindowHandle.h"
#include "hnrt/Buffer.h"
#include "hnrt/String.h"
#include "hnrt/StringCommons.h"
#include "hnrt/UnicodeEscape.h"
#include "hnrt/Exception.h"


#define REGVAL_INPUT_CODEPAGE L"InputCodePage"
#define REGVAL_OUTPUT_CODEPAGE L"OutputCodePage"
#define REGVAL_OUTPUT_BOM L"OutputBOM"
#define REGVAL_NATIVE_PATH L"NativePath"
#define REGVAL_ASCII_PATH L"AsciiPath"


using namespace hnrt;


NativeToAsciiDialogBox::NativeToAsciiDialogBox()
	: MyDialogBox(IDD_NTOA, L"NativeToAscii")
	, m_szNativePath()
	, m_szAsciiPath()
{
}


void NativeToAsciiDialogBox::OnCreate()
{
	MyDialogBox::OnCreate();
	RegistryKey hKey;
	LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, m_szRegistryKeyName);
	if (rc == ERROR_SUCCESS)
	{
		m_uInputCodePage = RegistryValue::GetDWORD(hKey, REGVAL_INPUT_CODEPAGE, CP_AUTODETECT);
		m_uOutputCodePage = RegistryValue::GetDWORD(hKey, REGVAL_OUTPUT_CODEPAGE, CP_UTF8);
		m_bOutputBOM = RegistryValue::GetDWORD(hKey, REGVAL_OUTPUT_BOM, 0) ? true : false;
		m_szNativePath = RegistryValue::GetSZ(hKey, REGVAL_NATIVE_PATH);
		m_szAsciiPath = RegistryValue::GetSZ(hKey, REGVAL_ASCII_PATH);
	}
	HFONT hFont = GetApp<MyToolbox>().GetFontForData();
	SetFont(IDC_NTOA_NATIVE_EDIT, hFont);
	SetFont(IDC_NTOA_ASCII_EDIT, hFont);
	m_menuView
		.Add(ResourceString(IDS_MENU_NTOA), IDM_VIEW_NTOA);
}


void NativeToAsciiDialogBox::OnDestroy()
{
	RegistryKey hKey;
	LSTATUS rc = hKey.Create(HKEY_CURRENT_USER, m_szRegistryKeyName);
	if (rc == ERROR_SUCCESS)
	{
		RegistryValue::SetDWORD(hKey, REGVAL_INPUT_CODEPAGE, m_uInputCodePage);
		RegistryValue::SetDWORD(hKey, REGVAL_OUTPUT_CODEPAGE, m_uOutputCodePage);
		RegistryValue::SetDWORD(hKey, REGVAL_OUTPUT_BOM, m_bOutputBOM ? 1 : 0);
		RegistryValue::SetSZ(hKey, REGVAL_NATIVE_PATH, m_szNativePath);
		RegistryValue::SetSZ(hKey, REGVAL_ASCII_PATH, m_szAsciiPath);
	}
	SetFont(IDC_NTOA_NATIVE_EDIT, NULL);
	SetFont(IDC_NTOA_ASCII_EDIT, NULL);
	MyDialogBox::OnDestroy();
}


void NativeToAsciiDialogBox::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
	WindowLayoutSnapshot before, after;

	before.AddAllChildren(hDlg).Clone(after);

	after[IDC_NTOA_NATIVE_EDIT].right += cxDelta;
	MoveHorizontally(after[IDC_NTOA_COPY1_BUTTON], cxDelta);
	CenterHorizontally(after[IDC_NTOA_NATIVE_EDIT], after[IDC_NTOA_ENCODE_BUTTON], after[IDC_NTOA_DECODE_BUTTON]);
	after[IDC_NTOA_ASCII_EDIT].right += cxDelta;
	MoveHorizontally(after[IDC_NTOA_COPY2_BUTTON], cxDelta);

	ExtendVertically(after[IDC_NTOA_NATIVE_EDIT], after[IDC_NTOA_ASCII_EDIT], cyDelta);
	LONG dy = after[IDC_NTOA_NATIVE_EDIT].bottom - before[IDC_NTOA_NATIVE_EDIT].bottom;
	MoveVertically(after[IDC_NTOA_ENCODE_BUTTON], dy);
	MoveVertically(after[IDC_NTOA_DECODE_BUTTON], dy);
	RepositionTop(after[IDC_NTOA_ASCII_RADIO], after[IDC_NTOA_ASCII_EDIT].top);
	RepositionTop(after[IDC_NTOA_COPY2_BUTTON], after[IDC_NTOA_ASCII_EDIT].top);
	dy = after[IDC_NTOA_COPY2_BUTTON].bottom - before[IDC_NTOA_COPY2_BUTTON].bottom;

	after.Apply();
}


void NativeToAsciiDialogBox::OnTabSelectionChanging()
{
	MyDialogBox::OnTabSelectionChanging();
	m_menuView
		.Enable(IDM_VIEW_NTOA, MF_ENABLED);
}


void NativeToAsciiDialogBox::OnTabSelectionChanged()
{
	MyDialogBox::OnTabSelectionChanged();
	UpdateMenus();
	m_menuView
		.Enable(IDM_VIEW_NTOA, MF_DISABLED);
	m_menuSettings
		.RemoveAll();
	AddInputCodePageSettingMenus();
	AddOutputCodePageSettingMenus();
}


INT_PTR NativeToAsciiDialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	if (m_cProcessing)
	{
		return TRUE;
	}
	UINT idChild = LOWORD(wParam);
	UINT idNotif = HIWORD(wParam);
	switch (idChild)
	{
	case IDC_NTOA_NATIVE_RADIO:
	case IDC_NTOA_ASCII_RADIO:
		if (idNotif == BN_CLICKED)
		{
			OnSelectSource(idChild);
		}
		break;
	case IDC_NTOA_COPY1_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			CopyAllText(IDC_NTOA_NATIVE_EDIT);
		}
		break;
	case IDC_NTOA_COPY2_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			CopyAllText(IDC_NTOA_ASCII_EDIT);
		}
		break;
	case IDC_NTOA_ENCODE_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			OnEncode();
			SetFocus(IDC_NTOA_COPY2_BUTTON);
		}
		break;
	case IDC_NTOA_DECODE_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			OnDecode();
			SetFocus(IDC_NTOA_COPY1_BUTTON);
		}
		break;
	case IDC_NTOA_NATIVE_EDIT:
	case IDC_NTOA_ASCII_EDIT:
		if (idNotif == EN_CHANGE)
		{
			UpdateMenus();
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}


void NativeToAsciiDialogBox::OnLoadFrom()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	LoadTextFromFile(CurrentEdit, CurrentPath);
	UpdateMenus();
}


void NativeToAsciiDialogBox::OnSaveAs()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	SaveTextAsFile(CurrentEdit, CurrentPath);
}


void NativeToAsciiDialogBox::OnCut()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	EditCut(CurrentEdit);
	UpdateMenus();
}


void NativeToAsciiDialogBox::OnCopy()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	EditCopy(CurrentEdit);
	UpdateMenus();
}


void NativeToAsciiDialogBox::OnPaste()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	EditPaste(CurrentEdit);
	UpdateMenus();
}


void NativeToAsciiDialogBox::OnDelete()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	EditDelete(CurrentEdit);
	UpdateMenus();
}


void NativeToAsciiDialogBox::OnSelectAll()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	EditSelectAll(CurrentEdit);
}


void NativeToAsciiDialogBox::OnCopyAll()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	CopyAllText(CurrentEdit);
}


void NativeToAsciiDialogBox::OnClear()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	EditClear(CurrentEdit);
	CurrentPath = String::Empty;
	UpdateMenus();
}


void NativeToAsciiDialogBox::OnExecute()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	switch (CurrentEdit)
	{
	case IDC_NTOA_NATIVE_EDIT:
		OnEncode();
		SetFocus(IDC_NTOA_COPY2_BUTTON);
		break;
	case IDC_NTOA_ASCII_EDIT:
		OnDecode();
		SetFocus(IDC_NTOA_COPY1_BUTTON);
		break;
	default:
		break;
	}
}


void NativeToAsciiDialogBox::OnSettingChanged(UINT uId)
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	if (ApplyToInputCodePage(uId))
	{
		return;
	}
	if (ApplyToOutputCodePage(uId))
	{
		return;
	}
}


void NativeToAsciiDialogBox::OnSelectSource(int id)
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	ButtonCheck(IDC_NTOA_NATIVE_RADIO, id == IDC_NTOA_NATIVE_RADIO);
	EditSetReadOnly(IDC_NTOA_NATIVE_EDIT, id != IDC_NTOA_NATIVE_RADIO);
	EnableWindow(IDC_NTOA_ENCODE_BUTTON, id == IDC_NTOA_NATIVE_RADIO);
	ButtonCheck(IDC_NTOA_ASCII_RADIO, id == IDC_NTOA_ASCII_RADIO);
	EditSetReadOnly(IDC_NTOA_ASCII_EDIT, id != IDC_NTOA_ASCII_RADIO);
	EnableWindow(IDC_NTOA_DECODE_BUTTON, id == IDC_NTOA_ASCII_RADIO);
	UpdateMenus();
}


void NativeToAsciiDialogBox::OnEncode()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	SetText(IDC_NTOA_ASCII_EDIT, FromNativeToAscii(GetText(IDC_NTOA_NATIVE_EDIT)));
}


void NativeToAsciiDialogBox::OnDecode()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	SetText(IDC_NTOA_NATIVE_EDIT, FromAsciiToNative(GetText(IDC_NTOA_ASCII_EDIT)));
}


void NativeToAsciiDialogBox::UpdateMenus()
{
	UINT flags = GetTextLength(CurrentEdit) > 0 ? MF_ENABLED : MF_DISABLED;
	m_menuFile
		.RemoveAll()
		.Add(ResourceString(IDS_MENU_LOADFROM), IDM_FILE_LOADFROM)
		.Add(ResourceString(IDS_MENU_SAVEAS), IDM_FILE_SAVEAS, flags)
		.AddSeparator()
		.Add(ResourceString(IDS_MENU_EXIT), IDM_FILE_EXIT);
	m_menuEdit
		.RemoveAll()
		.Add(ResourceString(CurrentEdit == IDC_NTOA_NATIVE_EDIT ? IDS_MENU_ENCODE : IDS_MENU_DECODE), IDM_EDIT_EXECUTE, flags)
		.AddSeparator()
		.Add(ResourceString(IDS_MENU_CUT), IDM_EDIT_CUT, flags)
		.Add(ResourceString(IDS_MENU_COPY), IDM_EDIT_COPY, flags)
		.Add(ResourceString(IDS_MENU_PASTE), IDM_EDIT_PASTE)
		.Add(ResourceString(IDS_MENU_DELETE), IDM_EDIT_DELETE, flags)
		.AddSeparator()
		.Add(ResourceString(IDS_MENU_SELECTALL), IDM_EDIT_SELECTALL, flags)
		.AddSeparator()
		.Add(ResourceString(IDS_MENU_COPYALL), IDM_EDIT_COPYALL, flags)
		.AddSeparator()
		.Add(ResourceString(IDS_MENU_CLEAR), IDM_EDIT_CLEAR, flags);
}


int NativeToAsciiDialogBox::get_CurrentEdit() const
{
	if (ButtonIsChecked(IDC_NTOA_NATIVE_RADIO))
	{
		return IDC_NTOA_NATIVE_EDIT;
	}
	else if (ButtonIsChecked(IDC_NTOA_ASCII_RADIO))
	{
		return IDC_NTOA_ASCII_EDIT;
	}
	else
	{
		throw Exception(L"NativeToAsciiDialogBox::get_CurrentEdit: Unexpected state.");
	}
}


String& NativeToAsciiDialogBox::get_CurrentPath() const
{
	switch (CurrentEdit)
	{
	case IDC_NTOA_NATIVE_EDIT:
		return m_szNativePath;
	case IDC_NTOA_ASCII_EDIT:
		return m_szAsciiPath;
	default:
		throw Exception(L"NativeToAsciiDialogBox::get_CurrentPath: Unexpected state.");
	}
}


void NativeToAsciiDialogBox::set_CurrentPath(const String& value)
{
	switch (CurrentEdit)
	{
	case IDC_NTOA_NATIVE_EDIT:
		m_szNativePath = value;
		break;
	case IDC_NTOA_ASCII_EDIT:
		m_szAsciiPath = value;
		break;
	default:
		throw Exception(L"NativeToAsciiDialogBox::set_CurrentPath: Unexpected state.");
	}
}
