#include "pch.h"
#include "PercentCodecDialogBox.h"
#include "resource.h"
#include "hnrt/RegistryKey.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/Menu.h"
#include "hnrt/WindowDesign.h"
#include "hnrt/WindowHandle.h"
#include "hnrt/WindowLayoutSnapshot.h"
#include "hnrt/Clipboard.h"
#include "hnrt/ResourceString.h"
#include "hnrt/Buffer.h"
#include "hnrt/Exception.h"
#include "hnrt/Debug.h"


using namespace hnrt;


#define REG_NAME_INPUT_CODEPAGE L"InputCodePage"
#define REG_NAME_OUTPUT_CODEPAGE L"OutputCodePage"
#define REG_NAME_OUTPUT_BOM L"OutputBOM"
#define REG_NAME_ORIGINAL_PATH L"OriginalPath"
#define REG_NAME_ENCODED_PATH L"EncodedPath"


PercentCodecDialogBox::PercentCodecDialogBox()
	: MyDialogBox(IDD_PCTC)
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
	LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, REG_SUBKEY_(PercentCodec));
	if (rc == ERROR_SUCCESS)
	{
		RegistryValue value;
		m_uInputCodePage = value.GetDWORD(hKey, REG_NAME_INPUT_CODEPAGE, CP_AUTODETECT);
		m_uOutputCodePage = value.GetDWORD(hKey, REG_NAME_OUTPUT_CODEPAGE, CP_UTF8);
		m_bOutputBOM = value.GetDWORD(hKey, REG_NAME_OUTPUT_BOM, 0) ? true : false;
		m_szOriginalPath = value.GetSZ(hKey, REG_NAME_ORIGINAL_PATH, L"");
		m_szEncodedPath = value.GetSZ(hKey, REG_NAME_ENCODED_PATH, L"");
	}
	InitializeCodePageComboBox(IDC_PCTC_ENCODING);
	ComboBoxRemove(IDC_PCTC_ENCODING, CP_UTF16);
	ButtonCheck(IDC_PCTC_USE_PLUS);
	OnSelectSource(IDC_PCTC_LABEL1);
	SetText(IDC_PCTC_STATUS1, L"");
	SetText(IDC_PCTC_STATUS2, L"");
	m_menuView
		.Add(ResourceString(IDS_MENU_PCTC), IDM_VIEW_PCTC);
}


void PercentCodecDialogBox::OnDestroy()
{
	RegistryKey hKey;
	LSTATUS rc = hKey.Create(HKEY_CURRENT_USER, REG_SUBKEY_(PercentCodec));
	if (rc == ERROR_SUCCESS)
	{
		RegistryValue::SetDWORD(hKey, REG_NAME_INPUT_CODEPAGE, m_uInputCodePage);
		RegistryValue::SetDWORD(hKey, REG_NAME_OUTPUT_CODEPAGE, m_uOutputCodePage);
		RegistryValue::SetDWORD(hKey, REG_NAME_OUTPUT_BOM, m_bOutputBOM ? 1 : 0);
		RegistryValue::SetSZ(hKey, REG_NAME_ORIGINAL_PATH, m_szOriginalPath);
		RegistryValue::SetSZ(hKey, REG_NAME_ENCODED_PATH, m_szEncodedPath);
	}
	MyDialogBox::OnDestroy();
}


