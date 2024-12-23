#include "pch.h"
#include "PercentCodecDialogBox.h"
#include "MyToolbox.h"
#include "resource.h"
#include "hnrt/RegistryKey.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/Menu.h"
#include "hnrt/WindowDesign.h"
#include "hnrt/WindowHandle.h"
#include "hnrt/WindowLayoutSnapshot.h"
#include "hnrt/WhileInScope.h"
#include "hnrt/Clipboard.h"
#include "hnrt/ResourceString.h"
#include "hnrt/Buffer.h"
#include "hnrt/StringCommons.h"
#include "hnrt/Exception.h"
#include "hnrt/Debug.h"


#define REGVAL_INPUT_CODEPAGE L"InputCodePage"
#define REGVAL_OUTPUT_CODEPAGE L"OutputCodePage"
#define REGVAL_OUTPUT_BOM L"OutputBOM"
#define REGVAL_ORIGINAL_PATH L"OriginalPath"
#define REGVAL_ENCODED_PATH L"EncodedPath"


#define PCTC_TIMER1000MS 10300


using namespace hnrt;


PercentCodecDialogBox::PercentCodecDialogBox()
	: MyDialogBox(IDD_PCTC, L"PercentCodec")
	, m_bEncodingError(false)
	, m_bDecodingError(false)
	, m_szOriginalPath()
	, m_szEncodedPath()
{
}


void PercentCodecDialogBox::OnCreate()
{
	MyDialogBox::OnCreate();
	RegistryKey hKey;
	LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, m_szRegistryKeyName);
	if (rc == ERROR_SUCCESS)
	{
		m_uInputCodePage = RegistryValue::GetDWORD(hKey, REGVAL_INPUT_CODEPAGE, CP_AUTODETECT);
		m_uOutputCodePage = RegistryValue::GetDWORD(hKey, REGVAL_OUTPUT_CODEPAGE, CP_UTF8);
		m_bOutputBOM = RegistryValue::GetDWORD(hKey, REGVAL_OUTPUT_BOM, 0) ? true : false;
		m_szOriginalPath = RegistryValue::GetSZ(hKey, REGVAL_ORIGINAL_PATH);
		m_szEncodedPath = RegistryValue::GetSZ(hKey, REGVAL_ENCODED_PATH);
	}
	HFONT hFont = GetApp<MyToolbox>().GetFontForData();
	SetFont(IDC_PCTC_ORG_EDIT, hFont);
	SetFont(IDC_PCTC_ENC_EDIT, hFont);
	InitializeCodePageComboBox(IDC_PCTC_CODEPAGE_COMBO);
	ButtonCheck(IDC_PCTC_USEPLUS_CHECK);
	ComboBoxRemove(IDC_PCTC_CODEPAGE_COMBO, CP_UTF16);
	SetText(IDC_PCTC_ORG_STATIC, ResourceString(IDS_ORIGINAL));
	SetText(IDC_PCTC_ENC_STATIC, ResourceString(IDS_ENCODED));
	m_menuView
		.Add(ResourceString(IDS_MENU_PCTC), IDM_VIEW_PCTC);
	UpdateControlsState(IDC_PCTC_ORG_EDIT);
	UpdateControlsState(IDC_PCTC_ENC_EDIT);
}


void PercentCodecDialogBox::OnDestroy()
{
	RegistryKey hKey;
	LSTATUS rc = hKey.Create(HKEY_CURRENT_USER, m_szRegistryKeyName);
	if (rc == ERROR_SUCCESS)
	{
		RegistryValue::SetDWORD(hKey, REGVAL_INPUT_CODEPAGE, m_uInputCodePage);
		RegistryValue::SetDWORD(hKey, REGVAL_OUTPUT_CODEPAGE, m_uOutputCodePage);
		RegistryValue::SetDWORD(hKey, REGVAL_OUTPUT_BOM, m_bOutputBOM ? 1 : 0);
		RegistryValue::SetSZ(hKey, REGVAL_ORIGINAL_PATH, m_szOriginalPath);
		RegistryValue::SetSZ(hKey, REGVAL_ENCODED_PATH, m_szEncodedPath);
	}
	SetFont(IDC_PCTC_ORG_EDIT, NULL);
	SetFont(IDC_PCTC_ENC_EDIT, NULL);
	MyDialogBox::OnDestroy();
}


