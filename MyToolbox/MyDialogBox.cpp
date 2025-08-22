#include "pch.h"
#include "MyDialogBox.h"
#include "MyToolbox.h"
#include "resource.h"
#include "hnrt/Menu.h"
#include "hnrt/ResourceString.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/FileMapper.h"
#include "hnrt/FileWriter.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/Buffer.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/Unicode.h"
#include "hnrt/WhileInScope.h"
#include "hnrt/StringCommons.h"
#include "hnrt/NumberText.h"
#include "hnrt/Debug.h"
#include <map>


using namespace hnrt;


MyDialogBox::MyDialogBox(UINT idTemplate, PCWSTR pszName, int StatusId, int Pane1Id, int Pane2Id)
	: DialogBox(idTemplate)
	, m_id(-1)
	, m_bActive(false)
	, m_szRegistryKeyName(GetApp<MyToolbox>().GetRegistryKeyName(pszName))
	, m_StatusId(StatusId)
	, m_Pane1Id(Pane1Id)
	, m_Pane2Id(Pane2Id)
	, m_menuFile(HMENU_NULL)
	, m_menuEdit(HMENU_NULL)
	, m_menuView(HMENU_NULL)
	, m_menuSettings(HMENU_NULL)
	, m_menuHelp(HMENU_NULL)
	, m_uInputCodePage(CP_AUTODETECT)
	, m_uOutputCodePage(CP_UTF8)
	, m_bOutputBOM(false)
	, m_OutputLineBreak(LineBreak::DONOTCARE)
	, m_timers()
	, m_dwFlags(0)
	, m_CurrentEdit(0)
	, m_LastModified()
	, m_cProcessing(0)
{
	memset(m_timers, 0, sizeof(m_timers));
}


void MyDialogBox::ProcessMessages()
{
	MyToolbox& app = GetApp<MyToolbox>();
	while (app.TryProcessMessage() > 0)
	{
		continue;
	}
}


void MyDialogBox::OnCreate()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	Menu menuBar(GetApp<MyToolbox>().hwnd);
	m_menuFile.Set(menuBar[(size_t)0]);
	m_menuEdit.Set(menuBar[1]);
	m_menuView.Set(menuBar[2]);
	m_menuSettings.Set(menuBar[3]);
	m_menuHelp.Set(menuBar[4]);
}


void MyDialogBox::OnDestroy()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
}


void MyDialogBox::OnTabSelectionChanging()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	KillTimers();
	m_bActive = false;
	m_LastModified.Forget();
}


void MyDialogBox::OnTabSelectionChanged()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	m_bActive = true;
	m_LastModified.Recall();
	SetTimer(1000);
	m_menuFile
		.RemoveAll()
		.Add(ResourceString(IDS_MENU_EXIT), IDM_FILE_EXIT);
	m_menuEdit
		.RemoveAll();
	AddEditControlMenus(m_CurrentEdit);
	m_menuSettings
		.RemoveAll();
}


INT_PTR MyDialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	DWORD wControlId = LOWORD(wParam);
	switch (wControlId)
	{
	case IDM_SETTINGS_IN_AUTO:
	case IDM_SETTINGS_IN_UTF8:
	case IDM_SETTINGS_IN_UTF16:
	case IDM_SETTINGS_IN_CP932:
	case IDM_SETTINGS_IN_CP936:
	case IDM_SETTINGS_IN_CP949:
	case IDM_SETTINGS_IN_CP950:
	case IDM_SETTINGS_IN_CP1250:
	case IDM_SETTINGS_IN_CP1251:
	case IDM_SETTINGS_IN_CP1252:
	case IDM_SETTINGS_IN_CP1253:
	case IDM_SETTINGS_IN_CP1254:
	case IDM_SETTINGS_IN_CP1255:
	case IDM_SETTINGS_IN_CP1256:
	case IDM_SETTINGS_IN_CP1257:
	case IDM_SETTINGS_IN_CP1258:
		ApplyToInputCodePage(wControlId);
		return TRUE;
	case IDM_SETTINGS_OUT_UTF8:
	case IDM_SETTINGS_OUT_UTF8BOM:
	case IDM_SETTINGS_OUT_UTF16:
	case IDM_SETTINGS_OUT_UTF16BOM:
	case IDM_SETTINGS_OUT_CP932:
	case IDM_SETTINGS_OUT_CP936:
	case IDM_SETTINGS_OUT_CP949:
	case IDM_SETTINGS_OUT_CP950:
	case IDM_SETTINGS_OUT_CP1250:
	case IDM_SETTINGS_OUT_CP1251:
	case IDM_SETTINGS_OUT_CP1252:
	case IDM_SETTINGS_OUT_CP1253:
	case IDM_SETTINGS_OUT_CP1254:
	case IDM_SETTINGS_OUT_CP1255:
	case IDM_SETTINGS_OUT_CP1256:
	case IDM_SETTINGS_OUT_CP1257:
	case IDM_SETTINGS_OUT_CP1258:
		ApplyToOutputCodePage(wControlId);
		return TRUE;
	default:
		return FALSE;
	}
}


INT_PTR MyDialogBox::OnTimer(WPARAM wParam, LPARAM lParam)
{
	//DBGFNC(L"MyDialogBox::OnTimer(%zu,%zu): Id=%u CurrentEdit=%u", wParam, lParam, Id, m_CurrentEdit);
	if (wParam == TIMERID(Id, 1000))
	{
		UpdateEditControlMenus(m_CurrentEdit);
	}
	return 0;
}


