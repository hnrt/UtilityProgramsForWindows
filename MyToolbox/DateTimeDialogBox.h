#pragma once


#include "hnrt/String.h"
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
		virtual void OnCopy();
		virtual void OnExecute();

	private:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnTimer(WPARAM wParam, LPARAM lParam);

		void OnOffsetChange();
		void ApplyModification();
		void GetDateTimeInUTC(SYSTEMTIME&) const;
		void SetDateTimeInUTC(SYSTEMTIME&) const;
		void UpdateDateTime() const;
		void FormatString(int id = 0);
		void UpdateEditReadOnly() const;
		void InitializeFixedButtons(DWORD) const;
		DWORD GetFixedButtonFlags() const;

		int m_offset;
		int m_format;
	};
}
