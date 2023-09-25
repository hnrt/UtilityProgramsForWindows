#pragma once


#include "hnrt/DialogBox.h"


namespace hnrt
{
	class GuidGenerator
		: public DialogBox
	{
	public:

		GuidGenerator();
		GuidGenerator(const GuidGenerator&) = delete;
		virtual ~GuidGenerator() = default;
		void operator =(const GuidGenerator&) = delete;
		void OnCopy();

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
