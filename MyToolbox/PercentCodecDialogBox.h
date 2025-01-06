#pragma once


#include "MyDialogBox.h"


namespace hnrt
{
	class PercentCodecDialogBox
		: public MyDialogBox
	{
	public:

		PercentCodecDialogBox();
		PercentCodecDialogBox(const PercentCodecDialogBox&) = delete;
		virtual ~PercentCodecDialogBox() = default;
		void operator =(const PercentCodecDialogBox&) = delete;
		virtual void OnTabSelectionChanging();
		virtual void OnTabSelectionChanged();
		virtual void OnLoad1From();
		virtual void OnSave1As();
		virtual void OnLoad2From();
		virtual void OnSave2As();
		virtual void OnClear();
		virtual void OnSettingChanged(UINT);

	private:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnControlColorStatic(WPARAM wParam, LPARAM lParam);
		virtual void UpdateControlsState(int id);
		bool OnEncode();
		bool OnDecode();
		UINT GetCodePage() const;
		void Encode(PCWSTR pszIn, UINT cchIn, UINT uCodePage, PWCHAR pOut, UINT cchOut, bool bUsePlus);
		void Encode(WCHAR c, UINT uCodePage, PWCHAR& pOut, PWCHAR pOutBound, UINT offset);
		void Decode(PCWSTR pszIn, UINT cchIn, UINT uCodePage, PWCHAR pOut, UINT cchOut);
		UINT GetDecodedOffset(PCWSTR pszIn, UINT cbOut);
		void ClearStatus(int id = 0);

		bool m_bEncodingError;
		bool m_bDecodingError;
		mutable String m_szOriginalPath;
		mutable String m_szEncodedPath;
	};
}
