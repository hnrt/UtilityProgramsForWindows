#pragma once


#include "hnrt/WindowApp.h"
#include "AboutBox.h"
#include "MyTabControl.h"
#include "HashDialogBox.h"
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
		ClipDialogBox m_clipTab;
	};
}