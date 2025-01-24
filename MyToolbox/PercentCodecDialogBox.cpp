#include "pch.h"
#include "PercentCodecDialogBox.h"
#include "MyToolbox.h"
#include "resource.h"
#include "hnrt/RegistryKey.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/ResourceString.h"
#include "hnrt/WindowLayoutSnapshot.h"
#include "hnrt/WhileInScope.h"
#include "hnrt/Buffer.h"
#include "hnrt/StringCommons.h"
#include "hnrt/NumberText.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/Debug.h"


constexpr auto REGVAL_CODEPAGE = L"CodePage";
constexpr auto REGVAL_USEPLUS = L"UsePlus";
constexpr auto REGVAL_INPUT_CODEPAGE = L"InputCodePage";
constexpr auto REGVAL_OUTPUT_CODEPAGE = L"OutputCodePage";
constexpr auto REGVAL_OUTPUT_BOM = L"OutputBOM";
constexpr auto REGVAL_ORIGINAL_PATH = L"OriginalPath";
constexpr auto REGVAL_ENCODED_PATH = L"EncodedPath";


using namespace hnrt;


PercentCodecDialogBox::PercentCodecDialogBox()
	: MyDialogBox(IDD_PCTC, L"PercentCodec")
	, m_CodePage(CP_UTF8)
	, m_bUsePlus(TRUE)
	, m_szOriginalPath()
	, m_szEncodedPath()
{
}


void PercentCodecDialogBox::OnCreate()
{
	MyDialogBox::OnCreate();
	HFONT hFont = GetApp<MyToolbox>().GetFontForData();
	SetFont(IDC_PCTC_ORG_EDIT, hFont);
	SetFont(IDC_PCTC_ENC_EDIT, hFont);
	RegistryKey hKey;
	LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, m_szRegistryKeyName);
	if (rc == ERROR_SUCCESS)
	{
		m_CodePage = RegistryValue::GetDWORD(hKey, REGVAL_CODEPAGE, m_CodePage);
		m_bUsePlus = RegistryValue::GetDWORD(hKey, REGVAL_USEPLUS, m_bUsePlus ? 1 : 0) ? TRUE : FALSE;
		m_uInputCodePage = RegistryValue::GetDWORD(hKey, REGVAL_INPUT_CODEPAGE, CP_AUTODETECT);
		m_uOutputCodePage = RegistryValue::GetDWORD(hKey, REGVAL_OUTPUT_CODEPAGE, CP_UTF8);
		m_bOutputBOM = RegistryValue::GetDWORD(hKey, REGVAL_OUTPUT_BOM, 0) ? true : false;
		m_szOriginalPath = RegistryValue::GetSZ(hKey, REGVAL_ORIGINAL_PATH);
		m_szEncodedPath = RegistryValue::GetSZ(hKey, REGVAL_ENCODED_PATH);
	}
	InitializeCodePageComboBox(IDC_PCTC_CODEPAGE_COMBO, m_CodePage);
	ComboBoxRemove(IDC_PCTC_CODEPAGE_COMBO, CP_UTF16);
	ButtonCheck(IDC_PCTC_USEPLUS_CHECK, m_bUsePlus);
	SetStatus();
	m_menuView
		.Add(ResourceString(IDS_MENU_PCTC), IDM_VIEW_PCTC);
}


