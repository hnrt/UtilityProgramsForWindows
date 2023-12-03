#include "pch.h"
#include "ClipDialogBox.h"
#include "ClipFile.h"
#include "resource.h"
#include "hnrt/WindowDesign.h"
#include "hnrt/WindowLayoutSnapshot.h"
#include "hnrt/ResourceString.h"
#include "hnrt/Path.h"
#include "hnrt/Time.h"
#include "hnrt/Debug.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/Buffer.h"
#include "hnrt/ByteDataFeeder.h"
#include "hnrt/Unicode.h"


using namespace hnrt;


ClipDialogBox::ClipDialogBox()
	: MyDialogBox(IDD_CLIP)
	, m_pCO(RefPtr<ClipboardObserver>(new MyClipboardObserver(*this)))
	, m_szDirectoryPath(Path::Combine(Path::GetKnownFolder(FOLDERID_LocalAppData), L"hnrt", L"MyToolbox", L"clippings"))
	, m_mapHash()
	, m_selected(-1)
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
	LONGLONG diff = FileTime(p1->ftLastWriteTime).Intervals - FileTime(p2->ftLastWriteTime).Intervals;
	if (diff > 0)
	{
		return 1;
	}
	else if (diff < 0)
	{
		return -1;
	}
	diff = FileTime(p1->ftCreationTime).Intervals - FileTime(p2->ftCreationTime).Intervals;
	if (diff > 0)
	{
		return 1;
	}
	else if (diff < 0)
	{
		return -1;
	}
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
			ClipFile file(szFilePath);
			m_mapHash.insert(ClipEntry(file.Hash, szFileName));
			String item(PRINTF, L"%s %s", szFileName.Ptr, file.Header.Ptr);
			SendMessage(IDC_CLIP_FILENAME_LIST, LB_INSERTSTRING, 0, reinterpret_cast<LPARAM>(item.Ptr));
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
				ListBoxSetText(IDC_CLIP_FILENAME_LIST, m_selected,
					String(PRINTF, L"%s %s", Path::GetFileName(m_szFilePath).Ptr, GetText(IDC_CLIP_HEADER_EDIT).Ptr).Ptr);
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
	String szContent = GetText(IDC_CLIP_BODY_EDIT);
	if (!Clipboard::Write(hwnd, szContent, szContent.Len))
	{
		MessageBoxW(hwnd, ResourceString(IDS_MSG_CLIPBOARD_COPY_ERROR), ResourceString(IDS_APP_TITLE), MB_ICONERROR | MB_OK);
	}
	m_bProcessing = false;
}


