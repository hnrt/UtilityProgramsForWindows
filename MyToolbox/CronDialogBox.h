#pragma once


#include "hnrt/DialogBox.h"
#include "Cron.h"


namespace hnrt
{
	class CronDialogBox
		: public DialogBox
	{
	public:

		CronDialogBox();
		CronDialogBox(const CronDialogBox&) = delete;
		virtual ~CronDialogBox() = default;
		void operator =(const CronDialogBox&) = delete;
		void OnCopy();

	private:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnTimer(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnControlColorStatic(WPARAM wParam, LPARAM lParam);
		void OnSourceSelection(int);
		void OnSecondChanged();
		void Parse();
		void Format();
		void SetEvalText(int, const union CronValue&);
		void ClearEvalStatics(int = 0);
		int GetEvalStatic(int);

		Cron m_cron;
		ULONGLONG m_LastModifiedAt;
		bool m_bParse;
		bool m_bParseSuccessful;
		UINT m_bFormat;
		UINT m_bFormatSuccessful;
	};
}