INT_PTR MyDialogBox::OnControlColorStatic(WPARAM wParam, LPARAM lParam)
{
	HDC hdc = reinterpret_cast<HDC>(wParam);
	int id = GetDlgCtrlID(reinterpret_cast<HWND>(lParam));
	if (id == m_StatusId)
	{
		SetTextColor(hdc,
			(m_dwFlags & FLAG_STATUS_ERROR) ? RGB_ERROR :
			(m_dwFlags & FLAG_STATUS_SUCCESSFUL) ? RGB_SUCCESSFUL :
			GetSysColor(COLOR_WINDOWTEXT));
		SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
		return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_3DFACE));
	}
	return 0;
}


INT_PTR MyDialogBox::OnControlColorEdit(WPARAM wParam, LPARAM lParam)
{
	HDC hdc = reinterpret_cast<HDC>(wParam);
	int id = GetDlgCtrlID(reinterpret_cast<HWND>(lParam));
	if (id == m_Pane1Id)
	{
		SetTextColor(hdc,
			(m_dwFlags & FLAG_PANE1_ERROR) ? RGB_ERROR :
			(m_dwFlags & FLAG_PANE1_SUCCESSFUL) ? RGB_SUCCESSFUL :
			GetSysColor(COLOR_WINDOWTEXT));
		SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
		return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_WINDOW));
	}
	else if (id == m_Pane2Id)
	{
		SetTextColor(hdc,
			(m_dwFlags & FLAG_PANE2_ERROR) ? RGB_ERROR :
			(m_dwFlags & FLAG_PANE2_SUCCESSFUL) ? RGB_SUCCESSFUL :
			GetSysColor(COLOR_WINDOWTEXT));
		SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
		return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_WINDOW));
	}
	return 0;
}


INT_PTR MyDialogBox::OnEditCommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	UINT idChild = LOWORD(wParam);
	UINT idNotif = HIWORD(wParam);
	switch (idNotif)
	{
	case EN_SETFOCUS:
		OnEditSetFocus(idChild);
		return TRUE;
	case EN_KILLFOCUS:
		OnEditKillFocus(idChild);
		return TRUE;
	case EN_CHANGE:
		OnEditChanged(idChild);
		return TRUE;
	default:
		return FALSE;
	}
}


void MyDialogBox::OnEditSetFocus(int id)
{
	DBGFNC(L"MyDialogBox::OnEditSetFocus(%d)", id);
	if (m_CurrentEdit != id)
	{
		m_CurrentEdit = id;
		UpdateEditControlMenus(m_CurrentEdit);
	}
}


void MyDialogBox::OnEditKillFocus(int id)
{
	DBGFNC(L"MyDialogBox::OnEditKillFocus(%d)", id);
	if (m_CurrentEdit != 0)
	{
		m_CurrentEdit = 0;
		UpdateEditControlMenus(0);
	}
}


void MyDialogBox::OnEditChanged(int id)
{
	DBGFNC(L"MyDialogBox::OnEditChanged(%d)", id);
	if (m_CurrentEdit == id)
	{
		UpdateEditControlMenus(m_CurrentEdit);
	}
	m_LastModified.By = id;
	UpdateControlsState(id);
}


DWORD MyDialogBox::SetStatus(DWORD dwSet, DWORD dwReset, PCWSTR pszFormat, ...)
{
	va_list argList;
	va_start(argList, pszFormat);
	DWORD dwRet = VaSetStatus(dwSet, dwReset, pszFormat, argList);
	va_end(argList);
	return dwRet;
}


DWORD MyDialogBox::VaSetStatus(DWORD dwSet, DWORD dwReset, PCWSTR pszFormat, va_list argList)
{
	if (m_StatusId)
	{
		String sz;
		sz.VaFormat(pszFormat, argList);
		SetText(m_StatusId, sz);
	}
	return SetFlags(dwSet, dwReset);
}


DWORD MyDialogBox::SetStatus(DWORD dwSet, DWORD dwReset, const SYSTEMTIME& st, PCWSTR pszFormat, ...)
{
	va_list argList;
	va_start(argList, pszFormat);
	DWORD dwRet = VaSetStatus(dwSet, dwReset, st, pszFormat, argList);
	va_end(argList);
	return dwRet;
}


DWORD MyDialogBox::VaSetStatus(DWORD dwSet, DWORD dwReset, const SYSTEMTIME& st, PCWSTR pszFormat, va_list argList)
{
	if (m_StatusId)
	{
		String sz = String::Format(L"%04d-%02d-%02d %02d:%02d:%02d  ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		sz.VaAppendFormat(pszFormat, argList);
		SetText(m_StatusId, sz);
	}
	return SetFlags(dwSet, dwReset);
}


DWORD MyDialogBox::SetFlags(DWORD dwSet, DWORD dwReset)
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
	DWORD dwChanged = (dwPrev ^ m_dwFlags) & (MASK_STATUS | MASK_PANE1 | MASK_PANE2);
	if (m_StatusId && (dwChanged & MASK_STATUS))
	{
		InvalidateRect(m_StatusId, NULL, TRUE);
	}
	else
	{
		dwChanged &= ~MASK_STATUS;
	}
	if (m_Pane1Id && (dwChanged & MASK_PANE1))
	{
		InvalidateRect(m_Pane1Id, NULL, TRUE);
	}
	else
	{
		dwChanged &= ~MASK_PANE1;
	}
	if (m_Pane2Id && (dwChanged & MASK_PANE2))
	{
		InvalidateRect(m_Pane2Id, NULL, TRUE);
	}
	else
	{
		dwChanged &= ~MASK_PANE2;
	}
	if ((dwSet & FLAG_BUSY))
	{
		UpdateControlsState(0);
		dwChanged |= FLAG_BUSY;
	}
	else if ((dwPrev & FLAG_BUSY))
	{
		m_dwFlags &= ~FLAG_BUSY;
		UpdateControlsState(1);
		dwChanged |= FLAG_BUSY;
	}
	if (dwChanged)
	{
		ProcessMessages();
	}
	return dwPrev;
}


