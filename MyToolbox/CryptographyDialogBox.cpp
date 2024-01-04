#include "pch.h"
#include "CryptographyDialogBox.h"
#include "resource.h"
#include "MyToolbox.h"
#include "hnrt/ResourceString.h"
#include "hnrt/RegistryKey.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/WindowLayoutSnapshot.h"
#include "hnrt/LogicalFont.h"
#include "hnrt/Base64.h"
#include "hnrt/StringUTF8.h"
#include "hnrt/StringACP.h"
#include "hnrt/BCryptKeyHandle.h"
#include "hnrt/BCryptAuthenticatedCipherModeInfo.h"
#include "hnrt/Buffer.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/WhileInScope.h"
#include "hnrt/FileMapper.h"
#include "hnrt/FileWriter.h"
#include <map>


#define REG_NAME_CHAININGMODE L"ChainingMode"
#define REG_NAME_KEYLENGTH L"KeyLength"
#define REG_NAME_KEY L"Key"
#define REG_NAME_IV L"IV"
#define REG_NAME_NONCE L"Nonce"
#define REG_NAME_NONCELENGTH L"NonceLength"
#define REG_NAME_TAG L"Tag"
#define REG_NAME_TAGLENGTH L"TagLength"
#define REG_NAME_ORGDISPMODE L"OriginalDataDisplayMode"
#define REG_NAME_ENCDISPMODE L"EncryptedDataDisplayMode"


#define IS_AES_CBC(x) (!wcscmp(x,BCRYPT_CHAIN_MODE_CBC))
#define IS_AES_ECB(x) (!wcscmp(x,BCRYPT_CHAIN_MODE_ECB))
#define IS_AES_CFB(x) (!wcscmp(x,BCRYPT_CHAIN_MODE_CFB))
#define IS_AES_CCM(x) (!wcscmp(x,BCRYPT_CHAIN_MODE_CCM))
#define IS_AES_GCM(x) (!wcscmp(x,BCRYPT_CHAIN_MODE_GCM))


#if 0
#define CRYPTOGRAPHY_TIMER1SEC 201
#endif


using namespace hnrt;


CryptographyDialogBox::CryptographyDialogBox()
	: MyDialogBox(IDD_CRPT)
	, m_hAlg()
	, m_KeyLength(256)
	, m_Key()
	, m_IV()
	, m_Nonce()
	, m_NonceLength(12)
	, m_Tag()
	, m_TagLength(16)
	, m_OriginalData()
	, m_OriginalDataDisplayMode(2)
	, m_OriginalDataCodePage(CP_UTF8)
	, m_EncryptedData()
	, m_EncryptedDataDisplayMode(1)
	, m_Processing(0)
	, m_Mode(MODE_IDLE)
	, m_bStatusSuccessful(TRUE)
	, m_szOriginalDataPath()
	, m_szEncryptedDataPath()
	, m_ActiveEditControlId(0)
	, m_bWrapData(TRUE)
{
	m_hAlg.Open(BCRYPT_AES_ALGORITHM);
	m_hAlg.ChainingMode = BCRYPT_CHAIN_MODE_CBC;
}


void CryptographyDialogBox::OnCreate()
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	MyDialogBox::OnCreate();
	HFONT hFont = GetApp<MyToolbox>().GetFontForData();
	SetFont(IDC_CRPT_KEY_EDIT, hFont);
	SetFont(IDC_CRPT_IV_EDIT, hFont);
	SetFont(IDC_CRPT_NONCE_EDIT, hFont);
	SetFont(IDC_CRPT_TAG_EDIT, hFont);
	SetFont(IDC_CRPT_ORG_EDIT, hFont);
	SetFont(IDC_CRPT_ENC_EDIT, hFont);
	int cm = ChainingModeToControlId(m_hAlg.ChainingMode);
	int od = OriginalDataDisplayModeToControlId(m_OriginalDataDisplayMode);
	int ed = EncryptedDataDisplayModeToControlId(m_EncryptedDataDisplayMode);
	RegistryKey hKey;
	LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, REG_SUBKEY_(Cryptography));
	if (rc == ERROR_SUCCESS)
	{
		cm = ChainingModeToControlId(RegistryValue::GetSZ(hKey, REG_NAME_CHAININGMODE, String(BCRYPT_CHAIN_MODE_CBC)));
		try
		{
			m_Key = ByteString::FromHex(RegistryValue::GetSZ(hKey, REG_NAME_KEY, String::Empty));
		}
		catch (...)
		{
			m_Key.Resize(0);
		}
		m_KeyLength = RegistryValue::GetDWORD(hKey, REG_NAME_KEYLENGTH, m_KeyLength);
		try
		{
			m_IV = ByteString::FromHex(RegistryValue::GetSZ(hKey, REG_NAME_IV, String::Empty));
		}
		catch (...)
		{
			m_IV.Resize(0);
		}
		try
		{
			m_Nonce = ByteString::FromHex(RegistryValue::GetSZ(hKey, REG_NAME_NONCE, String::Empty));
		}
		catch (...)
		{
			m_Nonce.Resize(0);
		}
		m_NonceLength = RegistryValue::GetDWORD(hKey, REG_NAME_NONCELENGTH, m_NonceLength);
		try
		{
			m_Tag = ByteString::FromHex(RegistryValue::GetSZ(hKey, REG_NAME_TAG, String::Empty));
		}
		catch (...)
		{
			m_Tag.Resize(0);
		}
		m_TagLength = RegistryValue::GetDWORD(hKey, REG_NAME_TAGLENGTH, m_TagLength);
		od = OriginalDataDisplayModeToControlId(RegistryValue::GetDWORD(hKey, REG_NAME_ORGDISPMODE));
		ed = EncryptedDataDisplayModeToControlId(RegistryValue::GetDWORD(hKey, REG_NAME_ENCDISPMODE));
	}
	SetText(IDC_CRPT_KEY_EDIT, String::ToHex(m_Key.Ptr, m_Key.Len));
	SetText(IDC_CRPT_IVLEN_STATIC, String(PRINTF, L"%lu bytes (Block Size)", m_hAlg.BlockLength));
	ButtonCheck(cm);
	ButtonCheck(KeyLengthToControlId(m_KeyLength));
	ButtonCheck(NonceLengthToControlId(m_NonceLength));
	ButtonCheck(TagLengthToControlId(m_TagLength));
	InitializeCodePageComboBox(IDC_CRPT_ORG_ENC_COMBO);
	ComboBoxSetSelection(IDC_CRPT_ORG_ENC_COMBO, m_OriginalDataCodePage);
	OnChainingModeChange(cm);
	ButtonCheck(od);
	OnOriginalDataDisplayModeChange(od);
	ButtonCheck(ed);
	OnEncryptedDataDisplayModeChange(ed);
	DisableWindow(IDC_CRPT_ENCRYPT_BUTTON);
	DisableWindow(IDC_CRPT_DECRYPT_BUTTON);
	ClearStatusText();
	InvalidateRect(GetChild(IDC_CRPT_KEY_EDIT), NULL, FALSE);
	InvalidateRect(GetChild(IDC_CRPT_IV_EDIT), NULL, FALSE);
	InvalidateRect(GetChild(IDC_CRPT_NONCE_EDIT), NULL, FALSE);
	InvalidateRect(GetChild(IDC_CRPT_TAG_EDIT), NULL, FALSE);
	m_menuView
		.Add(ResourceString(IDS_MENU_CRPT), IDM_VIEW_CRPT);
}


