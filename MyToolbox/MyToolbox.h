#pragma once


#include "Resource.h"
#include "AboutBox.h"
#include "HashCalculator.h"
#include "hnrt/WindowApp.h"
#include "hnrt/TabControl.h"


namespace hnrt
{
	class MyToolbox
		: public WindowApp
	{
	public:

		static MyToolbox& GetInstance();

		MyToolbox();
		MyToolbox(const MyToolbox&) = delete;
		virtual ~MyToolbox() = default;
		virtual void Open(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow);

	private:

		static MyToolbox* m_pInstance;

		HMENU CreateMenuBar();
		virtual void OnCreate();
		virtual LRESULT OnCommand(WPARAM wParam, LPARAM lParam);
		virtual LRESULT OnNotify(WPARAM wParam, LPARAM lParam);
		virtual void UpdateLayout(HWND hwnd, LONG cxDelta, LONG cyDelta);

		AboutBox m_about;
		TabControl m_tabs;
		HashCalculator m_hashTab;
		bool m_bSizing;
	};
}