void MyDialogBox::UpdateControlsState(int id)
{
	UNREFERENCED_PARAMETER(id);
}


void MyDialogBox::SetTimer(UINT uElapse)
{
	if (!uElapse)
	{
		throw Exception(L"MyDialogBox::SetTimer: Zero elapse.");
	}
	size_t indexUnused = _countof(m_timers);
	for (size_t index = 0; index < _countof(m_timers); index++)
	{
		if (m_timers[index] == uElapse)
		{
			// Already registered
			return;
		}
		else if (!m_timers[index] && indexUnused > index)
		{
			indexUnused = index;
		}
	}
	if (indexUnused < _countof(m_timers))
	{
		::SetTimer(hwnd, TIMERID(Id, uElapse), uElapse, NULL);
		m_timers[indexUnused] = uElapse;
	}
	else
	{
		throw Exception(L"MyDialogBox::SetTimer: Too many timers.");
	}
}


void MyDialogBox::KillTimers()
{
	for (size_t index = 0; index < _countof(m_timers); index++)
	{
		UINT uElapse = _InterlockedExchange(&m_timers[index], 0);
		if (uElapse)
		{
			::KillTimer(hwnd, TIMERID(Id, uElapse));
		}
	}
}


void MyDialogBox::AddEditControlMenus(int id)
{
	m_menuEdit
		.Add(ResourceString(IDS_MENU_CUT), IDM_EDIT_CUT)
		.Add(ResourceString(IDS_MENU_COPY), IDM_EDIT_COPY)
		.Add(ResourceString(IDS_MENU_PASTE), IDM_EDIT_PASTE)
		.Add(ResourceString(IDS_MENU_DELETE), IDM_EDIT_DELETE)
		.AddSeparator()
		.Add(ResourceString(IDS_MENU_SELECTALL), IDM_EDIT_SELECTALL)
		.AddSeparator()
		.Add(ResourceString(IDS_MENU_CLEAR), IDM_EDIT_CLEAR);
	UpdateEditControlMenus(id);
}


void MyDialogBox::UpdateEditControlMenus(int id)
{
	//DBGFNC(L"MyDialogBox::UpdateEditControlMenus(%d)", id);
	UINT fCut, fCopy, fPaste, fDelete, fSelectAll, fClear;
	if (id)
	{
		BOOL bEditable = !EditGetReadOnly(id);
		int nContent = GetTextLength(id);
		int nSelection = EditGetSelectionLength(id);
		fCut = bEditable && nSelection ? MF_ENABLED : MF_DISABLED;
		fCopy = nSelection ? MF_ENABLED : MF_DISABLED;
		fPaste = bEditable ? MF_ENABLED : MF_DISABLED;
		fDelete = bEditable && nSelection ? MF_ENABLED : MF_DISABLED;
		fSelectAll = nContent > 0 ? MF_ENABLED : MF_DISABLED;
		fClear = bEditable && nContent > 0 ? MF_ENABLED : MF_DISABLED;
	}
	else
	{
		fCut = MF_DISABLED;
		fCopy = MF_DISABLED;
		fPaste = MF_DISABLED;
		fDelete = MF_DISABLED;
		fSelectAll = MF_DISABLED;
		fClear = MF_DISABLED;
	}
	m_menuEdit
		.Enable(IDM_EDIT_CUT, fCut)
		.Enable(IDM_EDIT_COPY, fCopy)
		.Enable(IDM_EDIT_PASTE, fPaste)
		.Enable(IDM_EDIT_DELETE, fDelete)
		.Enable(IDM_EDIT_SELECTALL, fSelectAll)
		.Enable(IDM_EDIT_CLEAR, fClear);
}


void MyDialogBox::OnCut()
{
	DBGFNC(L"MyDialogBox::OnCut: m_CurrentEdit=%d", m_CurrentEdit);
	if (m_CurrentEdit)
	{
		EditCut(m_CurrentEdit);
		if (EditIsMultiLine(m_CurrentEdit))
		{
			OnEditChanged(m_CurrentEdit);
		}
	}
}


void MyDialogBox::OnCopy()
{
	DBGFNC(L"MyDialogBox::OnCopy: m_CurrentEdit=%d", m_CurrentEdit);
	if (m_CurrentEdit)
	{
		EditCopy(m_CurrentEdit);
	}
}


void MyDialogBox::OnPaste()
{
	DBGFNC(L"MyDialogBox::OnPaste: m_CurrentEdit=%d", m_CurrentEdit);
	if (m_CurrentEdit)
	{
		EditPaste(m_CurrentEdit);
		if (EditIsMultiLine(m_CurrentEdit))
		{
			OnEditChanged(m_CurrentEdit);
		}
	}
}


void MyDialogBox::OnDelete()
{
	DBGFNC(L"MyDialogBox::OnDelete: m_CurrentEdit=%d", m_CurrentEdit);
	if (m_CurrentEdit)
	{
		EditDelete(m_CurrentEdit);
		if (EditIsMultiLine(m_CurrentEdit))
		{
			OnEditChanged(m_CurrentEdit);
		}
	}
}


