#include "pch.h"
#include "CronDialogBox.h"
#include "resource.h"
#include "hnrt/WindowLayoutSnapshot.h"
#include "hnrt/WindowDesign.h"
#include "hnrt/Clipboard.h"
#include "hnrt/ResourceString.h"
#include "hnrt/Exception.h"
#include "hnrt/Buffer.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/Time.h"
#include "hnrt/Debug.h"


#define CRON_TIMER1SEC 1


#define CRON_FORMAT_SECOND		(1U<<0)
#define CRON_FORMAT_MINUTE		(1U<<1)
#define CRON_FORMAT_HOUR		(1U<<2)
#define CRON_FORMAT_DAYOFMONTH	(1U<<3)
#define CRON_FORMAT_MONTH		(1U<<4)
#define CRON_FORMAT_DAYOFWEEK	(1U<<5)
#define CRON_FORMAT_YEAR		(1U<<6)
#define CRON_FORMAT_ALL			((1U<<7)-1U)


using namespace hnrt;


CronDialogBox::CronDialogBox()
	: DialogBox(IDD_CRON)
	, m_LastModifiedAt(0)
	, m_bParse(false)
	, m_bParseSuccessful(true)
	, m_bFormat(0)
	, m_bFormatSuccessful(0)
{
}


void CronDialogBox::OnCreate()
{
	StringBuffer desc(260);
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	desc.AppendFormat(L"Year: %d-%d ( , - * / )", st.wYear, CronValue::Max(CRON_YEAR));
	desc.AppendFormat(L"  Month: %d-%d or %s-%s ( , - * / )", CronValue::Min(CRON_MONTH), CronValue::Max(CRON_MONTH), MonthWords[0], MonthWords[11]);
	desc.AppendFormat(L"  Day: %d-%d ( , - * / ? L W )", CronValue::Min(CRON_DAYOFMONTH), CronValue::Max(CRON_DAYOFMONTH));
	desc.AppendFormat(L"\nDay Of the Week: %d-%d or %s-%s ( , - * / ? L # )", CronValue::Min(CRON_DAYOFWEEK), CronValue::Max(CRON_DAYOFWEEK), DayOfWeekWords[0], DayOfWeekWords[6]);
	desc.AppendFormat(L"\nHour: %d-%d ( , - * / )", CronValue::Min(CRON_HOUR), CronValue::Max(CRON_HOUR));
	desc.AppendFormat(L"  Minute: %d-%d ( , - * / )", CronValue::Min(CRON_MINUTE), CronValue::Max(CRON_MINUTE));
	desc.AppendFormat(L"  Second: %d-%d ( , - * / )", CronValue::Min(CRON_SECOND), CronValue::Max(CRON_SECOND));
	desc.AppendFormat(L"\nAsterisk means every year|month|day|hour|minute|second.");
	desc.AppendFormat(L"  Question mark matches any.");
	desc.AppendFormat(L"  Comma separates numbers or expressions.");
	desc.AppendFormat(L"  Hyphen specifies a range.");
	desc.AppendFormat(L" X-Y is equivalent to X-Y/1.");
	desc.AppendFormat(L"  Slash followed by a number specifies a step.");
	desc.AppendFormat(L" X/N means starting from X and incrementing by N.");
	desc.AppendFormat(L" X-Y/N means starting from X and incrementing by N until Y.");
	SetText(IDC_CRON_DESC_STATIC, desc);
	CheckButton(IDC_CRON_EXPR_RADIO);
	SetTimer(hwnd, CRON_TIMER1SEC, 250, NULL);
	SetText(IDC_CRON_EXPR_STATIC);
	SetText(IDC_CRON_EXPR_EDIT, m_cron.SecondEnabled ? L"* * * * * ?" : L"* * * * ?");
	OnSourceSelection(IDC_CRON_EXPR_RADIO);
	CheckButton(IDC_CRON_SECOND_CHECK, m_cron.SecondEnabled ? TRUE : FALSE);
	SetText(IDC_CRON_YEAR_EVAL_STATIC);
	SetText(IDC_CRON_MONTH_EVAL_STATIC);
	SetText(IDC_CRON_DAY_EVAL_STATIC);
	SetText(IDC_CRON_DOW_EVAL_STATIC);
	SetText(IDC_CRON_HOUR_EVAL_STATIC);
	SetText(IDC_CRON_MINUTE_EVAL_STATIC);
	SetText(IDC_CRON_SECOND_EVAL_STATIC);
	WCHAR sz[64] = { 0 };
	for (int offset = -23; offset < 24; offset++)
	{
		if (offset < 0)
		{
			swprintf_s(sz, L"-%02d:30", -offset);
			AddStringToComboBox(IDC_CRON_OFFSET_COMBO, sz);
			swprintf_s(sz, L"-%02d:00", -offset);
			AddStringToComboBox(IDC_CRON_OFFSET_COMBO, sz);
		}
		else if (offset == 0)
		{
			swprintf_s(sz, L"-00:30");
			AddStringToComboBox(IDC_CRON_OFFSET_COMBO, sz);
			swprintf_s(sz, L"+00:00");
			AddStringToComboBox(IDC_CRON_OFFSET_COMBO, sz);
			swprintf_s(sz, L"+00:30");
			AddStringToComboBox(IDC_CRON_OFFSET_COMBO, sz);
		}
		else
		{
			swprintf_s(sz, L"+%02d:00", offset);
			AddStringToComboBox(IDC_CRON_OFFSET_COMBO, sz);
			swprintf_s(sz, L"+%02d:30", offset);
			AddStringToComboBox(IDC_CRON_OFFSET_COMBO, sz);
		}
	}
	SetComboBoxSelection(IDC_CRON_OFFSET_COMBO, L"+00:00");
	Parse();
}


void CronDialogBox::OnDestroy()
{
}


