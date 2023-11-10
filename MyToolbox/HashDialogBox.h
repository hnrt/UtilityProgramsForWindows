#pragma once


#include "MyDialogBox.h"
#include "hnrt/Hash.h"


namespace hnrt
{
	class HashDialogBox
		: public MyDialogBox
	{
	public:

		HashDialogBox();
		HashDialogBox(const HashDialogBox&) = delete;
		virtual ~HashDialogBox() = default;
		void operator =(const HashDialogBox&) = delete;
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
		virtual void OnFeederNotify(ULONGLONG);

	private:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);

		void OnBrowse();
		void OnCalculate();
		void OnSelectSource(UINT uSource);
		void OnSelectMethod(UINT uMethod);
		void OnUppercase();
		void SwitchMenu(bool bOnTabSelectionChanged = false);
		void Calculate(DataFeeder& rDataFeeder);
		UINT GetCodePage();
		UINT GetLineBreak();
		UINT ConvertToLF(PWCHAR pStart, UINT uLength);
		void SetResultHeader();
		void SetResultHeader(ULONGLONG nBytesIn);
		void SetResultHeader(ULONGLONG nBytesIn, ULONG nBytesOut);
		void SetResult(PCWSTR psz = L"");
		void SetResult(Hash& rHash);
		void ResetResultCase();

		Hash m_hash;
		UINT m_uSource;
		UINT m_uMethod;
		BOOL m_bUppercase;
		WCHAR m_szTextPath[MAX_PATH];
	};
}