void PercentCodecDialogBox::OnDestroy()
{
	RegistryKey hKey;
	LSTATUS rc = hKey.Create(HKEY_CURRENT_USER, m_szRegistryKeyName);
	if (rc == ERROR_SUCCESS)
	{
		RegistryValue::SetDWORD(hKey, REGVAL_CODEPAGE, m_CodePage);
		RegistryValue::SetDWORD(hKey, REGVAL_USEPLUS, m_bUsePlus);
		RegistryValue::SetDWORD(hKey, REGVAL_INPUT_CODEPAGE, m_uInputCodePage);
		RegistryValue::SetDWORD(hKey, REGVAL_OUTPUT_CODEPAGE, m_uOutputCodePage);
		RegistryValue::SetDWORD(hKey, REGVAL_OUTPUT_BOM, m_bOutputBOM ? 1 : 0);
		RegistryValue::SetSZ(hKey, REGVAL_ORIGINAL_PATH, m_szOriginalPath);
		RegistryValue::SetSZ(hKey, REGVAL_ENCODED_PATH, m_szEncodedPath);
	}
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

	after[IDC_PCTC_STATUS_STATIC].right += cxDelta;

	ExtendVertically(after[IDC_PCTC_ORG_EDIT], after[IDC_PCTC_ENC_EDIT], cyDelta);
	LONG dy = after[IDC_PCTC_ORG_EDIT].bottom - before[IDC_PCTC_ORG_EDIT].bottom;
	MoveVertically(after[IDC_PCTC_ENC_STATIC], dy);
	MoveVertically(after[IDC_PCTC_ENC_COPY_BUTTON], dy);
	MoveVertically(after[IDC_PCTC_DECODE_BUTTON], dy);

	MoveVertically(after[IDC_PCTC_STATUS_STATIC], cyDelta);

	after.Apply();
}


void PercentCodecDialogBox::OnTabSelectionChanging()
{
	MyDialogBox::OnTabSelectionChanging();
	m_menuView
		.Enable(IDM_VIEW_PCTC, MF_ENABLED);
}


void PercentCodecDialogBox::OnTabSelectionChanged()
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
		.Insert(1, ResourceString(IDS_MENU_COPYORIGINAL), IDM_EDIT_COPYRESULT1)
		.InsertSeparator(2)
		.Insert(3, ResourceString(IDS_MENU_DECODE), IDM_EDIT_EXECUTE2)
		.Insert(4, ResourceString(IDS_MENU_COPYENCODED), IDM_EDIT_COPYRESULT2)
		.InsertSeparator(5);
	m_menuView
		.Enable(IDM_VIEW_PCTC, MF_DISABLED);
	AddInputCodePageSettingMenus();
	AddOutputCodePageSettingMenus();
	UpdateControlsState(1);
}


