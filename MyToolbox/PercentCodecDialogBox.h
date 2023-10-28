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
		virtual void OnCopy();

	private:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnControlColorStatic(WPARAM wParam, LPARAM lParam);
		void OnSelectSource(int);
		void OnCopy1();
		void OnCopy2();
		bool OnEncode();
		bool OnDecode();
		UINT GetCodePage();
		void Encode(PCWSTR pszIn, UINT cchIn, UINT uCodePage, PWCHAR pOut, UINT cchOut, bool bUsePlus);
		void Encode(WCHAR c, UINT uCodePage, PWCHAR& pOut, PWCHAR pOutBound, UINT offset);
		void Decode(PCWSTR pszIn, UINT cchIn, UINT uCodePage, PWCHAR pOut, UINT cchOut);
		UINT GetDecodedOffset(PCWSTR pszIn, UINT cbOut);

		bool m_bEncodingError;
		bool m_bDecodingError;
	};
}
