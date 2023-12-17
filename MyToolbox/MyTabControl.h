#pragma once


#include "hnrt/TabControl.h"


namespace hnrt
{
	class MyDialogBox;

	class MyTabControl
		: public TabControl
	{
	public:

		MyTabControl();
		MyTabControl(const MyTabControl&) = delete;
		virtual ~MyTabControl() = default;
		void operator = (const MyTabControl&) = delete;
		void Open(HWND);
		void Add(PCWSTR, MyDialogBox*);
		const MyDialogBox& operator [](int) const;
		MyDialogBox& operator [](int);
		virtual void OnTabSelectionChanging();
		virtual void OnTabSelectionChanged();
		void LoadFromRegistry();
		void SaveToRegistry() const;

	private:

		MyDialogBox* m_pDialogBoxes[16];
	};
}