void CryptographyDialogBox::OnDestroy()
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	RegistryKey hKey;
	LSTATUS rc = hKey.Create(HKEY_CURRENT_USER, REG_SUBKEY_(Cryptography));
	if (rc == ERROR_SUCCESS)
	{
		RegistryValue::SetSZ(hKey, REG_NAME_CHAININGMODE, m_hAlg.ChainingMode);
		RegistryValue::SetDWORD(hKey, REG_NAME_KEYLENGTH, m_KeyLength);
		RegistryValue::SetSZ(hKey, REG_NAME_KEY, String::ToHex(m_Key.Ptr, m_Key.Len));
		RegistryValue::SetSZ(hKey, REG_NAME_IV, String::ToHex(m_IV.Ptr, m_IV.Len));
		RegistryValue::SetSZ(hKey, REG_NAME_NONCE, String::ToHex(m_Nonce.Ptr, m_Nonce.Len));
		RegistryValue::SetDWORD(hKey, REG_NAME_NONCELENGTH, m_NonceLength);
		RegistryValue::SetSZ(hKey, REG_NAME_TAG, String::ToHex(m_Tag.Ptr, m_Tag.Len));
		RegistryValue::SetDWORD(hKey, REG_NAME_TAGLENGTH, m_TagLength);
		RegistryValue::SetDWORD(hKey, REG_NAME_ORGDISPMODE, m_OriginalDataDisplayMode);
		RegistryValue::SetDWORD(hKey, REG_NAME_ENCDISPMODE, m_EncryptedDataDisplayMode);
	}
	SetFont(IDC_CRPT_KEY_EDIT, NULL);
	SetFont(IDC_CRPT_IV_EDIT, NULL);
	SetFont(IDC_CRPT_NONCE_EDIT, NULL);
	SetFont(IDC_CRPT_TAG_EDIT, NULL);
	SetFont(IDC_CRPT_ORG_EDIT, NULL);
	SetFont(IDC_CRPT_ENC_EDIT, NULL);
	MyDialogBox::OnDestroy();
}


void CryptographyDialogBox::OnTabSelectionChanging()
{
	MyDialogBox::OnTabSelectionChanging();
#ifdef CRYPTOGRAPHY_TIMER1SEC
	KillTimer(hwnd, CRYPTOGRAPHY_TIMER1SEC);
#endif
	m_menuView
		.Enable(IDM_VIEW_CRPT, MF_ENABLED);
}


void CryptographyDialogBox::OnTabSelectionChanged()
{
	MyDialogBox::OnTabSelectionChanged();
	UpdateMenus();
	m_menuView
		.Enable(IDM_VIEW_CRPT, MF_DISABLED);
#ifdef CRYPTOGRAPHY_TIMER1SEC
	SetTimer(hwnd, CRYPTOGRAPHY_TIMER1SEC, 1000, NULL);
#endif
}


void CryptographyDialogBox::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
	WindowLayoutSnapshot before, after;

	before.AddAllChildren(hDlg).Clone(after);

	after[IDC_CRPT_ORG_EDIT].right += cxDelta;
	after[IDC_CRPT_ENC_EDIT].right += cxDelta;
	MoveHorizontally(after[IDC_CRPT_ENCRYPT_BUTTON], cxDelta);
	MoveHorizontally(after[IDC_CRPT_DECRYPT_BUTTON], cxDelta);
	MoveHorizontally(after[IDC_CRPT_ORG_HEX_RADIO], cxDelta);
	MoveHorizontally(after[IDC_CRPT_ORG_BASE64_RADIO], cxDelta);
	MoveHorizontally(after[IDC_CRPT_ORG_RAW_RADIO], cxDelta);
	MoveHorizontally(after[IDC_CRPT_ORG_ENC_COMBO], cxDelta);
	MoveHorizontally(after[IDC_CRPT_ENC_HEX_RADIO], cxDelta);
	MoveHorizontally(after[IDC_CRPT_ENC_BASE64_RADIO], cxDelta);
	after[IDC_CRPT_STATUS_STATIC].cx += cxDelta;

	ExtendVertically(after[IDC_CRPT_ORG_EDIT], after[IDC_CRPT_ENC_EDIT], cyDelta);
	LONG dy = after[IDC_CRPT_ORG_EDIT].bottom - before[IDC_CRPT_ORG_EDIT].bottom;
	MoveVertically(after[IDC_CRPT_ENC_STATIC], dy);
	MoveVertically(after[IDC_CRPT_ENC_HEX_RADIO], dy);
	MoveVertically(after[IDC_CRPT_ENC_BASE64_RADIO], dy);
	MoveVertically(after[IDC_CRPT_STATUS_STATIC], cyDelta);

	after.Apply();
}


INT_PTR CryptographyDialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	if (m_Processing)
	{
		return TRUE;
	}
	UINT idChild = LOWORD(wParam);
	UINT idNotif = HIWORD(wParam);
	switch (idChild)
	{
	case IDC_CRPT_AESCBC_RADIO:
	case IDC_CRPT_AESECB_RADIO:
	case IDC_CRPT_AESCFB_RADIO:
	case IDC_CRPT_AESCCM_RADIO:
	case IDC_CRPT_AESGCM_RADIO:
		if (ButtonIsChecked(idChild))
		{
			OnChainingModeChange(idChild);
		}
		break;
	case IDC_CRPT_KEY128_RADIO:
	case IDC_CRPT_KEY192_RADIO:
	case IDC_CRPT_KEY256_RADIO:
		if (ButtonIsChecked(idChild))
		{
			OnKeyLengthChange(idChild);
		}
		break;
	case IDC_CRPT_NONCE11_RADIO:
	case IDC_CRPT_NONCE12_RADIO:
	case IDC_CRPT_NONCE13_RADIO:
	case IDC_CRPT_NONCE14_RADIO:
		if (ButtonIsChecked(idChild))
		{
			OnNonceLengthChange(idChild);
		}
		break;
	case IDC_CRPT_TAG4_RADIO:
	case IDC_CRPT_TAG6_RADIO:
	case IDC_CRPT_TAG8_RADIO:
	case IDC_CRPT_TAG10_RADIO:
	case IDC_CRPT_TAG12_RADIO:
	case IDC_CRPT_TAG14_RADIO:
	case IDC_CRPT_TAG16_RADIO:
		if (ButtonIsChecked(idChild))
		{
			OnTagLengthChange(idChild);
		}
		break;
	case IDC_CRPT_KEY_EDIT:
		switch (idNotif)
		{
		case EN_SETFOCUS:
			OnEditSetFocus(idChild);
			break;
		case EN_KILLFOCUS:
			OnEditKillFocus(idChild);
			break;
		case EN_CHANGE:
			OnKeyChange();
			break;
		default:
			break;
		}
		break;
	case IDC_CRPT_IV_EDIT:
		switch (idNotif)
		{
		case EN_SETFOCUS:
			OnEditSetFocus(idChild);
			break;
		case EN_KILLFOCUS:
			OnEditKillFocus(idChild);
			break;
		case EN_CHANGE:
			OnIVChange();
			break;
		default:
			break;
		}
		break;
	case IDC_CRPT_NONCE_EDIT:
		switch (idNotif)
		{
		case EN_SETFOCUS:
			OnEditSetFocus(idChild);
			break;
		case EN_KILLFOCUS:
			OnEditKillFocus(idChild);
			break;
		case EN_CHANGE:
			OnNonceChange();
			break;
		default:
			break;
		}
		break;
	case IDC_CRPT_TAG_EDIT:
		switch (idNotif)
		{
		case EN_SETFOCUS:
			OnEditSetFocus(idChild);
			break;
		case EN_KILLFOCUS:
			OnEditKillFocus(idChild);
			break;
		case EN_CHANGE:
			OnTagChange();
			break;
		default:
			break;
		}
		break;
	case IDC_CRPT_ORG_EDIT:
		switch (idNotif)
		{
		case EN_SETFOCUS:
			OnEditSetFocus(idChild);
			break;
		case EN_KILLFOCUS:
			OnEditKillFocus(idChild);
			break;
		case EN_CHANGE:
			OnOriginalDataChange();
			break;
		default:
			break;
		}
		break;
	case IDC_CRPT_ORG_HEX_RADIO:
	case IDC_CRPT_ORG_BASE64_RADIO:
	case IDC_CRPT_ORG_RAW_RADIO:
		if (ButtonIsChecked(idChild))
		{
			OnOriginalDataDisplayModeChange(idChild);
		}
		break;
	case IDC_CRPT_ORG_ENC_COMBO:
		if (idNotif == CBN_SELCHANGE)
		{
			OnOriginalDataDisplayCodePageChange();
		}
		break;
	case IDC_CRPT_ENC_EDIT:
		switch (idNotif)
		{
		case EN_SETFOCUS:
			OnEditSetFocus(idChild);
			break;
		case EN_KILLFOCUS:
			OnEditKillFocus(idChild);
			break;
		case EN_CHANGE:
			OnEncryptedDataChange();
			break;
		default:
			break;
		}
		break;
	case IDC_CRPT_ENC_HEX_RADIO:
	case IDC_CRPT_ENC_BASE64_RADIO:
		if (ButtonIsChecked(idChild))
		{
			OnEncryptedDataDisplayModeChange(idChild);
		}
		break;
	case IDC_CRPT_ENCRYPT_BUTTON:
		OnEncrypt();
		break;
	case IDC_CRPT_DECRYPT_BUTTON:
		OnDecrypt();
		break;
	default:
		return FALSE;
	}
	return TRUE;
}


