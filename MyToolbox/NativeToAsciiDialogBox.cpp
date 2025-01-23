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
#include "hnrt/NumberText.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/Win32Exception.h"


constexpr auto REGVAL_INPUT_CODEPAGE = L"InputCodePage";
constexpr auto REGVAL_OUTPUT_CODEPAGE = L"OutputCodePage";
constexpr auto REGVAL_OUTPUT_BOM = L"OutputBOM";
constexpr auto REGVAL_NATIVE_PATH = L"NativePath";
constexpr auto REGVAL_ASCII_PATH = L"AsciiPath";


using namespace hnrt;


NativeToAsciiDialogBox::NativeToAsciiDialogBox()
	: MyDialogBox(IDD_NTOA, L"NativeToAscii")
	, m_dwFlags(0)
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
	SetStatus();
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


INT_PTR NativeToAsciiDialogBox::OnControlColorStatic(WPARAM wParam, LPARAM lParam)
{
	HDC hdc = reinterpret_cast<HDC>(wParam);
	int id = GetDlgCtrlID(reinterpret_cast<HWND>(lParam));
	switch (id)
	{
	case IDC_NTOA_STATUS_STATIC:
		SetTextColor(hdc,
			(m_dwFlags & FLAG_STATUS_ERROR) ? RGB_ERROR :
			(m_dwFlags & FLAG_STATUS_SUCCESSFUL) ? RGB_SUCCESSFUL :
			GetSysColor(COLOR_WINDOWTEXT));
		SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
		return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_3DFACE));
	default:
		break;
	}
	return 0;
}


INT_PTR NativeToAsciiDialogBox::OnControlColorEdit(WPARAM wParam, LPARAM lParam)
{
	HDC hdc = reinterpret_cast<HDC>(wParam);
	int id = GetDlgCtrlID(reinterpret_cast<HWND>(lParam));
	switch (id)
	{
	case IDC_NTOA_NATIVE_EDIT:
		SetTextColor(hdc,
			(m_dwFlags & FLAG_PANE1_ERROR) ? RGB_ERROR :
			(m_dwFlags & FLAG_PANE1_SUCCESSFUL) ? RGB_SUCCESSFUL :
			GetSysColor(COLOR_WINDOWTEXT));
		SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
		return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_WINDOW));
	case IDC_NTOA_ASCII_EDIT:
		SetTextColor(hdc,
			(m_dwFlags & FLAG_PANE2_ERROR) ? RGB_ERROR :
			(m_dwFlags & FLAG_PANE2_SUCCESSFUL) ? RGB_SUCCESSFUL :
			GetSysColor(COLOR_WINDOWTEXT));
		SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
		return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_WINDOW));
	default:
		break;
	}
	return 0;
}


void NativeToAsciiDialogBox::OnEditChanged(int id)
{
	MyDialogBox::OnEditChanged(id);
	switch (id)
	{
	case IDC_NTOA_NATIVE_EDIT:
		SetStatus(L"", 0, MASK_PANE1 | MASK_STATUS);
		break;
	case IDC_NTOA_ASCII_EDIT:
		SetStatus(L"", 0, MASK_PANE2 | MASK_STATUS);
		break;
	default:
		break;
	}
	UpdateControlsState(id);
}


void NativeToAsciiDialogBox::OnNew()
{
	SetStatus(L"", 0, MASK_PANE1 | MASK_PANE2 | MASK_STATUS);
	SetTextAndNotify(IDC_NTOA_NATIVE_EDIT);
	SetTextAndNotify(IDC_NTOA_ASCII_EDIT);
}


void NativeToAsciiDialogBox::OnLoad1From()
{
	try
	{
		SetStatus(L"Loading Native...", FLAG_BUSY, MASK_STATUS);
		if (LoadTextFromFile(IDC_NTOA_NATIVE_EDIT, m_szNativePath))
		{
			SetStatus(String(PRINTF, L"Loading Native...Done:  %s chars in", NumberText(GetTextLength(IDC_NTOA_NATIVE_EDIT)).Ptr), FLAG_STATUS_SUCCESSFUL, MASK_PANE1);
		}
		else
		{
			SetStatus(L"");
		}
	}
	catch (Win32Exception e)
	{
		SetStatus(String(PRINTF, L"Loading Native...Failed: %s: %s", e.Message, ErrorMessage::Get(e.Error)), FLAG_STATUS_ERROR);
	}
	catch (Exception e)
	{
		SetStatus(String(PRINTF, L"Loading Native...Failed: %s", e.Message), FLAG_STATUS_ERROR);
	}
}