void PercentCodecDialogBox::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
	WindowLayoutSnapshot before, after;

	before.AddAllChildren(hDlg).Clone(after);

	after[IDC_PCTC_ORG_STATIC].right += cxDelta;
	after[IDC_PCTC_ORG_EDIT].right += cxDelta;

	MoveHorizontally(after[IDC_PCTC_ORG_COPY_BUTTON], cxDelta);
	MoveHorizontally(after[IDC_PCTC_ENCODE_BUTTON], cxDelta);
	MoveHorizontally(after[IDC_PCTC_USEPLUS_CHECK], cxDelta);
	MoveHorizontally(after[IDC_PCTC_CODEPAGE_COMBO], cxDelta);

	after[IDC_PCTC_ENC_STATIC].right += cxDelta;
	after[IDC_PCTC_ENC_EDIT].right += cxDelta;

	MoveHorizontally(after[IDC_PCTC_ENC_COPY_BUTTON], cxDelta);
	MoveHorizontally(after[IDC_PCTC_DECODE_BUTTON], cxDelta);

	ExtendVertically(after[IDC_PCTC_ORG_EDIT], after[IDC_PCTC_ENC_EDIT], cyDelta);

	LONG dy = after[IDC_PCTC_ORG_EDIT].bottom - before[IDC_PCTC_ORG_EDIT].bottom;

	MoveVertically(after[IDC_PCTC_ENC_STATIC], dy);
	MoveVertically(after[IDC_PCTC_ENC_COPY_BUTTON], dy);
	MoveVertically(after[IDC_PCTC_DECODE_BUTTON], dy);

	after.Apply();
}


void PercentCodecDialogBox::OnTabSelectionChanging()
{
	MyDialogBox::OnTabSelectionChanging();
	KillTimer(hwnd, PCTC_TIMER1000MS);
	m_menuView
		.Enable(IDM_VIEW_PCTC, MF_ENABLED);
}


void PercentCodecDialogBox::OnTabSelectionChanged()
{
	MyDialogBox::OnTabSelectionChanged();
	m_menuFile
		.RemoveAll()
		.Add(ResourceString(IDS_MENU_ORG_LOADFROM), IDM_FILE_LOAD1FROM)
		.Add(ResourceString(IDS_MENU_ORG_SAVEAS), IDM_FILE_SAVE1AS)
		.AddSeparator()
		.Add(ResourceString(IDS_MENU_ENC_LOADFROM), IDM_FILE_LOAD2FROM)
		.Add(ResourceString(IDS_MENU_ENC_SAVEAS), IDM_FILE_SAVE2AS)
		.AddSeparator()
		.Add(ResourceString(IDS_MENU_EXIT), IDM_FILE_EXIT);
	m_menuEdit
		.RemoveAll();
	AddEditControlMenus(m_CurrentEdit);
	m_menuEdit
		.AddSeparator()
		.Add(ResourceString(IDS_MENU_CLEAR), IDM_EDIT_CLEAR);
	m_menuView
		.Enable(IDM_VIEW_PCTC, MF_DISABLED);
	m_menuSettings
		.RemoveAll();
	AddInputCodePageSettingMenus();
	AddOutputCodePageSettingMenus();
	SetTimer(hwnd, PCTC_TIMER1000MS, 1000, NULL);
}