void PercentCodecDialogBox::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
	WindowLayoutSnapshot before, after;

	before.AddAllChildren(hDlg).Clone(after);

	after[IDC_PCTC_EDIT1].right += cxDelta;
	MoveHorizontally(after[IDC_PCTC_COPY1], cxDelta);
	MoveHorizontally(after[IDC_PCTC_USE_PLUS], cxDelta);

	CenterHorizontally(after[IDC_PCTC_EDIT1], after[IDC_PCTC_ENCODE], after[IDC_PCTC_ENCODING], after[IDC_PCTC_DECODE]);
	BindRightToLeft(after[IDC_PCTC_STATUS1], before[IDC_PCTC_ENCODE], after[IDC_PCTC_ENCODE]);
	BindLeftToRight(after[IDC_PCTC_STATUS2], before[IDC_PCTC_DECODE], after[IDC_PCTC_DECODE]);
	after[IDC_PCTC_STATUS2].right = after[IDC_PCTC_EDIT1].right;

	after[IDC_PCTC_EDIT2].right += cxDelta;
	MoveHorizontally(after[IDC_PCTC_COPY2], cxDelta);
	after[IDC_PCTC_LABEL3].right += cxDelta;

	ExtendVertically(after[IDC_PCTC_EDIT1], after[IDC_PCTC_EDIT2], cyDelta);

	RepositionBottom(after[IDC_PCTC_USE_PLUS], after[IDC_PCTC_EDIT1].bottom);

	LONG dy = after[IDC_PCTC_EDIT1].bottom - before[IDC_PCTC_EDIT1].bottom;
	MoveVertically(after[IDC_PCTC_STATUS1], dy);
	MoveVertically(after[IDC_PCTC_ENCODE], dy);
	MoveVertically(after[IDC_PCTC_ENCODING], dy);
	MoveVertically(after[IDC_PCTC_DECODE], dy);
	MoveVertically(after[IDC_PCTC_STATUS2], dy);

	RepositionTop(after[IDC_PCTC_LABEL2], after[IDC_PCTC_EDIT2].top);
	RepositionTop(after[IDC_PCTC_COPY2], after[IDC_PCTC_EDIT2].top);

	MoveVertically(after[IDC_PCTC_LABEL3], cyDelta);

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
		.Add(ResourceString(CurrentEdit == IDC_PCTC_EDIT1 ? IDS_MENU_ENCODE : IDS_MENU_DECODE), IDM_EDIT_EXECUTE)
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
	UNREFERENCED_PARAMETER(lParam);
	UINT idChild = LOWORD(wParam);
	switch (idChild)
	{
	case IDC_PCTC_LABEL1:
	case IDC_PCTC_LABEL2:
		OnSelectSource(idChild);
		break;
	case IDC_PCTC_COPY1:
		CopyAllText(IDC_PCTC_EDIT1);
		break;
	case IDC_PCTC_COPY2:
		CopyAllText(IDC_PCTC_EDIT2);
		break;
	case IDC_PCTC_ENCODE:
		if (OnEncode())
		{
			SetFocus(IDC_PCTC_COPY2);
		}
		break;
	case IDC_PCTC_DECODE:
		if (OnDecode())
		{
			SetFocus(IDC_PCTC_COPY1);
		}
		break;
	case IDC_PCTC_EDIT1:
	case IDC_PCTC_EDIT2:
		if (HIWORD(wParam) == EN_CHANGE)
		{
			if (m_bEncodingError)
			{
				m_bEncodingError = false;
				SetText(IDC_PCTC_STATUS1, L"");
			}
			if (m_bDecodingError)
			{
				m_bDecodingError = false;
				SetText(IDC_PCTC_STATUS2, L"");
			}
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
	case IDC_PCTC_STATUS1:
	case IDC_PCTC_STATUS2:
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
	LoadTextFromFile(CurrentEdit, CurrentPath);
}


void PercentCodecDialogBox::OnSaveAs()
{
	SaveTextAsFile(CurrentEdit, CurrentPath);
}


void PercentCodecDialogBox::OnCut()
{
	EditCut(CurrentEdit);
}


void PercentCodecDialogBox::OnCopy()
{
	EditCopy(CurrentEdit);
}


void PercentCodecDialogBox::OnPaste()
{
	EditPaste(CurrentEdit);
}


void PercentCodecDialogBox::OnDelete()
{
	EditDelete(CurrentEdit);
}


void PercentCodecDialogBox::OnSelectAll()
{
	EditSelectAll(CurrentEdit);
}


void PercentCodecDialogBox::OnCopyAll()
{
	CopyAllText(CurrentEdit);
}


void PercentCodecDialogBox::OnClear()
{
	EditClear(CurrentEdit);
	CurrentPath = String::Empty;
}


void PercentCodecDialogBox::OnExecute()
{
	switch (CurrentEdit)
	{
	case IDC_PCTC_EDIT1:
		if (OnEncode())
		{
			SetFocus(IDC_PCTC_COPY2);
		}
		break;
	case IDC_PCTC_EDIT2:
		if (OnDecode())
		{
			SetFocus(IDC_PCTC_COPY1);
		}
		break;
	default:
		break;
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


void PercentCodecDialogBox::OnSelectSource(int id)
{
	ButtonCheck(IDC_PCTC_LABEL1, id == IDC_PCTC_LABEL1);
	EditSetReadOnly(IDC_PCTC_EDIT1, id == IDC_PCTC_LABEL1 ? FALSE : TRUE);
	EnableWindow(IDC_PCTC_USE_PLUS, id == IDC_PCTC_LABEL1);
	EnableWindow(IDC_PCTC_ENCODE, id == IDC_PCTC_LABEL1);
	ButtonCheck(IDC_PCTC_LABEL2, id == IDC_PCTC_LABEL2);
	EditSetReadOnly(IDC_PCTC_EDIT2, id == IDC_PCTC_LABEL2 ? FALSE : TRUE);
	EnableWindow(IDC_PCTC_DECODE, id == IDC_PCTC_LABEL2);
	if (m_bEncodingError)
	{
		m_bEncodingError = false;
		SetText(IDC_PCTC_STATUS1, L"");
	}
	if (m_bDecodingError)
	{
		m_bDecodingError = false;
		SetText(IDC_PCTC_STATUS2, L"");
	}
	m_menuEdit
		.Modify(
			IDM_EDIT_EXECUTE, 0,
			IDM_EDIT_EXECUTE, ResourceString(id == IDC_PCTC_LABEL1 ? IDS_MENU_ENCODE : IDS_MENU_DECODE));
}


bool PercentCodecDialogBox::OnEncode()
{
	try
	{
		if (m_bEncodingError)
		{
			m_bEncodingError = false;
			SetText(IDC_PCTC_STATUS1, L"");
		}
		if (m_bDecodingError)
		{
			m_bDecodingError = false;
			SetText(IDC_PCTC_STATUS2, L"");
		}
		UINT cch = GetTextLength(IDC_PCTC_EDIT1) + 1;
		Buffer<WCHAR> buf1(cch);
		GetText(IDC_PCTC_EDIT1, buf1, cch);
		UINT uCodePage = GetCodePage();
		UINT size = cch * 4 * 3;
		Buffer<WCHAR> buf2(size);
		Encode(buf1, cch, uCodePage, buf2, static_cast<UINT>(buf2.Len), ButtonIsChecked(IDC_PCTC_USE_PLUS));
		SetText(IDC_PCTC_EDIT2, buf2);
		m_bDecodingError = false;
		return true;
	}
	catch (Exception e)
	{
		SetText(IDC_PCTC_STATUS1, e.Message);
		m_bEncodingError = true;
		return false;
	}
}


bool PercentCodecDialogBox::OnDecode()
{
	try
	{
		if (m_bEncodingError)
		{
			m_bEncodingError = false;
			SetText(IDC_PCTC_STATUS1, L"");
		}
		if (m_bDecodingError)
		{
			m_bDecodingError = false;
			SetText(IDC_PCTC_STATUS2, L"");
		}
		UINT cch = GetTextLength(IDC_PCTC_EDIT2) + 1;
		Buffer<WCHAR> buf2(cch);
		GetText(IDC_PCTC_EDIT2, buf2, cch);
		UINT uCodePage = GetCodePage();
		Buffer<WCHAR> buf1(cch);
		Decode(buf2, cch, uCodePage, buf1, cch);
		SetText(IDC_PCTC_EDIT1, buf1);
		m_bDecodingError = false;
		return true;
	}
	catch (Exception e)
	{
		SetText(IDC_PCTC_STATUS2, e.Message);
		m_bDecodingError = true;
		return false;
	}
}


UINT PercentCodecDialogBox::GetCodePage() const
{
	return ComboBoxGetSelection(IDC_PCTC_ENCODING, CP_UTF8);
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


int PercentCodecDialogBox::get_CurrentEdit() const
{
	if (ButtonIsChecked(IDC_PCTC_LABEL1))
	{
		return IDC_PCTC_EDIT1;
	}
	else if (ButtonIsChecked(IDC_PCTC_LABEL2))
	{
		return IDC_PCTC_EDIT2;
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
	case IDC_PCTC_EDIT1:
		return m_szOriginalPath;
	case IDC_PCTC_EDIT2:
		return m_szEncodedPath;
	default:
		throw Exception(L"PercentCodecDialogBox::get_CurrentPath: Unexpected state.");
	}
}


void PercentCodecDialogBox::set_CurrentPath(const String& value)
{
	switch (CurrentEdit)
	{
	case IDC_PCTC_EDIT1:
		m_szOriginalPath = value;
		break;
	case IDC_PCTC_EDIT2:
		m_szEncodedPath = value;
		break;
	default:
		throw Exception(L"PercentCodecDialogBox::set_CurrentPath: Unexpected state.");
	}
}