void ClipDialogBox::OnDelete()
{
	int index = ListBoxGetSelection(IDC_CLIP_FILENAME_LIST);
	if (index < 0)
	{
		return;
	}
	String szFilePath = m_szFilePath;
	ListBoxDelete(IDC_CLIP_FILENAME_LIST, index);
	SetText(IDC_CLIP_HEADER_EDIT);
	EditSetReadOnly(IDC_CLIP_HEADER_EDIT, TRUE);
	SetText(IDC_CLIP_BODY_EDIT);
	DisableWindow(IDC_CLIP_COPY_BUTTON);
	DisableWindow(IDC_CLIP_DELETE_BUTTON);
	m_menuEdit
		.Enable(IDM_EDIT_COPY, MF_DISABLED)
		.Enable(IDS_MENU_DELETE, MF_DISABLED);
	m_mapHash.erase(m_szHash);
	m_szFilePath = String::Empty;
	m_szHash = String::Empty;
	m_bChanged = false;
	if (!DeleteFileW(szFilePath))
	{
		DWORD dwError = GetLastError();
		Debug::Put(L"DeleteFile(%s) failed: %s", szFilePath.Ptr, ErrorMessage::Get(dwError));
	}
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
				ClipFile(Path::Combine(m_szDirectoryPath, szName)).Save(hash.Text, psz);
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
			int index = ListBoxFind(IDC_CLIP_FILENAME_LIST, szOldName);
			if (index >= 0)
			{
				if (ListBoxIsSelected(IDC_CLIP_FILENAME_LIST, index))
				{
					bSelect = true;
					m_szFilePath = Path::Combine(m_szDirectoryPath, szName);
				}
				SendMessage(IDC_CLIP_FILENAME_LIST, LB_DELETESTRING, index, 0);
			}
		}
		m_mapHash.insert(ClipEntry(hash.Text, szName));
		ClipFile file(Path::Combine(m_szDirectoryPath, szName));
		String item(PRINTF, L"%s %s", szName.Ptr, file.Header.Ptr);
		SendMessage(IDC_CLIP_FILENAME_LIST, LB_INSERTSTRING, 0, reinterpret_cast<LPARAM>(item.Ptr));
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
	try
	{
		int index = ListBoxGetSelection(IDC_CLIP_FILENAME_LIST, -1);
		if (index < 0)
		{
			return;
		}
		if (m_bChanged)
		{
			m_bChanged = false;
			WriteBackToFile();
			ListBoxSetText(IDC_CLIP_FILENAME_LIST, m_selected,
				String(PRINTF, L"%s %s", Path::GetFileName(m_szFilePath).Ptr, GetText(IDC_CLIP_HEADER_EDIT).Ptr).Ptr);
		}
		String szItem = ListBoxGetText(IDC_CLIP_FILENAME_LIST, index);
		int nameLen = szItem.IndexOf(L' ');
		String szName(szItem.Ptr, nameLen >= 0 ? nameLen : szItem.Len);
		String szFilePath = Path::Combine(m_szDirectoryPath, szName.Ptr);
		ClipFile file(szFilePath);
		String item(PRINTF, L"%s %s", szName.Ptr, file.Header.Ptr);
		ListBoxSetText(IDC_CLIP_FILENAME_LIST, index, item);
		SetText(IDC_CLIP_HEADER_EDIT, file.Header);
		EditSetReadOnly(IDC_CLIP_HEADER_EDIT, FALSE);
		SetText(IDC_CLIP_BODY_EDIT, file.Content);
		EnableWindow(IDC_CLIP_COPY_BUTTON);
		EnableWindow(IDC_CLIP_DELETE_BUTTON);
		m_menuEdit
			.Enable(IDM_EDIT_COPY)
			.Enable(IDS_MENU_DELETE);
		m_selected = index;
		m_szFilePath = szFilePath;
		m_szHash = file.Hash;
		m_bChanged = false;
		return;
	}
	catch (Win32Exception e)
	{
		SetText(IDC_CLIP_HEADER_EDIT, String(PRINTF, L"%s %s", e.Message, ErrorMessage::Get(e.Error)));
		EditSetReadOnly(IDC_CLIP_HEADER_EDIT, TRUE);
	}
	catch (Exception e)
	{
		SetText(IDC_CLIP_HEADER_EDIT, e.Message);
		EditSetReadOnly(IDC_CLIP_HEADER_EDIT, TRUE);
	}
	SetText(IDC_CLIP_BODY_EDIT);
	DisableWindow(IDC_CLIP_COPY_BUTTON);
	DisableWindow(IDC_CLIP_DELETE_BUTTON);
	m_menuEdit
		.Enable(IDM_EDIT_COPY, MF_DISABLED)
		.Enable(IDS_MENU_DELETE, MF_DISABLED);
	m_szFilePath = String::Empty;
	m_szHash = String::Empty;
	m_bChanged = false;
}


void ClipDialogBox::WriteBackToFile() const
{
	try
	{
		String szHeader = GetText(IDC_CLIP_HEADER_EDIT);
		String szContent = GetText(IDC_CLIP_BODY_EDIT);
		ClipFile(m_szFilePath).Save(szHeader, m_szHash, szContent);
	}
	catch (Win32Exception e)
	{
		String message(PRINTF, L"%s\n%s", m_szFilePath.Ptr, ErrorMessage::Get(GetLastError()));
		MessageBoxW(hwnd, message, ResourceString(IDS_APP_TITLE), MB_ICONERROR | MB_OK);
	}
}