INT_PTR PercentCodecDialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
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
	case IDC_PCTC_ORG_COPY_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			CopyAllText(IDC_PCTC_ORG_EDIT);
		}
		break;
	case IDC_PCTC_ENC_COPY_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			CopyAllText(IDC_PCTC_ENC_EDIT);
		}
		break;
	case IDC_PCTC_ENCODE_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			if (OnEncode())
			{
				SetFocus(IDC_PCTC_ENC_COPY_BUTTON);
			}
		}
		break;
	case IDC_PCTC_DECODE_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			if (OnDecode())
			{
				SetFocus(IDC_PCTC_ORG_COPY_BUTTON);
			}
		}
		break;
	case IDC_PCTC_ORG_EDIT:
	case IDC_PCTC_ENC_EDIT:
		switch (idNotif)
		{
		case EN_SETFOCUS:
			OnEditSetFocus(idChild);
			break;
		case EN_KILLFOCUS:
			OnEditKillFocus(idChild);
			break;
		case EN_CHANGE:
			OnEditChanged(idChild);
			break;
		default:
			break;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}


INT_PTR PercentCodecDialogBox::OnTimer(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case PCTC_TIMER1000MS:
		UpdateEditControlMenus(m_CurrentEdit);
		break;
	default:
		break;
	}
	return 0;
}


INT_PTR PercentCodecDialogBox::OnControlColorStatic(WPARAM wParam, LPARAM lParam)
{
	HDC hdc = reinterpret_cast<HDC>(wParam);
	int id = GetDlgCtrlID(reinterpret_cast<HWND>(lParam));
	switch (id)
	{
	case IDC_PCTC_ORG_STATIC:
		SetTextColor(hdc, m_bEncodingError ? RGB_ERROR : GetSysColor(COLOR_WINDOWTEXT));
		SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
		return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_3DFACE));
	case IDC_PCTC_ENC_STATIC:
		SetTextColor(hdc, m_bDecodingError ? RGB_ERROR : GetSysColor(COLOR_WINDOWTEXT));
		SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
		return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_3DFACE));
	default:
		break;
	}
	return 0;
}


void PercentCodecDialogBox::OnLoad1From()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	LoadTextFromFile(IDC_PCTC_ORG_EDIT, m_szOriginalPath);
}


void PercentCodecDialogBox::OnSave1As()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	SaveTextAsFile(IDC_PCTC_ORG_EDIT, m_szOriginalPath);
}


void PercentCodecDialogBox::OnLoad2From()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	LoadTextFromFile(IDC_PCTC_ENC_EDIT, m_szEncodedPath);
}


void PercentCodecDialogBox::OnSave2As()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	SaveTextAsFile(IDC_PCTC_ENC_EDIT, m_szEncodedPath);
}


void PercentCodecDialogBox::OnClear()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	SetText(IDC_PCTC_ORG_EDIT);
	SetText(IDC_PCTC_ENC_EDIT);
	OnEditChanged(IDC_PCTC_ORG_EDIT);
	OnEditChanged(IDC_PCTC_ENC_EDIT);
	m_LastModified.By = 0;
}


void PercentCodecDialogBox::OnSettingChanged(UINT uId)
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


bool PercentCodecDialogBox::OnEncode()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	try
	{
		String szText = GetText(IDC_PCTC_ORG_EDIT);
		UINT uCodePage = GetCodePage();
		Buffer<WCHAR> buf((szText.Len + 1ULL) * 4ULL * 3ULL);
		Encode(szText, static_cast<UINT>(szText.Len + 1ULL), uCodePage, buf, static_cast<UINT>(buf.Len), ButtonIsChecked(IDC_PCTC_USEPLUS_CHECK));
		SetText(IDC_PCTC_ENC_EDIT, buf);
		OnEditChanged(IDC_PCTC_ENC_EDIT);
		ClearStatus(IDC_PCTC_ORG_EDIT);
		return true;
	}
	catch (Exception e)
	{
		m_bEncodingError = true;
		SetText(IDC_PCTC_ORG_STATIC, String(PRINTF, L"%s [ %s ]", ResourceString(IDS_ORIGINAL), e.Message));
		return false;
	}
}


