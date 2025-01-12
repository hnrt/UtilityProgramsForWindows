#include "pch.h"
#include "CryptographyDialogBox.h"
#include "resource.h"
#include "MyToolbox.h"
#include "hnrt/ResourceString.h"
#include "hnrt/RegistryKey.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/WindowLayoutSnapshot.h"
#include "hnrt/LogicalFont.h"
#include "hnrt/StringBuffer.h"
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
#include "hnrt/StringCommons.h"
#include "hnrt/Debug.h"
#include <map>


#define AES_GCM_NONCE_LENGTH 12
#define AES_CCM_NONCE_LENGTH 12


#define REG_NAME_CHAININGMODE L"ChainingMode"
#define REG_NAME_KEYLENGTH L"KeyLength"
#define REG_NAME_KEY L"Key"
#define REG_NAME_IV L"IV"
#define REG_NAME_NONCE L"Nonce"
#define REG_NAME_CCMTAGLENGTH L"CCM_TagLength"
#define REG_NAME_GCMTAGLENGTH L"GCM_TagLength"
#define REG_NAME_ORGDISPMODE L"OriginalDataDisplayMode"
#define REG_NAME_ENCDISPMODE L"EncryptedDataDisplayMode"
#define REG_NAME_HEXFORMAT L"HexFormat"


#define IS_AES_CBC(x) (!wcscmp(x,BCRYPT_CHAIN_MODE_CBC))
#define IS_AES_ECB(x) (!wcscmp(x,BCRYPT_CHAIN_MODE_ECB))
#define IS_AES_CFB(x) (!wcscmp(x,BCRYPT_CHAIN_MODE_CFB))
#define IS_AES_CCM(x) (!wcscmp(x,BCRYPT_CHAIN_MODE_CCM))
#define IS_AES_GCM(x) (!wcscmp(x,BCRYPT_CHAIN_MODE_GCM))


using namespace hnrt;


CryptographyDialogBox::CryptographyDialogBox()
	: MyDialogBox(IDD_CRPT, L"Cryptography")
	, m_hAlg()
	, m_KeyLength(256)
	, m_Key()
	, m_IV()
	, m_Nonce()
	, m_CcmTagLength(16)
	, m_GcmTagLength(16)
	, m_AaDataDisplayMode(DataDisplayMode::TEXT)
	, m_OriginalData()
	, m_OriginalDataDisplayMode(DataDisplayMode::TEXT)
	, m_EncryptedData()
	, m_EncryptedDataDisplayMode(DataDisplayMode::HEX)
	, m_HexLetterCase(StringOptions::UPPERCASE)
	, m_CodePage(CP_UTF8)
	, m_LineBreak(LineBreak::CRLF)
	, m_Mode(MODE_IDLE)
	, m_bStatusSuccessful(TRUE)
	, m_szOriginalDataPath()
	, m_szEncryptedDataPath()
	, m_bWrapData(TRUE)
{
	m_hAlg.Open(BCRYPT_AES_ALGORITHM);
	m_hAlg.ChainingMode = BCRYPT_CHAIN_MODE_CBC;
}


void CryptographyDialogBox::OnCreate()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	MyDialogBox::OnCreate();
	HFONT hFont = GetApp<MyToolbox>().GetFontForData();
	SetFont(IDC_CRPT_KEY_EDIT, hFont);
	SetFont(IDC_CRPT_IV_EDIT, hFont);
	SetFont(IDC_CRPT_AAD_EDIT, hFont);
	SetFont(IDC_CRPT_ORG_EDIT, hFont);
	SetFont(IDC_CRPT_ENC_EDIT, hFont);
	int cm = ChainingModeToControlId(m_hAlg.ChainingMode);
	int od = OriginalDataDisplayModeToControlId(m_OriginalDataDisplayMode);
	int ed = EncryptedDataDisplayModeToControlId(m_EncryptedDataDisplayMode);
	RegistryKey hKey;
	LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, m_szRegistryKeyName);
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
		m_CcmTagLength = RegistryValue::GetDWORD(hKey, REG_NAME_CCMTAGLENGTH, m_CcmTagLength);
		m_GcmTagLength = RegistryValue::GetDWORD(hKey, REG_NAME_GCMTAGLENGTH, m_GcmTagLength);
		od = OriginalDataDisplayModeToControlId(static_cast<DataDisplayMode>(RegistryValue::GetDWORD(hKey, REG_NAME_ORGDISPMODE)));
		ed = EncryptedDataDisplayModeToControlId(static_cast<DataDisplayMode>(RegistryValue::GetDWORD(hKey, REG_NAME_ENCDISPMODE)));
		m_HexLetterCase = RegistryValue::GetDWORD(hKey, REG_NAME_HEXFORMAT) ? StringOptions::UPPERCASE : StringOptions::LOWERCASE;
	}
	SetText(IDC_CRPT_KEY_EDIT, m_Key.ToHex(m_HexLetterCase));
	ButtonCheck(cm);
	ButtonCheck(KeyLengthToControlId(m_KeyLength));
	ButtonCheck(IDC_CRPT_AAD_TEXT_RADIO);
	InitializeCodePageComboBox(IDC_CRPT_CODEPAGE_COMBO, m_CodePage);
	InitializeLineBreakComboBox(IDC_CRPT_LINEBREAK_COMBO, m_LineBreak);
	InitializeLetterCaseComboBox(IDC_CRPT_HEXLETTER_COMBO, m_HexLetterCase);
	OnChainingModeChange(cm);
	ButtonCheck(od);
	OnOriginalDataDisplayModeChange(od);
	ButtonCheck(ed);
	OnEncryptedDataDisplayModeChange(ed);
	DisableWindow(IDC_CRPT_ENCRYPT_BUTTON);
	DisableWindow(IDC_CRPT_DECRYPT_BUTTON);
	ClearStatusText();
	InvalidateRect(IDC_CRPT_KEY_EDIT, NULL, FALSE);
	InvalidateRect(IDC_CRPT_IV_EDIT, NULL, FALSE);
	m_menuView
		.Add(ResourceString(IDS_MENU_CRPT), IDM_VIEW_CRPT);
}


void CryptographyDialogBox::OnDestroy()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	RegistryKey hKey;
	LSTATUS rc = hKey.Create(HKEY_CURRENT_USER, m_szRegistryKeyName);
	if (rc == ERROR_SUCCESS)
	{
		RegistryValue::SetSZ(hKey, REG_NAME_CHAININGMODE, m_hAlg.ChainingMode);
		RegistryValue::SetDWORD(hKey, REG_NAME_KEYLENGTH, m_KeyLength);
		RegistryValue::SetSZ(hKey, REG_NAME_KEY, m_Key.ToHex());
		RegistryValue::SetSZ(hKey, REG_NAME_IV, m_IV.ToHex());
		RegistryValue::SetSZ(hKey, REG_NAME_NONCE, m_Nonce.ToHex());
		RegistryValue::SetDWORD(hKey, REG_NAME_CCMTAGLENGTH, m_CcmTagLength);
		RegistryValue::SetDWORD(hKey, REG_NAME_GCMTAGLENGTH, m_GcmTagLength);
		RegistryValue::SetDWORD(hKey, REG_NAME_ORGDISPMODE, m_OriginalDataDisplayMode);
		RegistryValue::SetDWORD(hKey, REG_NAME_ENCDISPMODE, m_EncryptedDataDisplayMode);
		RegistryValue::SetDWORD(hKey, REG_NAME_HEXFORMAT, m_HexLetterCase == StringOptions::UPPERCASE ? 1 : 0);
	}
	SetFont(IDC_CRPT_KEY_EDIT, NULL);
	SetFont(IDC_CRPT_IV_EDIT, NULL);
	SetFont(IDC_CRPT_AAD_EDIT, NULL);
	SetFont(IDC_CRPT_ORG_EDIT, NULL);
	SetFont(IDC_CRPT_ENC_EDIT, NULL);
	MyDialogBox::OnDestroy();
}


