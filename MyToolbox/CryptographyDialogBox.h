#pragma once

#include "MyDialogBox.h"
#include "hnrt/ByteString.h"
#include "hnrt/BCryptAlgHandle.h"

namespace hnrt
{
	class CryptographyDialogBox
		: public MyDialogBox
	{
	public:

		CryptographyDialogBox();
		CryptographyDialogBox(const CryptographyDialogBox&) = delete;
		virtual ~CryptographyDialogBox() = default;
		void operator =(const CryptographyDialogBox&) = delete;
		virtual void OnTabSelectionChanging();
		virtual void OnTabSelectionChanged();
		virtual void OnLoad1From();
		virtual void OnSave1As();
		virtual void OnLoad2From();
		virtual void OnSave2As();
		virtual void OnCut();
		virtual void OnPaste();
		virtual void OnDelete();
		virtual void OnClear();
		virtual void OnSettingChanged(UINT);

	private:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnControlColorEdit(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnControlColorStatic(WPARAM wParam, LPARAM lParam);
		void OnEncrypt();
		void OnDecrypt();
		void OnChainingModeChange(int);
		void OnKeyLengthChange(int);
		void OnTagLengthChange(int);
		void OnEditSetFocus(int);
		void OnEditKillFocus(int);
		void OnKeyChange();
		void OnIVChange();
		void OnOriginalDataChange();
		void OnOriginalDataDisplayModeChange(int);
		void OnOriginalDataDisplayCodePageChange();
		String OriginalDataToString();
		void OnEncryptedDataChange();
		void OnEncryptedDataDisplayModeChange(int);
		String EncryptedDataToString();
		void SetMode(int);
		void ClearStatusText();
		void SetStatusText(PCWSTR, ...) const;
		void SetStatusText(const SYSTEMTIME&, PCWSTR, ...) const;
		void SetStatusText(const SYSTEMTIME&, PCWSTR, va_list) const;
		void UpdateMenus();
		void UpdateButtons();

		static PCWSTR ControlIdToChainingMode(int);
		static int ChainingModeToControlId(PCWSTR);
		static int ControlIdToKeyLength(int);
		static int KeyLengthToControlId(int);
		static int ControlIdToTagLength(int);
		static int TagLengthToControlId(int);
		static int OriginalDataDisplayModeToControlId(int);
		static int EncryptedDataDisplayModeToControlId(int);

		BCryptAlgHandle m_hAlg;
		int m_KeyLength;
		ByteString m_Key;
		ByteString m_IV;
		ByteString m_Nonce;
		int m_TagLength;
		ByteString m_OriginalData;
		int m_OriginalDataDisplayMode; // 0=HEX 1=BASE64 2=TEXT
		UINT m_OriginalDataCodePage;
		ByteString m_EncryptedData;
		int m_EncryptedDataDisplayMode; // 0=HEX 1=BASE64
		int m_Mode;
		BOOL m_bStatusSuccessful;
		String m_szOriginalDataPath;
		String m_szEncryptedDataPath;
		BOOL m_bWrapData;
	};
}

#define MODE_IDLE 0
#define MODE_ENCRYPTION 1
#define MODE_DECRYPTION 2
