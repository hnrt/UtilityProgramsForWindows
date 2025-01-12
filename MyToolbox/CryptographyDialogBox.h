#pragma once

#include "MyDialogBox.h"
#include "DataDisplayMode.h"
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
		virtual void OnNew();
		virtual void OnLoad1From();
		virtual void OnSave1As();
		virtual void OnLoad2From();
		virtual void OnSave2As();
		virtual void OnSettingChanged(UINT);

	private:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnControlColorEdit(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnControlColorStatic(WPARAM wParam, LPARAM lParam);
		virtual void OnEditChanged(int id);
		void OnEncrypt();
		void OnDecrypt();
		void OnCopyOriginalData();
		void OnCopyEncryptedData();
		void OnAdjustKey();
		void OnAdjustIV();
		void OnChainingModeChange(int);
		void OnKeyLengthChange(int);
		void OnTagLengthChange(int);
		void OnKeyChange();
		void OnIVChange();
		void OnAaDataDisplayModeChange(int);
		void OnOriginalDataChange();
		void OnOriginalDataDisplayModeChange(int);
		String OriginalDataToString();
		void OnEncryptedDataChange();
		void OnEncryptedDataDisplayModeChange(int);
		String EncryptedDataToString();
		void OnHexFormatChange();
		void OnCodePageChange();
		void OnLineBreakChange();
		void SetMode(int);
		void ClearStatusText();
		void SetStatusText(PCWSTR, ...) const;
		void SetStatusText(const SYSTEMTIME&, PCWSTR, ...) const;
		void SetStatusText(const SYSTEMTIME&, PCWSTR, va_list) const;
		void UpdateMenus();
		void UpdateButtons();
		void UpdateTagSizeRadioBoxes();
		ByteString GetAaData() const;

		static PCWSTR ControlIdToChainingMode(int);
		static int ChainingModeToControlId(PCWSTR);
		static int ControlIdToKeyLength(int);
		static int KeyLengthToControlId(int);
		static int ControlIdToTagLength(int);
		static int TagLengthToControlId(int);
		static DataDisplayMode ControlIdToDataDisplayMode(int);
		static int OriginalDataDisplayModeToControlId(DataDisplayMode);
		static int EncryptedDataDisplayModeToControlId(DataDisplayMode);
		static int AaDataDisplayModeToControlId(DataDisplayMode);

		BCryptAlgHandle m_hAlg;
		int m_KeyLength;
		ByteString m_Key;
		ByteString m_IV;
		ByteString m_Nonce;
		int m_CcmTagLength;
		int m_GcmTagLength;
		DataDisplayMode m_AaDataDisplayMode;
		ByteString m_OriginalData;
		DataDisplayMode m_OriginalDataDisplayMode;
		ByteString m_EncryptedData;
		DataDisplayMode m_EncryptedDataDisplayMode;
		StringOptions m_HexLetterCase;
		UINT m_CodePage;
		LineBreak m_LineBreak;
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

#define FLAG_TAG32  (1UL<<4)
#define FLAG_TAG48  (1UL<<6)
#define FLAG_TAG64  (1UL<<8)
#define FLAG_TAG80  (1UL<<10)
#define FLAG_TAG96  (1UL<<12)
#define FLAG_TAG104 (1UL<<13)
#define FLAG_TAG112 (1UL<<14)
#define FLAG_TAG120 (1UL<<15)
#define FLAG_TAG128 (1UL<<16)
