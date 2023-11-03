#pragma once


#include "hnrt/WindowApp.h"
#include "resource.h"
#include "AboutBox.h"
#include "MyTabControl.h"
#include "HashDialogBox.h"
#include "GuidDialogBox.h"
#include "PercentCodecDialogBox.h"
#include "NativeToAsciiDialogBox.h"
#include "CronDialogBox.h"


namespace hnrt
{
	class MyToolbox
		: public WindowApp
	{
	public:

		MyToolbox();
		MyToolbox(const MyToolbox&) = delete;
		virtual ~MyToolbox() = default;
		virtual void Open(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow);
		bool OnFeederNotify(ULONGLONG);

	private:

		HMENU CreateMenuBar();
		virtual void OnCreate();
		void CreateChildren();
		void SetMinimumSize();
		virtual void ProcessMessage(MSG* pMsg);
		virtual LRESULT OnCommand(WPARAM wParam, LPARAM lParam);
		virtual LRESULT OnNotify(WPARAM wParam, LPARAM lParam);
		virtual void UpdateLayout(HWND hwnd, LONG cxDelta, LONG cyDelta);

		AboutBox m_about;
		MyTabControl m_tabs;
		HashDialogBox m_hashTab;
		GuidDialogBox m_guidTab;
		PercentCodecDialogBox m_pctcTab;
		CronDialogBox m_cronTab;
		NativeToAsciiDialogBox m_ntoaTab;
	};
}