#pragma warning(disable:4065)
INT_PTR CryptographyDialogBox::OnTimer(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
#ifdef CRYPTOGRAPHY_TIMER1SEC
	case CRYPTOGRAPHY_TIMER1SEC:
		break;
#endif
	default:
		break;
	}
	return 0;
}
#pragma warning(default:4065)


static const COLORREF RED = RGB(153, 0, 0);
static const COLORREF GREEN = RGB(51, 102, 0);
static const COLORREF BLUE = RGB(0, 0, 204);


INT_PTR CryptographyDialogBox::OnControlColorEdit(WPARAM wParam, LPARAM lParam)
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	HDC hdc = reinterpret_cast<HDC>(wParam);
	int id = GetDlgCtrlID(reinterpret_cast<HWND>(lParam));
	switch (id)
	{
	case IDC_CRPT_KEY_EDIT:
	{
		size_t required = m_KeyLength;
		SetTextColor(hdc, (m_Key.Len * 8 == required) ? GREEN : (m_Key.Len * 8 > required) ? BLUE : RED);
		break;
	}
	case IDC_CRPT_IV_EDIT:
	{
		size_t required = m_hAlg.BlockLength;
		SetTextColor(hdc, (m_IV.Len == required) ? GREEN : (m_IV.Len > required) ? BLUE : RED);
		break;
	}
	case IDC_CRPT_NONCE_EDIT:
	{
		size_t required = m_NonceLength;
		SetTextColor(hdc, (m_Nonce.Len == required) ? GREEN : (m_Nonce.Len > required) ? BLUE : RED);
		break;
	}
	case IDC_CRPT_TAG_EDIT:
	{
		size_t required = m_TagLength;
		SetTextColor(hdc, (m_Tag.Len == required) ? GREEN : (m_Tag.Len > required) ? BLUE : RED);
		break;
	}
	default:
		return 0;
	}
	return reinterpret_cast<INT_PTR>(GetStockObject(WHITE_BRUSH));
}


INT_PTR CryptographyDialogBox::OnControlColorStatic(WPARAM wParam, LPARAM lParam)
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	HDC hdc = reinterpret_cast<HDC>(wParam);
	int id = GetDlgCtrlID(reinterpret_cast<HWND>(lParam));
	switch (id)
	{
	case IDC_CRPT_STATUS_STATIC:
		if (m_bStatusSuccessful == FALSE)
		{
			SetTextColor(hdc, RED);
			SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
			break;
		}
		return 0;
	default:
		return 0;
	}
	return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_3DFACE));
}


void CryptographyDialogBox::OnLoad1From()
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	String szTitle(ResourceString(IDS_LOADTEXTFROMFILE));
	StringBuffer szPath2(MAX_PATH, m_szOriginalDataPath);
	OPENFILENAMEW ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szPath2;
	ofn.nMaxFile = static_cast<DWORD>(szPath2.Cap);
	ofn.lpstrTitle = szTitle;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (!GetOpenFileNameW(&ofn))
	{
		return;
	}
	m_szOriginalDataPath = szPath2;
	FileMapper fm(m_szOriginalDataPath);
	if (fm.Len > INT_MAX - 1)
	{
		MessageBoxW(hwnd, ResourceString(IDS_MSG_TOO_LARGE_FILE), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
		return;
	}
	SetMode(MODE_ENCRYPTION);
	m_OriginalData = ByteString(fm.Ptr, fm.Len);
	SetText(IDC_CRPT_ORG_EDIT, OriginalDataToString());
	UpdateMenus();
	UpdateButtons();
}


void CryptographyDialogBox::OnSave1As()
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	String szTitle(ResourceString(IDS_SAVETEXTASFILE));
	StringBuffer szPath2(MAX_PATH, m_szOriginalDataPath);
	OPENFILENAMEW ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szPath2;
	ofn.nMaxFile = static_cast<DWORD>(szPath2.Cap);
	ofn.lpstrTitle = szTitle;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
	if (!GetSaveFileNameW(&ofn))
	{
		return;
	}
	m_szOriginalDataPath = szPath2;
	try
	{
		if (!m_OriginalData.Len)
		{
			FileWriter(m_szOriginalDataPath).Write("", 0);
		}
		else
		{
			FileWriter(m_szOriginalDataPath).Write(m_OriginalData.Ptr, m_OriginalData.Len);
		}
	}
	catch (Win32Exception e)
	{
		String szMessage(PRINTF, L"%s\n%s", e.Message, ErrorMessage::Get(e.Error));
		MessageBoxW(hwnd, szMessage, ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
	}
	catch (Exception e)
	{
		MessageBoxW(hwnd, ResourceString(IDS_MSG_FILE_WRITE_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
	}
}


void CryptographyDialogBox::OnLoad2From()
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	String szTitle(ResourceString(IDS_LOADTEXTFROMFILE));
	StringBuffer szPath2(MAX_PATH, m_szEncryptedDataPath);
	OPENFILENAMEW ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szPath2;
	ofn.nMaxFile = static_cast<DWORD>(szPath2.Cap);
	ofn.lpstrTitle = szTitle;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (!GetOpenFileNameW(&ofn))
	{
		return;
	}
	m_szEncryptedDataPath = szPath2;
	FileMapper fm(m_szEncryptedDataPath);
	if (fm.Len > INT_MAX - 1)
	{
		MessageBoxW(hwnd, ResourceString(IDS_MSG_TOO_LARGE_FILE), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
		return;
	}
	SetMode(MODE_DECRYPTION);
	m_EncryptedData = ByteString(fm.Ptr, fm.Len);
	SetText(IDC_CRPT_ENC_EDIT, EncryptedDataToString());
	UpdateMenus();
	UpdateButtons();
}


void CryptographyDialogBox::OnSave2As()
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	String szTitle(ResourceString(IDS_SAVETEXTASFILE));
	StringBuffer szPath2(MAX_PATH, m_szEncryptedDataPath);
	OPENFILENAMEW ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szPath2;
	ofn.nMaxFile = static_cast<DWORD>(szPath2.Cap);
	ofn.lpstrTitle = szTitle;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
	if (!GetSaveFileNameW(&ofn))
	{
		return;
	}
	m_szEncryptedDataPath = szPath2;
	try
	{
		if (!m_EncryptedData.Len)
		{
			FileWriter(m_szEncryptedDataPath).Write("", 0);
		}
		else
		{
			FileWriter(m_szEncryptedDataPath).Write(m_EncryptedData.Ptr, m_EncryptedData.Len);
		}
	}
	catch (Win32Exception e)
	{
		String szMessage(PRINTF, L"%s\n%s", e.Message, ErrorMessage::Get(e.Error));
		MessageBoxW(hwnd, szMessage, ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
	}
	catch (Exception e)
	{
		MessageBoxW(hwnd, ResourceString(IDS_MSG_FILE_WRITE_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
	}
}


void CryptographyDialogBox::OnCut()
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	switch (m_ActiveEditControlId)
	{
	case IDC_CRPT_KEY_EDIT:
		EditCut(m_ActiveEditControlId);
		OnKeyChange();
		break;
	case IDC_CRPT_IV_EDIT:
		EditCut(m_ActiveEditControlId);
		OnIVChange();
		break;
	case IDC_CRPT_NONCE_EDIT:
		EditCut(m_ActiveEditControlId);
		OnNonceChange();
		break;
	case IDC_CRPT_TAG_EDIT:
		EditCut(m_ActiveEditControlId);
		OnTagChange();
		break;
	case IDC_CRPT_ORG_EDIT:
		if (m_Mode == MODE_IDLE || m_Mode == MODE_ENCRYPTION)
		{
			EditCut(m_ActiveEditControlId);
			OnOriginalDataChange();
		}
		break;
	case IDC_CRPT_ENC_EDIT:
		if (m_Mode == MODE_IDLE || m_Mode == MODE_DECRYPTION)
		{
			EditCut(m_ActiveEditControlId);
			OnEncryptedDataChange();
		}
		break;
	default:
		break;
	}
}


void CryptographyDialogBox::OnCopy()
{
	if (m_ActiveEditControlId)
	{
		EditCopy(m_ActiveEditControlId);
	}
}


void CryptographyDialogBox::OnPaste()
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	switch (m_ActiveEditControlId)
	{
	case IDC_CRPT_KEY_EDIT:
		EditPaste(m_ActiveEditControlId);
		OnKeyChange();
		break;
	case IDC_CRPT_IV_EDIT:
		EditPaste(m_ActiveEditControlId);
		OnIVChange();
		break;
	case IDC_CRPT_NONCE_EDIT:
		EditPaste(m_ActiveEditControlId);
		OnNonceChange();
		break;
	case IDC_CRPT_TAG_EDIT:
		EditPaste(m_ActiveEditControlId);
		OnTagChange();
		break;
	case IDC_CRPT_ORG_EDIT:
		if (m_Mode == MODE_IDLE || m_Mode == MODE_ENCRYPTION)
		{
			EditPaste(m_ActiveEditControlId);
			OnOriginalDataChange();
		}
		break;
	case IDC_CRPT_ENC_EDIT:
		if (m_Mode == MODE_IDLE || m_Mode == MODE_DECRYPTION)
		{
			EditPaste(m_ActiveEditControlId);
			OnEncryptedDataChange();
		}
		break;
	default:
		break;
	}
}


void CryptographyDialogBox::OnDelete()
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	switch (m_ActiveEditControlId)
	{
	case IDC_CRPT_KEY_EDIT:
		EditDelete(m_ActiveEditControlId);
		OnKeyChange();
		break;
	case IDC_CRPT_IV_EDIT:
		EditDelete(m_ActiveEditControlId);
		OnIVChange();
		break;
	case IDC_CRPT_NONCE_EDIT:
		EditDelete(m_ActiveEditControlId);
		OnNonceChange();
		break;
	case IDC_CRPT_TAG_EDIT:
		EditDelete(m_ActiveEditControlId);
		OnTagChange();
		break;
	case IDC_CRPT_ORG_EDIT:
		if (m_Mode == MODE_IDLE || m_Mode == MODE_ENCRYPTION)
		{
			EditDelete(m_ActiveEditControlId);
			OnOriginalDataChange();
		}
		break;
	case IDC_CRPT_ENC_EDIT:
		if (m_Mode == MODE_IDLE || m_Mode == MODE_DECRYPTION)
		{
			EditDelete(m_ActiveEditControlId);
			OnEncryptedDataChange();
		}
		break;
	default:
		break;
	}
}


void CryptographyDialogBox::OnSelectAll()
{
	if (m_ActiveEditControlId)
	{
		EditSelectAll(m_ActiveEditControlId);
	}
}


void CryptographyDialogBox::OnCopyAll()
{
	if (m_ActiveEditControlId)
	{
		EditSelectAll(m_ActiveEditControlId);
		EditCopy(m_ActiveEditControlId);
	}
}


void CryptographyDialogBox::OnClear()
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	SetMode(MODE_IDLE);
	m_OriginalData.Resize(0);
	SetText(IDC_CRPT_ORG_EDIT);
	m_EncryptedData.Resize(0);
	SetText(IDC_CRPT_ENC_EDIT);
	ClearStatusText();
	UpdateMenus();
	UpdateButtons();
}