INT_PTR PercentCodecDialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
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
		if (Encode())
		{
			SetFocus(IDC_PCTC_ENC_COPY_BUTTON);
		}
		break;
	case IDM_EDIT_COPYRESULT1:
		CopyAllText(IDC_PCTC_ORG_EDIT);
		break;
	case IDM_EDIT_EXECUTE2:
		if (Decode())
		{
			SetFocus(IDC_PCTC_ORG_COPY_BUTTON);
		}
		break;
	case IDM_EDIT_COPYRESULT2:
		CopyAllText(IDC_PCTC_ENC_EDIT);
		break;
	case IDC_PCTC_ENCODE_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			if (Encode())
			{
				SetFocus(IDC_PCTC_ENC_COPY_BUTTON);
			}
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_PCTC_ORG_COPY_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			CopyAllText(IDC_PCTC_ORG_EDIT);
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_PCTC_DECODE_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			if (Decode())
			{
				SetFocus(IDC_PCTC_ORG_COPY_BUTTON);
			}
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_PCTC_ENC_COPY_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			CopyAllText(IDC_PCTC_ENC_EDIT);
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_PCTC_ORG_EDIT:
	case IDC_PCTC_ENC_EDIT:
		return OnEditCommand(wParam, lParam);
	case IDC_PCTC_CODEPAGE_COMBO:
		if (idNotif == CBN_SELCHANGE)
		{
			m_CodePage = ComboBoxGetSelection(IDC_PCTC_CODEPAGE_COMBO, m_CodePage);
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_PCTC_USEPLUS_CHECK:
		if (idNotif == BN_CLICKED)
		{
			m_bUsePlus = ButtonIsChecked(IDC_PCTC_USEPLUS_CHECK);
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


void PercentCodecDialogBox::OnEditChanged(int id)
{
	MyDialogBox::OnEditChanged(id);
	switch (id)
	{
	case IDC_PCTC_ORG_EDIT:
		SetStatus(L"", 0, MASK_PANE1 | MASK_STATUS);
		break;
	case IDC_PCTC_ENC_EDIT:
		SetStatus(L"", 0, MASK_PANE2 | MASK_STATUS);
		break;
	default:
		return;
	}
	UpdateControlsState(id);
}


void PercentCodecDialogBox::OnNew()
{
	SetStatus(L"", 0, MASK_PANE1 | MASK_PANE2 | MASK_STATUS);
	SetTextAndNotify(IDC_PCTC_ORG_EDIT);
	SetTextAndNotify(IDC_PCTC_ENC_EDIT);
}


void PercentCodecDialogBox::OnLoad1From()
{
	try
	{
		SetStatus(L"Loading Original...", FLAG_BUSY, MASK_STATUS);
		if (LoadTextFromFile(IDC_PCTC_ORG_EDIT, m_szOriginalPath))
		{
			SetStatus(String(PRINTF, L"Loading Original...Done:  %s chars in",
				NumberText(GetTextLength(IDC_PCTC_ORG_EDIT)).Ptr),
				FLAG_STATUS_SUCCESSFUL, MASK_PANE1);
		}
		else
		{
			SetStatus(L"");
		}
	}
	catch (Win32Exception e)
	{
		SetStatus(String(PRINTF, L"Loading Original...Failed: %s: %s", e.Message, ErrorMessage::Get(e.Error)),
			FLAG_STATUS_ERROR);
	}
	catch (Exception e)
	{
		SetStatus(String(PRINTF, L"Loading Original...Failed: %s", e.Message),
			FLAG_STATUS_ERROR);
	}
}


void PercentCodecDialogBox::OnSave1As()
{
	try
	{
		SetStatus(L"Saving Original...", FLAG_BUSY, MASK_STATUS);
		if (SaveTextAsFile(IDC_PCTC_ORG_EDIT, m_szOriginalPath))
		{
			SetStatus(String(PRINTF, L"Saving Original...Done:  %s chars out", NumberText(GetTextLength(IDC_PCTC_ORG_EDIT)).Ptr), FLAG_STATUS_SUCCESSFUL);
		}
		else
		{
			SetStatus(L"");
		}
	}
	catch (Win32Exception e)
	{
		SetStatus(String(PRINTF, L"Saving Original...Failed: %s: %s", e.Message, ErrorMessage::Get(e.Error)), FLAG_STATUS_ERROR);
	}
	catch (Exception e)
	{
		SetStatus(String(PRINTF, L"Saving Original...Failed: %s", e.Message), FLAG_STATUS_ERROR);
	}
}


void PercentCodecDialogBox::OnLoad2From()
{
	try
	{
		SetStatus(L"Loading Percent-Encoded...", FLAG_BUSY, MASK_STATUS);
		if (LoadTextFromFile(IDC_PCTC_ENC_EDIT, m_szEncodedPath))
		{
			SetStatus(String(PRINTF, L"Loading Percent-Encoded...Done:  %s chars in", NumberText(GetTextLength(IDC_PCTC_ENC_EDIT)).Ptr), FLAG_STATUS_SUCCESSFUL, MASK_PANE2);
		}
		else
		{
			SetStatus(L"");
		}
	}
	catch (Win32Exception e)
	{
		SetStatus(String(PRINTF, L"Loading Percent-Encoded...Failed: %s: %s", e.Message, ErrorMessage::Get(e.Error)), FLAG_STATUS_ERROR);
	}
	catch (Exception e)
	{
		SetStatus(String(PRINTF, L"Loading Percent-Encoded...Failed: %s", e.Message), FLAG_STATUS_ERROR);
	}
}


void PercentCodecDialogBox::OnSave2As()
{
	try
	{
		SetStatus(L"Saving Percent-Encoded...", FLAG_BUSY, MASK_STATUS);
		if (SaveTextAsFile(IDC_PCTC_ENC_EDIT, m_szEncodedPath))
		{
			SetStatus(String(PRINTF, L"Saving Percent-Encoded...Done:  %s chars", NumberText(GetTextLength(IDC_PCTC_ENC_EDIT)).Ptr), FLAG_STATUS_SUCCESSFUL);
		}
		else
		{
			SetStatus(L"");
		}
	}
	catch (Win32Exception e)
	{
		SetStatus(String(PRINTF, L"Saving Percent-Encoded...Failed: %s: %s", e.Message, ErrorMessage::Get(e.Error)), FLAG_STATUS_ERROR);
	}
	catch (Exception e)
	{
		SetStatus(String(PRINTF, L"Saving Percent-Encoded...Failed: %s", e.Message), FLAG_STATUS_ERROR);
	}
}


void PercentCodecDialogBox::OnSettingChanged(UINT uId)
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


bool PercentCodecDialogBox::Encode()
{
	DBGFNC(L"PercentCodecDialogBox::Encode");
	try
	{
		SetStatus(L"Encoding...", FLAG_BUSY, MASK_STATUS);
		String szText = GetText(IDC_PCTC_ORG_EDIT);
		Buffer<WCHAR> buf((szText.Len + 1ULL) * 4ULL * 3ULL);
		Encode(szText, static_cast<UINT>(szText.Len + 1ULL), m_CodePage, buf, static_cast<UINT>(buf.Len), m_bUsePlus);
		SetText(IDC_PCTC_ENC_EDIT, buf);
		SetStatus(String(PRINTF, L"Encoding...Done:  %s chars in  >>>  %s chars out",
			NumberText(GetTextLength(IDC_PCTC_ORG_EDIT)).Ptr,
			NumberText(GetTextLength(IDC_PCTC_ENC_EDIT)).Ptr),
			FLAG_STATUS_SUCCESSFUL | FLAG_PANE2_SUCCESSFUL, FLAG_PANE1_ERROR | FLAG_PANE2_ERROR);
		return true;
	}
	catch (Exception e)
	{
		SetStatus(String(PRINTF, L"Encoding...Failed: %s", e.Message),
			FLAG_STATUS_ERROR | FLAG_PANE1_ERROR, FLAG_PANE2_SUCCESSFUL);
		return false;
	}
}


bool PercentCodecDialogBox::Decode()
{
	DBGFNC(L"PercentCodecDialogBox::Decode");
	try
	{
		SetStatus(L"Decoding...", FLAG_BUSY, MASK_STATUS);
		String szText = GetText(IDC_PCTC_ENC_EDIT);
		Buffer<WCHAR> buf(szText.Len + 1ULL);
		Decode(szText, static_cast<UINT>(szText.Len + 1ULL), m_CodePage, buf, static_cast<UINT>(buf.Len));
		SetText(IDC_PCTC_ORG_EDIT, buf);
		SetStatus(String(PRINTF, L"Decoding...Done:  %s chars in  >>>  %s chars out",
			NumberText(GetTextLength(IDC_PCTC_ENC_EDIT)).Ptr,
			NumberText(GetTextLength(IDC_PCTC_ORG_EDIT)).Ptr),
			FLAG_STATUS_SUCCESSFUL | FLAG_PANE1_SUCCESSFUL, FLAG_PANE1_ERROR | FLAG_PANE2_ERROR);
		return true;
	}
	catch (Exception e)
	{
		SetStatus(String(PRINTF, L"Decoding...Failed: %s", e.Message),
			FLAG_STATUS_ERROR | FLAG_PANE2_ERROR, FLAG_PANE1_SUCCESSFUL);
		return false;
	}
}


constexpr auto ENCODE_BUFFER_TOO_SHORT = L"PercentCodec::Encode: Buffer is too short.";


void PercentCodecDialogBox::Encode(PCWCH pszIn, UINT cchIn, UINT uCodePage, PWCH pOut, UINT cchOut, BOOL bUsePlus)
{
	PWCH pOutBound = pOut + cchOut;
	PCWCH pCur = pszIn;
	PCWCH pEnd = pszIn + cchIn;
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


void PercentCodecDialogBox::Encode(WCHAR c, UINT uCodePage, PWCH& pOut, PWCH pOutBound, UINT offset)
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


constexpr auto DECODE_BUFFER_TOO_SHORT = L"PercentCodec::Decode: Buffer is too short.";


void PercentCodecDialogBox::Decode(PCWCH pszIn, UINT cchIn, UINT uCodePage, PWCH pOut, UINT cchOut)
{
	Buffer<CHAR> mbBuf(cchIn);
	PBYTE pDst = reinterpret_cast<PBYTE>(mbBuf.Ptr);
	PBYTE pDstBound = pDst + mbBuf.Len;
	PCWCH pCur = pszIn;
	PCWCH pEnd = pCur + cchIn;
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
	DWORD dwFlags = ((uCodePage == CP_UTF8 || uCodePage == CP_GB18030) ? 0 : MB_PRECOMPOSED) | MB_ERR_INVALID_CHARS;
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


UINT PercentCodecDialogBox::GetDecodedOffset(PCWCH pszIn, UINT cbOut)
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


void PercentCodecDialogBox::SetStatus(PCWSTR psz, DWORD dwSet, DWORD dwReset)
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
		InvalidateRect(IDC_PCTC_ORG_EDIT, NULL, TRUE);
		InvalidateRect(IDC_PCTC_ENC_EDIT, NULL, TRUE);
	}
	SetText(IDC_PCTC_STATUS_STATIC, psz ? psz : L"");
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


void PercentCodecDialogBox::UpdateControlsState(int id)
{
	switch (id)
	{
	case 0:
		EditSetReadOnly(IDC_PCTC_ORG_EDIT, TRUE);
		EditSetReadOnly(IDC_PCTC_ENC_EDIT, TRUE);
		DisableWindow(IDC_PCTC_ENCODE_BUTTON);
		DisableWindow(IDC_PCTC_ORG_COPY_BUTTON);
		DisableWindow(IDC_PCTC_DECODE_BUTTON);
		DisableWindow(IDC_PCTC_ENC_COPY_BUTTON);
		m_menuEdit
			.Enable(IDM_EDIT_EXECUTE1, MF_DISABLED)
			.Enable(IDM_EDIT_COPYRESULT1, MF_DISABLED)
			.Enable(IDM_EDIT_EXECUTE2, MF_DISABLED)
			.Enable(IDM_EDIT_COPYRESULT2, MF_DISABLED);
		break;
	case 1:
		EditSetReadOnly(IDC_PCTC_ORG_EDIT, FALSE);
		EditSetReadOnly(IDC_PCTC_ENC_EDIT, FALSE);
		UpdateControlsState(IDC_PCTC_ORG_EDIT);
		UpdateControlsState(IDC_PCTC_ENC_EDIT);
		break;
	case IDC_PCTC_ORG_EDIT:
	{
		BOOL bEnabled = GetTextLength(IDC_PCTC_ORG_EDIT) > 0 ? TRUE : FALSE;
		EnableWindow(IDC_PCTC_ENCODE_BUTTON, bEnabled);
		EnableWindow(IDC_PCTC_ORG_COPY_BUTTON, bEnabled);
		m_menuEdit
			.Enable(IDM_EDIT_EXECUTE1, bEnabled ? MF_ENABLED : MF_DISABLED)
			.Enable(IDM_EDIT_COPYRESULT1, bEnabled ? MF_ENABLED : MF_DISABLED);
		break;
	}
	case IDC_PCTC_ENC_EDIT:
	{
		BOOL bEnabled = GetTextLength(IDC_PCTC_ENC_EDIT) > 0 ? TRUE : FALSE;
		EnableWindow(IDC_PCTC_DECODE_BUTTON, bEnabled);
		EnableWindow(IDC_PCTC_ENC_COPY_BUTTON, bEnabled);
		m_menuEdit
			.Enable(IDM_EDIT_EXECUTE2, bEnabled ? MF_ENABLED : MF_DISABLED)
			.Enable(IDM_EDIT_COPYRESULT2, bEnabled ? MF_ENABLED : MF_DISABLED);
		break;
	}
	default:
		break;
	}
}
