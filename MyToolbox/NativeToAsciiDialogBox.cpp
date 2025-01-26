#include "pch.h"
#include "NativeToAsciiDialogBox.h"
#include "MyToolbox.h"
#include "resource.h"
#include "hnrt/RegistryKey.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/ResourceString.h"
#include "hnrt/WindowLayoutSnapshot.h"
#include "hnrt/WhileInScope.h"
#include "hnrt/Buffer.h"
#include "hnrt/StringCommons.h"
#include "hnrt/UnicodeEscape.h"
#include "hnrt/NumberText.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/Debug.h"


using namespace hnrt;


constexpr auto REGVAL_INPUT_CODEPAGE = L"InputCodePage";
constexpr auto REGVAL_OUTPUT_CODEPAGE = L"OutputCodePage";
constexpr auto REGVAL_OUTPUT_BOM = L"OutputBOM";
constexpr auto REGVAL_NATIVE_PATH = L"NativePath";
constexpr auto REGVAL_ASCII_PATH = L"AsciiPath";


NativeToAsciiDialogBox::NativeToAsciiDialogBox()
	: MyDialogBox(IDD_NTOA, L"NativeToAscii", IDC_NTOA_STATUS_STATIC, IDC_NTOA_NATIVE_EDIT, IDC_NTOA_ASCII_EDIT)
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
	SetStatus(0, 0, L"");
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

	after[IDC_NTOA_STATUS_STATIC].right += cxDelta;

	ExtendVertically(after[IDC_NTOA_NATIVE_EDIT], after[IDC_NTOA_ASCII_EDIT], cyDelta);
	LONG dy = after[IDC_NTOA_NATIVE_EDIT].bottom - before[IDC_NTOA_NATIVE_EDIT].bottom;
	MoveVertically(after[IDC_NTOA_ASCII_STATIC], dy);
	MoveVertically(after[IDC_NTOA_COPY2_BUTTON], dy);
	MoveVertically(after[IDC_NTOA_DECODE_BUTTON], dy);

	MoveVertically(after[IDC_NTOA_STATUS_STATIC], cyDelta);

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
	UpdateControlsState(1);
}