void CryptographyDialogBox::OnSettingChanged(UINT id)
{
	switch (id)
	{
	case IDM_SETTINGS_WRAPDATA:
	{
		WhileInScope<int> wis(m_Processing, 1, m_Processing);
		m_bWrapData ^= TRUE;
		SetText(IDC_CRPT_ORG_EDIT, OriginalDataToString());
		SetText(IDC_CRPT_ENC_EDIT, EncryptedDataToString());
		UpdateMenus();
		break;
	}
	default:
		break;
	}
}


void CryptographyDialogBox::OnEncrypt()
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	try
	{
		if (m_Key.Len * 8 < m_KeyLength)
		{
			throw Exception(L"Private key is shorter than %d bytes long.", m_KeyLength / 8);
		}
		ByteString encrypted;
		BCryptKeyHandle hKey;
		hKey.Generate(m_hAlg, m_Key.Ptr, m_Key.Len);
		if (IS_AES_CBC(m_hAlg.ChainingMode) || IS_AES_CFB(m_hAlg.ChainingMode))
		{
			size_t required = m_hAlg.BlockLength;
			if (m_IV.Len < required)
			{
				throw Exception(L"IV is shorter than %zu bytes long.", required);
			}
			Buffer<CHAR> iv(required);
			memcpy_s(iv.Ptr, iv.Len, m_IV.Ptr, iv.Len);
			encrypted = hKey.Encrypt(m_OriginalData.Ptr, m_OriginalData.Len, iv.Ptr, iv.Len);
		}
		else if (IS_AES_ECB(m_hAlg.ChainingMode))
		{
			encrypted = hKey.Encrypt(m_OriginalData.Ptr, m_OriginalData.Len);
		}
		else if (IS_AES_CCM(m_hAlg.ChainingMode) || IS_AES_GCM(m_hAlg.ChainingMode))
		{
			if (m_Nonce.Len < m_NonceLength)
			{
				throw Exception(L"Nonce is shorter than %d bytes long.", m_NonceLength);
			}
			Buffer<CHAR> nonce(m_NonceLength);
			memcpy_s(nonce.Ptr, nonce.Len, m_Nonce.Ptr, nonce.Len);
			BCryptAuthenticatedCipherModeInfo info;
			info
				.SetNonce(nonce.Ptr, nonce.Len)
				.SetTagSize(m_TagLength);
			encrypted = hKey.Encrypt(m_OriginalData.Ptr, m_OriginalData.Len, info, NULL, 0);
			m_Tag = ByteString(info.pbTag, info.cbTag);
			SetText(IDC_CRPT_TAG_EDIT, String::ToHex(m_Tag.Ptr, m_Tag.Len));
		}
		else
		{
			// SHOULD NEVER REACH HERE
			throw Exception(L"Bad chaining mode: %s", m_hAlg.ChainingMode);
		}
		m_EncryptedData = encrypted;
		SetText(IDC_CRPT_ENC_EDIT, EncryptedDataToString());
		m_bStatusSuccessful = TRUE;
		SetStatusText(st, L"Encrypted; %zu bytes in, %zu bytes out.", m_OriginalData.Len, m_EncryptedData.Len);
	}
	catch (Exception e)
	{
		m_bStatusSuccessful = FALSE;
		SetStatusText(st, L"%s", e.Message);
	}
	UpdateMenus();
	UpdateButtons();
}


void CryptographyDialogBox::OnDecrypt()
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	try
	{
		if (m_Key.Len * 8 < m_KeyLength)
		{
			throw Exception(L"Private key is shorter than %d bytes long.", m_KeyLength / 8);
		}
		ByteString decrypted;
		BCryptKeyHandle hKey;
		hKey.Generate(m_hAlg, m_Key.Ptr, m_Key.Len);
		if (IS_AES_CBC(m_hAlg.ChainingMode) || IS_AES_CFB(m_hAlg.ChainingMode))
		{
			size_t required = m_hAlg.BlockLength;
			if (m_IV.Len < required)
			{
				throw Exception(L"IV is shorter than %zu bytes long.", required);
			}
			Buffer<CHAR> iv(required);
			memcpy_s(iv.Ptr, iv.Len, m_IV.Ptr, iv.Len);
			decrypted = hKey.Decrypt(m_EncryptedData.Ptr, m_EncryptedData.Len, iv.Ptr, iv.Len);
		}
		else if (IS_AES_ECB(m_hAlg.ChainingMode))
		{
			decrypted = hKey.Decrypt(m_EncryptedData.Ptr, m_EncryptedData.Len);
		}
		else if (IS_AES_CCM(m_hAlg.ChainingMode) || IS_AES_GCM(m_hAlg.ChainingMode))
		{
			if (m_Nonce.Len < m_NonceLength)
			{
				throw Exception(L"Nonce is shorter than %d bytes long.", m_NonceLength);
			}
			Buffer<CHAR> nonce(m_NonceLength);
			memcpy_s(nonce.Ptr, nonce.Len, m_Nonce.Ptr, nonce.Len);
			if (m_Tag.Len < m_TagLength)
			{
				throw Exception(L"Tag is shorter than %d bytes long.", m_TagLength);
			}
			Buffer<CHAR> tag(m_NonceLength);
			memcpy_s(tag.Ptr, tag.Len, m_Tag.Ptr, tag.Len);
			BCryptAuthenticatedCipherModeInfo info;
			info
				.SetNonce(nonce.Ptr, nonce.Len)
				.SetTag(tag.Ptr, tag.Len);
			decrypted = hKey.Decrypt(m_EncryptedData.Ptr, m_EncryptedData.Len, info, NULL, 0);
		}
		else
		{
			// SHOULD NEVER REACH HERE
			throw Exception(L"Bad chaining mode: %s", m_hAlg.ChainingMode);
		}
		m_OriginalData = decrypted;
		SetText(IDC_CRPT_ORG_EDIT, OriginalDataToString());
		m_bStatusSuccessful = TRUE;
		SetStatusText(st, L"Decrypted; %zu bytes in, %zu bytes out.", m_EncryptedData.Len, m_OriginalData.Len);
	}
	catch (Exception e)
	{
		m_bStatusSuccessful = FALSE;
		SetStatusText(st, L"%s", e.Message);
	}
	UpdateMenus();
	UpdateButtons();
}


