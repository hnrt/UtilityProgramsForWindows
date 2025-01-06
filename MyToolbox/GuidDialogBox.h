#pragma once


#include "hnrt/String.h"
#include "MyDialogBox.h"


namespace hnrt
{
	class GuidDialogBox
		: public MyDialogBox
	{
	public:

		GuidDialogBox();
		GuidDialogBox(const GuidDialogBox&) = delete;
		virtual ~GuidDialogBox() = default;
		void operator =(const GuidDialogBox&) = delete;
		virtual void OnTabSelectionChanging();
		virtual void OnTabSelectionChanged();
		virtual void OnCopyResult();
		virtual void OnExecute();

	private:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnControlColorEdit(WPARAM wParam, LPARAM lParam);

		void ChangeFormat(UINT uSelected = 0);
		void ChangeGuid();
		void ParseContent();
		static PCWCHAR Parse32Bits(PCWCHAR pCur, unsigned long& parsed);
		static PCWCHAR Parse16Bits(PCWCHAR pCur, unsigned short& parsed);
		static PCWCHAR Parse8Bits(PCWCHAR pCur, unsigned char parsed[], size_t length);
		static PCWCHAR ParseChar(PCWCHAR pCur, WCHAR c);
		static PCWCHAR ParseString(PCWCHAR pCur, PCWSTR psz);
		static PCWCHAR SkipTo(PCWCHAR pCur, WCHAR c);
		static PCWCHAR SkipTo(PCWCHAR pCur, PCWSTR psz);
		static PCWCHAR SkipWS(PCWCHAR pCur);

		GUID m_guid;
		String m_szFormatted;
		UINT m_uCurrentlySelected;
		BOOL m_bError;
	};
}