bool PercentCodecDialogBox::OnDecode()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	try
	{
		ClearStatus();
		String szText = GetText(IDC_PCTC_ENC_EDIT);
		UINT uCodePage = GetCodePage();
		Buffer<WCHAR> buf(szText.Len + 1ULL);
		Decode(szText, static_cast<UINT>(szText.Len + 1ULL), uCodePage, buf, static_cast<UINT>(buf.Len));
		SetText(IDC_PCTC_ORG_EDIT, buf);
		OnEditChanged(IDC_PCTC_ORG_EDIT);
		ClearStatus(IDC_PCTC_ENC_EDIT);
		return true;
	}
	catch (Exception e)
	{
		m_bDecodingError = true;
		SetText(IDC_PCTC_ENC_STATIC, String(PRINTF, L"%s [ %s ]", ResourceString(IDS_ENCODED), e.Message));
		return false;
	}
}


UINT PercentCodecDialogBox::GetCodePage() const
{
	return ComboBoxGetSelection(IDC_PCTC_CODEPAGE_COMBO, CP_UTF8);
}


#define ENCODE_BUFFER_TOO_SHORT L"PercentCodec::Encode: Buffer is too short."


void PercentCodecDialogBox::Encode(PCWSTR pszIn, UINT cchIn, UINT uCodePage, PWCHAR pOut, UINT cchOut, bool bUsePlus)
{
	PWCHAR pOutBound = pOut + cchOut;
	const WCHAR* pCur = pszIn;
	const WCHAR* pEnd = pszIn + cchIn;
	while (pCur < pEnd)
	{
		WCHAR c = *pCur++;
		switch (c)
		{
		case L' ':
			if (bUsePlus)
			{
				if (pOut < pOutBound)
				{

					*pOut++ = L'+';
				}
				else
				{
					throw Exception(ENCODE_BUFFER_TOO_SHORT);
				}
				break;
			}
			//FALLTHROUGH
		case L':':
		case L'/':
		case L'?':
		case L'#':
		case L'[':
		case L']':
		case L'@':
		case L'!':
		case L'$':
		case L'&':
		case L'\'':
		case L'(':
		case L')':
		case L'*':
		case L'+':
		case L',':
		case L';':
		case L'=':
		case L'%':
			if (pOut + 3 <= pOutBound)
			{
				pOut += swprintf_s(pOut, pOutBound - pOut, L"%%%02X", c);
			}
			else
			{
				throw Exception(ENCODE_BUFFER_TOO_SHORT);
			}
			break;
		default:
			if (c < 0x80)
			{
				if (pOut < pOutBound)
				{
					*pOut++ = c;
				}
				else
				{
					throw Exception(ENCODE_BUFFER_TOO_SHORT);
				}
			}
			else
			{
				Encode(c, uCodePage, pOut, pOutBound, static_cast<UINT>(pCur - 1 - pszIn));
			}
			break;
		}
	}
}


void PercentCodecDialogBox::Encode(WCHAR c, UINT uCodePage, PWCHAR& pOut, PWCHAR pOutBound, UINT offset)
{
	DWORD dwFlags = (uCodePage == CP_UTF8 || uCodePage == CP_GB18030) ? WC_ERR_INVALID_CHARS : 0;
	BOOL bDefaultCharUse = FALSE;
	PBOOL pbDefaultCharUse = uCodePage == CP_UTF8 ? NULL : &bDefaultCharUse;
	CHAR mb[16] = { 0 };
	int cb = WideCharToMultiByte(uCodePage, dwFlags, &c, 1, mb, _countof(mb), NULL, pbDefaultCharUse);
	if (!cb || bDefaultCharUse)
	{
		throw Exception(ResourceString(IDS_PCTC_ERROR), offset);
	}
	else if (pOut + cb * 3 > pOutBound)
	{
		throw Exception(ENCODE_BUFFER_TOO_SHORT);
	}
	PBYTE pCur = reinterpret_cast<PBYTE>(mb);
	PBYTE pEnd = pCur + cb;
	while (pCur < pEnd)
	{
		UINT b = *pCur++;
		pOut += swprintf_s(pOut, pOutBound - pOut, L"%%%02X", b);
	}
}