void MyDialogBox::OnSelectAll()
{
	DBGFNC(L"MyDialogBox::OnSelectAll: m_CurrentEdit=%d", m_CurrentEdit);
	if (m_CurrentEdit)
	{
		EditSelectAll(m_CurrentEdit);
	}
}


void MyDialogBox::OnCopyAll()
{
	DBGFNC(L"MyDialogBox::OnCopyAll: m_CurrentEdit=%d", m_CurrentEdit);
	if (m_CurrentEdit)
	{
		EditSelectAll(m_CurrentEdit);
		EditCopy(m_CurrentEdit);
	}
}


void MyDialogBox::OnClear()
{
	DBGFNC(L"MyDialogBox::OnClear: m_CurrentEdit=%d", m_CurrentEdit);
	if (m_CurrentEdit)
	{
		EditClear(m_CurrentEdit);
		if (EditIsMultiLine(m_CurrentEdit))
		{
			OnEditChanged(m_CurrentEdit);
		}
	}
}


static UINT GetInputCodePageMenuCommand(UINT uCodePage)
{
	switch (uCodePage)
	{
	case CP_AUTODETECT: return IDM_SETTINGS_IN_AUTO;
	case CP_UTF8: return IDM_SETTINGS_IN_UTF8;
	case CP_UTF16: return IDM_SETTINGS_IN_UTF16;
	case 932: return IDM_SETTINGS_IN_CP932;
	case 936: return IDM_SETTINGS_IN_CP936;
	case 949: return IDM_SETTINGS_IN_CP949;
	case 950: return IDM_SETTINGS_IN_CP950;
	case 1250: return IDM_SETTINGS_IN_CP1250;
	case 1251: return IDM_SETTINGS_IN_CP1251;
	case 1252: return IDM_SETTINGS_IN_CP1252;
	case 1253: return IDM_SETTINGS_IN_CP1253;
	case 1254: return IDM_SETTINGS_IN_CP1254;
	case 1255: return IDM_SETTINGS_IN_CP1255;
	case 1256: return IDM_SETTINGS_IN_CP1256;
	case 1257: return IDM_SETTINGS_IN_CP1257;
	case 1258: return IDM_SETTINGS_IN_CP1258;
	default: return IDM_SETTINGS_IN_AUTO;
	}
}


static UINT GetOutputCodePageMenuCommand(UINT uCodePage, bool bBOM)
{
	switch (uCodePage)
	{
	case CP_UTF8: return bBOM ? IDM_SETTINGS_OUT_UTF8BOM : IDM_SETTINGS_OUT_UTF8;
	case CP_UTF16: return bBOM ? IDM_SETTINGS_OUT_UTF16BOM : IDM_SETTINGS_OUT_UTF16;
	case 932: return IDM_SETTINGS_OUT_CP932;
	case 936: return IDM_SETTINGS_OUT_CP936;
	case 949: return IDM_SETTINGS_OUT_CP949;
	case 950: return IDM_SETTINGS_OUT_CP950;
	case 1250: return IDM_SETTINGS_OUT_CP1250;
	case 1251: return IDM_SETTINGS_OUT_CP1251;
	case 1252: return IDM_SETTINGS_OUT_CP1252;
	case 1253: return IDM_SETTINGS_OUT_CP1253;
	case 1254: return IDM_SETTINGS_OUT_CP1254;
	case 1255: return IDM_SETTINGS_OUT_CP1255;
	case 1256: return IDM_SETTINGS_OUT_CP1256;
	case 1257: return IDM_SETTINGS_OUT_CP1257;
	case 1258: return IDM_SETTINGS_OUT_CP1258;
	default: return IDM_SETTINGS_OUT_UTF8;
	}
}


static PCWSTR GetCodePageMenuString(UINT uCodePage, bool bBOM = false)
{
	static std::map<UINT, String> cpMap;
	static const String defaultCodePage(ResourceString(IDS_MENU_UTF8));
	if (!cpMap.size())
	{
#define ADD(cp,id) cpMap.insert(std::pair<UINT, String>(cp, String(ResourceString(id))))
		ADD(CP_AUTODETECT, IDS_MENU_AUTODETECT);
		ADD(CP_UTF8, IDS_MENU_UTF8);
		ADD(CP_UTF8 + 0x10000, IDS_MENU_UTF8BOM);
		ADD(CP_UTF16, IDS_MENU_UTF16);
		ADD(CP_UTF16 + 0x10000, IDS_MENU_UTF16BOM);
		ADD(932, IDS_MENU_CP932);
		ADD(936, IDS_MENU_CP936);
		ADD(949, IDS_MENU_CP949);
		ADD(950, IDS_MENU_CP950);
		ADD(1250, IDS_MENU_CP1250);
		ADD(1251, IDS_MENU_CP1251);
		ADD(1252, IDS_MENU_CP1252);
		ADD(1253, IDS_MENU_CP1253);
		ADD(1254, IDS_MENU_CP1254);
		ADD(1255, IDS_MENU_CP1255);
		ADD(1256, IDS_MENU_CP1256);
		ADD(1257, IDS_MENU_CP1257);
		ADD(1258, IDS_MENU_CP1258);
#undef ADD
	}
	if ((uCodePage == CP_UTF8 || uCodePage == CP_UTF16) && bBOM)
	{
		uCodePage += 0x10000;
	}
	std::map<UINT, String>::const_iterator iter = cpMap.find(uCodePage);
	return (iter != cpMap.cend()) ? iter->second : defaultCodePage;
}


