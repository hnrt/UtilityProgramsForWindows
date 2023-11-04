#include "pch.h"
#include "MyDialogBox.h"
#include "MyToolbox.h"
#include "resource.h"
#include "hnrt/Menu.h"
#include "hnrt/ResourceString.h"
#include "hnrt/String.h"
#include "hnrt/FileMapper.h"
#include "hnrt/FileWriter.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/Buffer.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/Unicode.h"


using namespace hnrt;


MyDialogBox::MyDialogBox(UINT idTemplate)
	: DialogBox(idTemplate)
	, m_id(-1)
	, m_bActive(false)
	, m_menuFile(HMENU_NULL)
	, m_menuEdit(HMENU_NULL)
	, m_menuView(HMENU_NULL)
	, m_menuSettings(HMENU_NULL)
	, m_menuHelp(HMENU_NULL)
	, m_uInputCodePage(0)
	, m_uOutputCodePage(CP_UTF8)
	, m_bOutputBOM(false)
{
}


void MyDialogBox::OnCreate()
{
	Menu menuBar(GetApp<MyToolbox>().hwnd);
	m_menuFile.Set(menuBar[(size_t)0]);
	m_menuEdit.Set(menuBar[1]);
	m_menuView.Set(menuBar[2]);
	m_menuSettings.Set(menuBar[3]);
	m_menuHelp.Set(menuBar[4]);
}


void MyDialogBox::OnDestroy()
{
}


void MyDialogBox::OnTabSelectionChanging()
{
	m_bActive = false;
	m_menuFile
		.RemoveAll()
		.Add(ResourceString(IDS_EXIT), IDM_FILE_EXIT);
	m_menuEdit
		.RemoveAll();
	m_menuSettings
		.RemoveAll();
}


void MyDialogBox::OnTabSelectionChanged()
{
	m_bActive = true;
}


void MyDialogBox::AddInputCodePageSettingMenus()
{
	m_menuSettings
		.Add(ResourceString(IDS_INPUTCODEPAGE),
			Menu()
			.Add(ResourceString(IDS_AUTODETECT), IDM_SETTINGS_ICP_AUTO, m_uInputCodePage == CP_AUTODETECT ? MF_CHECKED : MF_UNCHECKED)
			.Add(ResourceString(IDS_INUTF8), IDM_SETTINGS_ICP_UTF8, m_uInputCodePage == CP_UTF8 ? MF_CHECKED : MF_UNCHECKED)
			.Add(ResourceString(IDS_INUTF16), IDM_SETTINGS_ICP_UTF16, m_uInputCodePage == CP_UTF16 ? MF_CHECKED : MF_UNCHECKED)
			.Add(String::Format(ResourceString(IDS_INCPX), GetACP()), IDM_SETTINGS_ICP_ANSI,
				m_uInputCodePage == CP_AUTODETECT || m_uInputCodePage == CP_UTF8 || m_uInputCodePage == CP_UTF16 ? MF_UNCHECKED : MF_CHECKED));
}


void MyDialogBox::AddOutputCodePageSettingMenus()
{
	m_menuSettings
		.Add(ResourceString(IDS_OUTPUTCODEPAGE),
			Menu()
			.Add(ResourceString(IDS_INUTF8), IDM_SETTINGS_OCP_UTF8,
				m_uOutputCodePage == CP_UTF8 && m_bOutputBOM == false ? MF_CHECKED : MF_UNCHECKED)
			.Add(ResourceString(IDS_INUTF8BOM), IDM_SETTINGS_OCP_UTF8BOM,
				m_uOutputCodePage == CP_UTF8 && m_bOutputBOM == true ? MF_CHECKED : MF_UNCHECKED)
			.Add(ResourceString(IDS_INUTF16), IDM_SETTINGS_OCP_UTF16,
				m_uOutputCodePage == CP_UTF16 && m_bOutputBOM == false ? MF_CHECKED : MF_UNCHECKED)
			.Add(ResourceString(IDS_INUTF16BOM), IDM_SETTINGS_OCP_UTF16BOM,
				m_uOutputCodePage == CP_UTF16 && m_bOutputBOM == true ? MF_CHECKED : MF_UNCHECKED)
			.Add(String::Format(ResourceString(IDS_INCPX), GetACP()), IDM_SETTINGS_OCP_ANSI,
				m_uOutputCodePage == CP_UTF8 || m_uOutputCodePage == CP_UTF16 ? MF_UNCHECKED : MF_CHECKED));
}


