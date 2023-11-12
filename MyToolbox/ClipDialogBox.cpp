#include "pch.h"
#include "ClipDialogBox.h"
#include "resource.h"
#include "hnrt/WindowDesign.h"
#include "hnrt/WindowLayoutSnapshot.h"
#include "hnrt/ResourceString.h"
#include "hnrt/Path.h"
#include "hnrt/Debug.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/FileWriter.h"
#include "hnrt/FileMapper.h"
#include "hnrt/Buffer.h"
#include "hnrt/Hash.h"
#include "hnrt/ByteDataFeeder.h"


using namespace hnrt;


ClipDialogBox::ClipDialogBox()
	: MyDialogBox(IDD_CLIP)
	, m_pCO(RefPtr<ClipboardObserver>(new MyClipboardObserver(*this)))
	, m_pszDirectoryPath(Path::Combine(Path::GetKnownFolder(FOLDERID_LocalAppData), L"hnrt", L"MyToolbox", L"clippings"))
	, m_mapHash()
	, m_pszFilePath(nullptr)
	, m_bProcessing(false)
{
	Clipboard::Register(m_pCO);
}


ClipDialogBox::~ClipDialogBox()
{
	Clipboard::Unregister(m_pCO);
}


void ClipDialogBox::OnCreate()
{
	MyDialogBox::OnCreate();
	DisableWindow(IDC_CLIP_COPY_BUTTON);
	DisableWindow(IDC_CLIP_DELETE_BUTTON);
	m_menuView
		.Add(ResourceString(IDS_MENU_CLIP), IDM_VIEW_CLIP);
}


void ClipDialogBox::OnDestroy()
{
	MyDialogBox::OnDestroy();
}


void ClipDialogBox::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
	WindowLayoutSnapshot before, after;

	before.AddAllChildren(hDlg).Clone(after);

	after[IDC_CLIP_FILENAME_LIST].right += cxDelta;
	MoveHorizontally(after[IDC_CLIP_COPY_BUTTON], cxDelta);
	MoveHorizontally(after[IDC_CLIP_DELETE_BUTTON], cxDelta);
	after[IDC_CLIP_HEADER_EDIT].right += cxDelta;
	after[IDC_CLIP_BODY_EDIT].right += cxDelta;

	after[IDC_CLIP_BODY_EDIT].bottom += cyDelta;

	after.Apply();
}


void ClipDialogBox::OnTabSelectionChanging()
{
	MyDialogBox::OnTabSelectionChanging();
	m_menuView
		.Enable(IDM_VIEW_CLIP, MF_ENABLED);
}


void ClipDialogBox::OnTabSelectionChanged()
{
	MyDialogBox::OnTabSelectionChanged();
	m_menuEdit
		.RemoveAll()
		.Add(ResourceString(IDS_MENU_COPY), IDM_EDIT_COPY, m_pszFilePath ? MF_ENABLED : MF_DISABLED)
		.Add(ResourceString(IDS_MENU_DELETE), IDM_EDIT_DELETE, m_pszFilePath ? MF_ENABLED : MF_DISABLED);
	m_menuView
		.Enable(IDM_VIEW_CLIP, MF_DISABLED);
}


INT_PTR ClipDialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	UINT idChild = LOWORD(wParam);
	UINT idNotif = HIWORD(wParam);
	switch (idChild)
	{
	case IDC_CLIP_FILENAME_LIST:
		switch (idNotif)
		{
		case LBN_SELCHANGE:
			OnSelectionChange();
			break;
		default:
			break;
		}
		break;
	case IDC_CLIP_COPY_BUTTON:
		OnCopy();
		break;
	case IDC_CLIP_DELETE_BUTTON:
		OnDelete();
		break;
	default:
		return FALSE;
	}
	return TRUE;
}


void ClipDialogBox::OnCopy()
{
	m_bProcessing = true;
	int cch = GetTextLength(IDC_CLIP_BODY_EDIT);
	int cchSize = cch + 1;
	Buffer<WCHAR> content(cchSize);
	GetText(IDC_CLIP_BODY_EDIT, content, content.Len);
	if (!Clipboard::Copy(hwnd, content, cch))
	{
		MessageBoxW(hwnd, ResourceString(IDS_MSG_CLIPBOARD_COPY_ERROR), ResourceString(IDS_APP_TITLE), MB_ICONERROR | MB_OK);
	}
	m_bProcessing = false;
}


void ClipDialogBox::OnDelete()
{
}