void MyDialogBox::AddInputCodePageSettingMenus()
{
	String szInputEncoding(ResourceString(IDS_MENU_IENCODING));
	m_menuSettings
		.Add(szInputEncoding,
			Menu()
			.Add(GetCodePageMenuString(CP_AUTODETECT), IDM_SETTINGS_IN_AUTO,
				m_uInputCodePage == CP_AUTODETECT ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(CP_UTF8), IDM_SETTINGS_IN_UTF8,
				m_uInputCodePage == CP_UTF8 ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(CP_UTF16), IDM_SETTINGS_IN_UTF16,
				m_uInputCodePage == CP_UTF16 ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(932), IDM_SETTINGS_IN_CP932,
				m_uInputCodePage == 932 ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(936), IDM_SETTINGS_IN_CP936,
				m_uInputCodePage == 936 ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(949), IDM_SETTINGS_IN_CP949,
				m_uInputCodePage == 949 ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(950), IDM_SETTINGS_IN_CP950,
				m_uInputCodePage == 950 ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(1250), IDM_SETTINGS_IN_CP1250,
				m_uInputCodePage == 1250 ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(1251), IDM_SETTINGS_IN_CP1251,
				m_uInputCodePage == 1251 ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(1252), IDM_SETTINGS_IN_CP1252,
				m_uInputCodePage == 1252 ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(1253), IDM_SETTINGS_IN_CP1253,
				m_uInputCodePage == 1253 ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(1254), IDM_SETTINGS_IN_CP1254,
				m_uInputCodePage == 1254 ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(1255), IDM_SETTINGS_IN_CP1255,
				m_uInputCodePage == 1255 ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(1256), IDM_SETTINGS_IN_CP1256,
				m_uInputCodePage == 1256 ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(1257), IDM_SETTINGS_IN_CP1257,
				m_uInputCodePage == 1257 ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(1258), IDM_SETTINGS_IN_CP1258,
				m_uInputCodePage == 1258 ? MF_CHECKED : MF_UNCHECKED));
}


void MyDialogBox::AddOutputCodePageSettingMenus()
{
	String szOutputEncoding(ResourceString(IDS_MENU_OENCODING));
	m_menuSettings
		.Add(szOutputEncoding,
			Menu()
			.Add(GetCodePageMenuString(CP_UTF8), IDM_SETTINGS_OUT_UTF8,
				m_uOutputCodePage == CP_UTF8 && m_bOutputBOM == false ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(CP_UTF8, true), IDM_SETTINGS_OUT_UTF8BOM,
				m_uOutputCodePage == CP_UTF8 && m_bOutputBOM == true ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(CP_UTF16), IDM_SETTINGS_OUT_UTF16,
				m_uOutputCodePage == CP_UTF16 && m_bOutputBOM == false ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(CP_UTF16, true), IDM_SETTINGS_OUT_UTF16BOM,
				m_uOutputCodePage == CP_UTF16 && m_bOutputBOM == true ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(932), IDM_SETTINGS_OUT_CP932,
				m_uOutputCodePage == 932 ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(936), IDM_SETTINGS_OUT_CP936,
				m_uOutputCodePage == 936 ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(949), IDM_SETTINGS_OUT_CP949,
				m_uOutputCodePage == 949 ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(950), IDM_SETTINGS_OUT_CP950,
				m_uOutputCodePage == 950 ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(1250), IDM_SETTINGS_OUT_CP1250,
				m_uOutputCodePage == 1250 ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(1251), IDM_SETTINGS_OUT_CP1251,
				m_uOutputCodePage == 1251 ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(1252), IDM_SETTINGS_OUT_CP1252,
				m_uOutputCodePage == 1252 ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(1253), IDM_SETTINGS_OUT_CP1253,
				m_uOutputCodePage == 1253 ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(1254), IDM_SETTINGS_OUT_CP1254,
				m_uOutputCodePage == 1254 ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(1255), IDM_SETTINGS_OUT_CP1255,
				m_uOutputCodePage == 1255 ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(1256), IDM_SETTINGS_OUT_CP1256,
				m_uOutputCodePage == 1256 ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(1257), IDM_SETTINGS_OUT_CP1257,
				m_uOutputCodePage == 1257 ? MF_CHECKED : MF_UNCHECKED)
			.Add(GetCodePageMenuString(1258), IDM_SETTINGS_OUT_CP1258,
				m_uOutputCodePage == 1258 ? MF_CHECKED : MF_UNCHECKED));
}


bool MyDialogBox::ApplyToInputCodePage(UINT uId)
{
	UINT uInputCodePage = m_uInputCodePage;
	switch (uId)
	{
	case IDM_SETTINGS_IN_AUTO:
		m_uInputCodePage = CP_AUTODETECT;
		break;
	case IDM_SETTINGS_IN_UTF8:
		m_uInputCodePage = CP_UTF8;
		break;
	case IDM_SETTINGS_IN_UTF16:
		m_uInputCodePage = CP_UTF16;
		break;
	case IDM_SETTINGS_IN_CP932:
		m_uInputCodePage = 932;
		break;
	case IDM_SETTINGS_IN_CP936:
		m_uInputCodePage = 936;
		break;
	case IDM_SETTINGS_IN_CP949:
		m_uInputCodePage = 949;
		break;
	case IDM_SETTINGS_IN_CP950:
		m_uInputCodePage = 950;
		break;
	case IDM_SETTINGS_IN_CP1250:
		m_uInputCodePage = 1250;
		break;
	case IDM_SETTINGS_IN_CP1251:
		m_uInputCodePage = 1251;
		break;
	case IDM_SETTINGS_IN_CP1252:
		m_uInputCodePage = 1252;
		break;
	case IDM_SETTINGS_IN_CP1253:
		m_uInputCodePage = 1253;
		break;
	case IDM_SETTINGS_IN_CP1254:
		m_uInputCodePage = 1254;
		break;
	case IDM_SETTINGS_IN_CP1255:
		m_uInputCodePage = 1255;
		break;
	case IDM_SETTINGS_IN_CP1256:
		m_uInputCodePage = 1256;
		break;
	case IDM_SETTINGS_IN_CP1257:
		m_uInputCodePage = 1257;
		break;
	case IDM_SETTINGS_IN_CP1258:
		m_uInputCodePage = 1258;
		break;
	default:
		return false;
	}
	if (m_uInputCodePage != uInputCodePage)
	{
		m_menuSettings[(PCWSTR)ResourceString(IDS_MENU_IENCODING)]
			.Modify(
				GetInputCodePageMenuCommand(uInputCodePage), MF_UNCHECKED,
				GetInputCodePageMenuCommand(uInputCodePage), GetCodePageMenuString(uInputCodePage))
			.Modify(
				GetInputCodePageMenuCommand(m_uInputCodePage), MF_CHECKED,
				GetInputCodePageMenuCommand(m_uInputCodePage), GetCodePageMenuString(m_uInputCodePage));
	}
	return true;
}