INT_PTR NativeToAsciiDialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (m_cProcessing)
	{
		return TRUE;
	}
	if (MyDialogBox::OnCommand(wParam, lParam))
	{
		return TRUE;
	}
	UINT idChild = LOWORD(wParam);
	UINT idNotif = HIWORD(wParam);
	switch (idChild)
	{
	case IDM_EDIT_EXECUTE1:
		Encode();
		SetFocus(IDC_NTOA_COPY2_BUTTON);
		break;
	case IDM_EDIT_COPYRESULT1:
		CopyAllText(IDC_NTOA_NATIVE_EDIT);
		break;
	case IDM_EDIT_EXECUTE2:
		Decode();
		SetFocus(IDC_NTOA_COPY1_BUTTON);
		break;
	case IDM_EDIT_COPYRESULT2:
		CopyAllText(IDC_NTOA_ASCII_EDIT);
		break;
	case IDC_NTOA_ENCODE_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			Encode();
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
			Decode();
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


void NativeToAsciiDialogBox::OnEditChanged(int id)
{
	MyDialogBox::OnEditChanged(id);
	switch (id)
	{
	case IDC_NTOA_NATIVE_EDIT:
		SetStatus(0, MASK_PANE1 | MASK_STATUS, L"");
		break;
	case IDC_NTOA_ASCII_EDIT:
		SetStatus(0, MASK_PANE2 | MASK_STATUS, L"");
		break;
	default:
		return;
	}
	UpdateControlsState(id);
}


void NativeToAsciiDialogBox::OnNew()
{
	SetStatus(0, MASK_PANE1 | MASK_PANE2 | MASK_STATUS, L"");
	SetTextAndNotify(IDC_NTOA_NATIVE_EDIT);
	SetTextAndNotify(IDC_NTOA_ASCII_EDIT);
}


void NativeToAsciiDialogBox::OnLoad1From()
{
	LoadTextFromFile(IDC_NTOA_NATIVE_EDIT, ResourceString(IDS_LOADING_NATIVE), m_szNativePath);
}


void NativeToAsciiDialogBox::OnSave1As()
{
	SaveTextAsFile(IDC_NTOA_NATIVE_EDIT, ResourceString(IDS_SAVING_NATIVE), m_szNativePath);
}


void NativeToAsciiDialogBox::OnLoad2From()
{
	LoadTextFromFile(IDC_NTOA_ASCII_EDIT, ResourceString(IDS_LOADING_ASCII), m_szAsciiPath);
}


void NativeToAsciiDialogBox::OnSave2As()
{
	SaveTextAsFile(IDC_NTOA_ASCII_EDIT, ResourceString(IDS_SAVING_ASCII), m_szAsciiPath);
}


void NativeToAsciiDialogBox::Encode()
{
	DBGFNC(L"NativeToAsciiDialogBox::Encode");
	ResourceString szLeader(IDS_ENCODING_NATIVE);
	SetStatus(FLAG_BUSY, MASK_STATUS, szLeader);
	SetText(IDC_NTOA_ASCII_EDIT, FromNativeToAscii(GetText(IDC_NTOA_NATIVE_EDIT)));
	SetStatus(FLAG_STATUS_SUCCESSFUL | FLAG_PANE2_SUCCESSFUL, FLAG_PANE1_ERROR | FLAG_PANE2_ERROR,
		ResourceString(IDS_W_DONE_X_IN_Y_OUT), szLeader, NumberOfChars(GetTextLength(IDC_NTOA_NATIVE_EDIT)), NumberOfChars(GetTextLength(IDC_NTOA_ASCII_EDIT)));
}


void NativeToAsciiDialogBox::Decode()
{
	DBGFNC(L"NativeToAsciiDialogBox::Decode");
	ResourceString szLeader(IDS_DECODING_ASCII);
	SetStatus(FLAG_BUSY, MASK_STATUS, szLeader);
	SetText(IDC_NTOA_NATIVE_EDIT, FromAsciiToNative(GetText(IDC_NTOA_ASCII_EDIT)));
	SetStatus(FLAG_STATUS_SUCCESSFUL | FLAG_PANE1_SUCCESSFUL, FLAG_PANE2_ERROR | FLAG_PANE2_ERROR,
		ResourceString(IDS_W_DONE_X_IN_Y_OUT), szLeader, NumberOfChars(GetTextLength(IDC_NTOA_ASCII_EDIT)), NumberOfChars(GetTextLength(IDC_NTOA_NATIVE_EDIT)));
}


void NativeToAsciiDialogBox::UpdateControlsState(int id)
{
	switch (id)
	{
	case 0:
		EditSetReadOnly(IDC_NTOA_NATIVE_EDIT, TRUE);
		EditSetReadOnly(IDC_NTOA_ASCII_EDIT, TRUE);
		DisableWindow(IDC_NTOA_ENCODE_BUTTON);
		DisableWindow(IDC_NTOA_COPY1_BUTTON);
		DisableWindow(IDC_NTOA_DECODE_BUTTON);
		DisableWindow(IDC_NTOA_COPY2_BUTTON);
		m_menuEdit
			.Enable(IDM_EDIT_EXECUTE1, MF_DISABLED)
			.Enable(IDM_EDIT_COPYRESULT1, MF_DISABLED)
			.Enable(IDM_EDIT_EXECUTE2, MF_DISABLED)
			.Enable(IDM_EDIT_COPYRESULT2, MF_DISABLED);
		break;
	case 1:
		EditSetReadOnly(IDC_NTOA_NATIVE_EDIT, FALSE);
		EditSetReadOnly(IDC_NTOA_ASCII_EDIT, FALSE);
		UpdateControlsState(IDC_NTOA_NATIVE_EDIT);
		UpdateControlsState(IDC_NTOA_ASCII_EDIT);
		break;
	case IDC_NTOA_NATIVE_EDIT:
	{
		BOOL bEnabled = GetTextLength(IDC_NTOA_NATIVE_EDIT) > 0 ? TRUE : FALSE;
		EnableWindow(IDC_NTOA_ENCODE_BUTTON, bEnabled);
		EnableWindow(IDC_NTOA_COPY1_BUTTON, bEnabled);
		m_menuEdit
			.Enable(IDM_EDIT_EXECUTE1, bEnabled ? MF_ENABLED : MF_DISABLED)
			.Enable(IDM_EDIT_COPYRESULT1, bEnabled ? MF_ENABLED : MF_DISABLED);
		break;
	}
	case IDC_NTOA_ASCII_EDIT:
	{
		BOOL bEnabled = GetTextLength(IDC_NTOA_ASCII_EDIT) > 0 ? TRUE : FALSE;
		EnableWindow(IDC_NTOA_DECODE_BUTTON, bEnabled);
		EnableWindow(IDC_NTOA_COPY2_BUTTON, bEnabled);
		m_menuEdit
			.Enable(IDM_EDIT_EXECUTE2, bEnabled ? MF_ENABLED : MF_DISABLED)
			.Enable(IDM_EDIT_COPYRESULT2, bEnabled ? MF_ENABLED : MF_DISABLED);
		break;
	}
	default:
		break;
	}
}
