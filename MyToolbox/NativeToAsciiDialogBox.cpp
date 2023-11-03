#include "pch.h"
#include "NativeToAsciiDialogBox.h"
#include "resource.h"
#include "hnrt/ResourceString.h"
#include "hnrt/WindowDesign.h"
#include "hnrt/WindowLayoutSnapshot.h"
#include "hnrt/Menu.h"
#include "hnrt/WindowHandle.h"
#include "hnrt/Clipboard.h"
#include "hnrt/Buffer.h"
#include "MyToolbox.h"


static int FromNativeToAscii(PCWSTR psz, PWCHAR pOut = nullptr)
{
	WCHAR* pOutEnd = pOut;
	const WCHAR* pCur = psz;
	WCHAR c;
	if (pOut)
	{
		while ((c = *pCur++) != L'\0')
		{
			if (0 < c && c < 0x80)
			{
				*pOutEnd++ = c;
			}
			else
			{
				static const WCHAR szHex[] = { L"0123456789ABCDEF" };
				pOutEnd[5] = szHex[c & 0xF]; c >>= 4;
				pOutEnd[4] = szHex[c & 0xF]; c >>= 4;
				pOutEnd[3] = szHex[c & 0xF]; c >>= 4;
				pOutEnd[2] = szHex[c & 0xF];
				pOutEnd[1] = L'u';
				pOutEnd[0] = L'\\';
				pOutEnd += 6;
			}
		}
		*pOutEnd = L'\0';
	}
	else
	{
		while ((c = *pCur++) != L'\0')
		{
			if (0 < c && c < 0x80)
			{
				pOutEnd++;
			}
			else
			{
				pOutEnd += 6;
			}
		}
	}
	return static_cast<int>(pOutEnd - pOut);
}


static int ToHex(WCHAR c)
{
	return (L'0' <= c && c <= L'9') ? (c - L'0')
		: (L'A' <= c && c <= L'F') ? (10 + c - L'A')
		: (L'a' <= c && c <= L'f') ? (10 + c - L'a')
		: -1;
}


static int FromAsciiToNative(PCWSTR psz, PWCHAR pOut = nullptr)
{
	WCHAR* pOutEnd = pOut;
	const WCHAR* pCur = psz;
	WCHAR c;
	if (pOut)
	{
		while ((c = *pCur++) != L'\0')
		{
			if (c == L'\\')
			{
				c = *pCur++;
				if (c == L'\0')
				{
					*pOutEnd++ = L'\\';
					break;
				}
				else if ((c == L'u' || c == L'U') && ToHex(pCur[0]) >= 0 && ToHex(pCur[1]) >= 0 && ToHex(pCur[2]) >= 0 && ToHex(pCur[3]) >= 0)
				{
					c = ToHex(*pCur++);
					c <<= 4;
					c |= ToHex(*pCur++);
					c <<= 4;
					c |= ToHex(*pCur++);
					c <<= 4;
					c |= ToHex(*pCur++);
				}
				else
				{
					*pOutEnd++ = L'\\';
				}
			}
			*pOutEnd++ = c;
		}
		*pOutEnd = L'\0';
	}
	else
	{
		while ((c = *pCur++) != L'\0')
		{
			if (c == L'\\')
			{
				c = *pCur++;
				if (c == L'\0')
				{
					pOutEnd++;
					break;
				}
				else if ((c == L'u' || c == L'U') && ToHex(pCur[0]) >= 0 && ToHex(pCur[1]) >= 0 && ToHex(pCur[2]) >= 0 && ToHex(pCur[3]) >= 0)
				{
					pCur += 4;
				}
				else
				{
					pOutEnd++;
				}
			}
			pOutEnd++;
		}
	}
	return static_cast<int>(pOutEnd - pOut);
}


using namespace hnrt;


NativeToAsciiDialogBox::NativeToAsciiDialogBox()
	: MyDialogBox(IDD_NTOA)
{
}


void NativeToAsciiDialogBox::OnCreate()
{
}


void NativeToAsciiDialogBox::OnDestroy()
{
}


void NativeToAsciiDialogBox::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
	WindowLayoutSnapshot before, after;

	before.AddAllChildren(hDlg).Clone(after);

	after[IDC_NTOA_NATIVE_EDIT].right += cxDelta;
	MoveHorizontally(after[IDC_NTOA_COPY1_BUTTON], cxDelta);
	MoveHorizontally(after[IDC_NTOA_PASTE1_BUTTON], cxDelta);
	CenterHorizontally(after[IDC_NTOA_NATIVE_EDIT], after[IDC_NTOA_ENCODE_BUTTON], after[IDC_NTOA_DECODE_BUTTON]);
	after[IDC_NTOA_ASCII_EDIT].right += cxDelta;
	MoveHorizontally(after[IDC_NTOA_COPY2_BUTTON], cxDelta);
	MoveHorizontally(after[IDC_NTOA_PASTE2_BUTTON], cxDelta);

	ExtendVertically(after[IDC_NTOA_NATIVE_EDIT], after[IDC_NTOA_ASCII_EDIT], cyDelta);
	LONG dy = after[IDC_NTOA_NATIVE_EDIT].bottom - before[IDC_NTOA_NATIVE_EDIT].bottom;
	MoveVertically(after[IDC_NTOA_ENCODE_BUTTON], dy);
	MoveVertically(after[IDC_NTOA_DECODE_BUTTON], dy);
	RepositionTop(after[IDC_NTOA_ASCII_RADIO], after[IDC_NTOA_ASCII_EDIT].top);
	RepositionTop(after[IDC_NTOA_COPY2_BUTTON], after[IDC_NTOA_ASCII_EDIT].top);
	dy = after[IDC_NTOA_COPY2_BUTTON].bottom - before[IDC_NTOA_COPY2_BUTTON].bottom;
	MoveVertically(after[IDC_NTOA_PASTE2_BUTTON], dy);

	after.Apply();
}


