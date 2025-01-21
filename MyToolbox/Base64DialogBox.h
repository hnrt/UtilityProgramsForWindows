#pragma once

#include "MyDialogBox.h"
#include "DataDisplayMode.h"
#include "hnrt/ByteString.h"

namespace hnrt
{
	class Base64DialogBox
		: public MyDialogBox
	{
	public:
		Base64DialogBox();
		Base64DialogBox(const Base64DialogBox&) = delete;
		virtual ~Base64DialogBox() = default;
		void operator =(const Base64DialogBox&) = delete;
		virtual void OnTabSelectionChanging();
		virtual void OnTabSelectionChanged();
		virtual void OnNew();
		virtual void OnLoad1From();
		virtual void OnSave1As();
		virtual void OnLoad2From();
		virtual void OnSave2As();

	private:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnControlColorStatic(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnControlColorEdit(WPARAM wParam, LPARAM lParam);
		virtual void OnEditChanged(int id);

		void Encode();
		void Decode();
		void ChangeOriginalDataDisplayMode(DataDisplayMode mode);
		void ChangeLinesPerLine(UINT cch);
		ByteString GetOriginalData() const;
		void SetOriginalData(const void* ptr, size_t len);
		void SetOriginalData(const ByteString& bs);
		void SetOriginalData(const ByteString& bs, DataDisplayMode mode);
		ByteString GetEncodedData() const;
		ByteString GetDecodedData() const;
		void UpdateControlsState(int id = 0);
		void InitializeLineLengthComboBox(UINT cch) const;
		void SetStatus(PCWSTR psz = L"", DWORD dwSet = 0UL, DWORD dwReset = 0UL);
		BOOL GetLoadFromFileName(PWSTR psz, PCWSTR pszTitle) const;
		BOOL GetSaveAsFileName(PWSTR psz, PCWSTR pszTitle) const;

		static int DataDisplayModeToControlId(DataDisplayMode);
		static DataDisplayMode ControlIdToDataDisplayMode(int);

		DWORD m_dwFlags;
		DataDisplayMode m_OriginalDataDisplayMode;
		UINT m_CodePage;
		LineBreak m_LineBreak;
		UINT m_CharsPerLine;
		WCHAR m_szOriginalPath[MAX_PATH];
		WCHAR m_szEncodedPath[MAX_PATH];
	};
}