void ClipDialogBox::ClipboardCopy(HWND hwnd, PCWSTR psz)
{
	if (m_bProcessing)
	{
		return;
	}
	if (!Path::ValidateDirectory(m_pszDirectoryPath))
	{
		WCHAR szMessage[MAX_PATH * 2] = { 0 };
		swprintf_s(szMessage, L"%s\n%s", m_pszDirectoryPath, ErrorMessage::Get(GetLastError()));
		MessageBoxW(hwnd, szMessage, ResourceString(IDS_APP_TITLE), MB_ICONERROR | MB_OK);
		return;
	}
	size_t cch = wcslen(psz);
	ByteDataFeeder bdf(psz, cch * sizeof(WCHAR));
	MD5Hash hash(bdf);
	SYSTEMTIME t = { 0 };
	GetLocalTime(&t);
	PCWSTR pszName = String::Format(L"%04d%02d%02d_%02d%02d%02d_%s", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, hash.Text);
	try
	{
		bool bSelect = false;
		ClipMap::const_iterator iter = m_mapHash.find(hash.Text);
		if (iter == m_mapHash.cend())
		{
			if (!Path::IsFile(Path::Combine(m_pszDirectoryPath, pszName)))
			{
				static const WCHAR LineBreak[2] = { L'\r', L'\n' };
				FileWriter body(Path::Combine(m_pszDirectoryPath, pszName), CREATE_NEW);
				body.Write(LineBreak, 2 * sizeof(WCHAR));
				body.Write(psz, cch * sizeof(WCHAR));
				body.Close();
			}
		}
		else
		{
			PCWSTR pszOldName = iter->second;
			if (!MoveFileW(Path::Combine(m_pszDirectoryPath, pszOldName), Path::Combine(m_pszDirectoryPath, pszName)))
			{
				DWORD dwError = GetLastError();
				throw Win32Exception(dwError, L"Failed to rename %s to %s.", pszOldName, pszName);
			}
			m_mapHash.erase(iter);
			LRESULT index = SendMessage(IDC_CLIP_FILENAME_LIST, LB_FINDSTRING, -1, reinterpret_cast<LPARAM>(pszOldName));
			if (index != LB_ERR)
			{
				if (SendMessage(IDC_CLIP_FILENAME_LIST, LB_GETSEL, index, 0) > 0)
				{
					bSelect = true;
					m_pszFilePath = Path::Combine(m_pszDirectoryPath, pszName);
				}
				SendMessage(IDC_CLIP_FILENAME_LIST, LB_DELETESTRING, index, 0);
			}
		}
		m_mapHash.insert(ClipEntry(String::Copy(hash.Text), pszName));
		SendMessage(IDC_CLIP_FILENAME_LIST, LB_INSERTSTRING, 0, reinterpret_cast<LPARAM>(pszName));
		if (bSelect)
		{
			SendMessage(IDC_CLIP_FILENAME_LIST, LB_SETCURSEL, 0, 0);
		}
	}
	catch (Win32Exception e)
	{
		Debug::Put(L"%s %s", e.Message, ErrorMessage::Get(e.Error));
	}
}


void ClipDialogBox::OnSelectionChange()
{
	LRESULT index = SendMessage(IDC_CLIP_FILENAME_LIST, LB_GETCURSEL, 0, 0);
	if (index == LB_ERR)
	{
		Debug::Put(L"ClipDialogBox::OnSelectionChange: LB_GETCURSEL failed.");
		return;
	}
	LRESULT cch = SendMessage(IDC_CLIP_FILENAME_LIST, LB_GETTEXTLEN, index, 0);
	if (cch++ == LB_ERR)
	{
		Debug::Put(L"ClipDialogBox::OnSelectionChange: LB_GETTEXTLEN failed.");
		return;
	}
	Buffer<WCHAR> name(cch);
	if (SendMessage(IDC_CLIP_FILENAME_LIST, LB_GETTEXT, index, reinterpret_cast<LPARAM>(name.Ptr)) == LB_ERR)
	{
		Debug::Put(L"ClipDialogBox::OnSelectionChange: LB_GETTEXT failed.");
		return;
	}
	try
	{
		m_pszFilePath = Path::Combine(m_pszDirectoryPath, name.Ptr);
		FileMapper file(m_pszFilePath);
		Buffer<WCHAR> buf(file.Len / sizeof(WCHAR) + 1);
		memcpy_s(buf, buf.Len * sizeof(WCHAR), file.Ptr, file.Len);
		buf[buf.Len - 1] = L'\0';
		file.Close();
		PWCHAR pContent = wcschr(buf, L'\r');
		if (!pContent || pContent[1] != L'\n')
		{
			throw Exception(L"Looks broken!");
		}
		*pContent++ = L'\0';
		*pContent++ = L'\0';
		SetText(IDC_CLIP_HEADER_EDIT, buf);
		SetReadOnlyEdit(IDC_CLIP_HEADER_EDIT, FALSE);
		SetText(IDC_CLIP_BODY_EDIT, pContent ? pContent : L"");
		EnableWindow(IDC_CLIP_COPY_BUTTON);
		EnableWindow(IDC_CLIP_DELETE_BUTTON);
		m_menuEdit
			.Enable(IDM_EDIT_COPY)
			.Enable(IDS_MENU_DELETE);
		return;
	}
	catch (Win32Exception e)
	{
		WCHAR szMessage[MAX_PATH * 2] = { 0 };
		swprintf_s(szMessage, L"%s %s", e.Message, ErrorMessage::Get(e.Error));
		SetText(IDC_CLIP_HEADER_EDIT, szMessage);
		SetReadOnlyEdit(IDC_CLIP_HEADER_EDIT, TRUE);
	}
	catch (Exception e)
	{
		SetText(IDC_CLIP_HEADER_EDIT, e.Message);
		SetReadOnlyEdit(IDC_CLIP_HEADER_EDIT, TRUE);
	}
	SetText(IDC_CLIP_BODY_EDIT);
	m_pszFilePath = nullptr;
	DisableWindow(IDC_CLIP_COPY_BUTTON);
	DisableWindow(IDC_CLIP_DELETE_BUTTON);
	m_menuEdit
		.Enable(IDM_EDIT_COPY, MF_DISABLED)
		.Enable(IDS_MENU_DELETE, MF_DISABLED);
}
