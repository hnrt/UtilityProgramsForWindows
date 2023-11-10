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
#include <map>


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
	, m_uInputCodePage(CP_AUTODETECT)
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
		.Add(ResourceString(IDS_MENU_EXIT), IDM_FILE_EXIT);
	m_menuEdit
		.RemoveAll();
	m_menuSettings
		.RemoveAll();
}


void MyDialogBox::OnTabSelectionChanged()
{
	m_bActive = true;
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
	switch (uCodePage)
	{
	case CP_AUTODETECT: return String::Copy(ResourceString(IDS_MENU_AUTODETECT));
	case CP_UTF8: return bBOM ? String::Copy(ResourceString(IDS_MENU_UTF8BOM)) : String::Copy(ResourceString(IDS_MENU_UTF8));
	case CP_UTF16: return bBOM ? String::Copy(ResourceString(IDS_MENU_UTF16BOM)) : String::Copy(ResourceString(IDS_MENU_UTF16));
	case 932: return String::Copy(ResourceString(IDS_MENU_CP932));
	case 936: return String::Copy(ResourceString(IDS_MENU_CP936));
	case 949: return String::Copy(ResourceString(IDS_MENU_CP949));
	case 950: return String::Copy(ResourceString(IDS_MENU_CP950));
	case 1250: return String::Copy(ResourceString(IDS_MENU_CP1250));
	case 1251: return String::Copy(ResourceString(IDS_MENU_CP1251));
	case 1252: return String::Copy(ResourceString(IDS_MENU_CP1252));
	case 1253: return String::Copy(ResourceString(IDS_MENU_CP1253));
	case 1254: return String::Copy(ResourceString(IDS_MENU_CP1254));
	case 1255: return String::Copy(ResourceString(IDS_MENU_CP1255));
	case 1256: return String::Copy(ResourceString(IDS_MENU_CP1256));
	case 1257: return String::Copy(ResourceString(IDS_MENU_CP1257));
	case 1258: return String::Copy(ResourceString(IDS_MENU_CP1258));
	default: return String::Copy(ResourceString(IDS_MENU_UTF8));
	}
}


void MyDialogBox::AddInputCodePageSettingMenus()
{
	m_menuSettings
		.Add(String::Copy(ResourceString(IDS_MENU_IENCODING)),
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
	m_menuSettings
		.Add(String::Copy(ResourceString(IDS_MENU_OENCODING)),
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
		m_menuSettings[ResourceString(IDS_MENU_IENCODING)]
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
		m_menuSettings[ResourceString(IDS_MENU_OENCODING)]
			.Modify(
				GetOutputCodePageMenuCommand(uOutputCodePage, bOutputBOM), MF_UNCHECKED,
				GetOutputCodePageMenuCommand(uOutputCodePage, bOutputBOM), GetCodePageMenuString(uOutputCodePage, bOutputBOM))
			.Modify(
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
		int cch = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, mbs, cb, NULL, 0);
		if (!cch)
		{
			return static_cast<int>(len);
		}
		Buffer<WCHAR> wcs(cch);
		MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, mbs, cb, wcs, cch);
		return CountCharacter(wcs, cch, REPLACEMENT_CHARACTER) + CountCharacter(wcs, cch, 0x0000);
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
		return CountCharacter(wcs, cch, REPLACEMENT_CHARACTER) + CountCharacter(wcs, cch, 0x0000);
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
		static const BYTE bomUTF8[] = { 0xEF, 0xBB, 0xBF };
		static const BYTE bomUTF16[] = { 0xFF, 0xFE };
		if (fm.Len >= 3 && !memcmp(fm.Ptr, bomUTF8, 3))
		{
			uCodePage = CP_UTF8;
		}
		else if (fm.Len >= 2 && !memcmp(fm.Ptr, bomUTF16, 2))
		{
			uCodePage = CP_UTF16;
		}
		else
		{
			UINT cps[] = { CP_UTF8, 1200, GetACP() };
			std::map<UINT, int> errors;
			for (int i = 0; i < _countof(cps); i++)
			{
				errors.insert(std::pair<UINT, int>(cps[i], TranslateAs(fm.Ptr, fm.Len, cps[i])));
			}
			int least = 10;
			for (std::map<UINT, int>::const_iterator iter = errors.cbegin(); iter != errors.cend(); iter++)
			{
				if (least > iter->second)
				{
					uCodePage = iter->first;
					least = iter->second;
				}
			}
			if (uCodePage == 0)
			{
				UINT acps[] = { 932, 936, 949, 950, 1250, 1251, 1252, 1253, 1254, 1255, 1256, 1257, 1258 };
				for (int i = 0; i < _countof(acps); i++)
				{
					errors.insert(std::pair<UINT, int>(acps[i], TranslateAs(fm.Ptr, fm.Len, acps[i])));
				}
				uCodePage = CP_UTF8;
				least = INT_MAX;
				for (std::map<UINT, int>::const_iterator iter = errors.cbegin(); iter != errors.cend(); iter++)
				{
					if (least > iter->second)
					{
						uCodePage = iter->first;
						least = iter->second;
					}
				}
			}
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
