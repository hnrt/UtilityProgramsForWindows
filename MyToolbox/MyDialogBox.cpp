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
#include <map>


using namespace hnrt;


MyDialogBox::MyDialogBox(UINT idTemplate, PCWSTR pszName)
	: DialogBox(idTemplate)
	, m_id(-1)
	, m_bActive(false)
	, m_szRegistryKeyName(GetApp<MyToolbox>().GetRegistryKeyName(pszName))
	, m_cProcessing(0)
	, m_menuFile(HMENU_NULL)
	, m_menuEdit(HMENU_NULL)
	, m_menuView(HMENU_NULL)
	, m_menuSettings(HMENU_NULL)
	, m_menuHelp(HMENU_NULL)
	, m_uInputCodePage(CP_AUTODETECT)
	, m_uOutputCodePage(CP_UTF8)
	, m_bOutputBOM(false)
	, m_LastModified()
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


void MyDialogBox::AddHashAlgorithmSettingMenus(UINT uId)
{
	String szAlgorithm(ResourceString(IDS_MENU_ALGORITHM));
	String szMD5(ResourceString(IDS_MENU_MD5));
	String szSHA1(ResourceString(IDS_MENU_SHA1));
	String szSHA256(ResourceString(IDS_MENU_SHA256));
	String szSHA384(ResourceString(IDS_MENU_SHA384));
	String szSHA512(ResourceString(IDS_MENU_SHA512));
	m_menuSettings
		.Add(szAlgorithm, Menu()
			.Add(szMD5, IDM_SETTINGS_MD5, uId == IDM_SETTINGS_MD5 ? MF_CHECKED : MF_UNCHECKED)
			.Add(szSHA1, IDM_SETTINGS_SHA1, uId == IDM_SETTINGS_SHA1 ? MF_CHECKED : MF_UNCHECKED)
			.Add(szSHA256, IDM_SETTINGS_SHA256, uId == IDM_SETTINGS_SHA256 ? MF_CHECKED : MF_UNCHECKED)
			.Add(szSHA384, IDM_SETTINGS_SHA384, uId == IDM_SETTINGS_SHA384 ? MF_CHECKED : MF_UNCHECKED)
			.Add(szSHA512, IDM_SETTINGS_SHA512, uId == IDM_SETTINGS_SHA512 ? MF_CHECKED : MF_UNCHECKED));
}


