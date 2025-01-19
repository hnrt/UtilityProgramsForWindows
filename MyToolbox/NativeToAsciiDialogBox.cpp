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
	HFONT hFont = GetApp<MyToolbox>().GetFontForData();
	SetFont(IDC_NTOA_NATIVE_EDIT, hFont);
	SetFont(IDC_NTOA_ASCII_EDIT, hFont);
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
	m_menuView
		.Add(ResourceString(IDS_MENU_NTOA), IDM_VIEW_NTOA);
	UpdateControlsState();
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

	after[IDC_NTOA_NATIVE_STATIC].right += cxDelta;
	after[IDC_NTOA_NATIVE_EDIT].right += cxDelta;

	MoveHorizontally(after[IDC_NTOA_COPY1_BUTTON], cxDelta);
	MoveHorizontally(after[IDC_NTOA_ENCODE_BUTTON], cxDelta);

	after[IDC_NTOA_ASCII_STATIC].right += cxDelta;
	after[IDC_NTOA_ASCII_EDIT].right += cxDelta;

	MoveHorizontally(after[IDC_NTOA_COPY2_BUTTON], cxDelta);
	MoveHorizontally(after[IDC_NTOA_DECODE_BUTTON], cxDelta);

	ExtendVertically(after[IDC_NTOA_NATIVE_EDIT], after[IDC_NTOA_ASCII_EDIT], cyDelta);
	LONG dy = after[IDC_NTOA_NATIVE_EDIT].bottom - before[IDC_NTOA_NATIVE_EDIT].bottom;
	MoveVertically(after[IDC_NTOA_ASCII_STATIC], dy);
	MoveVertically(after[IDC_NTOA_COPY2_BUTTON], dy);
	MoveVertically(after[IDC_NTOA_DECODE_BUTTON], dy);

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
	m_menuFile
		.Insert(0, ResourceString(IDS_MENU_NEW), IDM_FILE_NEW)
		.InsertSeparator(1)
		.Insert(2, ResourceString(IDS_MENU_ORG_LOADFROM), IDM_FILE_LOAD1FROM)
		.Insert(3, ResourceString(IDS_MENU_ORG_SAVEAS), IDM_FILE_SAVE1AS)
		.InsertSeparator(4)
		.Insert(5, ResourceString(IDS_MENU_ENC_LOADFROM), IDM_FILE_LOAD2FROM)
		.Insert(6, ResourceString(IDS_MENU_ENC_SAVEAS), IDM_FILE_SAVE2AS)
		.InsertSeparator(7);
	m_menuEdit
		.Insert(0, ResourceString(IDS_MENU_ENCODE), IDM_EDIT_EXECUTE1)
		.Insert(1, ResourceString(IDS_MENU_COPYNATIVE), IDM_EDIT_COPYRESULT1)
		.InsertSeparator(2)
		.Insert(3, ResourceString(IDS_MENU_DECODE), IDM_EDIT_EXECUTE2)
		.Insert(4, ResourceString(IDS_MENU_COPYASCII), IDM_EDIT_COPYRESULT2)
		.InsertSeparator(5);
	m_menuView
		.Enable(IDM_VIEW_NTOA, MF_DISABLED);
	AddInputCodePageSettingMenus();
	AddOutputCodePageSettingMenus();
	UpdateControlsState();
}