#define DECODE_BUFFER_TOO_SHORT L"PercentCodec::Decode: Buffer is too short."


void PercentCodecDialogBox::Decode(PCWSTR pszIn, UINT cchIn, UINT uCodePage, PWCHAR pOut, UINT cchOut)
{
	Buffer<CHAR> mbBuf(cchIn);
	PBYTE pDst = reinterpret_cast<PBYTE>(mbBuf.Ptr);
	PBYTE pDstBound = pDst + mbBuf.Len;
	const WCHAR* pCur = pszIn;
	const WCHAR* pEnd = pCur + cchIn;
	while (pCur < pEnd)
	{
		WCHAR c = *pCur++;
		BYTE d;
		switch (c)
		{
		case L'+':
			if (pDst < pDstBound)
			{
				*pDst++ = ' ';
			}
			else
			{
				throw Exception(DECODE_BUFFER_TOO_SHORT);
			}
			break;
		case L'%':
			pCur += 2;
			if (pCur > pEnd)
			{
				throw Exception(ResourceString(IDS_PCTC_ERROR), static_cast<UINT>(pCur - 1 - pszIn));
			}
			if (L'0' <= pCur[-2] && pCur[-2] <= L'9')
			{
				d = pCur[-2] - L'0';
			}
			else if (L'A' <= pCur[-2] && pCur[-2] <= L'F')
			{
				d = pCur[-2] - L'A' + 10;
			}
			else if (L'a' <= pCur[-2] && pCur[-2] <= L'f')
			{
				d = pCur[-2] - L'a' + 10;
			}
			else
			{
				throw Exception(ResourceString(IDS_PCTC_ERROR), static_cast<UINT>(pCur - 2 - pszIn));
			}
			if (L'0' <= pCur[-1] && pCur[-1] <= L'9')
			{
				d = d * 16 + pCur[-1] - L'0';
			}
			else if (L'A' <= pCur[-1] && pCur[-1] <= L'F')
			{
				d = d * 16 + pCur[-1] - L'A' + 10;
			}
			else if (L'a' <= pCur[-1] && pCur[-1] <= L'f')
			{
				d = d * 16 + pCur[-1] - L'a' + 10;
			}
			else
			{
				throw Exception(ResourceString(IDS_PCTC_ERROR), static_cast<UINT>(pCur - 1 - pszIn));
			}
			if (pDst < pDstBound)
			{
				*pDst++ = d;
			}
			else
			{
				throw Exception(DECODE_BUFFER_TOO_SHORT);
			}
			break;
		case ' ':
		case ':':
		case '/':
		case '?':
		case '#':
		case '[':
		case ']':
		case '@':
		case '!':
		case '$':
		case '&':
		case '\'':
		case '(':
		case ')':
		case '*':
		case ',':
		case ';':
		case '=':
			throw Exception(ResourceString(IDS_PCTC_ERROR), static_cast<UINT>(pCur - 1 - pszIn));
		default:
			if (c < 0x80)
			{
				if (pDst < pDstBound)
				{
					*pDst++ = static_cast<BYTE>(c);
				}
				else
				{
					throw Exception(DECODE_BUFFER_TOO_SHORT);
				}
			}
			else
			{
				throw Exception(ResourceString(IDS_PCTC_ERROR), static_cast<UINT>(pCur - 1 - pszIn));
			}
			break;
		}
	}
	int cb = static_cast<int>(pDst - reinterpret_cast<PBYTE>(mbBuf.Ptr));
	DWORD dwFlags = ((uCodePage == CP_UTF8 || uCodePage == 54936) ? 0 : MB_PRECOMPOSED) | MB_ERR_INVALID_CHARS;
	int cch = MultiByteToWideChar(uCodePage, dwFlags, mbBuf, cb, NULL, 0);
	if (!cch)
	{
		while (cb > 0)
		{
			if (MultiByteToWideChar(uCodePage, dwFlags, mbBuf, --cb, NULL, 0))
			{
				break;
			}
		}
		throw Exception(ResourceString(IDS_PCTC_ERROR), GetDecodedOffset(pszIn, cb));
	}
	MultiByteToWideChar(uCodePage, dwFlags, mbBuf, cb, pOut, cchOut);
}


