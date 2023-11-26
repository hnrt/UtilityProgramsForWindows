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
		m_szNativePath = value.GetSZ(hKey, REG_NAME_NATIVE_PATH, L"");
		m_szAsciiPath = value.GetSZ(hKey, REG_NAME_ASCII_PATH, L"");
	}
	m_menuView
		.Add(ResourceString(IDS_MENU_NTOA), IDM_VIEW_NTOA);
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
		.Add(ResourceString(IDS_MENU_LOADFROM), IDM_FILE_LOADFROM)
		.Add(ResourceString(IDS_MENU_SAVEAS), IDM_FILE_SAVEAS)
		.AddSeparator()
		.Add(ResourceString(IDS_MENU_EXIT), IDM_FILE_EXIT);
	m_menuEdit
		.RemoveAll()
		.Add(ResourceString(CurrentEdit == IDC_NTOA_NATIVE_EDIT ? IDS_MENU_ENCODE : IDS_MENU_DECODE), IDM_EDIT_EXECUTE)
		.AddSeparator()
		.Add(ResourceString(IDS_MENU_CUT), IDM_EDIT_CUT)
		.Add(ResourceString(IDS_MENU_COPY), IDM_EDIT_COPY)
		.Add(ResourceString(IDS_MENU_PASTE), IDM_EDIT_PASTE)
		.Add(ResourceString(IDS_MENU_DELETE), IDM_EDIT_DELETE)
		.AddSeparator()
		.Add(ResourceString(IDS_MENU_SELECTALL), IDM_EDIT_SELECTALL)
		.AddSeparator()
		.Add(ResourceString(IDS_MENU_COPYALL), IDM_EDIT_COPYALL)
		.AddSeparator()
		.Add(ResourceString(IDS_MENU_CLEAR), IDM_EDIT_CLEAR);
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
		SetFocus(IDC_NTOA_COPY2_BUTTON);
		break;
	case IDC_NTOA_DECODE_BUTTON:
		OnDecode();
		SetFocus(IDC_NTOA_COPY1_BUTTON);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}


void NativeToAsciiDialogBox::OnLoadFrom()
{
	LoadTextFromFile(CurrentEdit, CurrentPath);
}


void NativeToAsciiDialogBox::OnSaveAs()
{
	SaveTextAsFile(CurrentEdit, CurrentPath);
}


void NativeToAsciiDialogBox::OnCut()
{
	EditCut(CurrentEdit);
}


void NativeToAsciiDialogBox::OnCopy()
{
	EditCopy(CurrentEdit);
}


void NativeToAsciiDialogBox::OnPaste()
{
	EditPaste(CurrentEdit);
}


void NativeToAsciiDialogBox::OnDelete()
{
	EditDelete(CurrentEdit);
}


void NativeToAsciiDialogBox::OnSelectAll()
{
	EditSelectAll(CurrentEdit);
}


void NativeToAsciiDialogBox::OnCopyAll()
{
	CopyAllText(CurrentEdit);
}


void NativeToAsciiDialogBox::OnClear()
{
	EditClear(CurrentEdit);
	CurrentPath = String::Empty;
}


void NativeToAsciiDialogBox::OnExecute()
{
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
	ButtonCheck(IDC_NTOA_NATIVE_RADIO, id == IDC_NTOA_NATIVE_RADIO);
	EditSetReadOnly(IDC_NTOA_NATIVE_EDIT, id != IDC_NTOA_NATIVE_RADIO);
	EnableWindow(IDC_NTOA_ENCODE_BUTTON, id == IDC_NTOA_NATIVE_RADIO);
	ButtonCheck(IDC_NTOA_ASCII_RADIO, id == IDC_NTOA_ASCII_RADIO);
	EditSetReadOnly(IDC_NTOA_ASCII_EDIT, id != IDC_NTOA_ASCII_RADIO);
	EnableWindow(IDC_NTOA_DECODE_BUTTON, id == IDC_NTOA_ASCII_RADIO);
	m_menuEdit
		.Modify(
			IDM_EDIT_EXECUTE, 0,
			IDM_EDIT_EXECUTE, ResourceString(id == IDC_NTOA_NATIVE_RADIO ? IDS_MENU_ENCODE : IDS_MENU_DECODE));
}


void NativeToAsciiDialogBox::OnEncode() const
{
	int cch = GetTextLength(IDC_NTOA_NATIVE_EDIT) + 1;
	Buffer<WCHAR> bufNative(cch);
	GetText(IDC_NTOA_NATIVE_EDIT, bufNative, bufNative.Len);
	cch = FromNativeToAscii(bufNative) + 1;
	Buffer<WCHAR> bufAscii(cch);
	FromNativeToAscii(bufNative, bufAscii);
	SetText(IDC_NTOA_ASCII_EDIT, bufAscii);
}


void NativeToAsciiDialogBox::OnDecode() const
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
