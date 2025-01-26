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
		virtual void OnClear();
		virtual void OnNew();
		virtual void OnExecute();
		virtual void OnCopyResult();
		virtual void OnFeederNotify(ULONGLONG);

	private:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnControlColorStatic(WPARAM wParam, LPARAM lParam);
		virtual void UpdateControlsState(int id);

		void OnBrowse();
		void OnSelectSource(UINT uSource);
		void OnSelectMethod(UINT uMethod);
		void OnUppercase();
		void SwitchMenu();
		void Calculate(RefPtr<DataFeeder> pDataFeeder);
		bool CanCalculate() const;
		void ClearValue();
		void SetValueHeader() const;
		void SetValueHeader(ULONGLONG nBytesIn) const;
		void SetValueHeader(ULONGLONG nBytesIn, ULONG nBytesOut) const;
		void SetValue(PCWSTR psz = L"");
		void SetValue(Hash& rHash);
		void ResetValueLetterCase();
		void SetError(PCWSTR psz);
		void VerifyValue();
		void SetVerificationResult(PCWSTR psz = L"");
		UINT GetCodePage() const;
		LineBreak GetLineBreak() const;

		Hash m_hash;
		UINT m_uSource;
		UINT m_uMethod;
		StringOptions m_uLettercase;
		String m_szTextPath;
	};
}
