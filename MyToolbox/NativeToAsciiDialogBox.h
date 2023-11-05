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
		virtual void OnLoadFrom();
		virtual void OnSaveAs();
		virtual void OnCut();
		virtual void OnCopy();
		virtual void OnPaste();
		virtual void OnSelectAll();
		virtual void OnClear();
		virtual void OnSettingChanged(UINT);

	private:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		void OnSelectSource(int);
		void OnEncode();
		void OnDecode();

		int get_CurrentEdit() const;
		PWSTR get_CurrentPath();

		__declspec(property(get = get_CurrentEdit)) int CurrentEdit;
		__declspec(property(get = get_CurrentPath)) PWSTR CurrentPath;

		WCHAR m_szNativePath[MAX_PATH];
		WCHAR m_szAsciiPath[MAX_PATH];
	};
}

