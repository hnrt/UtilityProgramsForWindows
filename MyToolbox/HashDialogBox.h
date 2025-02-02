#pragma once


#include "MyDialogBox.h"
#include "HashAlgorithm.h"
#include "hnrt/Hash.h"
#include "hnrt/String.h"


namespace hnrt
{
	enum HashSource;

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
		void ChangeSource(int id);
		void ChangeHashAlgorithm(int id);
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
		void AddHashAlgorithmSettingMenus(int idMenuItem);
		bool ApplyToHashAlgorithmSettingMenu(int idMenuItem, int& idRadio);
		static int HashAlgorithmToControlId(HashAlgorithm);
		static int HashSourceToControlId(HashSource);

		HashAlgorithm m_algorithm;
		Hash m_hash;
		HashSource m_source;
		StringOptions m_lettercase;
		String m_szTextPath;
	};
}