bool MyDialogBox::ApplyToOutputCodePage(UINT uId)
{
	UINT uOutputCodePage = m_uOutputCodePage;
	bool bOutputBOM = m_bOutputBOM;
	switch (uId)
	{
	case IDM_SETTINGS_OUT_UTF8:
		m_uOutputCodePage = CP_UTF8;
		m_bOutputBOM = false;
		break;
	case IDM_SETTINGS_OUT_UTF8BOM:
		m_uOutputCodePage = CP_UTF8;
		m_bOutputBOM = true;
		break;
	case IDM_SETTINGS_OUT_UTF16:
		m_uOutputCodePage = CP_UTF16;
		m_bOutputBOM = false;
		break;
	case IDM_SETTINGS_OUT_UTF16BOM:
		m_uOutputCodePage = CP_UTF16;
		m_bOutputBOM = true;
		break;
	case IDM_SETTINGS_OUT_CP932:
		m_uOutputCodePage = 932;
		break;
	case IDM_SETTINGS_OUT_CP936:
		m_uOutputCodePage = 936;
		break;
	case IDM_SETTINGS_OUT_CP949:
		m_uOutputCodePage = 949;
		break;
	case IDM_SETTINGS_OUT_CP950:
		m_uOutputCodePage = 950;
		break;
	case IDM_SETTINGS_OUT_CP1250:
		m_uOutputCodePage = 1250;
		break;
	case IDM_SETTINGS_OUT_CP1251:
		m_uOutputCodePage = 1251;
		break;
	case IDM_SETTINGS_OUT_CP1252:
		m_uOutputCodePage = 1252;
		break;
	case IDM_SETTINGS_OUT_CP1253:
		m_uOutputCodePage = 1253;
		break;
	case IDM_SETTINGS_OUT_CP1254:
		m_uOutputCodePage = 1254;
		break;
	case IDM_SETTINGS_OUT_CP1255:
		m_uOutputCodePage = 1255;
		break;
	case IDM_SETTINGS_OUT_CP1256:
		m_uOutputCodePage = 1256;
		break;
	case IDM_SETTINGS_OUT_CP1257:
		m_uOutputCodePage = 1257;
		break;
	case IDM_SETTINGS_OUT_CP1258:
		m_uOutputCodePage = 1258;
		break;
	default:
		return false;
	}
	if (m_uOutputCodePage != uOutputCodePage || m_bOutputBOM != bOutputBOM)
	{
		m_menuSettings[(PCWSTR)ResourceString(IDS_MENU_OENCODING)]
			.Modify(
				GetOutputCodePageMenuCommand(uOutputCodePage, bOutputBOM), MF_UNCHECKED,
				GetOutputCodePageMenuCommand(uOutputCodePage, bOutputBOM), GetCodePageMenuString(uOutputCodePage, bOutputBOM))
			.Modify(
				GetOutputCodePageMenuCommand(m_uOutputCodePage, m_bOutputBOM), MF_CHECKED,
				GetOutputCodePageMenuCommand(m_uOutputCodePage, m_bOutputBOM), GetCodePageMenuString(m_uOutputCodePage, m_bOutputBOM));
	}
	return true;
}


void MyDialogBox::AddLettercaseSettingMenus(UINT uId)
{
	String szFormat(ResourceString(IDS_MENU_FORMAT));
	String szUppercase(ResourceString(IDS_MENU_UPPERCASE));
	String szLowercase(ResourceString(IDS_MENU_LOWERCASE));
	m_menuSettings
		.Add(szFormat, Menu()
			.Add(szUppercase, IDM_SETTINGS_UPPERCASE, uId == IDM_SETTINGS_UPPERCASE ? MF_CHECKED : MF_UNCHECKED)
			.Add(szLowercase, IDM_SETTINGS_LOWERCASE, uId == IDM_SETTINGS_LOWERCASE ? MF_CHECKED : MF_UNCHECKED));
}


