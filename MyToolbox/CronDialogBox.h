#pragma once


#include "MyDialogBox.h"
#include "hnrt/Time.h"
#include "hnrt/Cron.h"


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
		virtual void OnNew();
		virtual void OnCopyResult();
		virtual void OnSettingChanged(UINT);

	private:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnTimer(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnControlColorStatic(WPARAM wParam, LPARAM lParam);
		virtual void OnEditChanged(int id);
		void OnSecondChanged();
		void OnOffsetChanged();
		void Parse();
		void Format();
		void ClearExpression() const;
		void AppendToExpression(const String&) const;
		void UpdateAllElements();
		void UpdateElement(const CronValue& value, int idAll, int idAny, int idLast, int idWeek, int idExpr, int idEdit, int idStatic);
		void SetEvalText(int, const CronValue&);
		void ClearEvalStatics(int = 0) const;
		void EnableControls(DWORD dwFlags) const;
		void ShowSecondControls() const;
		void InitializeDescriptionStatic() const;
		static DWORD ControlIdToFlag(int id);
		static int GetEvalStatic(int);

		Cron m_cron;
		int m_offset;
		DWORD m_dwChanged;
		DWORD m_dwSuccessful;
	};
}