void CronDialogBox::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
	WindowLayoutSnapshot before, after;

	before.AddAllChildren(hDlg).Clone(after);

	after[IDC_CRON_EXPR_EDIT].cx += cxDelta;
	after[IDC_CRON_EXPR_STATIC].cx += cxDelta;

	LONG cx, dx;

	cx = (after[IDC_CRON_EXPR_EDIT].cx - (before[IDC_CRON_YEAR_STATIC].HorizontalGap(before[IDC_CRON_MONTH_STATIC]) + before[IDC_CRON_MONTH_STATIC].HorizontalGap(before[IDC_CRON_DAY_STATIC]))) / 3;
	after[IDC_CRON_YEAR_STATIC].cx = cx;
	after[IDC_CRON_MONTH_STATIC].cx = cx;
	RepositionLeftByRight(after[IDC_CRON_MONTH_STATIC], before[IDC_CRON_YEAR_STATIC], after[IDC_CRON_YEAR_STATIC]);
	RepositionLeftByRight(after[IDC_CRON_DAY_STATIC], before[IDC_CRON_MONTH_STATIC], after[IDC_CRON_MONTH_STATIC]);
	after[IDC_CRON_DAY_STATIC].right = after[IDC_CRON_EXPR_EDIT].right;
	after[IDC_CRON_DOW_STATIC].left = after[IDC_CRON_DAY_STATIC].left;
	after[IDC_CRON_DOW_STATIC].right = after[IDC_CRON_DAY_STATIC].right;
	after[IDC_CRON_HOUR_STATIC].right = after[IDC_CRON_YEAR_STATIC].right;
	after[IDC_CRON_MINUTE_STATIC].left = after[IDC_CRON_MONTH_STATIC].left;
	after[IDC_CRON_MINUTE_STATIC].right = after[IDC_CRON_MONTH_STATIC].right;
	after[IDC_CRON_SECOND_STATIC].left = after[IDC_CRON_DAY_STATIC].left;
	after[IDC_CRON_SECOND_STATIC].right = after[IDC_CRON_DAY_STATIC].right;

	dx = after[IDC_CRON_YEAR_STATIC].cx - before[IDC_CRON_YEAR_STATIC].cx;
	after[IDC_CRON_YEAR_EDIT].cx += dx;
	after[IDC_CRON_YEAR_EVAL_STATIC].cx += dx;
	after[IDC_CRON_HOUR_EDIT].cx += dx;
	after[IDC_CRON_HOUR_EVAL_STATIC].cx += dx;

	dx = after[IDC_CRON_MONTH_STATIC].left - before[IDC_CRON_MONTH_STATIC].left;
	MoveHorizontally(after[IDC_CRON_MONTH_ALL_RADIO], dx);
	MoveHorizontally(after[IDC_CRON_MONTH_EXPR_RADIO], dx);
	MoveHorizontally(after[IDC_CRON_MONTH_EDIT], dx);
	MoveHorizontally(after[IDC_CRON_MONTH_EVAL_STATIC], dx);
	MoveHorizontally(after[IDC_CRON_MINUTE_ALL_RADIO], dx);
	MoveHorizontally(after[IDC_CRON_MINUTE_EXPR_RADIO], dx);
	MoveHorizontally(after[IDC_CRON_MINUTE_EDIT], dx);
	MoveHorizontally(after[IDC_CRON_MINUTE_EVAL_STATIC], dx);
	dx = after[IDC_CRON_MONTH_STATIC].cx - before[IDC_CRON_MONTH_STATIC].cx;
	after[IDC_CRON_MONTH_EDIT].cx += dx;
	after[IDC_CRON_MONTH_EVAL_STATIC].cx += dx;
	after[IDC_CRON_MINUTE_EDIT].cx += dx;
	after[IDC_CRON_MINUTE_EVAL_STATIC].cx += dx;

	dx = after[IDC_CRON_DAY_STATIC].left - before[IDC_CRON_DAY_STATIC].left;
	MoveHorizontally(after[IDC_CRON_DAY_ALL_RADIO], dx);
	MoveHorizontally(after[IDC_CRON_DAY_ANY_RADIO], dx);
	MoveHorizontally(after[IDC_CRON_DAY_LASTDAY_RADIO], dx);
	MoveHorizontally(after[IDC_CRON_DAY_WEEKDAY_RADIO], dx);
	MoveHorizontally(after[IDC_CRON_DAY_EXPR_RADIO], dx);
	MoveHorizontally(after[IDC_CRON_DAY_EDIT], dx);
	MoveHorizontally(after[IDC_CRON_DAY_EVAL_STATIC], dx);
	MoveHorizontally(after[IDC_CRON_DOW_ALL_RADIO], dx);
	MoveHorizontally(after[IDC_CRON_DOW_ANY_RADIO], dx);
	MoveHorizontally(after[IDC_CRON_DOW_LASTDAY_RADIO], dx);
	MoveHorizontally(after[IDC_CRON_DOW_EXPR_RADIO], dx);
	MoveHorizontally(after[IDC_CRON_DOW_EDIT], dx);
	MoveHorizontally(after[IDC_CRON_DOW_EVAL_STATIC], dx);
	MoveHorizontally(after[IDC_CRON_SECOND_ALL_RADIO], dx);
	MoveHorizontally(after[IDC_CRON_SECOND_EXPR_RADIO], dx);
	MoveHorizontally(after[IDC_CRON_SECOND_EDIT], dx);
	MoveHorizontally(after[IDC_CRON_SECOND_EVAL_STATIC], dx);
	dx = after[IDC_CRON_DAY_STATIC].cx - before[IDC_CRON_DAY_STATIC].cx;
	after[IDC_CRON_DAY_EDIT].cx += dx;
	after[IDC_CRON_DAY_EVAL_STATIC].cx += dx;
	after[IDC_CRON_DOW_EDIT].cx += dx;
	after[IDC_CRON_DOW_EVAL_STATIC].cx += dx;
	after[IDC_CRON_SECOND_EDIT].cx += dx;
	after[IDC_CRON_SECOND_EVAL_STATIC].cx += dx;

	after[IDC_CRON_DESC_STATIC].right = after[IDC_CRON_MONTH_STATIC].right;

	MoveHorizontally(after[IDC_CRON_COPY], cxDelta);
	MoveHorizontally(after[IDC_CRON_OFFSET_COMBO], cxDelta);
	MoveHorizontally(after[IDC_CRON_SECOND_CHECK], cxDelta);

	after.Apply();
}