bool MyDialogBox::ApplyToLettercase(UINT uId, StringOptions& uValue)
{
	switch (uId)
	{
	case IDM_SETTINGS_UPPERCASE:
		uValue = StringOptions::UPPERCASE;
		break;
	case IDM_SETTINGS_LOWERCASE:
		uValue = StringOptions::LOWERCASE;
		break;
	default:
		return false;
	}
	String szFormat(ResourceString(IDS_MENU_FORMAT));
	String szUppercase(ResourceString(IDS_MENU_UPPERCASE));
	String szLowercase(ResourceString(IDS_MENU_LOWERCASE));
	m_menuSettings[szFormat.Ptr]
		.Modify(
			IDM_SETTINGS_UPPERCASE, uId == IDM_SETTINGS_UPPERCASE ? MF_CHECKED : MF_UNCHECKED,
			IDM_SETTINGS_UPPERCASE, szUppercase)
		.Modify(
			IDM_SETTINGS_LOWERCASE, uId == IDM_SETTINGS_LOWERCASE ? MF_CHECKED : MF_UNCHECKED,
			IDM_SETTINGS_LOWERCASE, szLowercase);
	return true;
}


bool MyDialogBox::LoadTextFromFile(int id, PCWSTR pszDoingWhat, String& szPath)
{
	String szTitle(ResourceString(IDS_LOADTEXTFROMFILE));
	StringBuffer szPath2(MAX_PATH, szPath);
	OPENFILENAMEW ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szPath2;
	ofn.nMaxFile = static_cast<DWORD>(szPath2.Cap);
	ofn.lpstrTitle = szTitle;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (!GetOpenFileNameW(&ofn))
	{
		return false;
	}
	try
	{
		SetStatus(FLAG_BUSY, MASK_STATUS, pszDoingWhat);
		FileMapper fm(ofn.lpstrFile);
		if (fm.Len > INT_MAX - 1)
		{
			throw Exception(ResourceString(IDS_MSG_TOO_LARGE_FILE));
		}
		String sz = ByteString(fm.Ptr, fm.Len).ToString(m_uInputCodePage);
		bool bBOM = sz.Len && sz[0] == BYTE_ORDER_MARK;
		if (bBOM)
		{
			SetText(id, &sz[1]);
		}
		else
		{
			SetText(id, sz);
		}
		SetStatus(FLAG_STATUS_SUCCESSFUL,
			id == m_Pane1Id ? MASK_PANE1 :
			id == m_Pane2Id ? MASK_PANE2 : 0,
			ResourceString(IDS_W_DONE_X_Y_IN_Z),
			pszDoingWhat, NumberOfBytes(fm.Len), NumberOfChars(sz.Len), bBOM ? ResourceString(IDS_BOM_REMOVED) : L"");
		szPath = szPath2;
	}
	catch (Win32Exception e)
	{
		SetStatus(FLAG_STATUS_ERROR, 0, ResourceString(IDS_W_FAILED_X_Y), pszDoingWhat, e.Message, ErrorMessage::Get(e.Error));
	}
	catch (Exception e)
	{
		SetStatus(FLAG_STATUS_ERROR, 0, ResourceString(IDS_W_FAILED_X), pszDoingWhat, e.Message);
	}
	return true;
}


bool MyDialogBox::SaveTextAsFile(int id, PCWSTR pszDoingWhat, String& szPath)
{
	String szTitle(ResourceString(IDS_SAVETEXTASFILE));
	StringBuffer szPath2(MAX_PATH, szPath);
	OPENFILENAMEW ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szPath2;
	ofn.nMaxFile = static_cast<DWORD>(szPath2.Cap);
	ofn.lpstrTitle = szTitle;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
	if (!GetSaveFileNameW(&ofn))
	{
		return false;
	}
	try
	{
		SetStatus(FLAG_BUSY, MASK_STATUS, pszDoingWhat);
		String wcs = GetText(id);
		bool bBOM = (m_uOutputCodePage == CP_UTF8 || m_uOutputCodePage == CP_UTF16) && m_bOutputBOM;
		if (bBOM)
		{
			wcs = String::Format(L"%c%s", BYTE_ORDER_MARK, wcs);
		}
		ByteString serialized = ByteString::FromString(wcs, m_uOutputCodePage, m_OutputLineBreak);
		FileWriter(ofn.lpstrFile).Write(serialized.Ptr, serialized.Len);
		SetStatus(FLAG_STATUS_SUCCESSFUL, 0,
			ResourceString(IDS_W_DONE_X_Y_OUT_Z),
			pszDoingWhat, NumberOfBytes(serialized.Len), NumberOfChars(wcs.Len), bBOM ? ResourceString(IDS_BOM_PREPENDED) : L"");
		szPath = szPath2;
	}
	catch (Win32Exception e)
	{
		SetStatus(FLAG_STATUS_ERROR, 0, ResourceString(IDS_W_FAILED_X_Y), pszDoingWhat, e.Message, ErrorMessage::Get(e.Error));
	}
	catch (Exception e)
	{
		SetStatus(FLAG_STATUS_ERROR, 0, ResourceString(IDS_W_FAILED_X), pszDoingWhat, e.Message);
	}
	return true;
}


void MyDialogBox::InitializeCodePageComboBox(int id, int initialSelection) const
{
#define ADD(x,y) ComboBoxAdd(id,x,y)
#define ADDCP(x) ADD(L"CP" L#x,x)
	ADD(L"UTF-8", CP_UTF8);
	ADD(L"UTF-16", CP_UTF16);
	ADD(L"EUC-JP", CP_EUCJP);
	ADDCP(932);
	ADDCP(936);
	ADDCP(949);
	ADDCP(950);
	ADDCP(1250);
	ADDCP(1251);
	ADDCP(1252);
	ADDCP(1253);
	ADDCP(1254);
	ADDCP(1255);
	ADDCP(1256);
	ADDCP(1257);
	ADDCP(1258);
#undef ADD
#undef ADDCP
	ComboBoxSetSelection(id, initialSelection);
}


