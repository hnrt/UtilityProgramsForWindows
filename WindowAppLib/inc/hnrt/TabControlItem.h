#pragma once


#include <CommCtrl.h>


namespace hnrt
{
	struct TabControlItem
        : public TCITEMW
	{
	public:

		TabControlItem();
		TabControlItem(const TabControlItem&);
		~TabControlItem();
		TabControlItem& operator =(const TabControlItem&);
		TabControlItem& SetState(DWORD dwState, DWORD dwStateMask = ~0UL);
		TabControlItem& SetText(PCWSTR pszText);
		TabControlItem& SetImage(INT iImage);
		TabControlItem& SetParam(LPARAM lParam);
		INT InsertInto(HWND hwnd, INT iIndex = -1);
		TabControlItem& GetFrom(HWND hwnd, INT iIndex);
	};
}
