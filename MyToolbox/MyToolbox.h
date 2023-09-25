#pragma once


#include "Resource.h"
#include "AboutBox.h"
#include "MyTabControl.h"
#include "HashCalculator.h"
#include "GuidGenerator.h"
#include "hnrt/WindowApp.h"


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
		HWND GetChild(INT);

	private:

		static MyToolbox* m_pInstance;

		HMENU CreateMenuBar();
		virtual void OnCreate();
		void CreateChildren();
		void SetMinimumSize();
		virtual LRESULT OnCommand(WPARAM wParam, LPARAM lParam);
		virtual LRESULT OnNotify(WPARAM wParam, LPARAM lParam);
		virtual void UpdateLayout(HWND hwnd, LONG cxDelta, LONG cyDelta);

		AboutBox m_about;
		MyTabControl m_tabs;
		HashCalculator m_hashTab;
		GuidGenerator m_guidTab;
	};

	inline HWND MyToolbox::GetChild(INT iIndex)
	{
		switch (iIndex)
		{
		case 0:
			return m_hashTab;
		case 1:
			return m_guidTab;
		default:
			return nullptr;
		}
	}
}