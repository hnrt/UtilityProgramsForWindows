#pragma once

#include "MyDialogBox.h"

namespace hnrt
{
	class NativeToAsciiDialogBox
		: public MyDialogBox
	{
	public:

		NativeToAsciiDialogBox();
		NativeToAsciiDialogBox(const NativeToAsciiDialogBox&) = delete;
		virtual ~NativeToAsciiDialogBox() = default;
		void operator =(const NativeToAsciiDialogBox&) = delete;
		virtual void OnTabSelectionChanging();
		virtual void OnTabSelectionChanged();
		virtual void OnNew();
		virtual void OnLoad1From();
		virtual void OnSave1As();
		virtual void OnLoad2From();
		virtual void OnSave2As();
		virtual void OnSettingChanged(UINT);

	private:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		virtual void OnEditChanged(int id);
		void Encode();
		void Decode();
		void SetStatus(PCWSTR psz = L"", DWORD dwSet = 0UL, DWORD dwReset = 0UL);
		void UpdateControlsState(int id);

		mutable String m_szNativePath;
		mutable String m_szAsciiPath;
	};
}