void CryptographyDialogBox::OnChainingModeChange(int id)
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	switch (id)
	{
	case IDC_CRPT_AESCBC_RADIO:
	default:
		m_hAlg.ChainingMode = BCRYPT_CHAIN_MODE_CBC;
		EnableWindow(IDC_CRPT_IV_EDIT);
		DisableWindow(IDC_CRPT_NONCE_EDIT);
		DisableWindow(IDC_CRPT_NONCE11_RADIO);
		DisableWindow(IDC_CRPT_NONCE12_RADIO);
		DisableWindow(IDC_CRPT_NONCE13_RADIO);
		DisableWindow(IDC_CRPT_NONCE14_RADIO);
		DisableWindow(IDC_CRPT_TAG_EDIT);
		DisableWindow(IDC_CRPT_TAG4_RADIO);
		DisableWindow(IDC_CRPT_TAG6_RADIO);
		DisableWindow(IDC_CRPT_TAG8_RADIO);
		DisableWindow(IDC_CRPT_TAG10_RADIO);
		DisableWindow(IDC_CRPT_TAG12_RADIO);
		DisableWindow(IDC_CRPT_TAG14_RADIO);
		DisableWindow(IDC_CRPT_TAG16_RADIO);
		SetText(IDC_CRPT_IV_EDIT, String::ToHex(m_IV.Ptr, m_IV.Len));
		SetText(IDC_CRPT_NONCE_EDIT);
		SetText(IDC_CRPT_TAG_EDIT);
		break;
	case IDC_CRPT_AESECB_RADIO:
		m_hAlg.ChainingMode = BCRYPT_CHAIN_MODE_ECB;
		DisableWindow(IDC_CRPT_IV_EDIT);
		DisableWindow(IDC_CRPT_NONCE_EDIT);
		DisableWindow(IDC_CRPT_NONCE11_RADIO);
		DisableWindow(IDC_CRPT_NONCE12_RADIO);
		DisableWindow(IDC_CRPT_NONCE13_RADIO);
		DisableWindow(IDC_CRPT_NONCE14_RADIO);
		DisableWindow(IDC_CRPT_TAG_EDIT);
		DisableWindow(IDC_CRPT_TAG4_RADIO);
		DisableWindow(IDC_CRPT_TAG6_RADIO);
		DisableWindow(IDC_CRPT_TAG8_RADIO);
		DisableWindow(IDC_CRPT_TAG10_RADIO);
		DisableWindow(IDC_CRPT_TAG12_RADIO);
		DisableWindow(IDC_CRPT_TAG14_RADIO);
		DisableWindow(IDC_CRPT_TAG16_RADIO);
		SetText(IDC_CRPT_IV_EDIT);
		SetText(IDC_CRPT_NONCE_EDIT);
		SetText(IDC_CRPT_TAG_EDIT);
		break;
	case IDC_CRPT_AESCFB_RADIO:
		m_hAlg.ChainingMode = BCRYPT_CHAIN_MODE_CFB;
		EnableWindow(IDC_CRPT_IV_EDIT);
		DisableWindow(IDC_CRPT_NONCE_EDIT);
		DisableWindow(IDC_CRPT_NONCE11_RADIO);
		DisableWindow(IDC_CRPT_NONCE12_RADIO);
		DisableWindow(IDC_CRPT_NONCE13_RADIO);
		DisableWindow(IDC_CRPT_NONCE14_RADIO);
		DisableWindow(IDC_CRPT_TAG_EDIT);
		DisableWindow(IDC_CRPT_TAG4_RADIO);
		DisableWindow(IDC_CRPT_TAG6_RADIO);
		DisableWindow(IDC_CRPT_TAG8_RADIO);
		DisableWindow(IDC_CRPT_TAG10_RADIO);
		DisableWindow(IDC_CRPT_TAG12_RADIO);
		DisableWindow(IDC_CRPT_TAG14_RADIO);
		DisableWindow(IDC_CRPT_TAG16_RADIO);
		SetText(IDC_CRPT_IV_EDIT, String::ToHex(m_IV.Ptr, m_IV.Len));
		SetText(IDC_CRPT_NONCE_EDIT);
		SetText(IDC_CRPT_TAG_EDIT);
		break;
	case IDC_CRPT_AESCCM_RADIO:
		m_hAlg.ChainingMode = BCRYPT_CHAIN_MODE_CCM;
		EnableWindow(IDC_CRPT_IV_EDIT);
		EnableWindow(IDC_CRPT_NONCE_EDIT);
		EnableWindow(IDC_CRPT_NONCE11_RADIO);
		EnableWindow(IDC_CRPT_NONCE12_RADIO);
		EnableWindow(IDC_CRPT_NONCE13_RADIO);
		EnableWindow(IDC_CRPT_NONCE14_RADIO);
		EnableWindow(IDC_CRPT_TAG_EDIT);
		EnableWindow(IDC_CRPT_TAG4_RADIO);
		EnableWindow(IDC_CRPT_TAG6_RADIO);
		EnableWindow(IDC_CRPT_TAG8_RADIO);
		EnableWindow(IDC_CRPT_TAG10_RADIO);
		EnableWindow(IDC_CRPT_TAG12_RADIO);
		EnableWindow(IDC_CRPT_TAG14_RADIO);
		EnableWindow(IDC_CRPT_TAG16_RADIO);
		SetText(IDC_CRPT_IV_EDIT, String::ToHex(m_IV.Ptr, m_IV.Len));
		SetText(IDC_CRPT_NONCE_EDIT, String::ToHex(m_Nonce.Ptr, m_Nonce.Len));
		SetText(IDC_CRPT_TAG_EDIT, String::ToHex(m_Tag.Ptr, m_Tag.Len));
		break;
	case IDC_CRPT_AESGCM_RADIO:
		m_hAlg.ChainingMode = BCRYPT_CHAIN_MODE_GCM;
		EnableWindow(IDC_CRPT_IV_EDIT);
		EnableWindow(IDC_CRPT_NONCE_EDIT);
		EnableWindow(IDC_CRPT_NONCE11_RADIO);
		EnableWindow(IDC_CRPT_NONCE12_RADIO);
		EnableWindow(IDC_CRPT_NONCE13_RADIO);
		EnableWindow(IDC_CRPT_NONCE14_RADIO);
		EnableWindow(IDC_CRPT_TAG_EDIT);
		EnableWindow(IDC_CRPT_TAG4_RADIO);
		EnableWindow(IDC_CRPT_TAG6_RADIO);
		EnableWindow(IDC_CRPT_TAG8_RADIO);
		EnableWindow(IDC_CRPT_TAG10_RADIO);
		EnableWindow(IDC_CRPT_TAG12_RADIO);
		EnableWindow(IDC_CRPT_TAG14_RADIO);
		EnableWindow(IDC_CRPT_TAG16_RADIO);
		SetText(IDC_CRPT_IV_EDIT, String::ToHex(m_IV.Ptr, m_IV.Len));
		SetText(IDC_CRPT_NONCE_EDIT, String::ToHex(m_Nonce.Ptr, m_Nonce.Len));
		SetText(IDC_CRPT_TAG_EDIT, String::ToHex(m_Tag.Ptr, m_Tag.Len));
		break;
	}
	SetMode(m_Mode);
	UpdateButtons();
}


