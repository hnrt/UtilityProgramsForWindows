#pragma once


#include "hnrt/TabControl.h"
#include "MyDialogBox.h"


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
		void Add(PCWSTR, MyDialogBox*);
		const MyDialogBox& operator [](int) const;
		MyDialogBox& operator [](int);
		virtual void OnTabSelectionChanging();
		virtual void OnTabSelectionChanged();
		void LoadFromRegistry();
		void SaveToRegistry();

	private:

		MyToolbox* m_pApp;
		MyDialogBox* m_pDialogBoxes[16];
	};
}
