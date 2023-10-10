#pragma once


#include "resource.h"
#include "AboutBox.h"
#include "MyTabControl.h"
#include "HashCalculator.h"
#include "GuidGenerator.h"
#include "PercentCodec.h"
#include "hnrt/WindowApp.h"


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
		HWND GetChild(INT);
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
		HashCalculator m_hashTab;
		GuidGenerator m_guidTab;
		PercentCodec m_pctcTab;
	};

	inline HWND MyToolbox::GetChild(INT iIndex)
	{
		switch (iIndex)
		{
		case 0:
			return m_hashTab;
		case 1:
			return m_guidTab;
		case 2:
			return m_pctcTab;
		default:
			return nullptr;
		}
	}
}