UINT PercentCodecDialogBox::GetDecodedOffset(PCWSTR pszIn, UINT cbOut)
{
	const WCHAR* pCur = pszIn;
	while (cbOut > 0)
	{
		WCHAR c = *pCur++;
		switch (c)
		{
		case L'+':
			cbOut--;
			break;
		case L'%':
			pCur += 2;
			if (L'0' <= pCur[-2] && pCur[-2] <= L'9')
			{
				// OK
			}
			else if (L'A' <= pCur[-2] && pCur[-2] <= L'F')
			{
				// OK
			}
			else if (L'a' <= pCur[-2] && pCur[-2] <= L'f')
			{
				// OK
			}
			else
			{
				throw Exception(ResourceString(IDS_PCTC_ERROR), static_cast<UINT>(pCur - 2 - pszIn));
			}
			if (L'0' <= pCur[-1] && pCur[-1] <= L'9')
			{
				// OK
			}
			else if (L'A' <= pCur[-1] && pCur[-1] <= L'F')
			{
				// OK
			}
			else if (L'a' <= pCur[-1] && pCur[-1] <= L'f')
			{
				// OK
			}
			else
			{
				throw Exception(ResourceString(IDS_PCTC_ERROR), static_cast<UINT>(pCur - 1 - pszIn));
			}
			cbOut--;
			break;
		case ' ':
		case ':':
		case '/':
		case '?':
		case '#':
		case '[':
		case ']':
		case '@':
		case '!':
		case '$':
		case '&':
		case '\'':
		case '(':
		case ')':
		case '*':
		case ',':
		case ';':
		case '=':
			throw Exception(ResourceString(IDS_PCTC_ERROR), static_cast<UINT>(pCur - 1 - pszIn));
		default:
			if (c < 0x80)
			{
				cbOut--;
			}
			else
			{
				throw Exception(ResourceString(IDS_PCTC_ERROR), static_cast<UINT>(pCur - 1 - pszIn));
			}
			break;
		}
	}
	return static_cast<UINT>(pCur - pszIn);
}


void PercentCodecDialogBox::ClearStatus(int id)
{
	if ((id == 0 || id == IDC_PCTC_ORG_EDIT) && m_bEncodingError)
	{
		m_bEncodingError = false;
		SetText(IDC_PCTC_ORG_STATIC, ResourceString(IDS_ORIGINAL));
	}
	if ((id == 0 || id == IDC_PCTC_ENC_EDIT) && m_bDecodingError)
	{
		m_bDecodingError = false;
		SetText(IDC_PCTC_ENC_STATIC, ResourceString(IDS_ENCODED));
	}
}


void PercentCodecDialogBox::UpdateControlsState(int id)
{
	switch (id)
	{
	case IDC_PCTC_ORG_EDIT:
		if (GetTextLength(IDC_PCTC_ORG_EDIT) > 0)
		{
			EnableWindow(IDC_PCTC_ORG_COPY_BUTTON);
			EnableWindow(IDC_PCTC_ENCODE_BUTTON);
		}
		else
		{
			DisableWindow(IDC_PCTC_ORG_COPY_BUTTON);
			DisableWindow(IDC_PCTC_ENCODE_BUTTON);
		}
		ClearStatus(id);
		break;
	case IDC_PCTC_ENC_EDIT:
		if (GetTextLength(IDC_PCTC_ENC_EDIT) > 0)
		{
			EnableWindow(IDC_PCTC_ENC_COPY_BUTTON);
			EnableWindow(IDC_PCTC_DECODE_BUTTON);
		}
		else
		{
			DisableWindow(IDC_PCTC_ENC_COPY_BUTTON);
			DisableWindow(IDC_PCTC_DECODE_BUTTON);
		}
		ClearStatus(id);
		break;
	default:
		break;
	}
}
