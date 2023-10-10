#pragma once


#include "hnrt/DialogBox.h"


namespace hnrt
{
	class PercentCodec
		: public DialogBox
	{
	public:

		PercentCodec();
		PercentCodec(const PercentCodec&) = delete;
		virtual ~PercentCodec() = default;
		void operator =(const PercentCodec&) = delete;
		void OnCopy();

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
		void Encode(PCWSTR pszIn, UINT cchIn, UINT uCodePage, PWCHAR pOut, UINT cchOut);
		void Encode(WCHAR c, UINT uCodePage, PWCHAR& pOut, PWCHAR pOutBound, UINT offset);
		void Decode(PCWSTR pszIn, UINT cchIn, UINT uCodePage, PWCHAR pOut, UINT cchOut);
		UINT GetDecodedOffset(PCWSTR pszIn, UINT cbOut);

		bool m_bEncodingError;
		bool m_bDecodingError;
	};
}