static UINT GetInputCodePageMenuCommand(UINT uCodePage)
{
	switch (uCodePage)
	{
	case CP_AUTODETECT: return IDM_SETTINGS_ICP_AUTO;
	case CP_UTF8: return IDM_SETTINGS_ICP_UTF8;
	case CP_UTF16: return IDM_SETTINGS_ICP_UTF16;
	default: return IDM_SETTINGS_ICP_ANSI;
	}
}


static UINT GetOutputCodePageMenuCommand(UINT uCodePage, bool bBOM)
{
	switch (uCodePage)
	{
	case CP_UTF8: return bBOM ? IDM_SETTINGS_OCP_UTF8BOM : IDM_SETTINGS_OCP_UTF8;
	case CP_UTF16: return bBOM ? IDM_SETTINGS_OCP_UTF16BOM : IDM_SETTINGS_OCP_UTF16;
	default: return IDM_SETTINGS_OCP_ANSI;
	}
}


static PCWSTR GetCodePageMenuString(UINT uCodePage, bool bBOM = false)
{
	switch (uCodePage)
	{
	case CP_AUTODETECT: return String::Copy(ResourceString(IDS_AUTODETECT));
	case CP_UTF8: return bBOM ? String::Copy(ResourceString(IDS_INUTF8BOM)) : String::Copy(ResourceString(IDS_INUTF8));
	case CP_UTF16: return bBOM ? String::Copy(ResourceString(IDS_INUTF16BOM)) : String::Copy(ResourceString(IDS_INUTF16));
	default: return String::Format(ResourceString(IDS_INCPX), GetACP());
	}
}


