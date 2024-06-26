#pragma once

#include "MyDialogBox.h"
#include "hnrt/ByteString.h"

namespace hnrt
{
	class Base64DialogBox
		: public MyDialogBox
	{
	public:
		Base64DialogBox();
		Base64DialogBox(const Base64DialogBox&) = delete;
		virtual ~Base64DialogBox();
		void operator =(const Base64DialogBox&) = delete;
		virtual void OnTabSelectionChanging();
		virtual void OnTabSelectionChanged();
		virtual void OnLoad1From();
		virtual void OnSave1As();
		virtual void OnLoad2From();
		virtual void OnSave2As();
		virtual void OnClear();

	private:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnTimer(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnControlColorStatic(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnControlColorEdit(WPARAM wParam, LPARAM lParam);

		void Encode();
		void Decode();
		BOOL ParseOriginal(ByteString& bs);
		void ApplyOriginal();
		void SetOriginal(PCWSTR pszContent = L"");
		void SetOriginalError(PCWSTR pszReason);
		void SetEncoded(PCWSTR pszContent = L"", BOOL bEncoded = FALSE);
		void SetEncodedError(PCWSTR pszReason);
		void UpdateControlsState();
		void InitializeLineLengthComboBox() const;
		BOOL GetLoadFromFileName(PWSTR psz, PCWSTR pszTitle) const;
		BOOL GetSaveAsFileName(PWSTR psz, PCWSTR pszTitle) const;

		BOOL m_bEncodingError;
		BOOL m_bDecodingError;
		ByteString m_Original;
		BOOL m_bEncoded;
		WCHAR m_szOriginalPath[MAX_PATH];
		WCHAR m_szEncodedPath[MAX_PATH];
	};
}