static UINT GetFormatFlag(int id)
{
	switch (id)
	{
	case IDC_CRON_YEAR_EDIT:
	case IDC_CRON_YEAR_EVAL_STATIC:
		return CRON_FORMAT_YEAR;
	case IDC_CRON_MONTH_EDIT:
	case IDC_CRON_MONTH_EVAL_STATIC:
		return CRON_FORMAT_MONTH;
	case IDC_CRON_DAY_EDIT:
	case IDC_CRON_DAY_EVAL_STATIC:
		return CRON_FORMAT_DAYOFMONTH;
	case IDC_CRON_DOW_EDIT:
	case IDC_CRON_DOW_EVAL_STATIC:
		return CRON_FORMAT_DAYOFWEEK;
	case IDC_CRON_HOUR_EDIT:
	case IDC_CRON_HOUR_EVAL_STATIC:
		return CRON_FORMAT_HOUR;
	case IDC_CRON_MINUTE_EDIT:
	case IDC_CRON_MINUTE_EVAL_STATIC:
		return CRON_FORMAT_MINUTE;
	case IDC_CRON_SECOND_EDIT:
	case IDC_CRON_SECOND_EVAL_STATIC:
		return CRON_FORMAT_SECOND;
	default:
		return 0;
	}
}


INT_PTR CronDialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	UINT idChild = LOWORD(wParam);
	UINT idNotif = HIWORD(wParam);
	switch (idChild)
	{
	case IDC_CRON_EXPR_RADIO:
	case IDC_CRON_INDI_RADIO:
		OnSourceSelection(idChild);
		break;
	case IDC_CRON_EXPR_EDIT:
		if (idNotif == EN_CHANGE)
		{
			if (GetButtonState(IDC_CRON_EXPR_RADIO) == BST_CHECKED)
			{
				if (!m_bParse)
				{
					ClearEvalStatics();
				}
				m_LastModifiedAt = GetCurrentMicroSeconds();
				m_bParse = true;
			}
		}
		break;
	case IDC_CRON_COPY:
		OnCopy();
		break;
	case IDC_CRON_YEAR_ALL_RADIO:
		DisableWindow(IDC_CRON_YEAR_EDIT);
		SetText(IDC_CRON_YEAR_EDIT, L"*");
		m_LastModifiedAt = GetCurrentMicroSeconds();
		m_bFormat = true;
		break;
	case IDC_CRON_YEAR_EXPR_RADIO:
		EnableWindow(IDC_CRON_YEAR_EDIT);
		break;
	case IDC_CRON_MONTH_ALL_RADIO:
		DisableWindow(IDC_CRON_MONTH_EDIT);
		SetText(IDC_CRON_MONTH_EDIT, L"*");
		m_LastModifiedAt = GetCurrentMicroSeconds();
		m_bFormat = true;
		break;
	case IDC_CRON_MONTH_EXPR_RADIO:
		EnableWindow(IDC_CRON_MONTH_EDIT);
		break;
	case IDC_CRON_DAY_ALL_RADIO:
		DisableWindow(IDC_CRON_DAY_EDIT);
		SetText(IDC_CRON_DAY_EDIT, L"*");
		if (GetButtonState(IDC_CRON_DOW_ALL_RADIO) == BST_CHECKED)
		{
			UncheckButton(IDC_CRON_DOW_ALL_RADIO);
			CheckButton(IDC_CRON_DOW_ANY_RADIO);
			SetText(IDC_CRON_DOW_EDIT, L"?");
		}
		m_LastModifiedAt = GetCurrentMicroSeconds();
		m_bFormat = true;
		break;
	case IDC_CRON_DAY_ANY_RADIO:
		DisableWindow(IDC_CRON_DAY_EDIT);
		SetText(IDC_CRON_DAY_EDIT, L"?");
		if (GetButtonState(IDC_CRON_DOW_ANY_RADIO) == BST_CHECKED)
		{
			UncheckButton(IDC_CRON_DOW_ANY_RADIO);
			CheckButton(IDC_CRON_DOW_ALL_RADIO);
			SetText(IDC_CRON_DOW_EDIT, L"*");
		}
		m_LastModifiedAt = GetCurrentMicroSeconds();
		m_bFormat = true;
		break;
	case IDC_CRON_DAY_LASTDAY_RADIO:
		DisableWindow(IDC_CRON_DAY_EDIT);
		SetText(IDC_CRON_DAY_EDIT, L"L");
		m_LastModifiedAt = GetCurrentMicroSeconds();
		m_bFormat = true;
		break;
	case IDC_CRON_DAY_WEEKDAY_RADIO:
		DisableWindow(IDC_CRON_DAY_EDIT);
		SetText(IDC_CRON_DAY_EDIT, L"W");
		m_LastModifiedAt = GetCurrentMicroSeconds();
		m_bFormat = true;
		break;
	case IDC_CRON_DAY_EXPR_RADIO:
		EnableWindow(IDC_CRON_DAY_EDIT);
		break;
	case IDC_CRON_DOW_ALL_RADIO:
		DisableWindow(IDC_CRON_DOW_EDIT);
		SetText(IDC_CRON_DOW_EDIT, L"*");
		if (GetButtonState(IDC_CRON_DAY_ALL_RADIO) == BST_CHECKED)
		{
			UncheckButton(IDC_CRON_DAY_ALL_RADIO);
			CheckButton(IDC_CRON_DAY_ANY_RADIO);
			SetText(IDC_CRON_DAY_EDIT, L"?");
		}
		m_LastModifiedAt = GetCurrentMicroSeconds();
		m_bFormat = true;
		break;
	case IDC_CRON_DOW_ANY_RADIO:
		DisableWindow(IDC_CRON_DOW_EDIT);
		SetText(IDC_CRON_DOW_EDIT, L"?");
		if (GetButtonState(IDC_CRON_DAY_ANY_RADIO) == BST_CHECKED)
		{
			UncheckButton(IDC_CRON_DAY_ANY_RADIO);
			CheckButton(IDC_CRON_DAY_ALL_RADIO);
			SetText(IDC_CRON_DAY_EDIT, L"*");
		}
		m_LastModifiedAt = GetCurrentMicroSeconds();
		m_bFormat = true;
		break;
	case IDC_CRON_DOW_LASTDAY_RADIO:
		DisableWindow(IDC_CRON_DOW_EDIT);
		SetText(IDC_CRON_DOW_EDIT, L"L");
		m_LastModifiedAt = GetCurrentMicroSeconds();
		m_bFormat = true;
		break;
	case IDC_CRON_DOW_EXPR_RADIO:
		EnableWindow(IDC_CRON_DOW_EDIT);
		break;
	case IDC_CRON_HOUR_ALL_RADIO:
		DisableWindow(IDC_CRON_HOUR_EDIT);
		SetText(IDC_CRON_HOUR_EDIT, L"*");
		m_LastModifiedAt = GetCurrentMicroSeconds();
		m_bFormat = true;
		break;
	case IDC_CRON_HOUR_EXPR_RADIO:
		EnableWindow(IDC_CRON_HOUR_EDIT);
		break;
	case IDC_CRON_MINUTE_ALL_RADIO:
		DisableWindow(IDC_CRON_MINUTE_EDIT);
		SetText(IDC_CRON_MINUTE_EDIT, L"*");
		m_LastModifiedAt = GetCurrentMicroSeconds();
		m_bFormat = true;
		break;
	case IDC_CRON_MINUTE_EXPR_RADIO:
		EnableWindow(IDC_CRON_MINUTE_EDIT);
		break;
	case IDC_CRON_SECOND_ALL_RADIO:
		DisableWindow(IDC_CRON_SECOND_EDIT);
		SetText(IDC_CRON_SECOND_EDIT, L"*");
		m_LastModifiedAt = GetCurrentMicroSeconds();
		m_bFormat = true;
		break;
	case IDC_CRON_SECOND_EXPR_RADIO:
		EnableWindow(IDC_CRON_SECOND_EDIT);
		break;
	case IDC_CRON_YEAR_EDIT:
	case IDC_CRON_MONTH_EDIT:
	case IDC_CRON_DAY_EDIT:
	case IDC_CRON_DOW_EDIT:
	case IDC_CRON_HOUR_EDIT:
	case IDC_CRON_MINUTE_EDIT:
	case IDC_CRON_SECOND_EDIT:
		if (idNotif == EN_CHANGE)
		{
			if (GetButtonState(IDC_CRON_INDI_RADIO) == BST_CHECKED)
			{
				if (!(m_bFormat & GetFormatFlag(idChild)))
				{
					ClearEvalStatics(GetEvalStatic(idChild));
				}
				m_LastModifiedAt = GetCurrentMicroSeconds();
				m_bFormat |= GetFormatFlag(idChild);
			}
		}
		break;
	case IDC_CRON_SECOND_CHECK:
		OnSecondChanged();
		break;
	case IDC_CRON_OFFSET_COMBO:
		if (idNotif == CBN_SELCHANGE)
		{
			if (GetButtonState(IDC_CRON_INDI_RADIO) == BST_CHECKED)
			{
				m_bFormat = CRON_FORMAT_ALL;
				Format();
			}
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}


INT_PTR CronDialogBox::OnTimer(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case CRON_TIMER1SEC:
		if (m_bParse)
		{
			if (m_LastModifiedAt + 3000000 <= GetCurrentMicroSeconds())
			{
				Parse();
			}
		}
		else if (m_bFormat)
		{
			if (m_LastModifiedAt + 3000000 <= GetCurrentMicroSeconds())
			{
				Format();
			}
		}
		break;
	default:
		break;
	}
	return 0;
}