void NativeToAsciiDialogBox::OnTabSelectionChanging()
{
	MyDialogBox::OnTabSelectionChanging();
	Menu topLevel(GetApp<MyToolbox>().hwnd);
	Menu(topLevel[2])
		.Enable(IDM_VIEW_NTOA, MF_ENABLED);
}


void NativeToAsciiDialogBox::OnTabSelectionChanged()
{
	MyDialogBox::OnTabSelectionChanged();
	MyToolbox& app = GetApp<MyToolbox>();
	Menu topLevel(app.hwnd);
	Menu(topLevel[1])
		.RemoveAll()
		.Add(ResourceString(IDS_COPY), IDM_EDIT_COPY)
		.Add(ResourceString(IDS_PASTE), IDM_EDIT_PASTE)
		.Add(ResourceString(IDS_SELECTALL), IDM_EDIT_SELECTALL)
		.Add(ResourceString(IDS_CLEAR), IDM_EDIT_CLEAR);
	Menu(topLevel[2])
		.Enable(IDM_VIEW_NTOA, MF_DISABLED);
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
		OnCopy1();
		break;
	case IDC_NTOA_COPY2_BUTTON:
		OnCopy2();
		break;
	case IDC_NTOA_PASTE1_BUTTON:
		OnPaste1();
		break;
	case IDC_NTOA_PASTE2_BUTTON:
		OnPaste2();
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


void NativeToAsciiDialogBox::OnCopy()
{
	if (GetButtonState(IDC_NTOA_NATIVE_RADIO) == BST_CHECKED)
	{
		OnCopy1();
	}
	else if (GetButtonState(IDC_NTOA_ASCII_RADIO) == BST_CHECKED)
	{
		OnCopy2();
	}
}


void NativeToAsciiDialogBox::OnPaste()
{
	if (GetButtonState(IDC_NTOA_NATIVE_RADIO) == BST_CHECKED)
	{
		OnPaste1();
	}
	else if (GetButtonState(IDC_NTOA_ASCII_RADIO) == BST_CHECKED)
	{
		OnPaste2();
	}
}


void NativeToAsciiDialogBox::OnSelectAll()
{
	if (GetButtonState(IDC_NTOA_NATIVE_RADIO) == BST_CHECKED)
	{
		SelectAllInEdit(IDC_NTOA_NATIVE_EDIT);
	}
	else if (GetButtonState(IDC_NTOA_ASCII_RADIO) == BST_CHECKED)
	{
		SelectAllInEdit(IDC_NTOA_ASCII_EDIT);
	}
}


void NativeToAsciiDialogBox::OnClear()
{
	if (GetButtonState(IDC_NTOA_NATIVE_RADIO) == BST_CHECKED)
	{
		ClearEdit(IDC_NTOA_NATIVE_EDIT);
	}
	else if (GetButtonState(IDC_NTOA_ASCII_RADIO) == BST_CHECKED)
	{
		ClearEdit(IDC_NTOA_ASCII_EDIT);
	}
}


void NativeToAsciiDialogBox::OnSelectSource(int id)
{
	CheckButton(IDC_NTOA_NATIVE_RADIO, id == IDC_NTOA_NATIVE_RADIO ? BST_CHECKED : BST_UNCHECKED);
	EnableWindow(IDC_NTOA_NATIVE_EDIT, id == IDC_NTOA_NATIVE_RADIO);
	EnableWindow(IDC_NTOA_COPY1_BUTTON);
	EnableWindow(IDC_NTOA_PASTE1_BUTTON, id == IDC_NTOA_NATIVE_RADIO);
	EnableWindow(IDC_NTOA_ENCODE_BUTTON, id == IDC_NTOA_NATIVE_RADIO);
	CheckButton(IDC_NTOA_ASCII_RADIO, id == IDC_NTOA_ASCII_RADIO ? BST_CHECKED : BST_UNCHECKED);
	EnableWindow(IDC_NTOA_ASCII_EDIT, id == IDC_NTOA_ASCII_RADIO);
	EnableWindow(IDC_NTOA_COPY2_BUTTON);
	EnableWindow(IDC_NTOA_PASTE2_BUTTON, id == IDC_NTOA_ASCII_RADIO);
	EnableWindow(IDC_NTOA_DECODE_BUTTON, id == IDC_NTOA_ASCII_RADIO);
}


void NativeToAsciiDialogBox::OnCopy1()
{
	if (!Clipboard::Copy(hwnd, hwnd, IDC_NTOA_NATIVE_EDIT))
	{
		MessageBoxW(hwnd, ResourceString(IDS_MSG_CLIPBOARD_COPY_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
		return;
	}
}


void NativeToAsciiDialogBox::OnCopy2()
{
	if (!Clipboard::Copy(hwnd, hwnd, IDC_NTOA_ASCII_EDIT))
	{
		MessageBoxW(hwnd, ResourceString(IDS_MSG_CLIPBOARD_COPY_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
		return;
	}
}


void NativeToAsciiDialogBox::OnPaste1()
{
	PasteIntoEdit(IDC_NTOA_NATIVE_EDIT);
}


void NativeToAsciiDialogBox::OnPaste2()
{
	PasteIntoEdit(IDC_NTOA_ASCII_EDIT);
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
