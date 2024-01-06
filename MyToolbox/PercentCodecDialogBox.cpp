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
#include "hnrt/Exception.h"
#include "hnrt/Debug.h"


#define REGVAL_INPUT_CODEPAGE L"InputCodePage"
#define REGVAL_OUTPUT_CODEPAGE L"OutputCodePage"
#define REGVAL_OUTPUT_BOM L"OutputBOM"
#define REGVAL_ORIGINAL_PATH L"OriginalPath"
#define REGVAL_ENCODED_PATH L"EncodedPath"


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
	SetFont(IDC_PCTC_CONTENT1_EDIT, hFont);
	SetFont(IDC_PCTC_CONTENT2_EDIT, hFont);
	InitializeCodePageComboBox(IDC_PCTC_CODEPAGE_COMBO);
	ComboBoxRemove(IDC_PCTC_CODEPAGE_COMBO, CP_UTF16);
	ButtonCheck(IDC_PCTC_USEPLUS_CHECK);
	OnSelectSource(IDC_PCTC_CONTENT1_RADIO);
	SetText(IDC_PCTC_STATUS1_STATIC);
	SetText(IDC_PCTC_STATUS2_STATIC);
	m_menuView
		.Add(ResourceString(IDS_MENU_PCTC), IDM_VIEW_PCTC);
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
	SetFont(IDC_PCTC_CONTENT1_EDIT, NULL);
	SetFont(IDC_PCTC_CONTENT2_EDIT, NULL);
	MyDialogBox::OnDestroy();
}


void PercentCodecDialogBox::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
	WindowLayoutSnapshot before, after;

	before.AddAllChildren(hDlg).Clone(after);

	after[IDC_PCTC_CONTENT1_EDIT].right += cxDelta;
	MoveHorizontally(after[IDC_PCTC_COPY1_BUTTON], cxDelta);
	MoveHorizontally(after[IDC_PCTC_USEPLUS_CHECK], cxDelta);

	CenterHorizontally(after[IDC_PCTC_CONTENT1_EDIT], after[IDC_PCTC_ENCODE_BUTTON], after[IDC_PCTC_CODEPAGE_COMBO], after[IDC_PCTC_DECODE_BUTTON]);
	BindRightToLeft(after[IDC_PCTC_STATUS1_STATIC], before[IDC_PCTC_ENCODE_BUTTON], after[IDC_PCTC_ENCODE_BUTTON]);
	BindLeftToRight(after[IDC_PCTC_STATUS2_STATIC], before[IDC_PCTC_DECODE_BUTTON], after[IDC_PCTC_DECODE_BUTTON]);
	after[IDC_PCTC_STATUS2_STATIC].right = after[IDC_PCTC_CONTENT1_EDIT].right;

	after[IDC_PCTC_CONTENT2_EDIT].right += cxDelta;
	MoveHorizontally(after[IDC_PCTC_COPY2_BUTTON], cxDelta);
	after[IDC_PCTC_DESC_STATIC].right += cxDelta;

	ExtendVertically(after[IDC_PCTC_CONTENT1_EDIT], after[IDC_PCTC_CONTENT2_EDIT], cyDelta);

	RepositionBottom(after[IDC_PCTC_USEPLUS_CHECK], after[IDC_PCTC_CONTENT1_EDIT].bottom);

	LONG dy = after[IDC_PCTC_CONTENT1_EDIT].bottom - before[IDC_PCTC_CONTENT1_EDIT].bottom;
	MoveVertically(after[IDC_PCTC_STATUS1_STATIC], dy);
	MoveVertically(after[IDC_PCTC_ENCODE_BUTTON], dy);
	MoveVertically(after[IDC_PCTC_CODEPAGE_COMBO], dy);
	MoveVertically(after[IDC_PCTC_DECODE_BUTTON], dy);
	MoveVertically(after[IDC_PCTC_STATUS2_STATIC], dy);

	RepositionTop(after[IDC_PCTC_CONTENT2_RADIO], after[IDC_PCTC_CONTENT2_EDIT].top);
	RepositionTop(after[IDC_PCTC_COPY2_BUTTON], after[IDC_PCTC_CONTENT2_EDIT].top);

	MoveVertically(after[IDC_PCTC_DESC_STATIC], cyDelta);

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
		.RemoveAll()
		.Add(ResourceString(IDS_MENU_LOADFROM), IDM_FILE_LOADFROM)
		.Add(ResourceString(IDS_MENU_SAVEAS), IDM_FILE_SAVEAS)
		.AddSeparator()
		.Add(ResourceString(IDS_MENU_EXIT), IDM_FILE_EXIT);
	m_menuEdit
		.RemoveAll()
		.Add(ResourceString(CurrentEdit == IDC_PCTC_CONTENT1_EDIT ? IDS_MENU_ENCODE : IDS_MENU_DECODE), IDM_EDIT_EXECUTE)
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
		.Enable(IDM_VIEW_PCTC, MF_DISABLED);
	m_menuSettings
		.RemoveAll();
	AddInputCodePageSettingMenus();
	AddOutputCodePageSettingMenus();
}


