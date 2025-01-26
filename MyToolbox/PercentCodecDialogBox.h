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
		virtual void OnEditChanged(int id);
		bool Encode();
		bool Decode();
		static void Encode(PCWCH pszIn, UINT cchIn, UINT uCodePage, PWCH pOut, UINT cchOut, BOOL bUsePlus);
		static void Encode(WCHAR c, UINT uCodePage, PWCH& pOut, PWCH pOutBound, UINT offset);
		static void Decode(PCWCH pszIn, UINT cchIn, UINT uCodePage, PWCH pOut, UINT cchOut);
		static UINT GetDecodedOffset(PCWCH pszIn, UINT cbOut);
		void UpdateControlsState(int id);

		UINT m_CodePage;
		BOOL m_bUsePlus;
		mutable String m_szOriginalPath;
		mutable String m_szEncodedPath;
	};
}
