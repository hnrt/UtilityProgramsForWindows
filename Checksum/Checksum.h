#pragma once

#include "resource.h"
#include "hnrt/DialogApp.h"
#include "hnrt/Hash.h"

namespace hnrt
{
	class Checksum
		: public DialogApp
	{
	public:

		Checksum();
		Checksum(const Checksum&) = delete;
		virtual ~Checksum() = default;
		void operator =(const Checksum&) = delete;

	private:

		friend class FileDataFeederEx;

		virtual void OnCreate(HWND hDlg);
		virtual void OnDestroy(HWND hDlg);
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam);

		void OnBrowse(HWND hDlg);
		void OnCalculate(HWND hDlg);
		void OnCopy(HWND hDlg);
		void OnSelectSource(HWND hDlg, UINT uSource);
		void OnSelectMethod(HWND hDlg, UINT uMethod);
		void OnUppercase(HWND hDlg);
		void Calculate(HWND hDlg, DataFeeder& rDataFeeder);
		UINT GetCodePage(HWND hDlg);
		UINT GetLineBreak(HWND hDlg);
		UINT ConvertToLF(PWCHAR pStart, UINT uLength);
		void SetResultHeader(HWND hDlg);
		void SetResultHeader(HWND hDlg, ULONGLONG nBytesIn);
		void SetResultHeader(HWND hDlg, ULONGLONG nBytesIn, ULONG nBytesOut);
		void SetResult(HWND hDlg, PCWSTR psz = L"");
		void SetResult(HWND hDlg, Hash& rHash);
		void ResetResultCase(HWND hDlg);

		Hash m_hash;
		UINT m_uSource;
		UINT m_uMethod;
		BOOL m_bUppercase;
	};
}