INT_PTR PercentCodecDialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (m_cProcessing)
	{
		return TRUE;
	}
	UNREFERENCED_PARAMETER(lParam);
	UINT idChild = LOWORD(wParam);
	UINT idNotif = HIWORD(wParam);
	switch (idChild)
	{
	case IDC_PCTC_CONTENT1_RADIO:
	case IDC_PCTC_CONTENT2_RADIO:
		if (idNotif == BN_CLICKED)
		{
			OnSelectSource(idChild);
		}
		break;
	case IDC_PCTC_COPY1_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			CopyAllText(IDC_PCTC_CONTENT1_EDIT);
		}
		break;
	case IDC_PCTC_COPY2_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			CopyAllText(IDC_PCTC_CONTENT2_EDIT);
		}
		break;
	case IDC_PCTC_ENCODE_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			if (OnEncode())
			{
				SetFocus(IDC_PCTC_COPY2_BUTTON);
			}
		}
		break;
	case IDC_PCTC_DECODE_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			if (OnDecode())
			{
				SetFocus(IDC_PCTC_COPY1_BUTTON);
			}
		}
		break;
	case IDC_PCTC_CONTENT1_EDIT:
	case IDC_PCTC_CONTENT2_EDIT:
		if (idNotif == EN_CHANGE)
		{
			ClearStatus();
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}


INT_PTR PercentCodecDialogBox::OnControlColorStatic(WPARAM wParam, LPARAM lParam)
{
	HDC hdc = reinterpret_cast<HDC>(wParam);
	int id = GetDlgCtrlID(reinterpret_cast<HWND>(lParam));
	switch (id)
	{
	case IDC_PCTC_STATUS1_STATIC:
	case IDC_PCTC_STATUS2_STATIC:
		SetTextColor(hdc, RGB(240, 0, 0));
		SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
		return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_3DFACE));
	default:
		break;
	}
	return 0;
}


void PercentCodecDialogBox::OnLoadFrom()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	LoadTextFromFile(CurrentEdit, CurrentPath);
}


void PercentCodecDialogBox::OnSaveAs()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	SaveTextAsFile(CurrentEdit, CurrentPath);
}


void PercentCodecDialogBox::OnCut()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	EditCut(CurrentEdit);
	ClearStatus();
}


void PercentCodecDialogBox::OnCopy()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	EditCopy(CurrentEdit);
}


void PercentCodecDialogBox::OnPaste()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	EditPaste(CurrentEdit);
	ClearStatus();
}


void PercentCodecDialogBox::OnDelete()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	EditDelete(CurrentEdit);
	ClearStatus();
}


void PercentCodecDialogBox::OnSelectAll()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	EditSelectAll(CurrentEdit);
}


void PercentCodecDialogBox::OnCopyAll()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	CopyAllText(CurrentEdit);
}


void PercentCodecDialogBox::OnClear()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	EditClear(CurrentEdit);
	ClearStatus();
	CurrentPath = String::Empty;
}


void PercentCodecDialogBox::OnExecute()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	switch (CurrentEdit)
	{
	case IDC_PCTC_CONTENT1_EDIT:
		if (OnEncode())
		{
			SetFocus(IDC_PCTC_COPY2_BUTTON);
		}
		break;
	case IDC_PCTC_CONTENT2_EDIT:
		if (OnDecode())
		{
			SetFocus(IDC_PCTC_COPY1_BUTTON);
		}
		break;
	default:
		break;
	}
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


