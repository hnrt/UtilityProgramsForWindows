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
		virtual void OnDelete();
		virtual void OnSelectAll();
		virtual void OnCopyAll();
		virtual void OnClear();
		virtual void OnExecute();
		virtual void OnSettingChanged(UINT);

	private:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		void OnSelectSource(int);
		void OnEncode() const;
		void OnDecode() const;

		int get_CurrentEdit() const;
		String& get_CurrentPath() const;
		void set_CurrentPath(const String&);

		__declspec(property(get = get_CurrentEdit)) int CurrentEdit;
		__declspec(property(get = get_CurrentPath, put = set_CurrentPath)) String& CurrentPath;

		mutable String m_szNativePath;
		mutable String m_szAsciiPath;
	};
}