bool MyDialogBox::ApplyToInputCodePage(UINT uId)
{
	UINT uInputCodePage = m_uInputCodePage;
	switch (uId)
	{
	case IDM_SETTINGS_ICP_AUTO:
		m_uInputCodePage = CP_AUTODETECT;
		break;
	case IDM_SETTINGS_ICP_UTF8:
		m_uInputCodePage = CP_UTF8;
		break;
	case IDM_SETTINGS_ICP_UTF16:
		m_uInputCodePage = CP_UTF16;
		break;
	case IDM_SETTINGS_ICP_ANSI:
		m_uInputCodePage = GetACP();
		break;
	default:
		return false;
	}
	if (m_uInputCodePage != uInputCodePage)
	{
		m_menuSettings[ResourceString(IDS_INPUTCODEPAGE)].
			Modify(
				GetInputCodePageMenuCommand(uInputCodePage), MF_UNCHECKED,
				GetInputCodePageMenuCommand(uInputCodePage), GetCodePageMenuString(uInputCodePage));
		m_menuSettings[ResourceString(IDS_INPUTCODEPAGE)].
			Modify(
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
	case IDM_SETTINGS_OCP_UTF8:
		m_uOutputCodePage = CP_UTF8;
		m_bOutputBOM = false;
		break;
	case IDM_SETTINGS_OCP_UTF8BOM:
		m_uOutputCodePage = CP_UTF8;
		m_bOutputBOM = true;
		break;
	case IDM_SETTINGS_OCP_UTF16:
		m_uOutputCodePage = CP_UTF16;
		m_bOutputBOM = false;
		break;
	case IDM_SETTINGS_OCP_UTF16BOM:
		m_uOutputCodePage = CP_UTF16;
		m_bOutputBOM = true;
		break;
	case IDM_SETTINGS_OCP_ANSI:
		m_uOutputCodePage = GetACP();
		break;
	default:
		return false;
	}
	if (m_uOutputCodePage != uOutputCodePage || m_bOutputBOM != bOutputBOM)
	{
		m_menuSettings[ResourceString(IDS_OUTPUTCODEPAGE)].
			Modify(
				GetOutputCodePageMenuCommand(uOutputCodePage, bOutputBOM), MF_UNCHECKED,
				GetOutputCodePageMenuCommand(uOutputCodePage, bOutputBOM), GetCodePageMenuString(uOutputCodePage, bOutputBOM));
		m_menuSettings[ResourceString(IDS_OUTPUTCODEPAGE)].
			Modify(
				GetOutputCodePageMenuCommand(m_uOutputCodePage, m_bOutputBOM), MF_CHECKED,
				GetOutputCodePageMenuCommand(m_uOutputCodePage, m_bOutputBOM), GetCodePageMenuString(m_uOutputCodePage, m_bOutputBOM));
	}
	return true;
}


static int CountCharacter(const WCHAR* ptr, size_t len, WCHAR c)
{
	int count = 0;
	const WCHAR* pCur = ptr;
	const WCHAR* pEnd = ptr + len;
	while (pCur < pEnd)
	{
		pCur = wmemchr(pCur, c, pEnd - pCur);
		if (pCur)
		{
			count++;
			pCur++;
		}
		else
		{
			break;
		}
	}
	return count;
}


static int TranslateAs(LPCVOID ptr, size_t len, UINT uCodePage)
{
	if (uCodePage == 1200)
	{
		int cb = WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<const WCHAR*>(ptr), static_cast<int>(len / sizeof(WCHAR)), NULL, 0, NULL, NULL);
		if (!cb)
		{
			return static_cast<int>(len);
		}
		Buffer<CHAR> mbs(cb);
		WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<const WCHAR*>(ptr), static_cast<int>(len / sizeof(WCHAR)), mbs, cb, NULL, NULL);
		int cch = MultiByteToWideChar(uCodePage, MB_PRECOMPOSED, mbs, cb, NULL, 0);
		if (!cch)
		{
			return static_cast<int>(len);
		}
		Buffer<WCHAR> wcs(cch);
		MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, mbs, cb, wcs, cch);
		if (CountCharacter(wcs, cch, 0x0000))
		{
			return static_cast<int>(len);
		}
		return CountCharacter(wcs, cch, REPLACEMENT_CHARACTER);
	}
	else
	{
		int cch = MultiByteToWideChar(uCodePage, MB_PRECOMPOSED, reinterpret_cast<const CHAR*>(ptr), static_cast<int>(len), NULL, 0);
		if (!cch)
		{
			return static_cast<int>(len);
		}
		Buffer<WCHAR> wcs(cch);
		MultiByteToWideChar(uCodePage, MB_PRECOMPOSED, reinterpret_cast<const CHAR*>(ptr), static_cast<int>(len), wcs, cch);
		if (CountCharacter(wcs, cch, 0x0000))
		{
			return static_cast<int>(len);
		}
		return CountCharacter(wcs, cch, REPLACEMENT_CHARACTER);
	}
}


static void TerminateNull(WCHAR* ptr, size_t len)
{
	WCHAR* pCur = ptr;
	WCHAR* pEnd = ptr + len;
	while (pCur < pEnd)
	{
		pCur = wmemchr(pCur, L'\0', pEnd - pCur);
		if (pCur)
		{
			*pCur++ = REPLACEMENT_CHARACTER;
		}
		else
		{
			break;
		}
	}
	ptr[len] = L'\0';
}


