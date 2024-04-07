#pragma once

#include "MyDialogBox.h"

namespace hnrt
{
	class GtinDialogBox
		: public MyDialogBox
	{
	public:

		GtinDialogBox();
		GtinDialogBox(const GtinDialogBox&) = delete;
		virtual ~GtinDialogBox() = default;
		void operator =(const GtinDialogBox&) = delete;
		virtual void OnTabSelectionChanging();
		virtual void OnTabSelectionChanged();

	private:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnTimer(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnControlColorEdit(WPARAM wParam, LPARAM lParam);
		void SetGS1CPLength(int length);
		void GTIN13Add(int delta);
		void ApplyModification(int id);

		int m_GS1CPLength;
		int m_nGTIN13Change;
	};
}
