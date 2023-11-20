#pragma once


#include "MyDialogBox.h"
#include "hnrt/Hash.h"
#include "hnrt/String.h"


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
		virtual void OnExecute();
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
		void SwitchMenu();
		void Calculate(DataFeeder& rDataFeeder);
		bool CanCalculate() const;
		bool HasResult() const;
		void ClearResult();
		void SetResultHeader() const;
		void SetResultHeader(ULONGLONG nBytesIn) const;
		void SetResultHeader(ULONGLONG nBytesIn, ULONG nBytesOut) const;
		void SetResult(PCWSTR psz = L"");
		void SetResult(Hash& rHash);
		void ResetResultCase();
		void SetPath(PCWSTR psz = L"");
		UINT GetCodePage() const;
		UINT GetLineBreak() const;
		UINT ConvertToLF(PWCHAR pStart, UINT uLength);

		Hash m_hash;
		UINT m_uSource;
		UINT m_uMethod;
		UINT m_uLettercase;
		String m_szTextPath;
	};
}