INT_PTR NativeToAsciiDialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (m_cProcessing)
	{
		return TRUE;
	}
	UINT idChild = LOWORD(wParam);
	UINT idNotif = HIWORD(wParam);
	switch (idChild)
	{
	case IDM_EDIT_EXECUTE1:
		OnEncode();
		SetFocus(IDC_NTOA_COPY2_BUTTON);
		break;
	case IDM_EDIT_COPYRESULT1:
		CopyAllText(IDC_NTOA_NATIVE_EDIT);
		break;
	case IDM_EDIT_EXECUTE2:
		OnDecode();
		SetFocus(IDC_NTOA_COPY1_BUTTON);
		break;
	case IDM_EDIT_COPYRESULT2:
		CopyAllText(IDC_NTOA_ASCII_EDIT);
		break;
	case IDC_NTOA_ENCODE_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			OnEncode();
			SetFocus(IDC_NTOA_COPY2_BUTTON);
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_NTOA_COPY1_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			CopyAllText(IDC_NTOA_NATIVE_EDIT);
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_NTOA_DECODE_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			OnDecode();
			SetFocus(IDC_NTOA_COPY1_BUTTON);
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_NTOA_COPY2_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			CopyAllText(IDC_NTOA_ASCII_EDIT);
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_NTOA_NATIVE_EDIT:
	case IDC_NTOA_ASCII_EDIT:
		return OnEditCommand(wParam, lParam);
	default:
		return FALSE;
	}
	return TRUE;
}


void NativeToAsciiDialogBox::OnNew()
{
	SetTextAndNotify(IDC_NTOA_NATIVE_EDIT);
	SetTextAndNotify(IDC_NTOA_ASCII_EDIT);
}


void NativeToAsciiDialogBox::OnLoad1From()
{
	LoadTextFromFile(IDC_NTOA_NATIVE_EDIT, m_szNativePath);
}


void NativeToAsciiDialogBox::OnSave1As()
{
	SaveTextAsFile(IDC_NTOA_NATIVE_EDIT, m_szNativePath);
}


void NativeToAsciiDialogBox::OnLoad2From()
{
	LoadTextFromFile(IDC_NTOA_ASCII_EDIT, m_szAsciiPath);
}


void NativeToAsciiDialogBox::OnSave2As()
{
	SaveTextAsFile(IDC_NTOA_ASCII_EDIT, m_szAsciiPath);
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


void NativeToAsciiDialogBox::OnEncode()
{
	SetTextAndNotify(IDC_NTOA_ASCII_EDIT, FromNativeToAscii(GetText(IDC_NTOA_NATIVE_EDIT)));
}


void NativeToAsciiDialogBox::OnDecode()
{
	SetTextAndNotify(IDC_NTOA_NATIVE_EDIT, FromAsciiToNative(GetText(IDC_NTOA_ASCII_EDIT)));
}


void NativeToAsciiDialogBox::UpdateControlsState(int id)
{
	switch (id)
	{
	case IDC_NTOA_NATIVE_EDIT:
	{
		BOOL bNative = GetTextLength(IDC_NTOA_NATIVE_EDIT) > 0 ? TRUE : FALSE;
		EnableWindow(IDC_NTOA_COPY1_BUTTON, bNative);
		EnableWindow(IDC_NTOA_ENCODE_BUTTON, bNative);
		m_menuEdit
			.Enable(IDM_EDIT_EXECUTE1, bNative ? MF_ENABLED : MF_DISABLED)
			.Enable(IDM_EDIT_COPYRESULT1, bNative ? MF_ENABLED : MF_DISABLED);
		break;
	}
	case IDC_NTOA_ASCII_EDIT:
	{
		BOOL bAscii = GetTextLength(IDC_NTOA_ASCII_EDIT) > 0 ? TRUE : FALSE;
		EnableWindow(IDC_NTOA_COPY2_BUTTON, bAscii);
		EnableWindow(IDC_NTOA_DECODE_BUTTON, bAscii);
		m_menuEdit
			.Enable(IDM_EDIT_EXECUTE2, bAscii ? MF_ENABLED : MF_DISABLED)
			.Enable(IDM_EDIT_COPYRESULT2, bAscii ? MF_ENABLED : MF_DISABLED);
		break;
	}
	case 0:
		UpdateControlsState(IDC_NTOA_NATIVE_EDIT);
		UpdateControlsState(IDC_NTOA_ASCII_EDIT);
		break;
	default:
		break;
	}
}
