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
		virtual void Open(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow);
		bool OnFeederNotify(ULONGLONG);
		HFONT GetFont() const;
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
		ClipDialogBox m_clipTab;
		HFONT m_hFont;
		HFONT m_hFontForData;
	};
	
	inline HFONT MyToolbox::GetFont() const
	{
		return m_hFont;
	}

	inline HFONT MyToolbox::GetFontForData() const
	{
		return m_hFontForData;
	}
}

#define FACENAME L"Segoe UI"
#define POINTSIZE 8
#if 0
#define FACENAME_DATA L"Campbell"
#elif 0
#define FACENAME_DATA L"Courier New"
#else
#define FACENAME_DATA L"Lucida Console"
#endif
#define POINTSIZE_DATA 10