void PercentCodecDialogBox::OnSelectSource(int id)
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	ButtonCheck(IDC_PCTC_CONTENT1_RADIO, id == IDC_PCTC_CONTENT1_RADIO);
	EditSetReadOnly(IDC_PCTC_CONTENT1_EDIT, id == IDC_PCTC_CONTENT1_RADIO ? FALSE : TRUE);
	EnableWindow(IDC_PCTC_USEPLUS_CHECK, id == IDC_PCTC_CONTENT1_RADIO);
	EnableWindow(IDC_PCTC_ENCODE_BUTTON, id == IDC_PCTC_CONTENT1_RADIO);
	ButtonCheck(IDC_PCTC_CONTENT2_RADIO, id == IDC_PCTC_CONTENT2_RADIO);
	EditSetReadOnly(IDC_PCTC_CONTENT2_EDIT, id == IDC_PCTC_CONTENT2_RADIO ? FALSE : TRUE);
	EnableWindow(IDC_PCTC_DECODE_BUTTON, id == IDC_PCTC_CONTENT2_RADIO);
	ClearStatus();
	m_menuEdit
		.Modify(
			IDM_EDIT_EXECUTE, 0,
			IDM_EDIT_EXECUTE, ResourceString(id == IDC_PCTC_CONTENT1_RADIO ? IDS_MENU_ENCODE : IDS_MENU_DECODE));
}


bool PercentCodecDialogBox::OnEncode()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	try
	{
		ClearStatus();
		String szText = GetText(IDC_PCTC_CONTENT1_EDIT);
		UINT uCodePage = GetCodePage();
		Buffer<WCHAR> buf((szText.Len + 1ULL) * 4ULL * 3ULL);
		Encode(szText, static_cast<UINT>(szText.Len + 1ULL), uCodePage, buf, static_cast<UINT>(buf.Len), ButtonIsChecked(IDC_PCTC_USEPLUS_CHECK));
		SetText(IDC_PCTC_CONTENT2_EDIT, buf);
		m_bDecodingError = false;
		return true;
	}
	catch (Exception e)
	{
		SetText(IDC_PCTC_STATUS1_STATIC, e.Message);
		m_bEncodingError = true;
		return false;
	}
}


bool PercentCodecDialogBox::OnDecode()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	try
	{
		ClearStatus();
		String szText = GetText(IDC_PCTC_CONTENT2_EDIT);
		UINT uCodePage = GetCodePage();
		Buffer<WCHAR> buf(szText.Len + 1ULL);
		Decode(szText, static_cast<UINT>(szText.Len + 1ULL), uCodePage, buf, static_cast<UINT>(buf.Len));
		SetText(IDC_PCTC_CONTENT1_EDIT, buf);
		m_bDecodingError = false;
		return true;
	}
	catch (Exception e)
	{
		SetText(IDC_PCTC_STATUS2_STATIC, e.Message);
		m_bDecodingError = true;
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
	DWORD dwFlags = (uCodePage == CP_UTF8 || uCodePage == 54936) ? WC_ERR_INVALID_CHARS : 0;
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


void PercentCodecDialogBox::ClearStatus()
{
	if (m_bEncodingError)
	{
		m_bEncodingError = false;
		SetText(IDC_PCTC_STATUS1_STATIC);
	}
	if (m_bDecodingError)
	{
		m_bDecodingError = false;
		SetText(IDC_PCTC_STATUS2_STATIC);
	}
}


int PercentCodecDialogBox::get_CurrentEdit() const
{
	if (ButtonIsChecked(IDC_PCTC_CONTENT1_RADIO))
	{
		return IDC_PCTC_CONTENT1_EDIT;
	}
	else if (ButtonIsChecked(IDC_PCTC_CONTENT2_RADIO))
	{
		return IDC_PCTC_CONTENT2_EDIT;
	}
	else
	{
		throw Exception(L"PercentCodecDialogBox::get_CurrentEdit: Unexpected state.");
	}
}


String& PercentCodecDialogBox::get_CurrentPath() const
{
	switch (CurrentEdit)
	{
	case IDC_PCTC_CONTENT1_EDIT:
		return m_szOriginalPath;
	case IDC_PCTC_CONTENT2_EDIT:
		return m_szEncodedPath;
	default:
		throw Exception(L"PercentCodecDialogBox::get_CurrentPath: Unexpected state.");
	}
}


void PercentCodecDialogBox::set_CurrentPath(const String& value)
{
	switch (CurrentEdit)
	{
	case IDC_PCTC_CONTENT1_EDIT:
		m_szOriginalPath = value;
		break;
	case IDC_PCTC_CONTENT2_EDIT:
		m_szEncodedPath = value;
		break;
	default:
		throw Exception(L"PercentCodecDialogBox::set_CurrentPath: Unexpected state.");
	}
}
