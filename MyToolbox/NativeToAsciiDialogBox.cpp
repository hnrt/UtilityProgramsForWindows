#include "pch.h"
#include "NativeToAsciiDialogBox.h"
#include "resource.h"
#include "hnrt/RegistryKey.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/ResourceString.h"
#include "hnrt/WindowDesign.h"
#include "hnrt/WindowLayoutSnapshot.h"
#include "hnrt/Menu.h"
#include "hnrt/WindowHandle.h"
#include "hnrt/Buffer.h"
#include "hnrt/String.h"
#include "hnrt/UnicodeEscape.h"
#include "hnrt/Exception.h"


using namespace hnrt;


#define REG_NAME_INPUT_CODEPAGE L"InputCodePage"
#define REG_NAME_OUTPUT_CODEPAGE L"OutputCodePage"
#define REG_NAME_OUTPUT_BOM L"OutputBOM"
#define REG_NAME_NATIVE_PATH L"NativePath"
#define REG_NAME_ASCII_PATH L"AsciiPath"


NativeToAsciiDialogBox::NativeToAsciiDialogBox()
	: MyDialogBox(IDD_NTOA)
	, m_szNativePath()
	, m_szAsciiPath()
{
}


void NativeToAsciiDialogBox::OnCreate()
{
	MyDialogBox::OnCreate();
	RegistryKey hKey;
	LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, REG_SUBKEY_(NativeToAscii));
	if (rc == ERROR_SUCCESS)
	{
		RegistryValue value;
		m_uInputCodePage = value.GetDWORD(hKey, REG_NAME_INPUT_CODEPAGE, CP_AUTODETECT);
		m_uOutputCodePage = value.GetDWORD(hKey, REG_NAME_OUTPUT_CODEPAGE, CP_UTF8);
		m_bOutputBOM = value.GetDWORD(hKey, REG_NAME_OUTPUT_BOM, 0) ? true : false;
		wcscpy_s(m_szNativePath, value.GetSZ(hKey, REG_NAME_NATIVE_PATH, L""));
		wcscpy_s(m_szAsciiPath, value.GetSZ(hKey, REG_NAME_ASCII_PATH, L""));
	}
	m_menuView
		.Add(ResourceString(IDS_NTOA_TABLABEL), IDM_VIEW_NTOA);
}


void NativeToAsciiDialogBox::OnDestroy()
{
	RegistryKey hKey;
	LSTATUS rc = hKey.Create(HKEY_CURRENT_USER, REG_SUBKEY_(NativeToAscii));
	if (rc == ERROR_SUCCESS)
	{
		RegistryValue::SetDWORD(hKey, REG_NAME_INPUT_CODEPAGE, m_uInputCodePage);
		RegistryValue::SetDWORD(hKey, REG_NAME_OUTPUT_CODEPAGE, m_uOutputCodePage);
		RegistryValue::SetDWORD(hKey, REG_NAME_OUTPUT_BOM, m_bOutputBOM ? 1 : 0);
		RegistryValue::SetSZ(hKey, REG_NAME_NATIVE_PATH, m_szNativePath);
		RegistryValue::SetSZ(hKey, REG_NAME_ASCII_PATH, m_szAsciiPath);
	}
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
	m_menuFile
		.RemoveAll()
		.Add(ResourceString(IDS_LOADFROM), IDM_FILE_LOADFROM)
		.Add(ResourceString(IDS_SAVEAS), IDM_FILE_SAVEAS)
		.AddSeparator()
		.Add(ResourceString(IDS_EXIT), IDM_FILE_EXIT);
	m_menuEdit
		.RemoveAll()
		.Add(ResourceString(IDS_CUT), IDM_EDIT_CUT)
		.Add(ResourceString(IDS_COPY), IDM_EDIT_COPY)
		.Add(ResourceString(IDS_PASTE), IDM_EDIT_PASTE)
		.AddSeparator()
		.Add(ResourceString(IDS_SELECTALL), IDM_EDIT_SELECTALL)
		.AddSeparator()
		.Add(ResourceString(IDS_CLEAR), IDM_EDIT_CLEAR);
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
	UINT idChild = LOWORD(wParam);
	switch (idChild)
	{
	case IDC_NTOA_NATIVE_RADIO:
	case IDC_NTOA_ASCII_RADIO:
		OnSelectSource(idChild);
		break;
	case IDC_NTOA_COPY1_BUTTON:
		CopyAllText(IDC_NTOA_NATIVE_EDIT);
		break;
	case IDC_NTOA_COPY2_BUTTON:
		CopyAllText(IDC_NTOA_ASCII_EDIT);
		break;
	case IDC_NTOA_ENCODE_BUTTON:
		OnEncode();
		break;
	case IDC_NTOA_DECODE_BUTTON:
		OnDecode();
		break;
	default:
		return FALSE;
	}
	return TRUE;
}