void CryptographyDialogBox::OnTabSelectionChanging()
{
	MyDialogBox::OnTabSelectionChanging();
	m_menuView
		.Enable(IDM_VIEW_CRPT, MF_ENABLED);
}


void CryptographyDialogBox::OnTabSelectionChanged()
{
	MyDialogBox::OnTabSelectionChanged();
	m_menuFile
		.Insert(0, ResourceString(IDS_MENU_NEW), IDM_FILE_NEW)
		.InsertSeparator(1)
		.Insert(2, ResourceString(IDS_MENU_LOADORGFROM), IDM_FILE_LOAD1FROM)
		.Insert(3, ResourceString(IDS_MENU_SAVEORGAS), IDM_FILE_SAVE1AS)
		.InsertSeparator(4)
		.Insert(5, ResourceString(IDS_MENU_LOADENCFROM), IDM_FILE_LOAD2FROM)
		.Insert(6, ResourceString(IDS_MENU_SAVEENCAS), IDM_FILE_SAVE2AS)
		.InsertSeparator(7);
	m_menuSettings
		.Add(ResourceString(IDS_MENU_WRAPDATA), IDM_SETTINGS_WRAPDATA);
	m_menuView
		.Enable(IDM_VIEW_CRPT, MF_DISABLED);
	UpdateMenus();
}


void CryptographyDialogBox::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
	WindowLayoutSnapshot before, after;

	before.AddAllChildren(hDlg).Clone(after);

	after[IDC_CRPT_ALGORITHM_GROUP].right += cxDelta;
	after[IDC_CRPT_KEY_GROUP].right += cxDelta;
	after[IDC_CRPT_IV_GROUP].right += cxDelta;
	after[IDC_CRPT_TAG_GROUP].right += cxDelta;
	after[IDC_CRPT_AAD_GROUP].right += cxDelta;
	after[IDC_CRPT_ORG_GROUP].right += cxDelta;
	after[IDC_CRPT_ENC_GROUP].right += cxDelta;
	after[IDC_CRPT_OPTIONS_GROUP].right += cxDelta;
	after[IDC_CRPT_STATUS_STATIC].right += cxDelta;
	after[IDC_CRPT_ORG_EDIT].right += cxDelta;
	after[IDC_CRPT_ENC_EDIT].right += cxDelta;
	MoveHorizontally(after[IDC_CRPT_KEY_BUTTON], cxDelta);
	MoveHorizontally(after[IDC_CRPT_IV_BUTTON], cxDelta);
	MoveHorizontally(after[IDC_CRPT_ORG_HEX_RADIO], cxDelta);
	MoveHorizontally(after[IDC_CRPT_ORG_BASE64_RADIO], cxDelta);
	MoveHorizontally(after[IDC_CRPT_ORG_TEXT_RADIO], cxDelta);
	MoveHorizontally(after[IDC_CRPT_ENCRYPT_BUTTON], cxDelta);
	MoveHorizontally(after[IDC_CRPT_COPYORG_BUTTON], cxDelta);
	MoveHorizontally(after[IDC_CRPT_ENC_HEX_RADIO], cxDelta);
	MoveHorizontally(after[IDC_CRPT_ENC_BASE64_RADIO], cxDelta);
	MoveHorizontally(after[IDC_CRPT_DECRYPT_BUTTON], cxDelta);
	MoveHorizontally(after[IDC_CRPT_COPYENC_BUTTON], cxDelta);
	MoveVertically(after[IDC_CRPT_OPTIONS_GROUP], cyDelta);
	MoveVertically(after[IDC_CRPT_CODEPAGE_STATIC], cyDelta);
	MoveVertically(after[IDC_CRPT_CODEPAGE_COMBO], cyDelta);
	MoveVertically(after[IDC_CRPT_LINEBREAK_STATIC], cyDelta);
	MoveVertically(after[IDC_CRPT_LINEBREAK_COMBO], cyDelta);
	MoveVertically(after[IDC_CRPT_HEXLETTER_STATIC], cyDelta);
	MoveVertically(after[IDC_CRPT_HEXLETTER_COMBO], cyDelta);
	MoveVertically(after[IDC_CRPT_STATUS_STATIC], cyDelta);

	LONG gy = before[IDC_CRPT_ORG_GROUP].top - before[IDC_CRPT_AAD_GROUP].bottom;
	LONG cy = (after[IDC_CRPT_OPTIONS_GROUP].top - after[IDC_CRPT_AAD_GROUP].bottom - gy * 3) / 2;

	after[IDC_CRPT_ORG_GROUP].cy = cy;
	after[IDC_CRPT_ORG_EDIT].bottom = after[IDC_CRPT_ORG_GROUP].bottom - (before[IDC_CRPT_ORG_GROUP].bottom - before[IDC_CRPT_ORG_EDIT].bottom);

	after[IDC_CRPT_ENC_GROUP].top = after[IDC_CRPT_ORG_GROUP].bottom + gy;
	after[IDC_CRPT_ENC_GROUP].bottom = after[IDC_CRPT_OPTIONS_GROUP].top - gy;
	after[IDC_CRPT_ENC_EDIT].top = after[IDC_CRPT_ENC_GROUP].top + (before[IDC_CRPT_ENC_EDIT].top - before[IDC_CRPT_ENC_GROUP].top);
	after[IDC_CRPT_ENC_EDIT].bottom = after[IDC_CRPT_ENC_GROUP].bottom - (before[IDC_CRPT_ENC_GROUP].bottom - before[IDC_CRPT_ENC_EDIT].bottom);

	LONG dy = after[IDC_CRPT_ENC_GROUP].top - before[IDC_CRPT_ENC_GROUP].top;

	MoveVertically(after[IDC_CRPT_ENC_HEX_RADIO], dy);
	MoveVertically(after[IDC_CRPT_ENC_BASE64_RADIO], dy);
	MoveVertically(after[IDC_CRPT_DECRYPT_BUTTON], dy);
	MoveVertically(after[IDC_CRPT_COPYENC_BUTTON], dy);

	after.Apply();
}


