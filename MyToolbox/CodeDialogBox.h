#pragma once

#include "MyDialogBox.h"

namespace hnrt
{
	class CodeDialogBox
		: public MyDialogBox
	{
	public:

		CodeDialogBox();
		CodeDialogBox(const CodeDialogBox&) = delete;
		virtual ~CodeDialogBox() = default;
		void operator =(const CodeDialogBox&) = delete;
		virtual void OnTabSelectionChanging();
		virtual void OnTabSelectionChanged();

	private:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnTimer(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnControlColorEdit(WPARAM wParam, LPARAM lParam);
		void ApplyModification();
		void GTIN13Add(int delta);

		int m_GS1CPLength;
		int m_nGTIN13Change;
	};
}