void NativeToAsciiDialogBox::OnLoadFrom()
{
	LoadTextFromFile(CurrentEdit, CurrentPath, MAX_PATH);
}


void NativeToAsciiDialogBox::OnSaveAs()
{
	SaveTextAsFile(CurrentEdit, CurrentPath, MAX_PATH);
}


void NativeToAsciiDialogBox::OnCut()
{
	CutText(CurrentEdit);
}


void NativeToAsciiDialogBox::OnCopy()
{
	CopyAllText(CurrentEdit);
}


void NativeToAsciiDialogBox::OnPaste()
{
	PasteText(CurrentEdit);
}


void NativeToAsciiDialogBox::OnSelectAll()
{
	SelectAllText(CurrentEdit);
}


void NativeToAsciiDialogBox::OnClear()
{
	ClearEdit(CurrentEdit);
	wmemset(CurrentPath, L'\0', MAX_PATH);
}


void NativeToAsciiDialogBox::OnSettingChanged(UINT uId)
{
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
	CheckButton(IDC_NTOA_NATIVE_RADIO, id == IDC_NTOA_NATIVE_RADIO ? BST_CHECKED : BST_UNCHECKED);
	SetReadOnlyEdit(IDC_NTOA_NATIVE_EDIT, id == IDC_NTOA_NATIVE_RADIO ? FALSE : TRUE);
	EnableWindow(IDC_NTOA_COPY1_BUTTON);
	EnableWindow(IDC_NTOA_ENCODE_BUTTON, id == IDC_NTOA_NATIVE_RADIO);
	CheckButton(IDC_NTOA_ASCII_RADIO, id == IDC_NTOA_ASCII_RADIO ? BST_CHECKED : BST_UNCHECKED);
	SetReadOnlyEdit(IDC_NTOA_ASCII_EDIT, id == IDC_NTOA_ASCII_RADIO ? FALSE : TRUE);
	EnableWindow(IDC_NTOA_COPY2_BUTTON);
	EnableWindow(IDC_NTOA_DECODE_BUTTON, id == IDC_NTOA_ASCII_RADIO);
}


void NativeToAsciiDialogBox::OnEncode()
{
	int cch = GetTextLength(IDC_NTOA_NATIVE_EDIT) + 1;
	Buffer<WCHAR> bufNative(cch);
	GetText(IDC_NTOA_NATIVE_EDIT, bufNative, bufNative.Len);
	cch = FromNativeToAscii(bufNative) + 1;
	Buffer<WCHAR> bufAscii(cch);
	FromNativeToAscii(bufNative, bufAscii);
	SetText(IDC_NTOA_ASCII_EDIT, bufAscii);
}


void NativeToAsciiDialogBox::OnDecode()
{
	int cch = GetTextLength(IDC_NTOA_ASCII_EDIT) + 1;
	Buffer<WCHAR> bufAscii(cch);
	GetText(IDC_NTOA_ASCII_EDIT, bufAscii, bufAscii.Len);
	cch = FromAsciiToNative(bufAscii) + 1;
	Buffer<WCHAR> bufNative(cch);
	FromAsciiToNative(bufAscii, bufNative);
	SetText(IDC_NTOA_NATIVE_EDIT, bufNative);
}


int NativeToAsciiDialogBox::get_CurrentEdit() const
{
	if (GetButtonState(IDC_NTOA_NATIVE_RADIO) == BST_CHECKED)
	{
		return IDC_NTOA_NATIVE_EDIT;
	}
	else if (GetButtonState(IDC_NTOA_ASCII_RADIO) == BST_CHECKED)
	{
		return IDC_NTOA_ASCII_EDIT;
	}
	else
	{
		throw Exception(L"NativeToAsciiDialogBox::get_CurrentEdit: Unexpected state.");
	}
}


PWSTR NativeToAsciiDialogBox::get_CurrentPath()
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