void NativeToAsciiDialogBox::OnSave1As()
{
	try
	{
		SetStatus(L"Saving Native...", FLAG_BUSY, MASK_STATUS);
		if (SaveTextAsFile(IDC_NTOA_NATIVE_EDIT, m_szNativePath))
		{
			SetStatus(String(PRINTF, L"Saving Native...Done:  %s chars out", NumberText(GetTextLength(IDC_NTOA_NATIVE_EDIT)).Ptr), FLAG_STATUS_SUCCESSFUL);
		}
		else
		{
			SetStatus(L"");
		}
	}
	catch (Win32Exception e)
	{
		SetStatus(String(PRINTF, L"Saving Native...Failed: %s: %s", e.Message, ErrorMessage::Get(e.Error)), FLAG_STATUS_ERROR);
	}
	catch (Exception e)
	{
		SetStatus(String(PRINTF, L"Saving Native...Failed: %s", e.Message), FLAG_STATUS_ERROR);
	}
}


void NativeToAsciiDialogBox::OnLoad2From()
{
	try
	{
		SetStatus(L"Loading ASCII...", FLAG_BUSY, MASK_STATUS);
		if (LoadTextFromFile(IDC_NTOA_ASCII_EDIT, m_szAsciiPath))
		{
			SetStatus(String(PRINTF, L"Loading ASCII...Done:  %s chars in", NumberText(GetTextLength(IDC_NTOA_ASCII_EDIT)).Ptr), FLAG_STATUS_SUCCESSFUL, MASK_PANE2);
		}
		else
		{
			SetStatus(L"");
		}
	}
	catch (Win32Exception e)
	{
		SetStatus(String(PRINTF, L"Loading ASCII...Failed: %s: %s", e.Message, ErrorMessage::Get(e.Error)), FLAG_STATUS_ERROR);
	}
	catch (Exception e)
	{
		SetStatus(String(PRINTF, L"Loading ASCII...Failed: %s", e.Message), FLAG_STATUS_ERROR);
	}
}


void NativeToAsciiDialogBox::OnSave2As()
{
	try
	{
		SetStatus(L"Saving ASCII...", FLAG_BUSY, MASK_STATUS);
		if (SaveTextAsFile(IDC_NTOA_ASCII_EDIT, m_szAsciiPath))
		{
			SetStatus(String(PRINTF, L"Saving ASCII...Done:  %s chars", NumberText(GetTextLength(IDC_NTOA_NATIVE_EDIT)).Ptr), FLAG_STATUS_SUCCESSFUL);
		}
		else
		{
			SetStatus(L"");
		}
	}
	catch (Win32Exception e)
	{
		SetStatus(String(PRINTF, L"Saving ASCII...Failed: %s: %s", e.Message, ErrorMessage::Get(e.Error)), FLAG_STATUS_ERROR);
	}
	catch (Exception e)
	{
		SetStatus(String(PRINTF, L"Saving ASCII...Failed: %s", e.Message), FLAG_STATUS_ERROR);
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


void NativeToAsciiDialogBox::Encode()
{
	SetStatus(L"Encoding...", FLAG_BUSY, MASK_STATUS);
	SetText(IDC_NTOA_ASCII_EDIT, FromNativeToAscii(GetText(IDC_NTOA_NATIVE_EDIT)));
	SetStatus(String(PRINTF, L"Encoding...Done: %s chars in  >>>  %s chars out",
		NumberText(GetTextLength(IDC_NTOA_NATIVE_EDIT)).Ptr, NumberText(GetTextLength(IDC_NTOA_ASCII_EDIT)).Ptr),
		FLAG_PANE2_SUCCESSFUL);
}


void NativeToAsciiDialogBox::Decode()
{
	SetStatus(L"Decoding...", FLAG_BUSY, MASK_STATUS);
	SetText(IDC_NTOA_NATIVE_EDIT, FromAsciiToNative(GetText(IDC_NTOA_ASCII_EDIT)));
	SetStatus(String(PRINTF, L"Decoding...Done: %s chars in  >>>  %s chars out",
		NumberText(GetTextLength(IDC_NTOA_ASCII_EDIT)).Ptr, NumberText(GetTextLength(IDC_NTOA_NATIVE_EDIT)).Ptr),
		FLAG_PANE1_SUCCESSFUL);
}


void NativeToAsciiDialogBox::SetStatus(PCWSTR psz, DWORD dwSet, DWORD dwReset)
{
	DWORD dwPrev = m_dwFlags;
	if (dwSet)
	{
		m_dwFlags |= dwSet;
	}
	if (dwReset)
	{
		m_dwFlags &= ~dwReset;
	}
	if (m_dwFlags != dwPrev)
	{
		InvalidateRect(IDC_NTOA_NATIVE_EDIT, NULL, TRUE);
		InvalidateRect(IDC_NTOA_ASCII_EDIT, NULL, TRUE);
	}
	SetText(IDC_NTOA_STATUS_STATIC, psz ? psz : L"");
	if ((dwSet & FLAG_BUSY))
	{
		UpdateControlsState(0);
	}
	else if ((dwPrev & FLAG_BUSY))
	{
		m_dwFlags &= ~FLAG_BUSY;
		UpdateControlsState(1);
	}
	ProcessMessages();
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