INT_PTR CronDialogBox::OnControlColorStatic(WPARAM wParam, LPARAM lParam)
{
	HDC hdc = reinterpret_cast<HDC>(wParam);
	int id = GetDlgCtrlID(reinterpret_cast<HWND>(lParam));
	switch (id)
	{
	case IDC_CRON_EXPR_STATIC:
		SetTextColor(hdc, m_bParseSuccessful ? RGB(51, 102, 0) : RGB(153, 0, 0));
		SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
		return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_3DFACE));
	case IDC_CRON_SECOND_EVAL_STATIC:
	case IDC_CRON_MINUTE_EVAL_STATIC:
	case IDC_CRON_HOUR_EVAL_STATIC:
	case IDC_CRON_DAY_EVAL_STATIC:
	case IDC_CRON_MONTH_EVAL_STATIC:
	case IDC_CRON_DOW_EVAL_STATIC:
	case IDC_CRON_YEAR_EVAL_STATIC:
		SetTextColor(hdc, (m_bFormatSuccessful & GetFormatFlag(id)) != 0 ? RGB(51, 102, 0) : RGB(153, 0, 0));
		SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
		return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_3DFACE));
	default:
		break;
	}
	return 0;
}


void CronDialogBox::OnSourceSelection(int id)
{
	EnableWindow(IDC_CRON_EXPR_EDIT, id == IDC_CRON_EXPR_RADIO);
	BOOL bIndividual = id == IDC_CRON_INDI_RADIO;
	EnableWindow(IDC_CRON_YEAR_ALL_RADIO, bIndividual);
	EnableWindow(IDC_CRON_YEAR_EXPR_RADIO, bIndividual);
	EnableWindow(IDC_CRON_YEAR_EDIT, bIndividual && (GetButtonState(IDC_CRON_YEAR_EXPR_RADIO) == BST_CHECKED));
	EnableWindow(IDC_CRON_MONTH_ALL_RADIO, bIndividual);
	EnableWindow(IDC_CRON_MONTH_EXPR_RADIO, bIndividual);
	EnableWindow(IDC_CRON_MONTH_EDIT, bIndividual && (GetButtonState(IDC_CRON_MONTH_EXPR_RADIO) == BST_CHECKED));
	EnableWindow(IDC_CRON_DAY_ALL_RADIO, bIndividual);
	EnableWindow(IDC_CRON_DAY_ANY_RADIO, bIndividual);
	EnableWindow(IDC_CRON_DAY_LASTDAY_RADIO, bIndividual);
	EnableWindow(IDC_CRON_DAY_WEEKDAY_RADIO, bIndividual);
	EnableWindow(IDC_CRON_DAY_EXPR_RADIO, bIndividual);
	EnableWindow(IDC_CRON_DAY_EDIT, bIndividual && (GetButtonState(IDC_CRON_DAY_EXPR_RADIO) == BST_CHECKED));
	EnableWindow(IDC_CRON_DOW_ALL_RADIO, bIndividual);
	EnableWindow(IDC_CRON_DOW_ANY_RADIO, bIndividual);
	EnableWindow(IDC_CRON_DOW_LASTDAY_RADIO, bIndividual);
	EnableWindow(IDC_CRON_DOW_EXPR_RADIO, bIndividual);
	EnableWindow(IDC_CRON_DOW_EDIT, bIndividual && (GetButtonState(IDC_CRON_DOW_EXPR_RADIO) == BST_CHECKED));
	EnableWindow(IDC_CRON_HOUR_ALL_RADIO, bIndividual);
	EnableWindow(IDC_CRON_HOUR_EXPR_RADIO, bIndividual);
	EnableWindow(IDC_CRON_HOUR_EDIT, bIndividual && (GetButtonState(IDC_CRON_HOUR_EXPR_RADIO) == BST_CHECKED));
	EnableWindow(IDC_CRON_MINUTE_ALL_RADIO, bIndividual);
	EnableWindow(IDC_CRON_MINUTE_EXPR_RADIO, bIndividual);
	EnableWindow(IDC_CRON_MINUTE_EDIT, bIndividual && (GetButtonState(IDC_CRON_MINUTE_EXPR_RADIO) == BST_CHECKED));
	BOOL bSecond = bIndividual && (GetButtonState(IDC_CRON_SECOND_CHECK) == BST_CHECKED);
	EnableWindow(IDC_CRON_SECOND_ALL_RADIO, bSecond);
	EnableWindow(IDC_CRON_SECOND_EXPR_RADIO, bSecond);
	EnableWindow(IDC_CRON_SECOND_EDIT, bSecond && (GetButtonState(IDC_CRON_SECOND_EXPR_RADIO) == BST_CHECKED));
}


