#pragma once


#include "MyDialogBox.h"
#include "hnrt/Time.h"
#include "Cron.h"


namespace hnrt
{
	class CronDialogBox
		: public MyDialogBox
	{
	public:

		CronDialogBox();
		CronDialogBox(const CronDialogBox&) = delete;
		virtual ~CronDialogBox() = default;
		void operator =(const CronDialogBox&) = delete;
		virtual void OnTabSelectionChanging();
		virtual void OnTabSelectionChanged();
		virtual void OnCopy();
		virtual void OnPaste();
		virtual void OnSettingChanged(UINT);

	private:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnTimer(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnControlColorStatic(WPARAM wParam, LPARAM lParam);
		void OnSourceSelection(int) const;
		void OnSecondChanged();
		void OnOffsetChanged();
		void Parse();
		void Format();
		void ClearExpression() const;
		void AppendToExpression(const String&) const;
		void UpdateIndividualControls();
		void UpdateValueControls(const CronValue& value, int idAll, int idAny, int idLast, int idWeek, int idExpr, int idEdit, int idStatic);
		void SetEvalText(int, const CronValue&);
		void ClearEvalStatics(int = 0) const;
		int GetEvalStatic(int);
		void ShowSecondControls();
		void InitializeDescriptionStatic() const;

		Cron m_cron;
		int m_offset;
		FileTime m_LastModifiedAt;
		bool m_bParse;
		bool m_bParseSuccessful;
		UINT m_bFormat;
		UINT m_bFormatSuccessful;
	};
}
