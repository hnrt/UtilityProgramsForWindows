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
		virtual void OnCopy();
		virtual void OnExecute();

	private:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);

		void ChangeFormat(UINT uSelected = 0);
		void ChangeGuid();

		GUID m_guid;
		String m_szFormatted;
		UINT m_uCurrentlySelected;
	};
}