void CronDialogBox::OnSecondChanged()
{
	m_cron.SecondEnabled = GetButtonState(IDC_CRON_SECOND_CHECK) == BST_CHECKED;
	if (m_cron.SecondEnabled)
	{
		BOOL bSecond = GetButtonState(IDC_CRON_INDI_RADIO) == BST_CHECKED;
		EnableWindow(IDC_CRON_SECOND_ALL_RADIO, bSecond);
		EnableWindow(IDC_CRON_SECOND_EXPR_RADIO, bSecond);
		EnableWindow(IDC_CRON_SECOND_EDIT, bSecond);
		ShowWindow(GetDlgItem(hwnd, IDC_CRON_SECOND_STATIC), SW_SHOW);
		ShowWindow(GetDlgItem(hwnd, IDC_CRON_SECOND_ALL_RADIO), SW_SHOW);
		ShowWindow(GetDlgItem(hwnd, IDC_CRON_SECOND_EXPR_RADIO), SW_SHOW);
		ShowWindow(GetDlgItem(hwnd, IDC_CRON_SECOND_EDIT), SW_SHOW);
		ShowWindow(GetDlgItem(hwnd, IDC_CRON_SECOND_EVAL_STATIC), SW_SHOW);
	}
	else
	{
		ShowWindow(GetDlgItem(hwnd, IDC_CRON_SECOND_STATIC), SW_HIDE);
		ShowWindow(GetDlgItem(hwnd, IDC_CRON_SECOND_ALL_RADIO), SW_HIDE);
		ShowWindow(GetDlgItem(hwnd, IDC_CRON_SECOND_EXPR_RADIO), SW_HIDE);
		ShowWindow(GetDlgItem(hwnd, IDC_CRON_SECOND_EDIT), SW_HIDE);
		ShowWindow(GetDlgItem(hwnd, IDC_CRON_SECOND_EVAL_STATIC), SW_HIDE);
	}
	Format();
}


