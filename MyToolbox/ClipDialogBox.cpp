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
#include "hnrt/ByteDataFeeder.h"
#include "hnrt/Unicode.h"


using namespace hnrt;


static const WCHAR Separator = LINE_SEPARATOR;


ClipDialogBox::ClipDialogBox()
	: MyDialogBox(IDD_CLIP)
	, m_pCO(RefPtr<ClipboardObserver>(new MyClipboardObserver(*this)))
	, m_szDirectoryPath(Path::Combine(Path::GetKnownFolder(FOLDERID_LocalAppData), L"hnrt", L"MyToolbox", L"clippings"))
	, m_mapHash()
	, m_szFilePath()
	, m_szHash()
	, m_bChanged(false)
	, m_bProcessing(false)
{
	Clipboard::Register(m_pCO);
}


ClipDialogBox::~ClipDialogBox()
{
	Clipboard::Unregister(m_pCO);
}


static int __cdecl DirectoryEntryCompare(const void* a1, const void* a2)
{
	const DirectoryEntry* p1 = reinterpret_cast<const DirectoryEntry*>(a1);
	const DirectoryEntry* p2 = reinterpret_cast<const DirectoryEntry*>(a2);
	return String::Compare(p1->szFileName, p2->szFileName);
}


void ClipDialogBox::OnCreate()
{
	MyDialogBox::OnCreate();
	DisableWindow(IDC_CLIP_COPY_BUTTON);
	DisableWindow(IDC_CLIP_DELETE_BUTTON);
	m_menuView
		.Add(ResourceString(IDS_MENU_CLIP), IDM_VIEW_CLIP);
	std::vector<DirectoryEntry> entries;
	Path::ListFiles(entries, m_szDirectoryPath);
	qsort(&entries[0], entries.size(), sizeof(DirectoryEntry), DirectoryEntryCompare);
	for (std::vector<DirectoryEntry>::const_iterator iter = entries.cbegin(); iter != entries.cend(); iter++)
	{
		try
		{
			String szFileName(iter->szFileName);
			String szFilePath(Path::Combine(m_szDirectoryPath, szFileName));
			FileMapper file(szFilePath);
			const WCHAR* pStart = reinterpret_cast<const WCHAR*>(file.Ptr);
			const WCHAR* pEnd = pStart + file.Len / sizeof(WCHAR);
			const WCHAR* pHash = wmemchr(pStart, Separator, pEnd - pStart);
			if (!pHash++)
			{
				throw Exception(L"Looks broken!");
			}
			const WCHAR* pHashEnd = wmemchr(pHash, Separator, pEnd - pHash);
			if (!pHashEnd)
			{
				throw Exception(L"Looks broken!");
			}
			m_mapHash.insert(ClipEntry(String(pHash, pHashEnd - pHash), szFileName));
			SendMessage(IDC_CLIP_FILENAME_LIST, LB_INSERTSTRING, 0, reinterpret_cast<LPARAM>(szFileName.Ptr));
		}
		catch (Exception e)
		{
		}
	}
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
		.Add(ResourceString(IDS_MENU_COPY), IDM_EDIT_COPY, m_szFilePath ? MF_ENABLED : MF_DISABLED)
		.Add(ResourceString(IDS_MENU_DELETE), IDM_EDIT_DELETE, m_szFilePath ? MF_ENABLED : MF_DISABLED);
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
	case IDC_CLIP_HEADER_EDIT:
		switch (idNotif)
		{
		case EN_CHANGE:
			m_bChanged = true;
			break;
		case EN_KILLFOCUS:
			if (m_bChanged)
			{
				m_bChanged = false;
				WriteBackToFile();
			}
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
	if (!Clipboard::Write(hwnd, content, cch))
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
	if (!Path::ValidateDirectory(m_szDirectoryPath))
	{
		String szMessage(PRINTF, L"%s\n%s", m_szDirectoryPath.Ptr, ErrorMessage::Get(GetLastError()));
		MessageBoxW(hwnd, szMessage, ResourceString(IDS_APP_TITLE), MB_ICONERROR | MB_OK);
		return;
	}
	size_t cch = wcslen(psz);
	ByteDataFeeder bdf(psz, cch * sizeof(WCHAR));
	MD5Hash hash(bdf);
	SYSTEMTIME t = { 0 };
	GetLocalTime(&t);
	String szName(PRINTF, L"%04d%02d%02d_%02d%02d%02d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
	try
	{
		bool bSelect = false;
		ClipMap::const_iterator iter = m_mapHash.find(hash.Text);
		if (iter == m_mapHash.cend())
		{
			if (!Path::IsFile(Path::Combine(m_szDirectoryPath, szName)))
			{
				FileWriter body(Path::Combine(m_szDirectoryPath, szName), CREATE_NEW);
				body.Write(&Separator, sizeof(WCHAR));
				body.Write(hash.Text, wcslen(hash.Text) * sizeof(WCHAR));
				body.Write(&Separator, sizeof(WCHAR));
				body.Write(psz, cch * sizeof(WCHAR));
				body.Close();
			}
		}
		else
		{
			String szOldName = iter->second;
			if (!MoveFileW(Path::Combine(m_szDirectoryPath, szOldName), Path::Combine(m_szDirectoryPath, szName)))
			{
				DWORD dwError = GetLastError();
				throw Win32Exception(dwError, L"Failed to rename %s to %s.", szOldName.Ptr, szName.Ptr);
			}
			m_mapHash.erase(iter);
			LRESULT index = SendMessage(IDC_CLIP_FILENAME_LIST, LB_FINDSTRING, -1, reinterpret_cast<LPARAM>(szOldName.Ptr));
			if (index != LB_ERR)
			{
				if (SendMessage(IDC_CLIP_FILENAME_LIST, LB_GETSEL, index, 0) > 0)
				{
					bSelect = true;
					m_szFilePath = Path::Combine(m_szDirectoryPath, szName);
				}
				SendMessage(IDC_CLIP_FILENAME_LIST, LB_DELETESTRING, index, 0);
			}
		}
		m_mapHash.insert(ClipEntry(hash.Text, szName));
		SendMessage(IDC_CLIP_FILENAME_LIST, LB_INSERTSTRING, 0, reinterpret_cast<LPARAM>(szName.Ptr));
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
		if (m_bChanged)
		{
			m_bChanged = false;
			WriteBackToFile();
		}
		m_szFilePath = Path::Combine(m_szDirectoryPath, name.Ptr);
		FileMapper file(m_szFilePath);
		Buffer<WCHAR> buf(file.Len / sizeof(WCHAR) + 1);
		memcpy_s(buf, buf.Len * sizeof(WCHAR), file.Ptr, file.Len);
		buf[buf.Len - 1] = L'\0';
		file.Close();
		PWSTR pszHash = wcschr(buf, Separator);
		if (!pszHash)
		{
			throw Exception(L"Looks broken!");
		}
		*pszHash++ = L'\0';
		PWSTR pContent = wcschr(pszHash, Separator);
		if (!pContent)
		{
			throw Exception(L"Looks broken!");
		}
		*pContent++ = L'\0';
		SetText(IDC_CLIP_HEADER_EDIT, buf);
		SetReadOnlyEdit(IDC_CLIP_HEADER_EDIT, FALSE);
		m_szHash = pszHash;
		SetText(IDC_CLIP_BODY_EDIT, pContent);
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
	m_szFilePath = String::Empty;
	DisableWindow(IDC_CLIP_COPY_BUTTON);
	DisableWindow(IDC_CLIP_DELETE_BUTTON);
	m_menuEdit
		.Enable(IDM_EDIT_COPY, MF_DISABLED)
		.Enable(IDS_MENU_DELETE, MF_DISABLED);
}


void ClipDialogBox::WriteBackToFile()
{
	try
	{
		size_t cchHeader = GetTextLength(IDC_CLIP_HEADER_EDIT);
		Buffer<WCHAR> header(cchHeader + 1);
		GetText(IDC_CLIP_HEADER_EDIT, header, header.Len);
		for (PWCHAR pCur = header; (pCur = wcschr(pCur, Separator)) != nullptr ; pCur++)
		{
			*pCur = L' ';
		}
		size_t cchBody = GetTextLength(IDC_CLIP_BODY_EDIT);
		Buffer<WCHAR> body(cchBody + 1);
		GetText(IDC_CLIP_BODY_EDIT, body, body.Len);
		FileWriter file(m_szFilePath, CREATE_ALWAYS);
		file.Write(header, cchHeader * sizeof(WCHAR));
		file.Write(&Separator, sizeof(WCHAR));
		file.Write(m_szHash.Ptr, m_szHash.Len * sizeof(WCHAR));
		file.Write(&Separator, sizeof(WCHAR));
		file.Write(body, cchBody * sizeof(WCHAR));
	}
	catch (Win32Exception e)
	{
		String message(PRINTF, L"%s\n%s", m_szFilePath.Ptr, ErrorMessage::Get(GetLastError()));
		MessageBoxW(hwnd, message, ResourceString(IDS_APP_TITLE), MB_ICONERROR | MB_OK);
	}
}
