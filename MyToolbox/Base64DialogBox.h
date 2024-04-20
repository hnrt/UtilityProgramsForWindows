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
		virtual void OnClear();

	private:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnControlColorStatic(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnControlColorEdit(WPARAM wParam, LPARAM lParam);

		void Encode();
		void Decode();
		void SetDecoded();
		void UpdateControlsState();

		BOOL m_bEncodingError;
		BOOL m_bDecodingError;
		ByteString m_Decoded;
		BOOL m_bEncoded;
	};
}
