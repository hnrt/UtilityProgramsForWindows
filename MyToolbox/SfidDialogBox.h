#pragma once

#include "MyDialogBox.h"

namespace hnrt
{
	class SfidDialogBox
		: public MyDialogBox
	{
	public:

		SfidDialogBox();
		SfidDialogBox(const SfidDialogBox&) = delete;
		virtual ~SfidDialogBox() = default;
		void operator =(const SfidDialogBox&) = delete;
		virtual void OnTabSelectionChanging();
		virtual void OnTabSelectionChanged();
		virtual void OnCopyAll();
		virtual void OnClear();
		virtual void OnExecute();

	private:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnTimer(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnControlColorStatic(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnControlColorEdit(WPARAM wParam, LPARAM lParam);
		void OnEditChanged(int id);
		void UpdateControlsState();
		void ChangeContent(LONGLONG);
		void ApplyModification(int id);
		void SetStatusText(PCWSTR, ...);
		void SetStatusTextOnError(PCWSTR, ...);

		int m_State;
	};
}
