#pragma once

#include "hnrt/WindowApp.h"
#include "AboutBox.h"
#include "MyTabControl.h"
#include "HashDialogBox.h"
#include "CryptographyDialogBox.h"
#include "GuidDialogBox.h"
#include "PercentCodecDialogBox.h"
#include "CronDialogBox.h"
#include "NativeToAsciiDialogBox.h"
#include "DateTimeDialogBox.h"
#include "SfidDialogBox.h"
#include "GtinDialogBox.h"
#include "ClipDialogBox.h"

namespace hnrt
{
	class MyToolbox
		: public WindowApp
	{
	public:

		MyToolbox();
		MyToolbox(const MyToolbox&) = delete;
		virtual ~MyToolbox();
		String GetRegistryKeyName(PCWSTR = nullptr) const;
		String GetDirectoryPath(PCWSTR = nullptr) const;
		virtual void Open(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow);
		bool OnFeederNotify(ULONGLONG);
		HFONT GetFont() const;
		HFONT GetFontForTab() const;
		HFONT GetFontForData() const;

	private:

		HMENU CreateMenuBar();
		virtual void OnCreate();
		virtual void OnDestroy();
		void CreateChildren();
		void SetMinimumSize();
		virtual void ProcessMessage(MSG* pMsg);
		virtual LRESULT OnCommand(WPARAM wParam, LPARAM lParam);
		virtual LRESULT OnNotify(WPARAM wParam, LPARAM lParam);
		virtual void UpdateLayout(HWND hwnd, LONG cxDelta, LONG cyDelta);

		AboutBox m_about;
		MyTabControl m_tabs;
		HashDialogBox m_hashTab;
		CryptographyDialogBox m_crptTab;
		GuidDialogBox m_guidTab;
		PercentCodecDialogBox m_pctcTab;
		CronDialogBox m_cronTab;
		NativeToAsciiDialogBox m_ntoaTab;
		DateTimeDialogBox m_dttmTab;
		SfidDialogBox m_sfidTab;
		GtinDialogBox m_gtinTab;
		ClipDialogBox m_clipTab;
		HFONT m_hFont;
		HFONT m_hFontForTab;
		HFONT m_hFontForData;
	};
	
	inline HFONT MyToolbox::GetFont() const
	{
		return m_hFont;
	}

	inline HFONT MyToolbox::GetFontForTab() const
	{
		return m_hFontForTab;
	}

	inline HFONT MyToolbox::GetFontForData() const
	{
		return m_hFontForData;
	}
}

#define APP_GROUP L"hnrt"
#define APP_NAME L"MyToolbox"

#define REGKEY_ROOT L"SOFTWARE\\" APP_GROUP L"\\" APP_NAME

#define FACENAME L"Segoe UI"
#define POINTSIZE 8

#define FACENAME_TAB L"Segoe UI"
#define POINTSIZE_TAB 10

#if 0
#define FACENAME_DATA L"Campbell"
#elif 0
#define FACENAME_DATA L"Courier New"
#else
#define FACENAME_DATA L"Lucida Console"
#endif
#define POINTSIZE_DATA 9