void CryptographyDialogBox::OnKeyLengthChange(int id)
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	m_KeyLength = ControlIdToKeyLength(id);
	InvalidateRect(GetChild(IDC_CRPT_KEY_EDIT), NULL, FALSE);
	SetMode(m_Mode);
}


void CryptographyDialogBox::OnNonceLengthChange(int id)
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	m_NonceLength = ControlIdToNonceLength(id);
	InvalidateRect(GetChild(IDC_CRPT_NONCE_EDIT), NULL, FALSE);
	SetMode(m_Mode);
}


void CryptographyDialogBox::OnTagLengthChange(int id)
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	m_TagLength = ControlIdToTagLength(id);
	InvalidateRect(GetChild(IDC_CRPT_TAG_EDIT), NULL, FALSE);
	SetMode(m_Mode);
}


void CryptographyDialogBox::OnEditSetFocus(int id)
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	_InterlockedExchange(&m_ActiveEditControlId, id);
	UpdateMenus();
}


void CryptographyDialogBox::OnEditKillFocus(int id)
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	if (_InterlockedCompareExchange(&m_ActiveEditControlId, 0, id) == id)
	{
		UpdateMenus();
	}
}


void CryptographyDialogBox::OnKeyChange()
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	try
	{
		m_Key = ByteString::FromHex(GetText(IDC_CRPT_KEY_EDIT).Trim());
	}
	catch (...)
	{
		m_Key.Resize(0);
	}
	InvalidateRect(GetChild(IDC_CRPT_KEY_EDIT), NULL, FALSE);
	SetMode(m_Mode);
	UpdateMenus();
	UpdateButtons();
}


void CryptographyDialogBox::OnIVChange()
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	try
	{
		m_IV = ByteString::FromHex(GetText(IDC_CRPT_IV_EDIT).Trim());
	}
	catch (...)
	{
		m_IV.Resize(0);
	}
	InvalidateRect(GetChild(IDC_CRPT_IV_EDIT), NULL, FALSE);
	SetMode(m_Mode);
	UpdateMenus();
	UpdateButtons();
}


void CryptographyDialogBox::OnNonceChange()
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	try
	{
		m_Nonce = ByteString::FromHex(GetText(IDC_CRPT_NONCE_EDIT).Trim());
	}
	catch (...)
	{
		m_Nonce.Resize(0);
	}
	InvalidateRect(GetChild(IDC_CRPT_NONCE_EDIT), NULL, FALSE);
	SetMode(m_Mode);
	UpdateMenus();
	UpdateButtons();
}


void CryptographyDialogBox::OnTagChange()
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	try
	{
		m_Tag = ByteString::FromHex(GetText(IDC_CRPT_TAG_EDIT).Trim());
	}
	catch (...)
	{
		m_Tag.Resize(0);
	}
	InvalidateRect(GetChild(IDC_CRPT_TAG_EDIT), NULL, FALSE);
	SetMode(m_Mode);
	UpdateMenus();
	UpdateButtons();
}


void CryptographyDialogBox::OnOriginalDataChange()
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	String szText = GetText(IDC_CRPT_ORG_EDIT);
	if (!szText.Len)
	{
		SetMode(MODE_IDLE);
		m_OriginalData.Resize(0);
		ClearStatusText();
	}
	else
	{
		SetMode(MODE_ENCRYPTION);
		if (m_OriginalDataDisplayMode == 0)
		{
			try
			{
				m_OriginalData = ByteString::FromHex(szText);
				ClearStatusText();
			}
			catch (Exception e)
			{
				m_OriginalData.Resize(0);
				m_bStatusSuccessful = FALSE;
				SetStatusText(L"%s", e.Message);
			}
		}
		else if (m_OriginalDataDisplayMode == 1)
		{
			Base64Decoder dec;
			if (dec.Parse(szText))
			{
				m_OriginalData = dec;
				ClearStatusText();
			}
			else
			{
				m_OriginalData.Resize(0);
				m_bStatusSuccessful = FALSE;
				SetStatusText(L"BASE64 parse error.");
			}
		}
		else // if (m_OriginalDataDisplayMode == 2)
		{
			if (m_OriginalDataCodePage == 1200)
			{
				m_OriginalData = ByteString(szText.Ptr, szText.Len * sizeof(WCHAR));
				ClearStatusText();
			}
			else if (m_OriginalDataCodePage == CP_UTF8)
			{
				StringUTF8 translated(szText);
				m_OriginalData = ByteString(translated.Ptr, translated.Len);
				ClearStatusText();
			}
			else
			{
				int cb = WideCharToMultiByte(m_OriginalDataCodePage, 0, szText, (int)szText.Len, NULL, 0, NULL, NULL);
				if (cb > 0)
				{
					Buffer<CHAR> mbs(cb);
					WideCharToMultiByte(m_OriginalDataCodePage, 0, szText, (int)szText.Len, mbs, cb, NULL, NULL);
					m_OriginalData = ByteString(mbs.Ptr, mbs.Len);
					ClearStatusText();
				}
				else
				{
					m_OriginalData.Resize(0);
					m_bStatusSuccessful = FALSE;
					SetStatusText(L"Unconvertible character(s) contained.");
				}
			}
		}
	}
	UpdateMenus();
	UpdateButtons();
}


void CryptographyDialogBox::OnOriginalDataDisplayModeChange(int id)
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	if (m_Mode == MODE_ENCRYPTION)
	{
		ClearStatusText();
	}
	m_OriginalDataDisplayMode = id - IDC_CRPT_ORG_HEX_RADIO;
	SetText(IDC_CRPT_ORG_EDIT, OriginalDataToString());
}


void CryptographyDialogBox::OnOriginalDataDisplayCodePageChange()
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	m_OriginalDataCodePage = ComboBoxGetSelection(IDC_CRPT_ORG_ENC_COMBO, CP_UTF8);
	if (m_OriginalDataDisplayMode == 2)
	{
		SetText(IDC_CRPT_ORG_EDIT, OriginalDataToString());
	}
}


String CryptographyDialogBox::OriginalDataToString()
{
	static const size_t N = 54;
	switch (m_OriginalDataDisplayMode)
	{
	case 0:
		if (m_bWrapData)
		{
			return String::ToHex(m_OriginalData.Ptr, m_OriginalData.Len).Wrap(N * 2);
		}
		else
		{
			return String::ToHex(m_OriginalData.Ptr, m_OriginalData.Len);
		}
	case 1:
		if (m_bWrapData)
		{
			return String(Base64Encoder().Append(m_OriginalData.Ptr, m_OriginalData.Len).End()).Wrap((N / 3) * 4);
		}
		else
		{
			return String(Base64Encoder().Append(m_OriginalData.Ptr, m_OriginalData.Len).End());
		}
	case 2:
		if (m_OriginalDataCodePage == CP_UTF16)
		{
			return String((PCWSTR)m_OriginalData.Ptr, m_OriginalData.Len / sizeof(WCHAR));
		}
		else
		{
			return String(m_OriginalDataCodePage, (PCSTR)m_OriginalData.Ptr, m_OriginalData.Len);
		}
	default: // SHOULD NEVER REACH HERE
		return String(L"BAD DISPLAY MODE");
	}
}


void CryptographyDialogBox::OnEncryptedDataChange()
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	String szText = GetText(IDC_CRPT_ENC_EDIT);
	if (!szText.Len)
	{
		SetMode(MODE_IDLE);
		m_EncryptedData.Resize(0);
		ClearStatusText();
	}
	else
	{
		SetMode(MODE_DECRYPTION);
		if (m_EncryptedDataDisplayMode == 0)
		{
			try
			{
				m_EncryptedData = ByteString::FromHex(szText);
				ClearStatusText();
			}
			catch (Exception e)
			{
				m_EncryptedData.Resize(0);
				m_bStatusSuccessful = FALSE;
				SetStatusText(L"%s", e.Message);
			}
		}
		else if (m_EncryptedDataDisplayMode == 1)
		{
			Base64Decoder dec;
			if (dec.Parse(szText))
			{
				m_EncryptedData = dec;
				ClearStatusText();
			}
			else
			{
				m_EncryptedData.Resize(0);
				m_bStatusSuccessful = FALSE;
				SetStatusText(L"BASE64 parse error.");
			}
		}
	}
	UpdateMenus();
	UpdateButtons();
}