bool MyDialogBox::ApplyToHashAlgorithm(UINT uId, UINT& uValue, UINT uBase)
{
	switch (uId)
	{
	case IDM_SETTINGS_MD5:
	case IDM_SETTINGS_SHA1:
	case IDM_SETTINGS_SHA256:
	case IDM_SETTINGS_SHA384:
	case IDM_SETTINGS_SHA512:
		uValue = uBase + uId - IDM_SETTINGS_MD5;
		break;
	default:
		return false;
	}
	String szAlgorithm(ResourceString(IDS_MENU_ALGORITHM));
	String szMD5(ResourceString(IDS_MENU_MD5));
	String szSHA1(ResourceString(IDS_MENU_SHA1));
	String szSHA256(ResourceString(IDS_MENU_SHA256));
	String szSHA384(ResourceString(IDS_MENU_SHA384));
	String szSHA512(ResourceString(IDS_MENU_SHA512));
	m_menuSettings[szAlgorithm.Ptr]
		.Modify(
			IDM_SETTINGS_MD5, uId == IDM_SETTINGS_MD5 ? MF_CHECKED : MF_UNCHECKED,
			IDM_SETTINGS_MD5, szMD5)
		.Modify(
			IDM_SETTINGS_SHA1, uId == IDM_SETTINGS_SHA1 ? MF_CHECKED : MF_UNCHECKED,
			IDM_SETTINGS_SHA1, szSHA1)
		.Modify(
			IDM_SETTINGS_SHA256, uId == IDM_SETTINGS_SHA256 ? MF_CHECKED : MF_UNCHECKED,
			IDM_SETTINGS_SHA256, szSHA256)
		.Modify(
			IDM_SETTINGS_SHA384, uId == IDM_SETTINGS_SHA384 ? MF_CHECKED : MF_UNCHECKED,
			IDM_SETTINGS_SHA384, szSHA384)
		.Modify(
			IDM_SETTINGS_SHA512, uId == IDM_SETTINGS_SHA512 ? MF_CHECKED : MF_UNCHECKED,
			IDM_SETTINGS_SHA512, szSHA512);
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


bool MyDialogBox::ApplyToLettercase(UINT uId, UINT& uValue, UINT uBase)
{
	switch (uId)
	{
	case IDM_SETTINGS_UPPERCASE:
	case IDM_SETTINGS_LOWERCASE:
		uValue = uBase + uId - IDM_SETTINGS_UPPERCASE;
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


void MyDialogBox::LoadTextFromFile(int id) const
{
	String szPath;
	LoadTextFromFile(id, szPath);
}


void MyDialogBox::LoadTextFromFile(int id, String& szPath) const
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
		return;
	}
	FileMapper fm(ofn.lpstrFile);
	if (fm.Len > INT_MAX - 1)
	{
		MessageBoxW(hwnd, ResourceString(IDS_MSG_TOO_LARGE_FILE), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
		return;
	}
	szPath = szPath2;
	if (!fm.Len)
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


void MyDialogBox::SaveTextAsFile(int id) const
{
	String szPath;
	SaveTextAsFile(id, szPath);
}


void MyDialogBox::SaveTextAsFile(int id, String& szPath) const
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
		return;
	}
	String wcs = GetText(id);
	if ((m_uOutputCodePage == CP_UTF8 || m_uOutputCodePage == CP_UTF16) && m_bOutputBOM)
	{
		wcs.Format(L"%c%s", BYTE_ORDER_MARK, wcs);
	}
	try
	{
		if (!wcs.Len)
		{
			FileWriter(ofn.lpstrFile).Write("", 0);
		}
		else if (m_uOutputCodePage == CP_UTF16)
		{
			FileWriter(ofn.lpstrFile).Write(wcs, wcs.Len * sizeof(WCHAR));
		}
		else
		{
			int cb = WideCharToMultiByte(m_uOutputCodePage, 0, wcs, static_cast<int>(wcs.Len), NULL, 0, NULL, NULL);
			if (cb)
			{
				Buffer<CHAR> mbs(cb);
				WideCharToMultiByte(m_uOutputCodePage, 0, wcs, static_cast<int>(wcs.Len), mbs, cb, NULL, NULL);
				FileWriter(ofn.lpstrFile).Write(mbs, cb);
			}
			else
			{
				DWORD dwError = GetLastError();
				String szMessage(PRINTF, L"%s\n%s",
					ResourceString(IDS_MSG_TEXT_ENCODING_CONVERSION_ERROR).Ptr,
					ErrorMessage::Get(dwError));
				MessageBoxW(hwnd, szMessage, ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
			}
		}
		szPath = szPath2;
	}
	catch (Win32Exception e)
	{
		String szMessage(PRINTF, L"%s\n%s", e.Message, ErrorMessage::Get(e.Error));
		MessageBoxW(hwnd, szMessage, ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
	}
	catch (Exception e)
	{
		MessageBoxW(hwnd, ResourceString(IDS_MSG_FILE_WRITE_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
	}
}


void MyDialogBox::InitializeCodePageComboBox(int id, int initialSelection) const
{
#define ADD(x,y) ComboBoxAdd(id,x,y)
#define ADDCP(x) ADD(L"CP" L#x,x)
	ADD(L"UTF-8", CP_UTF8);
	ADD(L"UTF-16", CP_UTF16);
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


void MyDialogBox::InitializeLineBreakComboBox(int id, int initialSelection) const
{
	ComboBoxAdd(id, L"CRLF", 0x0d0a);
	ComboBoxAdd(id, L"LF", 0x0a);
	ComboBoxSetSelection(id, initialSelection);
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
