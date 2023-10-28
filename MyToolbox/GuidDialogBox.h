#pragma once


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
		virtual void OnCopy();

	private:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);

		void ChangeFormat(UINT uSelected = 0);
		void ChangeGuid();

		GUID m_guid;
		WCHAR m_szFormatted[260];
		UINT m_uCurrentlySelected;
	};
}