void CronDialogBox::OnCopy()
{
	if (!Clipboard::Copy(hwnd, hwnd, IDC_CRON_EXPR_EDIT))
	{
		MessageBoxW(hwnd, ResourceString(IDS_MSG_CLIPBOARD_COPY_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
	}
}


void CronDialogBox::Parse()
{
	try
	{
		m_bParse = false;
		UINT cch = GetTextLength(IDC_CRON_EXPR_EDIT) + 1;
		Buffer<WCHAR> buf(cch);
		GetText(IDC_CRON_EXPR_EDIT, buf, cch);
		m_cron.Parse(buf);
		m_bParseSuccessful = true;
		SetText(IDC_CRON_EXPR_STATIC, L"OK");
	}
	catch (Exception ex)
	{
		m_bParseSuccessful = false;
		SetText(IDC_CRON_EXPR_STATIC, ex.Message);
	}

	if (m_cron.Year.type == CRON_ALL)
	{
		CheckButton(IDC_CRON_YEAR_ALL_RADIO);
		UncheckButton(IDC_CRON_YEAR_EXPR_RADIO);
	}
	else
	{
		UncheckButton(IDC_CRON_YEAR_ALL_RADIO);
		CheckButton(IDC_CRON_YEAR_EXPR_RADIO);
	}
	SetText(IDC_CRON_YEAR_EDIT, m_cron.Year);
	SetEvalText(IDC_CRON_YEAR_EVAL_STATIC, m_cron.Year);

	if (m_cron.Month.type == CRON_ALL)
	{
		CheckButton(IDC_CRON_MONTH_ALL_RADIO);
		UncheckButton(IDC_CRON_MONTH_EXPR_RADIO);
	}
	else
	{
		UncheckButton(IDC_CRON_MONTH_ALL_RADIO);
		CheckButton(IDC_CRON_MONTH_EXPR_RADIO);
	}
	SetText(IDC_CRON_MONTH_EDIT, m_cron.Month);
	SetEvalText(IDC_CRON_MONTH_EVAL_STATIC, m_cron.Month);

	if (m_cron.DayOfMonth.type == CRON_ALL)
	{
		CheckButton(IDC_CRON_DAY_ALL_RADIO);
		UncheckButton(IDC_CRON_DAY_ANY_RADIO);
		UncheckButton(IDC_CRON_DAY_LASTDAY_RADIO);
		UncheckButton(IDC_CRON_DAY_WEEKDAY_RADIO);
		UncheckButton(IDC_CRON_DAY_EXPR_RADIO);
	}
	else if (m_cron.DayOfMonth.type == CRON_ANY)
	{
		UncheckButton(IDC_CRON_DAY_ALL_RADIO);
		CheckButton(IDC_CRON_DAY_ANY_RADIO);
		UncheckButton(IDC_CRON_DAY_LASTDAY_RADIO);
		UncheckButton(IDC_CRON_DAY_WEEKDAY_RADIO);
		UncheckButton(IDC_CRON_DAY_EXPR_RADIO);
	}
	else if (m_cron.DayOfMonth.type == CRON_LASTDAY)
	{
		UncheckButton(IDC_CRON_DAY_ALL_RADIO);
		UncheckButton(IDC_CRON_DAY_ANY_RADIO);
		CheckButton(IDC_CRON_DAY_LASTDAY_RADIO);
		UncheckButton(IDC_CRON_DAY_WEEKDAY_RADIO);
		UncheckButton(IDC_CRON_DAY_EXPR_RADIO);
	}
	else if (m_cron.DayOfMonth.type == CRON_WEEKDAY)
	{
		UncheckButton(IDC_CRON_DAY_ALL_RADIO);
		UncheckButton(IDC_CRON_DAY_ANY_RADIO);
		UncheckButton(IDC_CRON_DAY_LASTDAY_RADIO);
		CheckButton(IDC_CRON_DAY_WEEKDAY_RADIO);
		UncheckButton(IDC_CRON_DAY_EXPR_RADIO);
	}
	else
	{
		UncheckButton(IDC_CRON_DAY_ALL_RADIO);
		UncheckButton(IDC_CRON_DAY_ANY_RADIO);
		UncheckButton(IDC_CRON_DAY_LASTDAY_RADIO);
		UncheckButton(IDC_CRON_DAY_WEEKDAY_RADIO);
		CheckButton(IDC_CRON_DAY_EXPR_RADIO);
	}
	SetText(IDC_CRON_DAY_EDIT, m_cron.DayOfMonth);
	SetEvalText(IDC_CRON_DAY_EVAL_STATIC, m_cron.DayOfMonth);

	if (m_cron.DayOfWeek.type == CRON_ALL)
	{
		CheckButton(IDC_CRON_DOW_ALL_RADIO);
		UncheckButton(IDC_CRON_DOW_ANY_RADIO);
		UncheckButton(IDC_CRON_DOW_LASTDAY_RADIO);
		UncheckButton(IDC_CRON_DOW_EXPR_RADIO);
	}
	else if (m_cron.DayOfWeek.type == CRON_ANY)
	{
		UncheckButton(IDC_CRON_DOW_ALL_RADIO);
		CheckButton(IDC_CRON_DOW_ANY_RADIO);
		UncheckButton(IDC_CRON_DOW_LASTDAY_RADIO);
		UncheckButton(IDC_CRON_DOW_EXPR_RADIO);
	}
	else if (m_cron.DayOfWeek.type == CRON_LASTDAY)
	{
		UncheckButton(IDC_CRON_DOW_ALL_RADIO);
		UncheckButton(IDC_CRON_DOW_ANY_RADIO);
		CheckButton(IDC_CRON_DOW_LASTDAY_RADIO);
		UncheckButton(IDC_CRON_DOW_EXPR_RADIO);
	}
	else
	{
		UncheckButton(IDC_CRON_DOW_ALL_RADIO);
		UncheckButton(IDC_CRON_DOW_ANY_RADIO);
		UncheckButton(IDC_CRON_DOW_LASTDAY_RADIO);
		CheckButton(IDC_CRON_DOW_EXPR_RADIO);
	}
	SetText(IDC_CRON_DOW_EDIT, m_cron.DayOfWeek);
	SetEvalText(IDC_CRON_DOW_EVAL_STATIC, m_cron.DayOfWeek);

	if (m_cron.Hour.type == CRON_ALL)
	{
		CheckButton(IDC_CRON_HOUR_ALL_RADIO);
		UncheckButton(IDC_CRON_HOUR_EXPR_RADIO);
	}
	else
	{
		UncheckButton(IDC_CRON_HOUR_ALL_RADIO);
		CheckButton(IDC_CRON_HOUR_EXPR_RADIO);
	}
	SetText(IDC_CRON_HOUR_EDIT, m_cron.Hour);
	SetEvalText(IDC_CRON_HOUR_EVAL_STATIC, m_cron.Hour);

	if (m_cron.Minute.type == CRON_ALL)
	{
		CheckButton(IDC_CRON_MINUTE_ALL_RADIO);
		UncheckButton(IDC_CRON_MINUTE_EXPR_RADIO);
	}
	else
	{
		UncheckButton(IDC_CRON_MINUTE_ALL_RADIO);
		CheckButton(IDC_CRON_MINUTE_EXPR_RADIO);
	}
	SetText(IDC_CRON_MINUTE_EDIT, m_cron.Minute);
	SetEvalText(IDC_CRON_MINUTE_EVAL_STATIC, m_cron.Minute);

	if (m_cron.Second.type == CRON_ALL)
	{
		CheckButton(IDC_CRON_SECOND_ALL_RADIO);
		UncheckButton(IDC_CRON_SECOND_EXPR_RADIO);
	}
	else
	{
		UncheckButton(IDC_CRON_SECOND_ALL_RADIO);
		CheckButton(IDC_CRON_SECOND_EXPR_RADIO);
	}
	SetText(IDC_CRON_SECOND_EDIT, m_cron.Second);
	SetEvalText(IDC_CRON_SECOND_EVAL_STATIC, m_cron.Second);
}


static PCWSTR Trim(PWSTR buf)
{
	PWCHAR p1 = buf;
	while (*p1 == L' ')
	{
		p1++;
	}
	PWCHAR p2 = p1 + wcslen(p1);
	while (p1 < p2 && p2[-1] == L' ')
	{
		*(--p2) = L'\0';
	}
	return p1;
}


void CronDialogBox::Format()
{
	Buffer<WCHAR> buf1;
	StringBuffer buf2(260);

	buf1.Resize(static_cast<size_t>(GetTextLength(IDC_CRON_SECOND_EDIT)) + 1);
	GetText(IDC_CRON_SECOND_EDIT, buf1, buf1.Len);
	if ((m_bFormat & CRON_FORMAT_SECOND))
	{
		try
		{
			m_cron.ParseSecond(buf1);
			m_bFormatSuccessful |= CRON_FORMAT_SECOND;
			SetEvalText(IDC_CRON_SECOND_EVAL_STATIC, m_cron.Second);
		}
		catch (Exception ex)
		{
			m_bFormatSuccessful &= ~CRON_FORMAT_SECOND;
			SetText(IDC_CRON_SECOND_EVAL_STATIC, ex.Message);
		}
	}
	if (GetButtonState(IDC_CRON_SECOND_CHECK) == BST_CHECKED)
	{
		buf2.AppendFormat(L"%s", Trim(buf1));
	}

	buf1.Resize(static_cast<size_t>(GetTextLength(IDC_CRON_MINUTE_EDIT)) + 1);
	GetText(IDC_CRON_MINUTE_EDIT, buf1, buf1.Len);
	if ((m_bFormat & CRON_FORMAT_MINUTE))
	{
		try
		{
			m_cron.ParseMinute(buf1);
			m_bFormatSuccessful |= CRON_FORMAT_MINUTE;
			SetEvalText(IDC_CRON_MINUTE_EVAL_STATIC, m_cron.Minute);
		}
		catch (Exception ex)
		{
			m_bFormatSuccessful &= ~CRON_FORMAT_MINUTE;
			SetText(IDC_CRON_MINUTE_EVAL_STATIC, ex.Message);
		}
	}
	buf2.AppendFormat(GetButtonState(IDC_CRON_SECOND_CHECK) == BST_CHECKED ? L" %s" : L"%s", Trim(buf1));

	buf1.Resize(static_cast<size_t>(GetTextLength(IDC_CRON_HOUR_EDIT)) + 1);
	GetText(IDC_CRON_HOUR_EDIT, buf1, buf1.Len);
	if ((m_bFormat & CRON_FORMAT_HOUR))
	{
		try
		{
			m_cron.ParseHour(buf1);
			m_bFormatSuccessful |= CRON_FORMAT_HOUR;
			SetEvalText(IDC_CRON_HOUR_EVAL_STATIC, m_cron.Hour);
		}
		catch (Exception ex)
		{
			m_bFormatSuccessful &= ~CRON_FORMAT_HOUR;
			SetText(IDC_CRON_HOUR_EVAL_STATIC, ex.Message);
		}
	}
	buf2.AppendFormat(L" %s", Trim(buf1));

	buf1.Resize(static_cast<size_t>(GetTextLength(IDC_CRON_DAY_EDIT)) + 1);
	GetText(IDC_CRON_DAY_EDIT, buf1, buf1.Len);
	if ((m_bFormat & CRON_FORMAT_DAYOFMONTH))
	{
		try
		{
			m_cron.ParseDayOfMonth(buf1);
			m_bFormatSuccessful |= CRON_FORMAT_DAYOFMONTH;
			SetEvalText(IDC_CRON_DAY_EVAL_STATIC, m_cron.DayOfMonth);
		}
		catch (Exception ex)
		{
			m_bFormatSuccessful &= ~CRON_FORMAT_DAYOFMONTH;
			SetText(IDC_CRON_DAY_EVAL_STATIC, ex.Message);
		}
	}
	buf2.AppendFormat(L" %s", Trim(buf1));

	buf1.Resize(static_cast<size_t>(GetTextLength(IDC_CRON_MONTH_EDIT)) + 1);
	GetText(IDC_CRON_MONTH_EDIT, buf1, buf1.Len);
	if ((m_bFormat & CRON_FORMAT_MONTH))
	{
		try
		{
			m_cron.ParseMonth(buf1);
			m_bFormatSuccessful |= CRON_FORMAT_MONTH;
			SetEvalText(IDC_CRON_MONTH_EVAL_STATIC, m_cron.Month);
		}
		catch (Exception ex)
		{
			m_bFormatSuccessful &= ~CRON_FORMAT_MONTH;
			SetText(IDC_CRON_MONTH_EVAL_STATIC, ex.Message);
		}
	}
	buf2.AppendFormat(L" %s", Trim(buf1));

	buf1.Resize(static_cast<size_t>(GetTextLength(IDC_CRON_DOW_EDIT)) + 1);
	GetText(IDC_CRON_DOW_EDIT, buf1, buf1.Len);
	if ((m_bFormat & CRON_FORMAT_DAYOFWEEK))
	{
		try
		{
			m_cron.ParseDayOfWeek(buf1);
			m_bFormatSuccessful |= CRON_FORMAT_DAYOFWEEK;
			SetEvalText(IDC_CRON_DOW_EVAL_STATIC, m_cron.DayOfWeek);
		}
		catch (Exception ex)
		{
			m_bFormatSuccessful &= ~CRON_FORMAT_DAYOFWEEK;
			SetText(IDC_CRON_DOW_EVAL_STATIC, ex.Message);
		}
	}
	buf2.AppendFormat(L" %s", Trim(buf1));

	buf1.Resize(static_cast<size_t>(GetTextLength(IDC_CRON_YEAR_EDIT)) + 1);
	GetText(IDC_CRON_YEAR_EDIT, buf1, buf1.Len);
	if ((m_bFormat & CRON_FORMAT_YEAR))
	{
		try
		{
			m_cron.ParseYear(buf1);
			m_bFormatSuccessful |= CRON_FORMAT_YEAR;
			SetEvalText(IDC_CRON_YEAR_EVAL_STATIC, m_cron.Year);
		}
		catch (Exception ex)
		{
			m_bFormatSuccessful &= ~CRON_FORMAT_YEAR;
			SetText(IDC_CRON_YEAR_EVAL_STATIC, ex.Message);
		}
	}
	buf2.AppendFormat(L" %s", Trim(buf1));

	m_bFormat = 0;

	SetText(IDC_CRON_EXPR_EDIT, Trim(buf2));

	try
	{
		m_cron.Parse(buf2);
		m_bParseSuccessful = true;
		SetText(IDC_CRON_EXPR_STATIC, L"OK");
	}
	catch (Exception ex)
	{
		m_bParseSuccessful = false;
		SetText(IDC_CRON_EXPR_STATIC, ex.Message);
	}
}


void CronDialogBox::SetEvalText(int id, const union CronValue& cv)
{
	if (cv.type == CRON_INVALID)
	{
		m_bFormatSuccessful &= ~GetFormatFlag(id);
		SetText(id);
	}
	else if (cv.type == CRON_ALL)
	{
		m_bFormatSuccessful |= GetFormatFlag(id);
		SetText(id);
	}
	else
	{
		m_bFormatSuccessful |= GetFormatFlag(id);
		SetText(id, cv.Evaluate(GetOffset()));
	}
}


void CronDialogBox::ClearEvalStatics(int id)
{
	SetText(IDC_CRON_EXPR_STATIC);
	if (id)
	{
		SetText(id);
	}
	else
	{
		SetText(IDC_CRON_SECOND_EVAL_STATIC);
		SetText(IDC_CRON_MINUTE_EVAL_STATIC);
		SetText(IDC_CRON_HOUR_EVAL_STATIC);
		SetText(IDC_CRON_DAY_EVAL_STATIC);
		SetText(IDC_CRON_MONTH_EVAL_STATIC);
		SetText(IDC_CRON_DOW_EVAL_STATIC);
		SetText(IDC_CRON_YEAR_EVAL_STATIC);
	}
}


int CronDialogBox::GetEvalStatic(int id)
{
	switch (id)
	{
	case IDC_CRON_YEAR_EDIT: return IDC_CRON_YEAR_EVAL_STATIC;
	case IDC_CRON_MONTH_EDIT: return IDC_CRON_MONTH_EVAL_STATIC;
	case IDC_CRON_DAY_EDIT: return IDC_CRON_DAY_EVAL_STATIC;
	case IDC_CRON_DOW_EDIT: return IDC_CRON_DOW_EVAL_STATIC;
	case IDC_CRON_HOUR_EDIT: return IDC_CRON_HOUR_EVAL_STATIC;
	case IDC_CRON_MINUTE_EDIT: return IDC_CRON_MINUTE_EVAL_STATIC;
	case IDC_CRON_SECOND_EDIT: return IDC_CRON_SECOND_EVAL_STATIC;
	default: throw Exception(L"CronDialogBox::GetEvalStatic: Bad id: %d", id);
	}
}


int CronDialogBox::GetOffset()
{
	int index = GetComboBoxSelection(IDC_CRON_OFFSET_COMBO);
	UINT cch = GetListBoxTextLength(IDC_CRON_OFFSET_COMBO, index);
	WCHAR sz[64];
	GetListBoxText(IDC_CRON_OFFSET_COMBO, index, sz);
	int sign = sz[0] == L'-' ? -1 : 1;
	int hour = wcstol(&sz[1], NULL, 10);
	int minute = wcstol(&sz[4], NULL, 10);
	return sign * (hour * 60 + minute);
}