void MyDialogBox::LoadTextFromFile(int id, PWSTR psz, DWORD dwLen)
{
	WCHAR szPath[MAX_PATH] = { 0 };
	OPENFILENAMEW ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = psz ? psz : szPath;
	ofn.nMaxFile = psz ? dwLen : MAX_PATH;
	ofn.lpstrTitle = String::Format(ResourceString(IDS_LOADTEXTFROMFILE));
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (!GetOpenFileNameW(&ofn))
	{
		return;
	}
	FileMapper fm(ofn.lpstrFile);
	fm.Open();
	if (fm.Len > INT_MAX - 1)
	{
		MessageBoxW(hwnd, ResourceString(IDS_MSG_TOO_LARGE_FILE), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
		return;
	}
	else if (!fm.Len)
	{
		SetText(id);
		return;
	}
	UINT uCodePage = m_uInputCodePage;
	if (uCodePage == 0)
	{
		int errorsUTF8 = TranslateAs(fm.Ptr, fm.Len, CP_UTF8);
		int errorsACP = TranslateAs(fm.Ptr, fm.Len, GetACP());
		int errorsUTF16 = TranslateAs(fm.Ptr, fm.Len, 1200);
		int errors = 10;
		if (errorsUTF8 < errors)
		{
			errors = errorsUTF8;
			uCodePage = CP_UTF8;
		}
		if (errorsACP < errors)
		{
			errors = errorsACP;
			uCodePage = GetACP();
		}
		if (errorsUTF16 < errors)
		{
			errors = errorsUTF16;
			uCodePage = 1200;
		}
		if (uCodePage == 0)
		{
			uCodePage = CP_UTF8;
		}
	}
	if (uCodePage == 1200)
	{
		size_t cch = fm.Len / sizeof(WCHAR);
		Buffer<WCHAR> buf(cch + 1);
		wmemcpy_s(buf, buf.Len, reinterpret_cast<const WCHAR*>(fm.Ptr), cch);
		TerminateNull(buf, cch);
		if (buf[0] == BYTE_ORDER_MARK)
		{
			SetText(id, &buf[1]);
		}
		else
		{
			SetText(id, buf);
		}
	}
	else
	{
		int cch = MultiByteToWideChar(uCodePage, MB_PRECOMPOSED, reinterpret_cast<const CHAR*>(fm.Ptr), static_cast<int>(fm.Len), NULL, 0);
		if (cch)
		{
			int cchSize = cch + 1;
			Buffer<WCHAR> buf(cchSize);
			MultiByteToWideChar(uCodePage, MB_PRECOMPOSED, reinterpret_cast<const CHAR*>(fm.Ptr), static_cast<int>(fm.Len), buf, cch);
			TerminateNull(buf, cch);
			if (buf[0] == BYTE_ORDER_MARK)
			{
				SetText(id, &buf[1]);
			}
			else
			{
				SetText(id, buf);
			}
		}
		else
		{
			MessageBoxW(hwnd, ErrorMessage::Get(GetLastError()), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
		}
	}
	fm.Close();
}


void MyDialogBox::SaveTextAsFile(int id, PWSTR psz, DWORD dwLen)
{
	WCHAR szPath[MAX_PATH] = { 0 };
	OPENFILENAMEW ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = psz ? psz : szPath;
	ofn.nMaxFile = psz ? dwLen : MAX_PATH;
	ofn.lpstrTitle = String::Format(ResourceString(IDS_SAVETEXTASFILE));
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
	if (!GetSaveFileNameW(&ofn))
	{
		return;
	}
	int cch = GetTextLength(id);
	int cchSize = cch + 2;
	Buffer<WCHAR> wcs(cchSize);
	if ((m_uOutputCodePage == CP_UTF8 || m_uOutputCodePage == CP_UTF16) && m_bOutputBOM)
	{
		wcs[0] = BYTE_ORDER_MARK;
		GetText(id, &wcs[1], wcs.Len - 1);
		cch++;
	}
	else
	{
		GetText(id, wcs, wcs.Len);
	}
	try
	{
		if (!cch)
		{
			FileWriter(ofn.lpstrFile).Write("", 0);
		}
		else if (m_uOutputCodePage == CP_UTF16)
		{
			int cb = cch * sizeof(WCHAR);
			FileWriter(ofn.lpstrFile).Write(wcs, cb);
		}
		else
		{
			int cb = WideCharToMultiByte(m_uOutputCodePage, 0, wcs, cch, NULL, 0, NULL, NULL);
			if (cb)
			{
				Buffer<CHAR> mbs(cb);
				WideCharToMultiByte(m_uOutputCodePage, 0, wcs, cch, mbs, cb, NULL, NULL);
				FileWriter(ofn.lpstrFile).Write(mbs, cb);
			}
			else
			{
				DWORD dwError = GetLastError();
				PCWSTR pszMessage = String::Format(L"%s\n%s",
					ResourceString(IDS_MSG_TEXT_ENCODING_CONVERSION_ERROR).Ptr,
					ErrorMessage::Get(dwError));
				MessageBoxW(hwnd, pszMessage, ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
			}
		}
	}
	catch (Win32Exception e)
	{
		PCWSTR pszMessage = String::Format(L"%s\n%s", e.Message, ErrorMessage::Get(e.Error));
		MessageBoxW(hwnd, pszMessage, ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
	}
	catch (Exception e)
	{
		MessageBoxW(hwnd, ResourceString(IDS_MSG_FILE_WRITE_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
	}
}
