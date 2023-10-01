#pragma once


#include "hnrt/DialogBox.h"
#include "hnrt/Hash.h"


namespace hnrt
{
	class HashCalculator
		: public DialogBox
	{
	public:

		HashCalculator();
		HashCalculator(const HashCalculator&) = delete;
		virtual ~HashCalculator() = default;
		void operator =(const HashCalculator&) = delete;
		void OnCopy();
		void SetResultHeader(ULONGLONG nBytesIn);

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
		void Calculate(DataFeeder& rDataFeeder);
		UINT GetCodePage();
		UINT GetLineBreak();
		UINT ConvertToLF(PWCHAR pStart, UINT uLength);
		void SetResultHeader();
		void SetResultHeader(ULONGLONG nBytesIn, ULONG nBytesOut);
		void SetResult(PCWSTR psz = L"");
		void SetResult(Hash& rHash);
		void ResetResultCase();

		Hash m_hash;
		UINT m_uSource;
		UINT m_uMethod;
		BOOL m_bUppercase;
	};
}
