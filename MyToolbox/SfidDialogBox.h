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
		virtual void OnCut();
		virtual void OnCopy();
		virtual void OnPaste();
		virtual void OnDelete();
		virtual void OnSelectAll();
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

		void NewContent();
		void ChangeContent(LONGLONG);
		void ApplyModification();
		String ComputeChecksum(String) const;
		String Base62Encode(ULONGLONG, int) const;
		LONGLONG ComputeSerialNumber(String) const;
		bool ParseBase62(PCWSTR, int, PCWSTR) const;

		int m_State;
		int m_ActiveEdit;
	};
}
