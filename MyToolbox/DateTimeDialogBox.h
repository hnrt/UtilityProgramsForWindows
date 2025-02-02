#pragma once

#include "MyDialogBox.h"

namespace hnrt
{
	class DateTimeDialogBox
		: public MyDialogBox
	{
	public:

		DateTimeDialogBox();
		DateTimeDialogBox(const DateTimeDialogBox&) = delete;
		virtual ~DateTimeDialogBox() = default;
		void operator =(const DateTimeDialogBox&) = delete;
		virtual void OnTabSelectionChanging();
		virtual void OnTabSelectionChanged();
		virtual void OnNew();
		virtual void OnCopyResult();

	private:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnTimer(WPARAM wParam, LPARAM lParam);

		void OnOffsetChange();
		void ApplyModification();
		void GetDateTimeInUTC(SYSTEMTIME&) const;
		void SetDateTimeInUTC(SYSTEMTIME&);
		void UpdateDateTime();
		void FormatString(int id = 0);
		void UpdateEditReadOnly();
		void InitializeFixedButtons(DWORD) const;
		DWORD GetFixedButtonFlags() const;

		int m_offset;
		int m_format;
	};
}