INT_PTR CryptographyDialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (m_cProcessing)
	{
		DBGPUT(L"CryptographyDialogBox::OnCommand(%zx,%zx): m_cProcessing=%d", wParam, lParam, m_cProcessing);
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
		if (idNotif == BN_CLICKED)
		{
			if (ButtonIsChecked(idChild))
			{
				OnChainingModeChange(idChild);
			}
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRPT_KEY128_RADIO:
	case IDC_CRPT_KEY192_RADIO:
	case IDC_CRPT_KEY256_RADIO:
		if (idNotif == BN_CLICKED)
		{
			if (ButtonIsChecked(idChild))
			{
				OnKeyLengthChange(idChild);
			}
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRPT_TAG32_RADIO:
	case IDC_CRPT_TAG48_RADIO:
	case IDC_CRPT_TAG64_RADIO:
	case IDC_CRPT_TAG80_RADIO:
	case IDC_CRPT_TAG96_RADIO:
	case IDC_CRPT_TAG104_RADIO:
	case IDC_CRPT_TAG112_RADIO:
	case IDC_CRPT_TAG120_RADIO:
	case IDC_CRPT_TAG128_RADIO:
		if (idNotif == BN_CLICKED)
		{
			if (ButtonIsChecked(idChild))
			{
				OnTagLengthChange(idChild);
			}
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRPT_AAD_HEX_RADIO:
	case IDC_CRPT_AAD_BASE64_RADIO:
	case IDC_CRPT_AAD_TEXT_RADIO:
		if (idNotif == BN_CLICKED)
		{
			if (ButtonIsChecked(idChild))
			{
				OnAaDataDisplayModeChange(idChild);
			}
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRPT_KEY_EDIT:
	case IDC_CRPT_IV_EDIT:
	case IDC_CRPT_AAD_EDIT:
	case IDC_CRPT_ORG_EDIT:
	case IDC_CRPT_ENC_EDIT:
		return OnEditCommand(wParam, lParam);
	case IDC_CRPT_ORG_HEX_RADIO:
	case IDC_CRPT_ORG_BASE64_RADIO:
	case IDC_CRPT_ORG_TEXT_RADIO:
		if (idNotif == BN_CLICKED)
		{
			if (ButtonIsChecked(idChild))
			{
				OnOriginalDataDisplayModeChange(idChild);
			}
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRPT_ENC_HEX_RADIO:
	case IDC_CRPT_ENC_BASE64_RADIO:
		if (idNotif == BN_CLICKED)
		{
			if (ButtonIsChecked(idChild))
			{
				OnEncryptedDataDisplayModeChange(idChild);
			}
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRPT_CODEPAGE_COMBO:
		if (idNotif == CBN_SELCHANGE)
		{
			OnCodePageChange();
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRPT_LINEBREAK_COMBO:
		if (idNotif == CBN_SELCHANGE)
		{
			OnLineBreakChange();
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRPT_HEXLETTER_COMBO:
		if (idNotif == CBN_SELCHANGE)
		{
			OnHexFormatChange();
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRPT_ENCRYPT_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			OnEncrypt();
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRPT_DECRYPT_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			OnDecrypt();
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRPT_COPYORG_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			OnCopyOriginalData();
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRPT_COPYENC_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			OnCopyEncryptedData();
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRPT_KEY_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			OnAdjustKey();
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRPT_IV_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			OnAdjustIV();
		}
		else
		{
			return FALSE;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}


INT_PTR CryptographyDialogBox::OnControlColorEdit(WPARAM wParam, LPARAM lParam)
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	HDC hdc = reinterpret_cast<HDC>(wParam);
	int id = GetDlgCtrlID(reinterpret_cast<HWND>(lParam));
	switch (id)
	{
	case IDC_CRPT_KEY_EDIT:
	{
		size_t required = m_KeyLength;
		SetTextColor(hdc, (m_Key.Len * 8 == required) ? RGB_GOOD : (m_Key.Len * 8 > required) ? RGB_TOO_MANY : RGB_TOO_FEW);
		break;
	}
	case IDC_CRPT_IV_EDIT:
	{
		if (IS_AES_CCM(m_hAlg.ChainingMode))
		{
			size_t required = AES_CCM_NONCE_LENGTH;
			SetTextColor(hdc, (m_Nonce.Len == required) ? RGB_GOOD : (m_Nonce.Len > required) ? RGB_TOO_MANY : RGB_TOO_FEW);
		}
		else if (IS_AES_GCM(m_hAlg.ChainingMode))
		{
			size_t required = AES_GCM_NONCE_LENGTH;
			SetTextColor(hdc, (m_Nonce.Len == required) ? RGB_GOOD : (m_Nonce.Len > required) ? RGB_TOO_MANY : RGB_TOO_FEW);
		}
		else
		{
			size_t required = m_hAlg.BlockLength;
			SetTextColor(hdc, (m_IV.Len == required) ? RGB_GOOD : (m_IV.Len > required) ? RGB_TOO_MANY : RGB_TOO_FEW);
		}
		break;
	}
	default:
		return 0;
	}
	return reinterpret_cast<INT_PTR>(GetStockObject(WHITE_BRUSH));
}


INT_PTR CryptographyDialogBox::OnControlColorStatic(WPARAM wParam, LPARAM lParam)
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	HDC hdc = reinterpret_cast<HDC>(wParam);
	int id = GetDlgCtrlID(reinterpret_cast<HWND>(lParam));
	switch (id)
	{
	case IDC_CRPT_STATUS_STATIC:
		if (m_bStatusSuccessful == FALSE)
		{
			SetTextColor(hdc, RGB_ERROR);
			SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
			break;
		}
		return 0;
	default:
		return 0;
	}
	return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_3DFACE));
}


void CryptographyDialogBox::OnEditChanged(int id)
{
	MyDialogBox::OnEditChanged(id);
	switch (id)
	{
	case IDC_CRPT_KEY_EDIT:
		OnKeyChange();
		break;
	case IDC_CRPT_IV_EDIT:
		OnIVChange();
		break;
	case IDC_CRPT_ORG_EDIT:
		OnOriginalDataChange();
		break;
	case IDC_CRPT_ENC_EDIT:
		OnEncryptedDataChange();
		break;
	default:
		break;
	}
}


void CryptographyDialogBox::OnNew()
{
	SetMode(MODE_IDLE);
	UpdateMenus();
	UpdateButtons();
}


void CryptographyDialogBox::OnLoad1From()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
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
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
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
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
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
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
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


void CryptographyDialogBox::OnSettingChanged(UINT id)
{
	switch (id)
	{
	case IDM_SETTINGS_WRAPDATA:
	{
		WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
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
	DBGFNC(L"CryptographyDialogBox::OnEncrypt");
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
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
		ByteString key = m_Key.Clone().Resize(m_KeyLength / 8);
		DBGPUT(L"key[%zu]=%s", key.Len, key.ToHex());
		hKey.Generate(m_hAlg, key.Ptr, key.Len);
		if (IS_AES_CBC(m_hAlg.ChainingMode) || IS_AES_CFB(m_hAlg.ChainingMode))
		{
			ByteString plaintext = m_OriginalData.Pkcs5Padding(m_hAlg.BlockLength);
			ByteString iv = m_IV.Clone().Resize(m_hAlg.BlockLength);
			DBGPUT(L"iv[%zu]=%s", iv.Len, iv.ToHex());
			encrypted = hKey.Encrypt(plaintext.Ptr, plaintext.Len, iv.Ptr, iv.Len, 0);
			DBGPUT(L"Encrypted %zu bytes", encrypted.Len);
		}
		else if (IS_AES_ECB(m_hAlg.ChainingMode))
		{
			ByteString plaintext = m_OriginalData.Pkcs5Padding(m_hAlg.BlockLength);
			encrypted = hKey.Encrypt(plaintext.Ptr, plaintext.Len, nullptr, 0, 0);
			DBGPUT(L"Encrypted %zu bytes", encrypted.Len);
		}
		else if (IS_AES_CCM(m_hAlg.ChainingMode))
		{
			ByteString nonce = m_Nonce.Clone().Resize(AES_CCM_NONCE_LENGTH);
			BCryptAuthenticatedCipherModeInfo info;
			info
				.SetNonce(nonce.Ptr, nonce.Len)
				.SetTagSize(m_CcmTagLength);
			DBGPUT(L"nonce[%lu]=%s", info.cbNonce, ByteString(info.pbNonce, info.cbNonce).ToHex());
			DBGPUT(L"tag[%lu]", info.cbTag);
			if (GetTextLength(IDC_CRPT_AAD_EDIT) > 0)
			{
				ByteString aad = GetAaData();
				info.SetAuthData(aad.Ptr, aad.Len);
				DBGPUT(L"aad[%lu]=%s", info.cbAuthData, ByteString(info.pbAuthData, info.cbAuthData).ToHex());
			}
			encrypted = hKey.Encrypt(m_OriginalData.Ptr, m_OriginalData.Len, info, NULL, 0);
			DBGPUT(L"Encrypted %zu bytes", encrypted.Len);
			ByteString tagFollows(encrypted.Len + info.cbTag);
			memcpy_s(reinterpret_cast<PBYTE>(tagFollows.Ptr), tagFollows.Len, encrypted.Ptr, encrypted.Len);
			memcpy_s(reinterpret_cast<PBYTE>(tagFollows.Ptr) + encrypted.Len, tagFollows.Len - encrypted.Len, info.pbTag, info.cbTag);
			encrypted = tagFollows;
			DBGPUT(L"tag[%lu]=%s", info.cbTag, ByteString(info.pbTag, info.cbTag).ToHex());
		}
		else if (IS_AES_GCM(m_hAlg.ChainingMode))
		{
			ByteString nonce = m_Nonce.Clone().Resize(AES_GCM_NONCE_LENGTH);
			BCryptAuthenticatedCipherModeInfo info;
			info
				.SetNonce(nonce.Ptr, nonce.Len)
				.SetTagSize(m_GcmTagLength);
			DBGPUT(L"nonce[%lu]=%s", info.cbNonce, ByteString(info.pbNonce, info.cbNonce).ToHex());
			DBGPUT(L"tag[%lu]", info.cbTag);
			if (GetTextLength(IDC_CRPT_AAD_EDIT) > 0)
			{
				ByteString aad = GetAaData();
				info.SetAuthData(aad.Ptr, aad.Len);
				DBGPUT(L"aad[%lu]=%s", info.cbAuthData, ByteString(info.pbAuthData, info.cbAuthData).ToHex());
			}
			encrypted = hKey.Encrypt(m_OriginalData.Ptr, m_OriginalData.Len, info, NULL, 0);
			DBGPUT(L"Encrypted %zu bytes", encrypted.Len);
			ByteString tagFollows(encrypted.Len + info.cbTag);
			memcpy_s(reinterpret_cast<PBYTE>(tagFollows.Ptr), tagFollows.Len, encrypted.Ptr, encrypted.Len);
			memcpy_s(reinterpret_cast<PBYTE>(tagFollows.Ptr) + encrypted.Len, tagFollows.Len - encrypted.Len, info.pbTag, info.cbTag);
			encrypted = tagFollows;
			DBGPUT(L"tag[%lu]=%s", info.cbTag, ByteString(info.pbTag, info.cbTag).ToHex());
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
	DBGFNC(L"CryptographyDialogBox::OnDecrypt");
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
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
		ByteString key = m_Key.Clone().Resize(m_KeyLength / 8);
		DBGPUT(L"key[%zu]=%s", key.Len, key.ToHex());
		hKey.Generate(m_hAlg, key.Ptr, key.Len);
		if (IS_AES_CBC(m_hAlg.ChainingMode) || IS_AES_CFB(m_hAlg.ChainingMode))
		{
			ByteString iv = m_IV.Clone().Resize(m_hAlg.BlockLength);
			DBGPUT(L"iv[%zu]=%s", iv.Len, iv.ToHex());
			decrypted = hKey.Decrypt(m_EncryptedData.Ptr, m_EncryptedData.Len, iv.Ptr, iv.Len, 0);
			decrypted.RemovePkcs5Padding(m_hAlg.BlockLength);
			DBGPUT(L"Decrypted %zu bytes", decrypted.Len);
		}
		else if (IS_AES_ECB(m_hAlg.ChainingMode))
		{
			decrypted = hKey.Decrypt(m_EncryptedData.Ptr, m_EncryptedData.Len, nullptr, 0, 0);
			decrypted.RemovePkcs5Padding(m_hAlg.BlockLength);
			DBGPUT(L"Decrypted %zu bytes", decrypted.Len);
		}
		else if (IS_AES_CCM(m_hAlg.ChainingMode))
		{
			if (m_EncryptedData.Len < m_CcmTagLength)
			{
				throw Exception(L"Encrypted data is too short.");
			}
			ByteString nonce = m_Nonce.Clone().Resize(AES_CCM_NONCE_LENGTH);
			ByteString tag(m_CcmTagLength);
			memcpy_s(tag.Ptr, tag.Len, reinterpret_cast<PBYTE>(m_EncryptedData.Ptr) + m_EncryptedData.Len - tag.Len, tag.Len);
			BCryptAuthenticatedCipherModeInfo info;
			info
				.SetNonce(nonce.Ptr, nonce.Len)
				.SetTag(tag.Ptr, tag.Len);
			DBGPUT(L"nonce[%lu]=%s", info.cbNonce, ByteString(info.pbNonce, info.cbNonce).ToHex());
			DBGPUT(L"tag[%lu]=%s", info.cbTag, ByteString(info.pbTag, info.cbTag).ToHex());
			if (GetTextLength(IDC_CRPT_AAD_EDIT) > 0)
			{
				ByteString aad = GetAaData();
				info.SetAuthData(aad.Ptr, aad.Len);
				DBGPUT(L"aad[%lu]=%s", info.cbAuthData, ByteString(info.pbAuthData, info.cbAuthData).ToHex());
			}
			decrypted = hKey.Decrypt(m_EncryptedData.Ptr, m_EncryptedData.Len - tag.Len, info, NULL, 0);
			DBGPUT(L"Decrypted %zu bytes", decrypted.Len);
		}
		else if (IS_AES_GCM(m_hAlg.ChainingMode))
		{
			if (m_EncryptedData.Len < m_GcmTagLength)
			{
				throw Exception(L"Encrypted data is too short.");
			}
			ByteString nonce = m_Nonce.Clone().Resize(AES_GCM_NONCE_LENGTH);
			ByteString tag(m_GcmTagLength);
			memcpy_s(tag.Ptr, tag.Len, reinterpret_cast<PBYTE>(m_EncryptedData.Ptr) + m_EncryptedData.Len - tag.Len, tag.Len);
			BCryptAuthenticatedCipherModeInfo info;
			info
				.SetNonce(nonce.Ptr, nonce.Len)
				.SetTag(tag.Ptr, tag.Len);
			DBGPUT(L"nonce[%lu]=%s", info.cbNonce, ByteString(info.pbNonce, info.cbNonce).ToHex());
			DBGPUT(L"tag[%lu]=%s", info.cbTag, ByteString(info.pbTag, info.cbTag).ToHex());
			if (GetTextLength(IDC_CRPT_AAD_EDIT) > 0)
			{
				ByteString aad = GetAaData();
				info.SetAuthData(aad.Ptr, aad.Len);
				DBGPUT(L"aad[%lu]=%s", info.cbAuthData, ByteString(info.pbAuthData, info.cbAuthData).ToHex());
			}
			decrypted = hKey.Decrypt(m_EncryptedData.Ptr, m_EncryptedData.Len - tag.Len, info, NULL, 0);
			DBGPUT(L"Decrypted %zu bytes", decrypted.Len);
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


void CryptographyDialogBox::OnCopyOriginalData()
{
	if (GetTextLength(IDC_CRPT_ORG_EDIT) > 0)
	{
		if (!Clipboard::Write(hwnd, GetText(IDC_CRPT_ORG_EDIT)))
		{
			MessageBoxW(hwnd, ResourceString(IDS_MSG_CLIPBOARD_COPY_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
		}
	}
}


void CryptographyDialogBox::OnCopyEncryptedData()
{
	if (GetTextLength(IDC_CRPT_ENC_EDIT) > 0)
	{
		if (!Clipboard::Write(hwnd, GetText(IDC_CRPT_ENC_EDIT)))
		{
			MessageBoxW(hwnd, ResourceString(IDS_MSG_CLIPBOARD_COPY_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
		}
	}
}


void CryptographyDialogBox::OnAdjustKey()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	SIZE_T required = m_KeyLength / 8;
	if (m_Key.Len < required)
	{
		ByteString key(required);
		key.Fill(0);
		memcpy_s(key, key.Len, m_Key.Ptr, m_Key.Len);
		m_Key = key;
		SetText(IDC_CRPT_KEY_EDIT, m_Key.ToHex(m_HexLetterCase));
	}
	else if (m_Key.Len > required)
	{
		ByteString key(required);
		memcpy_s(key, key.Len, m_Key.Ptr, key.Len);
		m_Key = key;
		SetText(IDC_CRPT_KEY_EDIT, m_Key.ToHex(m_HexLetterCase));
	}
}


void CryptographyDialogBox::OnAdjustIV()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	if (IS_AES_CBC(m_hAlg.ChainingMode) || IS_AES_CFB(m_hAlg.ChainingMode))
	{
		SIZE_T required = m_hAlg.BlockLength;
		if (m_IV.Len < required)
		{
			ByteString iv(required);
			iv.Fill(0);
			memcpy_s(iv, iv.Len, m_IV.Ptr, m_IV.Len);
			m_IV = iv;
			SetText(IDC_CRPT_IV_EDIT, m_IV.ToHex(m_HexLetterCase));
		}
		else if (m_IV.Len > required)
		{
			ByteString iv(required);
			memcpy_s(iv, iv.Len, m_IV.Ptr, iv.Len);
			m_IV = iv;
			SetText(IDC_CRPT_IV_EDIT, m_IV.ToHex(m_HexLetterCase));
		}
	}
	else if (IS_AES_CCM(m_hAlg.ChainingMode) || IS_AES_GCM(m_hAlg.ChainingMode))
	{
		SIZE_T required = IS_AES_CCM(m_hAlg.ChainingMode) ? AES_CCM_NONCE_LENGTH : AES_GCM_NONCE_LENGTH;
		if (m_Nonce.Len < required)
		{
			ByteString nonce(required);
			nonce.Fill(0);
			memcpy_s(nonce, nonce.Len, m_Nonce.Ptr, m_Nonce.Len);
			m_Nonce = nonce;
			SetText(IDC_CRPT_IV_EDIT, m_Nonce.ToHex(m_HexLetterCase));
		}
		else if (m_Nonce.Len > required)
		{
			ByteString nonce(required);
			memcpy_s(nonce, nonce.Len, m_Nonce.Ptr, nonce.Len);
			m_Nonce = nonce;
			SetText(IDC_CRPT_IV_EDIT, m_Nonce.ToHex(m_HexLetterCase));
		}
	}
}


void CryptographyDialogBox::OnChainingModeChange(int id)
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	switch (id)
	{
	case IDC_CRPT_AESCBC_RADIO:
	default:
		m_hAlg.ChainingMode = BCRYPT_CHAIN_MODE_CBC;
		EnableWindow(IDC_CRPT_IV_GROUP);
		EnableWindow(IDC_CRPT_IV_EDIT);
		EnableWindow(IDC_CRPT_IVLEN_STATIC);
		SetText(IDC_CRPT_IV_GROUP, L"Initial Vector");
		SetText(IDC_CRPT_IV_EDIT, m_IV.ToHex(m_HexLetterCase));
		SetText(IDC_CRPT_IVLEN_STATIC, String(PRINTF, L"%lu bytes (Block Size)", m_hAlg.BlockLength));
		UpdateTagSizeRadioBoxes();
		DisableWindow(IDC_CRPT_AAD_GROUP);
		DisableWindow(IDC_CRPT_AAD_EDIT);
		DisableWindow(IDC_CRPT_AAD_HEX_RADIO);
		DisableWindow(IDC_CRPT_AAD_BASE64_RADIO);
		DisableWindow(IDC_CRPT_AAD_TEXT_RADIO);
		break;
	case IDC_CRPT_AESECB_RADIO:
		m_hAlg.ChainingMode = BCRYPT_CHAIN_MODE_ECB;
		DisableWindow(IDC_CRPT_IV_GROUP);
		DisableWindow(IDC_CRPT_IV_EDIT);
		DisableWindow(IDC_CRPT_IVLEN_STATIC);
		SetText(IDC_CRPT_IV_GROUP, L"Initial Vector");
		SetText(IDC_CRPT_IV_EDIT);
		SetText(IDC_CRPT_IVLEN_STATIC, String(PRINTF, L"%lu bytes (Block Size)", m_hAlg.BlockLength));
		UpdateTagSizeRadioBoxes();
		DisableWindow(IDC_CRPT_AAD_GROUP);
		DisableWindow(IDC_CRPT_AAD_EDIT);
		DisableWindow(IDC_CRPT_AAD_HEX_RADIO);
		DisableWindow(IDC_CRPT_AAD_BASE64_RADIO);
		DisableWindow(IDC_CRPT_AAD_TEXT_RADIO);
		break;
	case IDC_CRPT_AESCFB_RADIO:
		m_hAlg.ChainingMode = BCRYPT_CHAIN_MODE_CFB;
		EnableWindow(IDC_CRPT_IV_GROUP);
		EnableWindow(IDC_CRPT_IV_EDIT);
		EnableWindow(IDC_CRPT_IVLEN_STATIC);
		SetText(IDC_CRPT_IV_GROUP, L"Initial Vector");
		SetText(IDC_CRPT_IV_EDIT, m_IV.ToHex(m_HexLetterCase));
		SetText(IDC_CRPT_IVLEN_STATIC, String(PRINTF, L"%lu bytes (Block Size)", m_hAlg.BlockLength));
		UpdateTagSizeRadioBoxes();
		DisableWindow(IDC_CRPT_AAD_GROUP);
		DisableWindow(IDC_CRPT_AAD_EDIT);
		DisableWindow(IDC_CRPT_AAD_HEX_RADIO);
		DisableWindow(IDC_CRPT_AAD_BASE64_RADIO);
		DisableWindow(IDC_CRPT_AAD_TEXT_RADIO);
		break;
	case IDC_CRPT_AESCCM_RADIO:
		m_hAlg.ChainingMode = BCRYPT_CHAIN_MODE_CCM;
		EnableWindow(IDC_CRPT_IV_GROUP);
		EnableWindow(IDC_CRPT_IV_EDIT);
		EnableWindow(IDC_CRPT_IVLEN_STATIC);
		SetText(IDC_CRPT_IV_GROUP, L"Nonce");
		SetText(IDC_CRPT_IV_EDIT, m_Nonce.ToHex(m_HexLetterCase));
		SetText(IDC_CRPT_IVLEN_STATIC, String(PRINTF, L"%lu bytes", AES_CCM_NONCE_LENGTH));
		UpdateTagSizeRadioBoxes();
		EnableWindow(IDC_CRPT_AAD_GROUP);
		EnableWindow(IDC_CRPT_AAD_EDIT);
		EnableWindow(IDC_CRPT_AAD_HEX_RADIO);
		EnableWindow(IDC_CRPT_AAD_BASE64_RADIO);
		EnableWindow(IDC_CRPT_AAD_TEXT_RADIO);
		break;
	case IDC_CRPT_AESGCM_RADIO:
		m_hAlg.ChainingMode = BCRYPT_CHAIN_MODE_GCM;
		EnableWindow(IDC_CRPT_IV_GROUP);
		EnableWindow(IDC_CRPT_IV_EDIT);
		EnableWindow(IDC_CRPT_IVLEN_STATIC);
		SetText(IDC_CRPT_IV_GROUP, L"Nonce");
		SetText(IDC_CRPT_IV_EDIT, m_Nonce.ToHex(m_HexLetterCase));
		SetText(IDC_CRPT_IVLEN_STATIC, String(PRINTF, L"%lu bytes", AES_GCM_NONCE_LENGTH));
		UpdateTagSizeRadioBoxes();
		EnableWindow(IDC_CRPT_AAD_GROUP);
		EnableWindow(IDC_CRPT_AAD_EDIT);
		EnableWindow(IDC_CRPT_AAD_HEX_RADIO);
		EnableWindow(IDC_CRPT_AAD_BASE64_RADIO);
		EnableWindow(IDC_CRPT_AAD_TEXT_RADIO);
		break;
	}
	SetMode(m_Mode);
	UpdateButtons();
}


void CryptographyDialogBox::OnKeyLengthChange(int id)
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	m_KeyLength = ControlIdToKeyLength(id);
	InvalidateRect(IDC_CRPT_KEY_EDIT, NULL, FALSE);
	SetMode(m_Mode);
}


void CryptographyDialogBox::OnTagLengthChange(int id)
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	if (IS_AES_CCM(m_hAlg.ChainingMode))
	{
		m_CcmTagLength = ControlIdToTagLength(id);
	}
	else if (IS_AES_CCM(m_hAlg.ChainingMode))
	{
		m_GcmTagLength = ControlIdToTagLength(id);
	}
	SetMode(m_Mode);
}


void CryptographyDialogBox::OnKeyChange()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	try
	{
		m_Key = ByteString::FromHex(GetText(IDC_CRPT_KEY_EDIT).Trim());
	}
	catch (...)
	{
		m_Key.Resize(0);
	}
	InvalidateRect(IDC_CRPT_KEY_EDIT, NULL, FALSE);
	SetMode(m_Mode);
	UpdateMenus();
	UpdateButtons();
}


void CryptographyDialogBox::OnIVChange()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	if (IS_AES_CBC(m_hAlg.ChainingMode) || IS_AES_CFB(m_hAlg.ChainingMode))
	{
		try
		{
			m_IV = ByteString::FromHex(GetText(IDC_CRPT_IV_EDIT).Trim());
		}
		catch (...)
		{
			m_IV.Resize(0);
		}
	}
	else if (IS_AES_CCM(m_hAlg.ChainingMode) || IS_AES_GCM(m_hAlg.ChainingMode))
	{
		try
		{
			m_Nonce = ByteString::FromHex(GetText(IDC_CRPT_IV_EDIT).Trim());
		}
		catch (...)
		{
			m_Nonce.Resize(0);
		}
	}
	InvalidateRect(IDC_CRPT_IV_EDIT, NULL, FALSE);
	SetMode(m_Mode);
	UpdateMenus();
	UpdateButtons();
}


void CryptographyDialogBox::OnAaDataDisplayModeChange(int id)
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	DataDisplayMode current = m_AaDataDisplayMode;
	m_AaDataDisplayMode = ControlIdToDataDisplayMode(id);
	if (m_AaDataDisplayMode == current || GetTextLength(IDC_CRPT_AAD_EDIT) == 0)
	{
		return;
	}
	try
	{
		String text = GetText(IDC_CRPT_AAD_EDIT);
		ByteString data;
		switch (current)
		{
		case DataDisplayMode::HEX:
			data = ByteString::FromHex(text);
			break;
		case DataDisplayMode::BASE64:
			data = ByteString::FromBase64(text);
			break;
		default:
			data = ByteString::FromString(text, m_CodePage);
			break;
		}
		switch (m_AaDataDisplayMode)
		{
		case DataDisplayMode::HEX:
			text = data.ToHex(m_HexLetterCase);
			break;
		case DataDisplayMode::BASE64:
			text = data.ToBase64();
			break;
		default:
			text = data.ToString(m_CodePage);
			break;
		}
		SetText(IDC_CRPT_AAD_EDIT, text);
		ClearStatusText();
	}
	catch (Exception e)
	{
		ButtonUncheck(AaDataDisplayModeToControlId(m_AaDataDisplayMode));
		m_AaDataDisplayMode = current;
		ButtonCheck(AaDataDisplayModeToControlId(m_AaDataDisplayMode));
		m_bStatusSuccessful = FALSE;
		SetStatusText(L"%s", e.Message);
	}
}


void CryptographyDialogBox::OnOriginalDataChange()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	String szText = GetText(IDC_CRPT_ORG_EDIT);
	if (!szText.Len)
	{
		SetMode(MODE_IDLE);
	}
	else
	{
		SetMode(MODE_ENCRYPTION);
		try
		{
			switch (m_OriginalDataDisplayMode)
			{
			case DataDisplayMode::HEX:
				m_OriginalData = ByteString::FromHex(szText);
				break;
			case DataDisplayMode::BASE64:
				m_OriginalData = ByteString::FromBase64(szText);
				break;
			case DataDisplayMode::TEXT:
				m_OriginalData = ByteString::FromString(szText, m_CodePage, m_LineBreak);
				break;
			default:
				throw Exception(L"Bad Data Display Mode.");
			}
			ClearStatusText();
		}
		catch (Exception e)
		{
			m_OriginalData.Resize(0);
			m_bStatusSuccessful = FALSE;
			SetStatusText(L"%s", e.Message);
		}
	}
	UpdateMenus();
	UpdateButtons();
}


void CryptographyDialogBox::OnOriginalDataDisplayModeChange(int id)
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	DataDisplayMode current = m_OriginalDataDisplayMode;
	if (m_Mode == MODE_ENCRYPTION)
	{
		ClearStatusText();
	}
	m_OriginalDataDisplayMode =
		id == IDC_CRPT_ORG_HEX_RADIO ? DataDisplayMode::HEX :
		id == IDC_CRPT_ORG_BASE64_RADIO ? DataDisplayMode::BASE64 :
		id == IDC_CRPT_ORG_TEXT_RADIO ? DataDisplayMode::TEXT :
		current;
	if (m_OriginalDataDisplayMode != current)
	{
		SetText(IDC_CRPT_ORG_EDIT, OriginalDataToString());
		if (m_Mode == MODE_ENCRYPTION && m_OriginalDataDisplayMode == DataDisplayMode::TEXT)
		{
			m_OriginalData = ByteString::FromString(GetText(IDC_CRPT_ORG_EDIT), m_CodePage, m_LineBreak);
		}
	}
}


String CryptographyDialogBox::OriginalDataToString()
{
	try
	{
		static const size_t N = 54;
		switch (m_OriginalDataDisplayMode)
		{
		case DataDisplayMode::HEX:
			if (m_bWrapData)
			{
				return m_OriginalData.ToHex(m_HexLetterCase).Wrap(N * 2);
			}
			else
			{
				return m_OriginalData.ToHex(m_HexLetterCase);
			}
		case DataDisplayMode::BASE64:
			if (m_bWrapData)
			{
				return m_OriginalData.ToBase64().Wrap((N / 3) * 4);
			}
			else
			{
				return m_OriginalData.ToBase64();
			}
		case DataDisplayMode::TEXT:
			return m_OriginalData.ToString(m_CodePage).ChangeLineBreak(LineBreak::CRLF);
		default: // SHOULD NEVER REACH HERE
			return String(L"BAD DISPLAY MODE");
		}
	}
	catch (Exception e)
	{
		m_bStatusSuccessful = FALSE;
		SetStatusText(L"%s", e.Message);
		return String::Empty;
	}
}


void CryptographyDialogBox::OnEncryptedDataChange()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	String szText = GetText(IDC_CRPT_ENC_EDIT);
	if (!szText.Len)
	{
		SetMode(MODE_IDLE);
	}
	else
	{
		SetMode(MODE_DECRYPTION);
		try
		{
			switch (m_EncryptedDataDisplayMode)
			{
			case DataDisplayMode::HEX:
				m_EncryptedData = ByteString::FromHex(szText);
				break;
			case DataDisplayMode::BASE64:
				m_EncryptedData = ByteString::FromBase64(szText);
				break;
			default:
				throw Exception(L"Bad DataDisplayMode for Encrypted Data.");
			}
			ClearStatusText();
		}
		catch (Exception e)
		{
			m_EncryptedData.Resize(0);
			m_bStatusSuccessful = FALSE;
			SetStatusText(L"%s", e.Message);
		}
	}
	UpdateMenus();
	UpdateButtons();
}


void CryptographyDialogBox::OnEncryptedDataDisplayModeChange(int id)
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	DataDisplayMode current = m_EncryptedDataDisplayMode;
	if (m_Mode == MODE_DECRYPTION)
	{
		ClearStatusText();
	}
	m_EncryptedDataDisplayMode =
		id == IDC_CRPT_ENC_HEX_RADIO ? DataDisplayMode::HEX :
		id == IDC_CRPT_ENC_BASE64_RADIO ? DataDisplayMode::BASE64 :
		current;
	if (m_EncryptedDataDisplayMode != current)
	{
		SetText(IDC_CRPT_ENC_EDIT, EncryptedDataToString());
	}
}


String CryptographyDialogBox::EncryptedDataToString()
{
	static const size_t N = 54;
	switch (m_EncryptedDataDisplayMode)
	{
	case DataDisplayMode::HEX:
		if (m_bWrapData)
		{
			return m_EncryptedData.ToHex(m_HexLetterCase).Wrap(N * 2);
		}
		else
		{
			return m_EncryptedData.ToHex(m_HexLetterCase);
		}
	case DataDisplayMode::BASE64:
		if (m_bWrapData)
		{
			return m_EncryptedData.ToBase64().Wrap((N / 3) * 4);
		}
		else
		{
			return m_EncryptedData.ToBase64();
		}
	default: // SHOULD NEVER REACH HERE
		return String(L"BAD DISPLAY MODE");
	}
}


void CryptographyDialogBox::OnHexFormatChange()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	StringOptions current = m_HexLetterCase;
	m_HexLetterCase = (StringOptions)ComboBoxGetSelection(IDC_CRPT_HEXLETTER_COMBO, StringOptions::UPPERCASE);
	if (m_HexLetterCase == current)
	{
		return;
	}
	if (GetTextLength(IDC_CRPT_KEY_EDIT) > 0)
	{
		SetText(IDC_CRPT_KEY_EDIT, GetText(IDC_CRPT_KEY_EDIT).Lettercase(m_HexLetterCase));
	}
	if (GetTextLength(IDC_CRPT_IV_EDIT) > 0)
	{
		SetText(IDC_CRPT_IV_EDIT, GetText(IDC_CRPT_IV_EDIT).Lettercase(m_HexLetterCase));
	}
	if (m_AaDataDisplayMode == DataDisplayMode::HEX && GetTextLength(IDC_CRPT_AAD_EDIT) > 0)
	{
		SetText(IDC_CRPT_AAD_EDIT, GetText(IDC_CRPT_AAD_EDIT).Lettercase(m_HexLetterCase));
	}
	if (m_OriginalDataDisplayMode == DataDisplayMode::HEX && m_OriginalData.Len > 0)
	{
		SetText(IDC_CRPT_ORG_EDIT, OriginalDataToString());
	}
	if (m_EncryptedDataDisplayMode == DataDisplayMode::HEX && m_EncryptedData.Len > 0)
	{
		SetText(IDC_CRPT_ENC_EDIT, EncryptedDataToString());
	}
}


void CryptographyDialogBox::OnCodePageChange()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	UINT previous = m_CodePage;
	m_CodePage = ComboBoxGetSelection(IDC_CRPT_CODEPAGE_COMBO, CP_UTF8);
	int checkPoint;
	try
	{
		checkPoint = 1;
		ByteString bsOriginalData;
		String strOriginal;
		if (m_Mode == MODE_ENCRYPTION)
		{
			if (m_OriginalDataDisplayMode == DataDisplayMode::TEXT)
			{
				bsOriginalData = ByteString::FromString(GetText(IDC_CRPT_ORG_EDIT), m_CodePage, m_LineBreak);
			}
		}
		else if (m_Mode == MODE_DECRYPTION)
		{
			if (m_OriginalDataDisplayMode == DataDisplayMode::TEXT)
			{
				strOriginal = m_OriginalData.ToString(m_CodePage).ChangeLineBreak(LineBreak::CRLF);
			}
		}
		checkPoint = 2;
		if (m_AaDataDisplayMode == DataDisplayMode::TEXT)
		{
			ByteString::FromString(GetText(IDC_CRPT_AAD_EDIT), m_CodePage, m_LineBreak);
		}
		if (m_Mode == MODE_ENCRYPTION)
		{
			if (m_OriginalDataDisplayMode == DataDisplayMode::TEXT)
			{
				m_OriginalData = bsOriginalData;
			}
		}
		else if (m_Mode == MODE_DECRYPTION)
		{
			if (m_OriginalDataDisplayMode == DataDisplayMode::TEXT)
			{
				SetText(IDC_CRPT_ORG_EDIT, strOriginal);
			}
		}
		ClearStatusText();
	}
	catch (Exception e)
	{
		m_bStatusSuccessful = FALSE;
		SetStatusText(L"%s: %s", checkPoint == 1 ? L"Original Data" : L"Additional Authenticated Data", e.Message);
		m_CodePage = previous;
		ComboBoxSetSelection(IDC_CRPT_CODEPAGE_COMBO, m_CodePage);
	}
}