void MyDialogBox::InitializeLineBreakComboBox(int id, LineBreak initialSelection) const
{
	ComboBoxAdd(id, L"CRLF", static_cast<int>(LineBreak::CRLF));
	ComboBoxAdd(id, L"LF", static_cast<int>(LineBreak::LF));
	ComboBoxSetSelection(id, static_cast<int>(initialSelection));
}


void MyDialogBox::InitializeLetterCaseComboBox(int id, StringOptions initialSelection) const
{
	ComboBoxAdd(id, L"Uppercase", static_cast<int>(StringOptions::UPPERCASE));
	ComboBoxAdd(id, L"Lowercase", static_cast<int>(StringOptions::LOWERCASE));
	ComboBoxSetSelection(id, static_cast<int>(initialSelection));
}


// from UTC-12:00 (Baker Island/Howland Island) through UTC+14:00 (Kiribati)
void MyDialogBox::InitializeOffsetComboBox(int id, int initialSelection) const
{
	static const struct {
		PCWSTR psz;
		int offset;
	} data[] = {
#define OFFSET(h,m) ((h)*60+(m))
		{ L"-12:00", -OFFSET(12, 0) },
		{ L"-11:00", -OFFSET(11, 0) },
		{ L"-10:00", -OFFSET(10, 0) },
		{ L"-09:30", -OFFSET(9, 30) },
		{ L"-09:00", -OFFSET(9, 0) },
		{ L"-08:00", -OFFSET(8, 0) },
		{ L"-07:00", -OFFSET(7, 0) },
		{ L"-06:00", -OFFSET(6, 0) },
		{ L"-05:00", -OFFSET(5, 0) },
		{ L"-04:30", -OFFSET(4, 30) },
		{ L"-04:00", -OFFSET(4, 0) },
		{ L"-03:30", -OFFSET(3, 30) },
		{ L"-03:00", -OFFSET(3, 0) },
		{ L"-02:00", -OFFSET(2, 0) },
		{ L"-01:00", -OFFSET(1, 0) },
		{ L"+00:00", OFFSET(0, 0) },
		{ L"+01:00", OFFSET(1, 0) },
		{ L"+02:00", OFFSET(2, 0) },
		{ L"+03:00", OFFSET(3, 0) },
		{ L"+03:30", OFFSET(3, 30) },
		{ L"+04:00", OFFSET(4, 0) },
		{ L"+04:30", OFFSET(4, 30) },
		{ L"+05:00", OFFSET(5, 0) },
		{ L"+05:30", OFFSET(5, 30) },
		{ L"+05:45", OFFSET(5, 45) },
		{ L"+06:00", OFFSET(6, 0) },
		{ L"+06:30", OFFSET(6, 30) },
		{ L"+07:00", OFFSET(7, 0) },
		{ L"+08:00", OFFSET(8, 0) },
		{ L"+08:30", OFFSET(8, 30) },
		{ L"+08:45", OFFSET(8, 45) },
		{ L"+09:00", OFFSET(9, 0) },
		{ L"+09:30", OFFSET(9, 30) },
		{ L"+10:00", OFFSET(10, 0) },
		{ L"+10:30", OFFSET(10, 30) },
		{ L"+11:00", OFFSET(11, 0) },
		{ L"+11:45", OFFSET(11, 45) },
		{ L"+12:00", OFFSET(12, 0) },
		{ L"+12:45", OFFSET(12, 45) },
		{ L"+13:00", OFFSET(13, 0) },
		{ L"+14:00", OFFSET(14, 0) }
#undef OFFSET
	};
	for (int i = 0; i < _countof(data); i++)
	{
		ComboBoxAdd(id, data[i].psz, data[i].offset);
	}
	ComboBoxSetSelection(id, initialSelection);
}


void MyDialogBox::SetLengthText(int id, int expected, int actual) const
{
	String sz = String::Format(ResourceString(IDS_LENGTH_FORMAT), expected);
	if (actual > 0)
	{
		if (actual < expected)
		{
			sz.AppendFormat(L" [-%d]", expected - actual);
		}
		else if (actual > expected)
		{
			sz.AppendFormat(L" [+%d]", actual - expected);
		}
	}
	SetText(id, sz);
}


void MyDialogBox::FilterText(int id, BOOL(*pfnIsValid)(WCHAR))
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	int start = 0;
	int end = 0;
	EditGetSelection(id, start, end);
	String sz = GetText(id);
	Buffer<WCHAR> buf(sz.Len + 1);
	const WCHAR* pQ = &sz[0];
	const WCHAR* pR = pQ;
	WCHAR* pW = &buf[0];
	while (*pR)
	{
		if ((*pfnIsValid)(*pR))
		{
			*pW++ = *pR++;
		}
		else
		{
			int cur = static_cast<int>(pR - pQ);
			if (cur <= start)
			{
				start--;
				end--;
			}
			else if (cur <= end)
			{
				end--;
			}
			pQ++;
			pR++;
		}
	}
	if (&sz[0] < pQ)
	{
		*pW = L'\0';
		SetText(id, &buf[0]);
		EditSetSelection(id, start, end);
	}
}


String MyDialogBox::NumberOfBytes(SIZE_T n)
{
	return String::Format(L"%s %s", NumberText(n).Ptr, n > 1ULL ? L"bytes" : L"byte");
}


String MyDialogBox::NumberOfBits(SIZE_T n)
{
	return String::Format(L"%s %s", NumberText(n).Ptr, n > 1ULL ? L"bits" : L"bit");
}


String MyDialogBox::NumberOfChars(SIZE_T n)
{
	return String::Format(L"%s %s", NumberText(n).Ptr, n > 1ULL ? L"chars" : L"char");
}