void CryptographyDialogBox::OnEncryptedDataDisplayModeChange(int id)
{
	WhileInScope<int> wis(m_Processing, 1, m_Processing);
	if (m_Mode == MODE_DECRYPTION)
	{
		ClearStatusText();
	}
	m_EncryptedDataDisplayMode = id - IDC_CRPT_ENC_HEX_RADIO;
	SetText(IDC_CRPT_ENC_EDIT, EncryptedDataToString());
}


String CryptographyDialogBox::EncryptedDataToString()
{
	static const size_t N = 54;
	switch (m_EncryptedDataDisplayMode)
	{
	case 0:
		if (m_bWrapData)
		{
			return String::ToHex(m_EncryptedData.Ptr, m_EncryptedData.Len).Wrap(N * 2);
		}
		else
		{
			return String::ToHex(m_EncryptedData.Ptr, m_EncryptedData.Len);
		}
	case 1:
		if (m_bWrapData)
		{
			return String(Base64Encoder().Append(m_EncryptedData.Ptr, m_EncryptedData.Len).End()).Wrap((N / 3) * 4);
		}
		else
		{
			return String(Base64Encoder().Append(m_EncryptedData.Ptr, m_EncryptedData.Len).End());
		}
	default: // SHOULD NEVER REACH HERE
		return String(L"BAD DISPLAY MODE");
	}
}


void CryptographyDialogBox::SetMode(int value)
{
	switch (value)
	{
	case MODE_IDLE:
		switch (m_Mode)
		{
		case MODE_IDLE:
			break;
		case MODE_ENCRYPTION:
			if (m_EncryptedData.Len)
			{
				ClearStatusText();
				m_EncryptedData.Resize(0);
				SetText(IDC_CRPT_ENC_EDIT);
			}
			EditSetReadOnly(IDC_CRPT_ENC_EDIT, FALSE);
			if (IS_AES_CCM(m_hAlg.ChainingMode) || IS_AES_GCM(m_hAlg.ChainingMode))
			{
				EditSetReadOnly(IDC_CRPT_TAG_EDIT, FALSE);
			}
			EnableWindow(IDC_CRPT_ORG_ENC_COMBO);
			break;
		case MODE_DECRYPTION:
			if (m_OriginalData.Len)
			{
				ClearStatusText();
				m_OriginalData.Resize(0);
				SetText(IDC_CRPT_ORG_EDIT);
			}
			EditSetReadOnly(IDC_CRPT_ORG_EDIT, FALSE);
			break;
		default:
			break;
		}
		break;
	case MODE_ENCRYPTION:
		switch (m_Mode)
		{
		case MODE_IDLE:
			EditSetReadOnly(IDC_CRPT_ENC_EDIT);
			if (IS_AES_CCM(m_hAlg.ChainingMode) || IS_AES_GCM(m_hAlg.ChainingMode))
			{
				EditSetReadOnly(IDC_CRPT_TAG_EDIT);
			}
			DisableWindow(IDC_CRPT_ORG_ENC_COMBO);
			break;
		case MODE_ENCRYPTION:
			if (m_EncryptedData.Len)
			{
				ClearStatusText();
				m_EncryptedData.Resize(0);
				SetText(IDC_CRPT_ENC_EDIT);
			}
			break;
		case MODE_DECRYPTION:
		default:
			break;
		}
		break;
	case MODE_DECRYPTION:
		switch (m_Mode)
		{
		case MODE_IDLE:
			EditSetReadOnly(IDC_CRPT_ORG_EDIT);
			break;
		case MODE_DECRYPTION:
			if (m_OriginalData.Len)
			{
				ClearStatusText();
				m_OriginalData.Resize(0);
				SetText(IDC_CRPT_ORG_EDIT);
			}
			break;
		case MODE_ENCRYPTION:
		default:
			break;
		}
		break;
	default:
		break;
	}
	m_Mode = value;
}


void CryptographyDialogBox::ClearStatusText()
{
	if (m_bStatusSuccessful == TRUE || m_bStatusSuccessful == FALSE)
	{
		SetText(IDC_CRPT_STATUS_STATIC);
		m_bStatusSuccessful = -1;
	}
}


void CryptographyDialogBox::SetStatusText(PCWSTR pszFormat, ...) const
{
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	va_list argList;
	va_start(argList, pszFormat);
	SetStatusText(st, pszFormat, argList);
	va_end(argList);
}


void CryptographyDialogBox::SetStatusText(const SYSTEMTIME& st, PCWSTR pszFormat, ...) const
{
	va_list argList;
	va_start(argList, pszFormat);
	SetStatusText(st, pszFormat, argList);
	va_end(argList);
}