void CryptographyDialogBox::OnLineBreakChange()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	LineBreak previous = m_LineBreak;
	m_LineBreak = static_cast<LineBreak>(ComboBoxGetSelection(IDC_CRPT_LINEBREAK_COMBO, LineBreak::CRLF));
	if (m_Mode == MODE_ENCRYPTION)
	{
		if (m_OriginalDataDisplayMode == DataDisplayMode::TEXT)
		{
			m_OriginalData = ByteString::FromString(GetText(IDC_CRPT_ORG_EDIT), m_CodePage, m_LineBreak);
			ClearStatusText();
		}
	}
}


void CryptographyDialogBox::SetMode(int value)
{
	switch (value)
	{
	case MODE_IDLE:
		if (m_OriginalData.Len)
		{
			m_OriginalData.Resize(0);
			SetText(IDC_CRPT_ORG_EDIT);
		}
		if (m_EncryptedData.Len)
		{
			m_EncryptedData.Resize(0);
			SetText(IDC_CRPT_ENC_EDIT);
		}
		EditSetReadOnly(IDC_CRPT_ORG_EDIT, FALSE);
		EditSetReadOnly(IDC_CRPT_ENC_EDIT, FALSE);
		ClearStatusText();
		UpdateButtons();
		break;
	case MODE_ENCRYPTION:
		switch (m_Mode)
		{
		case MODE_IDLE:
			EditSetReadOnly(IDC_CRPT_ENC_EDIT);
			break;
		case MODE_ENCRYPTION:
			if (m_EncryptedData.Len)
			{
				m_EncryptedData.Resize(0);
				SetText(IDC_CRPT_ENC_EDIT);
				ClearStatusText();
				UpdateButtons();
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
				m_OriginalData.Resize(0);
				SetText(IDC_CRPT_ORG_EDIT);
				ClearStatusText();
				UpdateButtons();
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
		.Enable(IDM_FILE_LOAD1FROM, (m_Mode == MODE_IDLE || m_Mode == MODE_ENCRYPTION) ? MF_ENABLED : MF_DISABLED)
		.Enable(IDM_FILE_SAVE1AS, (m_OriginalData.Len > 0) ? MF_ENABLED : MF_DISABLED)
		.Enable(IDM_FILE_LOAD2FROM, (m_Mode == MODE_IDLE || m_Mode == MODE_DECRYPTION) ? MF_ENABLED : MF_DISABLED)
		.Enable(IDM_FILE_SAVE2AS, (m_EncryptedData.Len > 0) ? MF_ENABLED : MF_DISABLED);
	m_menuSettings
		.Modify(IDM_SETTINGS_WRAPDATA, MF_BYCOMMAND | (m_bWrapData ? MF_CHECKED : MFS_UNCHECKED), IDM_SETTINGS_WRAPDATA, ResourceString(IDS_MENU_WRAPDATA));
}


void CryptographyDialogBox::UpdateButtons()
{
	BOOL bEncrypt = FALSE;
	BOOL bDecrypt = FALSE;
	if (IS_AES_CBC(m_hAlg.ChainingMode) || IS_AES_CFB(m_hAlg.ChainingMode))
	{
		if (m_Key.Len * 8 >= m_KeyLength && m_IV.Len >= m_hAlg.BlockLength)
		{
			bEncrypt = m_OriginalData.Len > 0 ? TRUE : FALSE;
			bDecrypt = m_EncryptedData.Len > 0 ? TRUE : FALSE;
		}
	}
	else if (IS_AES_ECB(m_hAlg.ChainingMode))
	{
		if (m_Key.Len * 8 >= m_KeyLength)
		{
			bEncrypt = m_OriginalData.Len > 0 ? TRUE : FALSE;
			bDecrypt = m_EncryptedData.Len > 0 ? TRUE : FALSE;
		}
	}
	else if (IS_AES_CCM(m_hAlg.ChainingMode))
	{
		if (m_Key.Len * 8 >= m_KeyLength)
		{
			bEncrypt = m_Nonce.Len >= AES_CCM_NONCE_LENGTH && m_OriginalData.Len > 0 ? TRUE : FALSE;
			bDecrypt = m_Nonce.Len >= AES_CCM_NONCE_LENGTH && m_EncryptedData.Len > m_CcmTagLength ? TRUE : FALSE;
		}
	}
	else if (IS_AES_GCM(m_hAlg.ChainingMode))
	{
		if (m_Key.Len * 8 >= m_KeyLength)
		{
			bEncrypt = m_Nonce.Len >= AES_GCM_NONCE_LENGTH && m_OriginalData.Len > 0 ? TRUE : FALSE;
			bDecrypt = m_Nonce.Len >= AES_GCM_NONCE_LENGTH && m_EncryptedData.Len > m_GcmTagLength ? TRUE : FALSE;
		}
	}
	EnableWindow(IDC_CRPT_ENCRYPT_BUTTON, bEncrypt);
	EnableWindow(IDC_CRPT_DECRYPT_BUTTON, bDecrypt);
	EnableWindow(IDC_CRPT_COPYORG_BUTTON, GetTextLength(IDC_CRPT_ORG_EDIT) > 0 ? TRUE : FALSE);
	EnableWindow(IDC_CRPT_COPYENC_BUTTON, GetTextLength(IDC_CRPT_ENC_EDIT) > 0 ? TRUE : FALSE);
}


void CryptographyDialogBox::UpdateTagSizeRadioBoxes()
{
	DWORD dwFlags = 0UL;
	if (IS_AES_CCM(m_hAlg.ChainingMode) || IS_AES_GCM(m_hAlg.ChainingMode))
	{
		std::vector<DWORD> tagLengths = m_hAlg.AuthTagLengths;
		for (size_t index = 0; index < tagLengths.size(); index++)
		{
			dwFlags |= 1UL << tagLengths[index];
		}
	}
	EnableWindow(IDC_CRPT_TAG_GROUP, !!dwFlags);
	EnableWindow(IDC_CRPT_TAG32_RADIO, !!(dwFlags & FLAG_TAG32));
	EnableWindow(IDC_CRPT_TAG48_RADIO, !!(dwFlags & FLAG_TAG48));
	EnableWindow(IDC_CRPT_TAG64_RADIO, !!(dwFlags & FLAG_TAG64));
	EnableWindow(IDC_CRPT_TAG80_RADIO, !!(dwFlags & FLAG_TAG80));
	EnableWindow(IDC_CRPT_TAG96_RADIO, !!(dwFlags & FLAG_TAG96));
	EnableWindow(IDC_CRPT_TAG104_RADIO, !!(dwFlags & FLAG_TAG104));
	EnableWindow(IDC_CRPT_TAG112_RADIO, !!(dwFlags & FLAG_TAG112));
	EnableWindow(IDC_CRPT_TAG120_RADIO, !!(dwFlags & FLAG_TAG120));
	EnableWindow(IDC_CRPT_TAG128_RADIO, !!(dwFlags & FLAG_TAG128));
	if (IS_AES_CCM(m_hAlg.ChainingMode))
	{
		ButtonUncheck(TagLengthToControlId(m_GcmTagLength));
		ButtonCheck(TagLengthToControlId(m_CcmTagLength));
	}
	else if (IS_AES_GCM(m_hAlg.ChainingMode))
	{
		ButtonUncheck(TagLengthToControlId(m_CcmTagLength));
		ButtonCheck(TagLengthToControlId(m_GcmTagLength));
	}
	else
	{
		ButtonUncheck(TagLengthToControlId(m_CcmTagLength));
		ButtonUncheck(TagLengthToControlId(m_GcmTagLength));
	}
}


ByteString CryptographyDialogBox::GetAaData() const
{
	String aadString = GetText(IDC_CRPT_AAD_EDIT);
	switch (m_AaDataDisplayMode)
	{
	case DataDisplayMode::HEX:
		return ByteString::FromHex(aadString);
	case DataDisplayMode::BASE64:
		return ByteString::FromBase64(aadString);
	case DataDisplayMode::TEXT:
	default:
		return ByteString::FromString(aadString, m_CodePage);
	}
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


int CryptographyDialogBox::ControlIdToTagLength(int id)
{
	switch (id)
	{
	case IDC_CRPT_TAG32_RADIO:  return  4;
	case IDC_CRPT_TAG48_RADIO:  return  6;
	case IDC_CRPT_TAG64_RADIO:  return  8;
	case IDC_CRPT_TAG80_RADIO:  return 10;
	case IDC_CRPT_TAG96_RADIO:  return 12;
	case IDC_CRPT_TAG104_RADIO: return 13;
	case IDC_CRPT_TAG112_RADIO: return 14;
	case IDC_CRPT_TAG120_RADIO: return 15;
	case IDC_CRPT_TAG128_RADIO: return 16;
	default:
		throw Exception(L"CryptographyDialogBox::ControlIdToTagLength: Bad ID: %d", id);
	}
}


int CryptographyDialogBox::TagLengthToControlId(int length)
{
	switch (length)
	{
	case  4: return IDC_CRPT_TAG32_RADIO;
	case  6: return IDC_CRPT_TAG48_RADIO;
	case  8: return IDC_CRPT_TAG64_RADIO;
	case 10: return IDC_CRPT_TAG80_RADIO;
	case 12: return IDC_CRPT_TAG96_RADIO;
	case 13: return IDC_CRPT_TAG104_RADIO;
	case 14: return IDC_CRPT_TAG112_RADIO;
	case 15: return IDC_CRPT_TAG120_RADIO;
	case 16: return IDC_CRPT_TAG128_RADIO;
	default:
		throw Exception(L"CryptographyDialogBox::TagLengthToControlId: Bad length: %d", length);
	}
}


DataDisplayMode CryptographyDialogBox::ControlIdToDataDisplayMode(int id)
{
	switch (id)
	{
	case IDC_CRPT_ORG_HEX_RADIO:
	case IDC_CRPT_ENC_HEX_RADIO:
	case IDC_CRPT_AAD_HEX_RADIO:
		return DataDisplayMode::HEX;
	case IDC_CRPT_ORG_BASE64_RADIO:
	case IDC_CRPT_ENC_BASE64_RADIO:
	case IDC_CRPT_AAD_BASE64_RADIO:
		return DataDisplayMode::BASE64;
	case IDC_CRPT_ORG_TEXT_RADIO:
	case IDC_CRPT_AAD_TEXT_RADIO:
		return DataDisplayMode::TEXT;
	default:
		throw Exception(L"CryptographyDialogBox::ControlIdToDataDisplayMode: Bad value: %d", id);
	}
}


int CryptographyDialogBox::OriginalDataDisplayModeToControlId(DataDisplayMode mode)
{
	switch (mode)
	{
	case DataDisplayMode::HEX:
		return IDC_CRPT_ORG_HEX_RADIO;
	case DataDisplayMode::BASE64:
		return IDC_CRPT_ORG_BASE64_RADIO;
	case DataDisplayMode::TEXT:
	default:
		return IDC_CRPT_ORG_TEXT_RADIO;
	}
}


int CryptographyDialogBox::EncryptedDataDisplayModeToControlId(DataDisplayMode mode)
{
	switch (mode)
	{
	case DataDisplayMode::HEX:
	default:
		return IDC_CRPT_ENC_HEX_RADIO;
	case DataDisplayMode::BASE64:
		return IDC_CRPT_ENC_BASE64_RADIO;
	}
}


int CryptographyDialogBox::AaDataDisplayModeToControlId(DataDisplayMode mode)
{
	switch (mode)
	{
	case DataDisplayMode::HEX:
		return IDC_CRPT_AAD_HEX_RADIO;
	case DataDisplayMode::BASE64:
		return IDC_CRPT_AAD_BASE64_RADIO;
	case DataDisplayMode::TEXT:
	default:
		return IDC_CRPT_AAD_TEXT_RADIO;
	}
}
