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
		void OnPaste();

	private:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnTimer(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnControlColorStatic(WPARAM wParam, LPARAM lParam);
		void OnSourceSelection(int);
		void OnSecondChanged();
		void OnOffsetChanged();
		void Parse();
		void Format();
		void ClearExpression();
		void AppendToExpression(PCWSTR);
		void UpdateIndividualControls();
		void UpdateValueControls(const CronValue& value, int idAll, int idAny, int idLast, int idWeek, int idExpr, int idEdit, int idStatic);
		void SetEvalText(int, const CronValue&);
		void ClearEvalStatics(int = 0);
		int GetEvalStatic(int);
		void ShowSecondControls();
		void InitializeOffsetComboBox(int);
		void SetOffsetComboBox(int, int offset);
		int GetOffsetComboBox(int);
		void InitializeDescriptionStatic();

		Cron m_cron;
		int m_offset;
		ULONGLONG m_LastModifiedAt;
		bool m_bParse;
		bool m_bParseSuccessful;
		UINT m_bFormat;
		UINT m_bFormatSuccessful;
	};
}
