#pragma once


#include "hnrt/TabControl.h"


namespace hnrt
{
	class MyToolbox;

	class MyTabControl
		: public TabControl
	{
	public:

		MyTabControl();
		MyTabControl(const MyTabControl&) = delete;
		virtual ~MyTabControl() = default;
		void operator = (const MyTabControl&) = delete;
		void Open(MyToolbox*);
		virtual void OnTabSelectionChanging();
		virtual void OnTabSelectionChanged();
		void LoadFromRegistry();
		void SaveToRegistry();

	private:

		MyToolbox* m_pApp;
	};
}