void CryptographyDialogBox::SetStatusText(const SYSTEMTIME& st, PCWSTR pszFormat, va_list argList) const
{
	SetText(IDC_CRPT_STATUS_STATIC,
		String(PRINTF, L"%04d-%02d-%02d %02d:%02d:%02d  ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond)
		.VaAppendFormat(pszFormat, argList));
}


void CryptographyDialogBox::UpdateMenus()
{
	UINT uFlagsR = 0U;
	UINT uFlagsW = 0U;
	m_menuFile
		.RemoveAll()
		.Add(ResourceString(IDS_MENU_LOADORGFROM), IDM_FILE_LOAD1FROM, (m_Mode == MODE_IDLE || m_Mode == MODE_ENCRYPTION) ? 0U : MF_DISABLED)
		.Add(ResourceString(IDS_MENU_SAVEORGAS), IDM_FILE_SAVE1AS, (m_OriginalData.Len > 0) ? 0U : MF_DISABLED)
		.AddSeparator()
		.Add(ResourceString(IDS_MENU_LOADENCFROM), IDM_FILE_LOAD2FROM, (m_Mode == MODE_IDLE || m_Mode == MODE_DECRYPTION) ? 0U : MF_DISABLED)
		.Add(ResourceString(IDS_MENU_SAVEENCAS), IDM_FILE_SAVE2AS, (m_EncryptedData.Len > 0) ? 0U : MF_DISABLED)
		.AddSeparator()
		.Add(ResourceString(IDS_MENU_EXIT), IDM_FILE_EXIT);
	switch (m_ActiveEditControlId)
	{
	case IDC_CRPT_KEY_EDIT:
	case IDC_CRPT_IV_EDIT:
	case IDC_CRPT_NONCE_EDIT:
	case IDC_CRPT_TAG_EDIT:
		uFlagsR = 0U;
		uFlagsW = 0U;
		break;
	case IDC_CRPT_ORG_EDIT:
		uFlagsR = 0U;
		uFlagsW = (m_Mode == MODE_IDLE || m_Mode == MODE_ENCRYPTION) ? 0U : MF_DISABLED;
		break;
	case IDC_CRPT_ENC_EDIT:
		uFlagsR = 0U;
		uFlagsW = (m_Mode == MODE_IDLE || m_Mode == MODE_DECRYPTION) ? 0U : MF_DISABLED;
		break;
	default:
		uFlagsR = MF_DISABLED;
		uFlagsW = MF_DISABLED;
		break;
	}
	m_menuEdit
		.RemoveAll()
		.Add(ResourceString(IDS_MENU_CUT), IDM_EDIT_CUT, uFlagsW)
		.Add(ResourceString(IDS_MENU_COPY), IDM_EDIT_COPY, uFlagsR)
		.Add(ResourceString(IDS_MENU_PASTE), IDM_EDIT_PASTE, uFlagsW)
		.Add(ResourceString(IDS_MENU_DELETE), IDM_EDIT_DELETE, uFlagsW)
		.AddSeparator()
		.Add(ResourceString(IDS_MENU_SELECTALL), IDM_EDIT_SELECTALL, uFlagsR)
		.AddSeparator()
		.Add(ResourceString(IDS_MENU_COPYALL), IDM_EDIT_COPYALL, uFlagsR)
		.AddSeparator()
		.Add(ResourceString(IDS_MENU_CLEAR), IDM_EDIT_CLEAR, (m_Mode == MODE_ENCRYPTION || m_Mode == MODE_DECRYPTION) ? 0U : MF_DISABLED);
	m_menuSettings
		.RemoveAll()
		.Add(ResourceString(IDS_MENU_WRAPDATA), IDM_SETTINGS_WRAPDATA, m_bWrapData ? MF_CHECKED : MFS_UNCHECKED);
}


void CryptographyDialogBox::UpdateButtons()
{
	BOOL bEncrypt = FALSE;
	BOOL bDecrypt = FALSE;
	if (IS_AES_CBC(m_hAlg.ChainingMode) || IS_AES_CFB(m_hAlg.ChainingMode))
	{
		if (m_Key.Len * 8 >= m_KeyLength && m_IV.Len >= m_hAlg.BlockLength)
		{
			bEncrypt = m_OriginalData.Len > 0 && m_EncryptedData.Len == 0 ? TRUE : FALSE;
			bDecrypt = m_EncryptedData.Len > 0 && m_OriginalData.Len == 0 ? TRUE : FALSE;
		}
	}
	else if (IS_AES_ECB(m_hAlg.ChainingMode))
	{
		if (m_Key.Len * 8 >= m_KeyLength)
		{
			bEncrypt = m_OriginalData.Len > 0 && m_EncryptedData.Len == 0 ? TRUE : FALSE;
			bDecrypt = m_EncryptedData.Len > 0 && m_OriginalData.Len == 0 ? TRUE : FALSE;
		}
	}
	else if (IS_AES_CCM(m_hAlg.ChainingMode) || IS_AES_GCM(m_hAlg.ChainingMode))
	{
		if (m_Key.Len * 8 >= m_KeyLength)
		{
			bEncrypt = m_Nonce.Len >= m_NonceLength && m_OriginalData.Len > 0 && m_EncryptedData.Len == 0 ? TRUE : FALSE;
			bDecrypt = m_Nonce.Len >= m_NonceLength && m_Tag.Len >= m_TagLength && m_EncryptedData.Len > 0 && m_OriginalData.Len == 0 ? TRUE : FALSE;
		}
	}
	EnableWindow(IDC_CRPT_ENCRYPT_BUTTON, bEncrypt);
	EnableWindow(IDC_CRPT_DECRYPT_BUTTON, bDecrypt);
}


PCWSTR CryptographyDialogBox::ControlIdToChainingMode(int id)
{
	switch (id)
	{
	case IDC_CRPT_AESCBC_RADIO: return BCRYPT_CHAIN_MODE_CBC;
	case IDC_CRPT_AESECB_RADIO: return BCRYPT_CHAIN_MODE_ECB;
	case IDC_CRPT_AESCFB_RADIO: return BCRYPT_CHAIN_MODE_CFB;
	case IDC_CRPT_AESCCM_RADIO: return BCRYPT_CHAIN_MODE_CCM;
	case IDC_CRPT_AESGCM_RADIO: return BCRYPT_CHAIN_MODE_GCM;
	default:
		throw Exception(L"CryptographyDialogBox::ControlIdToChainingMode: Bad ID: %d", id);
	}
}


int CryptographyDialogBox::ChainingModeToControlId(PCWSTR pszValue)
{
	static std::map<PCWSTR, int, StringLessThan> m;
	if (!m.size())
	{
		m.insert(std::pair<PCWSTR, int>(BCRYPT_CHAIN_MODE_CBC, IDC_CRPT_AESCBC_RADIO));
		m.insert(std::pair<PCWSTR, int>(BCRYPT_CHAIN_MODE_ECB, IDC_CRPT_AESECB_RADIO));
		m.insert(std::pair<PCWSTR, int>(BCRYPT_CHAIN_MODE_CFB, IDC_CRPT_AESCFB_RADIO));
		m.insert(std::pair<PCWSTR, int>(BCRYPT_CHAIN_MODE_CCM, IDC_CRPT_AESCCM_RADIO));
		m.insert(std::pair<PCWSTR, int>(BCRYPT_CHAIN_MODE_GCM, IDC_CRPT_AESGCM_RADIO));
	}
	std::map<PCWSTR, int, StringLessThan>::const_iterator iter = m.find(pszValue);
	if (iter == m.cend())
	{
		throw Exception(L"CryptographyDialogBox::ChainingModeToControlId: Bad value: %s", pszValue);
	}
	return iter->second;
}


int CryptographyDialogBox::ControlIdToKeyLength(int id)
{
	switch (id)
	{
	case IDC_CRPT_KEY128_RADIO: return 128;
	case IDC_CRPT_KEY192_RADIO: return 192;
	case IDC_CRPT_KEY256_RADIO: return 256;
	default:
		throw Exception(L"CryptographyDialogBox::ControlIdToKeyLength: Bad ID: %d", id);
	}
}


int CryptographyDialogBox::KeyLengthToControlId(int length)
{
	switch (length)
	{
	case 128: return IDC_CRPT_KEY128_RADIO;
	case 192: return IDC_CRPT_KEY192_RADIO;
	case 256: return IDC_CRPT_KEY256_RADIO;
	default:
		throw Exception(L"CryptographyDialogBox::KeyLengthToControlId: Bad length: %d", length);
	}
}


int CryptographyDialogBox::ControlIdToNonceLength(int id)
{
	switch (id)
	{
	case IDC_CRPT_NONCE11_RADIO: return 11;
	case IDC_CRPT_NONCE12_RADIO: return 12;
	case IDC_CRPT_NONCE13_RADIO: return 13;
	case IDC_CRPT_NONCE14_RADIO: return 14;
	default:
		throw Exception(L"CryptographyDialogBox::ControlIdToNonceLength: Bad ID: %d", id);
	}
}


int CryptographyDialogBox::NonceLengthToControlId(int length)
{
	switch (length)
	{
	case 11: return IDC_CRPT_NONCE11_RADIO;
	case 12: return IDC_CRPT_NONCE12_RADIO;
	case 13: return IDC_CRPT_NONCE13_RADIO;
	case 14: return IDC_CRPT_NONCE14_RADIO;
	default:
		throw Exception(L"CryptographyDialogBox::NonceLengthToControlId: Bad length: %d", length);
	}
}


int CryptographyDialogBox::ControlIdToTagLength(int id)
{
	switch (id)
	{
	case IDC_CRPT_TAG4_RADIO: return 4;
	case IDC_CRPT_TAG6_RADIO: return 6;
	case IDC_CRPT_TAG8_RADIO: return 8;
	case IDC_CRPT_TAG10_RADIO: return 10;
	case IDC_CRPT_TAG12_RADIO: return 12;
	case IDC_CRPT_TAG14_RADIO: return 14;
	case IDC_CRPT_TAG16_RADIO: return 16;
	default:
		throw Exception(L"CryptographyDialogBox::ControlIdToTagLength: Bad ID: %d", id);
	}
}


int CryptographyDialogBox::TagLengthToControlId(int length)
{
	switch (length)
	{
	case 4: return IDC_CRPT_TAG4_RADIO;
	case 6: return IDC_CRPT_TAG6_RADIO;
	case 8: return IDC_CRPT_TAG8_RADIO;
	case 10: return IDC_CRPT_TAG10_RADIO;
	case 12: return IDC_CRPT_TAG12_RADIO;
	case 14: return IDC_CRPT_TAG14_RADIO;
	case 16: return IDC_CRPT_TAG16_RADIO;
	default:
		throw Exception(L"CryptographyDialogBox::TagLengthToControlId: Bad length: %d", length);
	}
}


int CryptographyDialogBox::OriginalDataDisplayModeToControlId(int value)
{
	int id = value + IDC_CRPT_ORG_HEX_RADIO;
	switch (id)
	{
	case IDC_CRPT_ORG_HEX_RADIO:
	case IDC_CRPT_ORG_BASE64_RADIO:
	case IDC_CRPT_ORG_RAW_RADIO:
		return id;
	default:
		throw Exception(L"CryptographyDialogBox::OriginalDataDisplayModeToControlId: Bad value: %d", value);
	}
}


int CryptographyDialogBox::EncryptedDataDisplayModeToControlId(int value)
{
	int id = value + IDC_CRPT_ENC_HEX_RADIO;
	switch (id)
	{
	case IDC_CRPT_ENC_HEX_RADIO:
	case IDC_CRPT_ENC_BASE64_RADIO:
		return id;
	default:
		throw Exception(L"CryptographyDialogBox::EncryptedDataDisplayModeToControlId: Bad value: %d", value);
	}
}
