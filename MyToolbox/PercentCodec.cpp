#include "pch.h"
#include "PercentCodec.h"
#include "resource.h"
#include "hnrt/WindowDesign.h"
#include "hnrt/WindowHandle.h"
#include "hnrt/WindowLayoutSnapshot.h"
#include "hnrt/Clipboard.h"
#include "hnrt/ResourceString.h"
#include "hnrt/Buffer.h"
#include "hnrt/Exception.h"
#include "hnrt/Debug.h"


#define LABEL_UTF8 L"UTF-8"
#define LABEL_CP932 L"CP 932"


using namespace hnrt;


PercentCodec::PercentCodec()
	: DialogBox(IDD_PCTC)
	, m_bEncodingError(false)
	, m_bDecodingError(false)
{
}


void PercentCodec::OnCreate()
{
	AddStringToComboBox(IDC_PCTC_ENCODING, LABEL_UTF8);
	AddStringToComboBox(IDC_PCTC_ENCODING, LABEL_CP932);
	SetComboBoxSelection(IDC_PCTC_ENCODING, LABEL_UTF8);
	CheckButton(IDC_PCTC_USE_PLUS);
	OnSelectSource(IDC_PCTC_LABEL1);
	SetText(IDC_PCTC_STATUS1, L"");
	SetText(IDC_PCTC_STATUS2, L"");
}


void PercentCodec::OnDestroy()
{
}


void PercentCodec::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
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

	RepositionTopByBottom(after[IDC_PCTC_ENCODE], before[IDC_PCTC_EDIT1], after[IDC_PCTC_EDIT1]);
	RepositionTopByBottom(after[IDC_PCTC_ENCODING], before[IDC_PCTC_EDIT1], after[IDC_PCTC_EDIT1]);
	RepositionTopByBottom(after[IDC_PCTC_DECODE], before[IDC_PCTC_EDIT1], after[IDC_PCTC_EDIT1]);
	RepositionTopByBottom(after[IDC_PCTC_STATUS1], before[IDC_PCTC_EDIT1], after[IDC_PCTC_EDIT1]);
	RepositionTopByBottom(after[IDC_PCTC_STATUS2], before[IDC_PCTC_EDIT1], after[IDC_PCTC_EDIT1]);

	RepositionTop(after[IDC_PCTC_LABEL2], after[IDC_PCTC_EDIT2].top);
	RepositionTop(after[IDC_PCTC_COPY2], after[IDC_PCTC_EDIT2].top);

	MoveVertically(after[IDC_PCTC_LABEL3], cyDelta);

	after.Apply();
}


INT_PTR PercentCodec::OnCommand(WPARAM wParam, LPARAM lParam)
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
		OnCopy1();
		break;
	case IDC_PCTC_COPY2:
		OnCopy2();
		break;
	case IDC_PCTC_ENCODE:
		if (OnEncode())
		{
			OnSelectSource(IDC_PCTC_LABEL2);
			SetFocus(GetDlgItem(hwnd, IDC_PCTC_COPY2));
		}
		break;
	case IDC_PCTC_DECODE:
		if (OnDecode())
		{
			OnSelectSource(IDC_PCTC_LABEL1);
			SetFocus(GetDlgItem(hwnd, IDC_PCTC_COPY1));
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


INT_PTR PercentCodec::OnControlColorStatic(WPARAM wParam, LPARAM lParam)
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


void PercentCodec::OnCopy()
{
	if (GetButtonState(IDC_PCTC_LABEL1) == BST_CHECKED)
	{
		OnCopy1();
	}
	else if (GetButtonState(IDC_PCTC_LABEL2) == BST_CHECKED)
	{
		OnCopy2();
	}
}


void PercentCodec::OnSelectSource(int id)
{
	CheckButton(IDC_PCTC_LABEL1, id == IDC_PCTC_LABEL1 ? BST_CHECKED : BST_UNCHECKED);
	EnableWindow(IDC_PCTC_EDIT1, id == IDC_PCTC_LABEL1);
	EnableWindow(IDC_PCTC_COPY1, id == IDC_PCTC_LABEL1);
	EnableWindow(IDC_PCTC_USE_PLUS, id == IDC_PCTC_LABEL1);
	EnableWindow(IDC_PCTC_ENCODE, id == IDC_PCTC_LABEL1);
	CheckButton(IDC_PCTC_LABEL2, id == IDC_PCTC_LABEL2 ? BST_CHECKED : BST_UNCHECKED);
	EnableWindow(IDC_PCTC_EDIT2, id == IDC_PCTC_LABEL2);
	EnableWindow(IDC_PCTC_COPY2, id == IDC_PCTC_LABEL2);
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
}


void PercentCodec::OnCopy1()
{
	if (!Clipboard::Copy(hwnd, hwnd, IDC_PCTC_EDIT1))
	{
		MessageBoxW(hwnd, ResourceString(IDS_MSG_CLIPBOARD_COPY_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
		return;
	}
}


void PercentCodec::OnCopy2()
{
	if (!Clipboard::Copy(hwnd, hwnd, IDC_PCTC_EDIT2))
	{
		MessageBoxW(hwnd, ResourceString(IDS_MSG_CLIPBOARD_COPY_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
		return;
	}
}


bool PercentCodec::OnEncode()
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
		Buffer<WCHAR> buf2(static_cast<size_t>(cch * 4 * 3));
		Encode(buf1, cch, uCodePage, buf2, static_cast<UINT>(buf2.Len), GetButtonState(IDC_PCTC_USE_PLUS) == BST_CHECKED);
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


bool PercentCodec::OnDecode()
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


UINT PercentCodec::GetCodePage()
{
	int index = GetComboBoxSelection(IDC_PCTC_ENCODING);
	UINT length = GetListBoxTextLength(IDC_PCTC_ENCODING, index);
	if (length < 16)
	{
		length = 16;
	}
	Buffer<WCHAR> buf(length + 1);
	GetListBoxText(IDC_PCTC_ENCODING, index, buf, LABEL_UTF8);
	if (!wcscmp(buf, LABEL_CP932))
	{
		return 932;
	}
	else
	{
		return CP_UTF8;
	}
}


#define ENCODE_BUFFER_TOO_SHORT L"PercentCodec::Encode: Buffer is too short."


void PercentCodec::Encode(PCWSTR pszIn, UINT cchIn, UINT uCodePage, PWCHAR pOut, UINT cchOut, bool bUsePlus)
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


void PercentCodec::Encode(WCHAR c, UINT uCodePage, PWCHAR& pOut, PWCHAR pOutBound, UINT offset)
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


void PercentCodec::Decode(PCWSTR pszIn, UINT cchIn, UINT uCodePage, PWCHAR pOut, UINT cchOut)
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


UINT PercentCodec::GetDecodedOffset(PCWSTR pszIn, UINT cbOut)
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
