#include "pch.h"
#include "CronDialogBox.h"
#include "resource.h"
#include "hnrt/CronError.h"
#include "hnrt/CronTokenizer.h"
#include "hnrt/Menu.h"
#include "hnrt/WindowLayoutSnapshot.h"
#include "hnrt/WindowDesign.h"
#include "hnrt/Clipboard.h"
#include "hnrt/ResourceString.h"
#include "hnrt/Exception.h"
#include "hnrt/Buffer.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/StringCommons.h"
#include "hnrt/Time.h"
#include "hnrt/RegistryKey.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/Debug.h"


#define REG_NAME_BASEOFFSET L"BaseOffset"
#define REG_NAME_DISPLAYOFFSET L"DisplayOffset"
#define REG_NAME_SECOND L"Second"
#define REG_NAME_EXPRESSION L"Expression"


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


static PCWSTR GetCronErrorText(CronError e, bool bDetails = false)
{
	static WCHAR sz[260];
	switch (e.type)
	{
	case CRON_ERROR_BADSEQUENCE:
		if (bDetails && e.element != CRON_ELEMENT_UNSPECIFIED)
		{
			swprintf_s(sz, L"%s: Bad sequence at %d.", CronValue::Name(e.element), e.offset);
		}
		else
		{
			swprintf_s(sz, L"Bad sequence at %d.", e.offset);
		}
		break;
	case CRON_ERROR_OUTOFRANGE:
		if (bDetails && e.element != CRON_ELEMENT_UNSPECIFIED)
		{
			swprintf_s(sz, L"%s: Value is out of range at %d.", CronValue::Name(e.element), e.offset);
		}
		else
		{
			swprintf_s(sz, L"Value is out of range at %d.", e.offset);
		}
		break;
	case CRON_ERROR_NOSTEP:
		if (bDetails && e.element != CRON_ELEMENT_UNSPECIFIED)
		{
			swprintf_s(sz, L"%s: Step number is missing right after / at %d.", CronValue::Name(e.element), e.offset);
		}
		else
		{
			swprintf_s(sz, L"Step number is missing right after / at % d.", e.offset);
		}
		break;
	case CRON_ERROR_NOUPPERBOUND:
		if (bDetails && e.element != CRON_ELEMENT_UNSPECIFIED)
		{
			swprintf_s(sz, L"%s: Upper bound number is missing right after - at %d.", CronValue::Name(e.element), e.offset);
		}
		else
		{
			swprintf_s(sz, L"Upper bound number is missing right after - at %d.", e.offset);
		}
		break;
	case CRON_ERROR_NOORDINALNUMBER:
		if (bDetails && e.element != CRON_ELEMENT_UNSPECIFIED)
		{
			swprintf_s(sz, L"%s: Ordinal number is missing right after # at %d.", CronValue::Name(e.element), e.offset);
		}
		else
		{
			swprintf_s(sz, L"Ordinal number is missing right after # at %d.", e.offset);
		}
		break;
	case CRON_ERROR_EXTRACHARACTER:
		swprintf_s(sz, L"Extra character at %d.", e.offset);
		break;
	case CRON_ERROR_DUPLICATEALL:
		swprintf_s(sz, L"Both day of the month and day of the week cannot be *.");
		break;
	case CRON_ERROR_DUPLICATEANY:
		swprintf_s(sz, L"Both day of the month and day of the week cannot be ?.");
		break;
	case CRON_ERROR_ANYDAYOFMONTHREQUIRED:
		swprintf_s(sz, L"Day of the month must be ? when day of the month is *.");
		break;
	case CRON_ERROR_ANYDAYOFWEEKREQUIRED:
		swprintf_s(sz, L"Day of the week must be ? when day of the week is *.");
		break;
	case CRON_ERROR_ANYDAYREQUIRED:
		swprintf_s(sz, L"Either day of the month or day of the week must be ?.");
		break;
	case CRON_ERROR_MULTIPLEORDINAL:
		swprintf_s(sz, L"# cannot be specified two or more times.");
		break;
	default:
		swprintf_s(sz, L"Unknown error: type=%d element=%d offset=%d", e.type, e.element, e.offset);
		break;
	}
	return sz;
}


CronDialogBox::CronDialogBox()
	: MyDialogBox(IDD_CRON, L"Cron")
	, m_offset(0)
	, m_LastModifiedAt(0)
	, m_bParse(false)
	, m_bParseSuccessful(true)
	, m_bFormat(0)
	, m_bFormatSuccessful(0)
{
}


void CronDialogBox::OnCreate()
{
	MyDialogBox::OnCreate();
	DWORD dwBaseOffset = UINT_MAX;
	DWORD dwDisplayOffset = UINT_MAX;
	String szExpression = String::Empty;
	RegistryKey hKey;
	LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, m_szRegistryKeyName);
	if (rc == ERROR_SUCCESS)
	{
		m_cron.SecondEnabled = RegistryValue::GetDWORD(hKey, REG_NAME_SECOND, 1) != 0;
		dwBaseOffset = RegistryValue::GetDWORD(hKey, REG_NAME_BASEOFFSET, UINT_MAX);
		dwDisplayOffset = RegistryValue::GetDWORD(hKey, REG_NAME_DISPLAYOFFSET, UINT_MAX);
		szExpression = RegistryValue::GetSZ(hKey, REG_NAME_EXPRESSION);
	}
	ClearEvalStatics();
	InitializeOffsetComboBox(IDC_CRON_EXPR_COMBO);
	InitializeOffsetComboBox(IDC_CRON_OFFSET_COMBO);
	InitializeDescriptionStatic();
	ShowSecondControls();
	ButtonCheck(IDC_CRON_EXPR_RADIO);
	OnSourceSelection(IDC_CRON_EXPR_RADIO);
	ButtonCheck(IDC_CRON_SECOND_CHECK, m_cron.SecondEnabled);
	if (dwBaseOffset != UINT_MAX)
	{
		ComboBoxSetSelection(IDC_CRON_EXPR_COMBO, dwBaseOffset);
		if (dwDisplayOffset != UINT_MAX)
		{
			ComboBoxSetSelection(IDC_CRON_OFFSET_COMBO, dwDisplayOffset);
			m_offset = dwDisplayOffset - dwBaseOffset;
		}
		else
		{
			ComboBoxSetSelection(IDC_CRON_OFFSET_COMBO, dwBaseOffset);
		}
	}
	if (szExpression.Len)
	{
		SetText(IDC_CRON_EXPR_EDIT, szExpression);
	}
	else
	{
		m_cron.SetAll();
		SetText(IDC_CRON_EXPR_EDIT, m_cron);
	}
	Parse();
	UpdateIndividualControls();
	m_menuView
		.Add(ResourceString(IDS_MENU_CRON), IDM_VIEW_CRON);
}


void CronDialogBox::OnDestroy()
{
	RegistryKey hKey;
	LSTATUS rc = hKey.Create(HKEY_CURRENT_USER, m_szRegistryKeyName);
	if (rc == ERROR_SUCCESS)
	{
		RegistryValue::SetDWORD(hKey, REG_NAME_SECOND, m_cron.SecondEnabled ? 1U : 0U);
		RegistryValue::SetDWORD(hKey, REG_NAME_BASEOFFSET, ComboBoxGetSelection(IDC_CRON_EXPR_COMBO));
		RegistryValue::SetDWORD(hKey, REG_NAME_DISPLAYOFFSET, ComboBoxGetSelection(IDC_CRON_OFFSET_COMBO));
		RegistryValue::SetSZ(hKey, REG_NAME_EXPRESSION, GetText(IDC_CRON_EXPR_EDIT));
	}
	MyDialogBox::OnDestroy();
}


void CronDialogBox::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
	WindowLayoutSnapshot before, after;

	before.AddAllChildren(hDlg).Clone(after);

	// RIGHT COLUMN
	{
		MoveHorizontally(after[IDC_CRON_COPY_BUTTON], cxDelta);
		MoveHorizontally(after[IDC_CRON_PASTE_BUTTON], cxDelta);
	}

	// TOP ROW
	{
		after[IDC_CRON_EXPR_EDIT].cx += cxDelta;
		MoveHorizontally(after[IDC_CRON_EXPR_COMBO], cxDelta);
		after[IDC_CRON_EXPR_STATIC].cx += cxDelta;
	}

	LONG dx, dcx, dy, dcy;

	// MIDDLE ROW
	{
		// ALL GROUPS (HORIZONTAL)
		ExtendHorizontally(after[IDC_CRON_YEAR_STATIC], after[IDC_CRON_MONTH_STATIC], after[IDC_CRON_DAY_STATIC], cxDelta);
		after[IDC_CRON_DOW_STATIC].left = after[IDC_CRON_DAY_STATIC].left;
		after[IDC_CRON_DOW_STATIC].right = after[IDC_CRON_DAY_STATIC].right;
		after[IDC_CRON_HOUR_STATIC].left = after[IDC_CRON_YEAR_STATIC].left;
		after[IDC_CRON_HOUR_STATIC].right = after[IDC_CRON_YEAR_STATIC].right;
		after[IDC_CRON_MINUTE_STATIC].left = after[IDC_CRON_MONTH_STATIC].left;
		after[IDC_CRON_MINUTE_STATIC].right = after[IDC_CRON_MONTH_STATIC].right;
		after[IDC_CRON_SECOND_STATIC].left = after[IDC_CRON_DAY_STATIC].left;
		after[IDC_CRON_SECOND_STATIC].right = after[IDC_CRON_DAY_STATIC].right;
		// COLUMN:YEAR-DESC-HOUR (HORIZONTAL)
		dx = 0;
		dcx = after[IDC_CRON_YEAR_STATIC].cx - before[IDC_CRON_YEAR_STATIC].cx;
		after[IDC_CRON_YEAR_EDIT].cx += dcx;
		after[IDC_CRON_YEAR_EVAL_STATIC].cx += dcx;
		after[IDC_CRON_DESC_STATIC].right = after[IDC_CRON_MONTH_STATIC].right + before[IDC_CRON_DESC_STATIC].right - before[IDC_CRON_MONTH_STATIC].right;
		after[IDC_CRON_HOUR_EDIT].cx += dcx;
		after[IDC_CRON_HOUR_EVAL_STATIC].cx += dcx;
		// COLUMN:MONTH-MINUTE (HORIZONTAL)
		dx = after[IDC_CRON_MONTH_STATIC].left - before[IDC_CRON_MONTH_STATIC].left;
		dcx = after[IDC_CRON_MONTH_STATIC].cx - before[IDC_CRON_MONTH_STATIC].cx;
		MoveHorizontally(after[IDC_CRON_MONTH_ALL_RADIO], dx);
		MoveHorizontally(after[IDC_CRON_MONTH_EXPR_RADIO], dx);
		MoveHorizontallyAndExtend(after[IDC_CRON_MONTH_EDIT], dx, dcx);
		MoveHorizontallyAndExtend(after[IDC_CRON_MONTH_EVAL_STATIC], dx, dcx);
		MoveHorizontally(after[IDC_CRON_MINUTE_ALL_RADIO], dx);
		MoveHorizontally(after[IDC_CRON_MINUTE_EXPR_RADIO], dx);
		MoveHorizontallyAndExtend(after[IDC_CRON_MINUTE_EDIT], dx, dcx);
		MoveHorizontallyAndExtend(after[IDC_CRON_MINUTE_EVAL_STATIC], dx, dcx);
		// COLUMN:DAY-DOW-SECOND (HORIZONTAL)
		dx = after[IDC_CRON_DAY_STATIC].left - before[IDC_CRON_DAY_STATIC].left;
		dcx = after[IDC_CRON_DAY_STATIC].cx - before[IDC_CRON_DAY_STATIC].cx;
		MoveHorizontally(after[IDC_CRON_DAY_ALL_RADIO], dx);
		MoveHorizontally(after[IDC_CRON_DAY_ANY_RADIO], dx);
		MoveHorizontally(after[IDC_CRON_DAY_LASTDAY_RADIO], dx);
		MoveHorizontally(after[IDC_CRON_DAY_WEEKDAY_RADIO], dx);
		MoveHorizontally(after[IDC_CRON_DAY_EXPR_RADIO], dx);
		MoveHorizontallyAndExtend(after[IDC_CRON_DAY_EDIT], dx, dcx);
		MoveHorizontallyAndExtend(after[IDC_CRON_DAY_EVAL_STATIC], dx, dcx);
		MoveHorizontally(after[IDC_CRON_DOW_ALL_RADIO], dx);
		MoveHorizontally(after[IDC_CRON_DOW_ANY_RADIO], dx);
		MoveHorizontally(after[IDC_CRON_DOW_EXPR_RADIO], dx);
		MoveHorizontallyAndExtend(after[IDC_CRON_DOW_EDIT], dx, dcx);
		MoveHorizontallyAndExtend(after[IDC_CRON_DOW_EVAL_STATIC], dx, dcx);
		MoveHorizontally(after[IDC_CRON_SECOND_ALL_RADIO], dx);
		MoveHorizontally(after[IDC_CRON_SECOND_EXPR_RADIO], dx);
		MoveHorizontallyAndExtend(after[IDC_CRON_SECOND_EDIT], dx, dcx);
		MoveHorizontallyAndExtend(after[IDC_CRON_SECOND_EVAL_STATIC], dx, dcx);
		// ALL GROUPS (VERTICAL)
		ExtendVertically(after[IDC_CRON_DAY_STATIC], after[IDC_CRON_DOW_STATIC], after[IDC_CRON_SECOND_STATIC], cyDelta);
		after[IDC_CRON_YEAR_STATIC].bottom = after[IDC_CRON_MONTH_STATIC].bottom = after[IDC_CRON_DAY_STATIC].bottom;
		after[IDC_CRON_HOUR_STATIC].top = after[IDC_CRON_MINUTE_STATIC].top = after[IDC_CRON_SECOND_STATIC].top;
		after[IDC_CRON_HOUR_STATIC].bottom = after[IDC_CRON_MINUTE_STATIC].bottom = after[IDC_CRON_SECOND_STATIC].bottom;
		// ROW:YEAR-MONTH-DAY (VERTICAL)
		dy = 0;
		dcy = after[IDC_CRON_YEAR_STATIC].cy - before[IDC_CRON_YEAR_STATIC].cy;
		after[IDC_CRON_YEAR_EVAL_STATIC].cy += dcy;
		after[IDC_CRON_MONTH_EVAL_STATIC].cy += dcy;
		after[IDC_CRON_DAY_EVAL_STATIC].cy += dcy;
		// ROW:DESC-DOW (VERTICAL)
		dy = after[IDC_CRON_DOW_STATIC].y - before[IDC_CRON_DOW_STATIC].y;
		dcy = after[IDC_CRON_DOW_STATIC].cy - before[IDC_CRON_DOW_STATIC].cy;
		MoveVerticallyAndExtend(after[IDC_CRON_DESC_STATIC], dy, dcy);
		MoveVertically(after[IDC_CRON_DOW_ALL_RADIO], dy);
		MoveVertically(after[IDC_CRON_DOW_ANY_RADIO], dy);
		MoveVertically(after[IDC_CRON_DOW_EXPR_RADIO], dy);
		MoveVertically(after[IDC_CRON_DOW_EDIT], dy);
		MoveVerticallyAndExtend(after[IDC_CRON_DOW_EVAL_STATIC], dy, dcy);
		// ROW:HOUR-MINUTE-SECOND (VERTICAL)
		dy = after[IDC_CRON_SECOND_STATIC].y - before[IDC_CRON_SECOND_STATIC].y;
		dcy = after[IDC_CRON_SECOND_STATIC].cy - before[IDC_CRON_SECOND_STATIC].cy;
		MoveVertically(after[IDC_CRON_HOUR_ALL_RADIO], dy);
		MoveVertically(after[IDC_CRON_HOUR_EXPR_RADIO], dy);
		MoveVertically(after[IDC_CRON_HOUR_EDIT], dy);
		MoveVerticallyAndExtend(after[IDC_CRON_HOUR_EVAL_STATIC], dy, dcy);
		MoveVertically(after[IDC_CRON_MINUTE_ALL_RADIO], dy);
		MoveVertically(after[IDC_CRON_MINUTE_EXPR_RADIO], dy);
		MoveVertically(after[IDC_CRON_MINUTE_EDIT], dy);
		MoveVerticallyAndExtend(after[IDC_CRON_MINUTE_EVAL_STATIC], dy, dcy);
		MoveVertically(after[IDC_CRON_SECOND_ALL_RADIO], dy);
		MoveVertically(after[IDC_CRON_SECOND_EXPR_RADIO], dy);
		MoveVertically(after[IDC_CRON_SECOND_EDIT], dy);
		MoveVerticallyAndExtend(after[IDC_CRON_SECOND_EVAL_STATIC], dy, dcy);
	}

	// BOTTOM ROW
	{
		RepositionLeft(after[IDC_CRON_SECOND_CHECK], after[IDC_CRON_SECOND_STATIC].left + before[IDC_CRON_SECOND_CHECK].left - before[IDC_CRON_SECOND_STATIC].left);
		MoveVertically(after[IDC_CRON_OFFSET_STATIC], cyDelta);
		MoveVertically(after[IDC_CRON_OFFSET_COMBO], cyDelta);
		MoveVertically(after[IDC_CRON_SECOND_CHECK], cyDelta);
	}

	after.Apply();
}


void CronDialogBox::OnTabSelectionChanging()
{
	MyDialogBox::OnTabSelectionChanging();
	KillTimer(hwnd, CRON_TIMER1SEC);
	m_menuView
		.Enable(IDM_VIEW_CRON, MF_ENABLED);
}


void CronDialogBox::OnTabSelectionChanged()
{
	MyDialogBox::OnTabSelectionChanged();
	m_menuEdit
		.RemoveAll()
		.Add(ResourceString(IDS_MENU_COPY), IDM_EDIT_COPY)
		.Add(ResourceString(IDS_MENU_PASTE), IDM_EDIT_PASTE);
	m_menuView
		.Enable(IDM_VIEW_CRON, MF_DISABLED);
	m_menuSettings
		.RemoveAll()
		.Add(ResourceString(IDS_MENU_USESECOND), IDM_SETTINGS_USESECOND, m_cron.SecondEnabled ? MF_CHECKED : MF_UNCHECKED);
	SetTimer(hwnd, CRON_TIMER1SEC, 1000, NULL);
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
			if (ButtonIsChecked(IDC_CRON_EXPR_RADIO))
			{
				if (!m_bParse)
				{
					m_bParse = true;
					ClearEvalStatics();
				}
				m_LastModifiedAt.FromSystemTime();
			}
		}
		break;
	case IDC_CRON_COPY_BUTTON:
		OnCopy();
		break;
	case IDC_CRON_PASTE_BUTTON:
		OnPaste();
		break;
	case IDC_CRON_YEAR_ALL_RADIO:
		DisableWindow(IDC_CRON_YEAR_EDIT);
		SetText(IDC_CRON_YEAR_EDIT, L"*");
		m_LastModifiedAt.FromSystemTime();
		m_bFormat |= CRON_FORMAT_YEAR;
		break;
	case IDC_CRON_YEAR_EXPR_RADIO:
		EnableWindow(IDC_CRON_YEAR_EDIT);
		break;
	case IDC_CRON_MONTH_ALL_RADIO:
		DisableWindow(IDC_CRON_MONTH_EDIT);
		SetText(IDC_CRON_MONTH_EDIT, L"*");
		m_LastModifiedAt.FromSystemTime();
		m_bFormat |= CRON_FORMAT_MONTH;
		break;
	case IDC_CRON_MONTH_EXPR_RADIO:
		EnableWindow(IDC_CRON_MONTH_EDIT);
		break;
	case IDC_CRON_DAY_ALL_RADIO:
		DisableWindow(IDC_CRON_DAY_EDIT);
		SetText(IDC_CRON_DAY_EDIT, L"*");
		if (!ButtonIsChecked(IDC_CRON_DOW_ANY_RADIO))
		{
			ButtonUncheck(IDC_CRON_DOW_ALL_RADIO);
			ButtonCheck(IDC_CRON_DOW_ANY_RADIO);
			ButtonUncheck(IDC_CRON_DOW_EXPR_RADIO);
			SetText(IDC_CRON_DOW_EDIT, L"?");
		}
		m_LastModifiedAt.FromSystemTime();
		m_bFormat |= CRON_FORMAT_DAYOFMONTH;
		break;
	case IDC_CRON_DAY_ANY_RADIO:
		DisableWindow(IDC_CRON_DAY_EDIT);
		SetText(IDC_CRON_DAY_EDIT, L"?");
		if (!ButtonIsChecked(IDC_CRON_DOW_ALL_RADIO))
		{
			ButtonCheck(IDC_CRON_DOW_ALL_RADIO);
			ButtonUncheck(IDC_CRON_DOW_ANY_RADIO);
			ButtonUncheck(IDC_CRON_DOW_EXPR_RADIO);
			SetText(IDC_CRON_DOW_EDIT, L"*");
		}
		m_LastModifiedAt.FromSystemTime();
		m_bFormat |= CRON_FORMAT_DAYOFMONTH;
		break;
	case IDC_CRON_DAY_LASTDAY_RADIO:
		DisableWindow(IDC_CRON_DAY_EDIT);
		SetText(IDC_CRON_DAY_EDIT, L"L");
		if (!ButtonIsChecked(IDC_CRON_DOW_ANY_RADIO))
		{
			ButtonUncheck(IDC_CRON_DOW_ALL_RADIO);
			ButtonCheck(IDC_CRON_DOW_ANY_RADIO);
			ButtonUncheck(IDC_CRON_DOW_EXPR_RADIO);
			SetText(IDC_CRON_DOW_EDIT, L"?");
		}
		m_LastModifiedAt.FromSystemTime();
		m_bFormat |= CRON_FORMAT_DAYOFMONTH;
		break;
	case IDC_CRON_DAY_WEEKDAY_RADIO:
		DisableWindow(IDC_CRON_DAY_EDIT);
		SetText(IDC_CRON_DAY_EDIT, L"W");
		if (!ButtonIsChecked(IDC_CRON_DOW_ANY_RADIO))
		{
			ButtonUncheck(IDC_CRON_DOW_ALL_RADIO);
			ButtonCheck(IDC_CRON_DOW_ANY_RADIO);
			ButtonUncheck(IDC_CRON_DOW_EXPR_RADIO);
			SetText(IDC_CRON_DOW_EDIT, L"?");
		}
		m_LastModifiedAt.FromSystemTime();
		m_bFormat |= CRON_FORMAT_DAYOFWEEK;
		break;
	case IDC_CRON_DAY_EXPR_RADIO:
		EnableWindow(IDC_CRON_DAY_EDIT);
		SetText(IDC_CRON_DAY_EDIT);
		if (!ButtonIsChecked(IDC_CRON_DOW_ANY_RADIO))
		{
			ButtonUncheck(IDC_CRON_DOW_ALL_RADIO);
			ButtonCheck(IDC_CRON_DOW_ANY_RADIO);
			ButtonUncheck(IDC_CRON_DOW_EXPR_RADIO);
			SetText(IDC_CRON_DOW_EDIT, L"?");
		}
		break;
	case IDC_CRON_DOW_ALL_RADIO:
		DisableWindow(IDC_CRON_DOW_EDIT);
		SetText(IDC_CRON_DOW_EDIT, L"*");
		if (!ButtonIsChecked(IDC_CRON_DAY_ANY_RADIO))
		{
			ButtonUncheck(IDC_CRON_DAY_ALL_RADIO);
			ButtonCheck(IDC_CRON_DAY_ANY_RADIO);
			ButtonUncheck(IDC_CRON_DAY_LASTDAY_RADIO);
			ButtonUncheck(IDC_CRON_DAY_WEEKDAY_RADIO);
			ButtonUncheck(IDC_CRON_DAY_EXPR_RADIO);
			SetText(IDC_CRON_DAY_EDIT, L"?");
		}
		m_LastModifiedAt.FromSystemTime();
		m_bFormat |= CRON_FORMAT_DAYOFWEEK;
		break;
	case IDC_CRON_DOW_ANY_RADIO:
		DisableWindow(IDC_CRON_DOW_EDIT);
		SetText(IDC_CRON_DOW_EDIT, L"?");
		if (!ButtonIsChecked(IDC_CRON_DAY_ALL_RADIO))
		{
			ButtonCheck(IDC_CRON_DAY_ALL_RADIO);
			ButtonUncheck(IDC_CRON_DAY_ANY_RADIO);
			ButtonUncheck(IDC_CRON_DAY_LASTDAY_RADIO);
			ButtonUncheck(IDC_CRON_DAY_WEEKDAY_RADIO);
			ButtonUncheck(IDC_CRON_DAY_EXPR_RADIO);
			SetText(IDC_CRON_DAY_EDIT, L"*");
		}
		m_LastModifiedAt.FromSystemTime();
		m_bFormat |= CRON_FORMAT_DAYOFWEEK;
		break;
	case IDC_CRON_DOW_EXPR_RADIO:
		EnableWindow(IDC_CRON_DOW_EDIT);
		SetText(IDC_CRON_DOW_EDIT);
		if (!ButtonIsChecked(IDC_CRON_DAY_ANY_RADIO))
		{
			ButtonUncheck(IDC_CRON_DAY_ALL_RADIO);
			ButtonCheck(IDC_CRON_DAY_ANY_RADIO);
			ButtonUncheck(IDC_CRON_DAY_LASTDAY_RADIO);
			ButtonUncheck(IDC_CRON_DAY_WEEKDAY_RADIO);
			ButtonUncheck(IDC_CRON_DAY_EXPR_RADIO);
			SetText(IDC_CRON_DAY_EDIT, L"?");
		}
		break;
	case IDC_CRON_HOUR_ALL_RADIO:
		DisableWindow(IDC_CRON_HOUR_EDIT);
		SetText(IDC_CRON_HOUR_EDIT, L"*");
		m_LastModifiedAt.FromSystemTime();
		m_bFormat |= CRON_FORMAT_HOUR;
		break;
	case IDC_CRON_HOUR_EXPR_RADIO:
		EnableWindow(IDC_CRON_HOUR_EDIT);
		break;
	case IDC_CRON_MINUTE_ALL_RADIO:
		DisableWindow(IDC_CRON_MINUTE_EDIT);
		SetText(IDC_CRON_MINUTE_EDIT, L"*");
		m_LastModifiedAt.FromSystemTime();
		m_bFormat |= CRON_FORMAT_MINUTE;
		break;
	case IDC_CRON_MINUTE_EXPR_RADIO:
		EnableWindow(IDC_CRON_MINUTE_EDIT);
		break;
	case IDC_CRON_SECOND_ALL_RADIO:
		DisableWindow(IDC_CRON_SECOND_EDIT);
		SetText(IDC_CRON_SECOND_EDIT, L"*");
		m_LastModifiedAt.FromSystemTime();
		m_bFormat |= CRON_FORMAT_SECOND;
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
			if (ButtonIsChecked(IDC_CRON_INDI_RADIO))
			{
				if (!(m_bFormat & GetFormatFlag(idChild)))
				{
					m_bFormat |= GetFormatFlag(idChild);
					ClearEvalStatics(GetEvalStatic(idChild));
				}
				m_LastModifiedAt.FromSystemTime();
			}
		}
		break;
	case IDC_CRON_SECOND_CHECK:
		OnSecondChanged();
		break;
	case IDC_CRON_EXPR_COMBO:
	case IDC_CRON_OFFSET_COMBO:
		if (idNotif == CBN_SELCHANGE)
		{
			OnOffsetChanged();
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
			if (m_LastModifiedAt <= FileTime().AddSeconds(-3))
			{
				Parse();
				UpdateIndividualControls();
			}
		}
		else if (m_bFormat)
		{
			if (m_LastModifiedAt <= FileTime().AddSeconds(-3))
			{
				Format();
				Parse();
			}
		}
		else if (m_bParseSuccessful)
		{
			int offset = ComboBoxGetSelection(IDC_CRON_EXPR_COMBO);
			SYSTEMTIME st = { 0 };
			GetSystemTime(&st);
			FileTime ft(st);
			ft.AddMinutes(offset);
			ft.ToSystemTime(st);
			if (m_cron.GetNextTime(st, st))
			{
				FileTime ft(st);
				ft.AddMinutes(m_offset);
				ft.ToSystemTime(st);
				WCHAR buf[260] = { 0 };
				swprintf_s(buf, L"OK  [ %04d-%02d-%02d %02d:%02d:%02d ]", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
				SetText(IDC_CRON_EXPR_STATIC, buf);
				DBGPUT(L"Cron: %s", buf);
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


void CronDialogBox::OnSourceSelection(int id) const
{
	BOOL bExpression = id == IDC_CRON_EXPR_RADIO;
	EnableWindow(IDC_CRON_EXPR_EDIT, bExpression);
	BOOL bIndividual = id == IDC_CRON_INDI_RADIO;
	EnableWindow(IDC_CRON_YEAR_ALL_RADIO, bIndividual);
	EnableWindow(IDC_CRON_YEAR_EXPR_RADIO, bIndividual);
	EnableWindow(IDC_CRON_YEAR_EDIT, bIndividual && ButtonIsChecked(IDC_CRON_YEAR_EXPR_RADIO));
	EnableWindow(IDC_CRON_MONTH_ALL_RADIO, bIndividual);
	EnableWindow(IDC_CRON_MONTH_EXPR_RADIO, bIndividual);
	EnableWindow(IDC_CRON_MONTH_EDIT, bIndividual && ButtonIsChecked(IDC_CRON_MONTH_EXPR_RADIO));
	EnableWindow(IDC_CRON_DAY_ALL_RADIO, bIndividual);
	EnableWindow(IDC_CRON_DAY_ANY_RADIO, bIndividual);
	EnableWindow(IDC_CRON_DAY_LASTDAY_RADIO, bIndividual);
	EnableWindow(IDC_CRON_DAY_WEEKDAY_RADIO, bIndividual);
	EnableWindow(IDC_CRON_DAY_EXPR_RADIO, bIndividual);
	EnableWindow(IDC_CRON_DAY_EDIT, bIndividual && ButtonIsChecked(IDC_CRON_DAY_EXPR_RADIO));
	EnableWindow(IDC_CRON_DOW_ALL_RADIO, bIndividual);
	EnableWindow(IDC_CRON_DOW_ANY_RADIO, bIndividual);
	EnableWindow(IDC_CRON_DOW_EXPR_RADIO, bIndividual);
	EnableWindow(IDC_CRON_DOW_EDIT, bIndividual && ButtonIsChecked(IDC_CRON_DOW_EXPR_RADIO));
	EnableWindow(IDC_CRON_HOUR_ALL_RADIO, bIndividual);
	EnableWindow(IDC_CRON_HOUR_EXPR_RADIO, bIndividual);
	EnableWindow(IDC_CRON_HOUR_EDIT, bIndividual && ButtonIsChecked(IDC_CRON_HOUR_EXPR_RADIO));
	EnableWindow(IDC_CRON_MINUTE_ALL_RADIO, bIndividual);
	EnableWindow(IDC_CRON_MINUTE_EXPR_RADIO, bIndividual);
	EnableWindow(IDC_CRON_MINUTE_EDIT, bIndividual && ButtonIsChecked(IDC_CRON_MINUTE_EXPR_RADIO));
	BOOL bSecond = bIndividual && ButtonIsChecked(IDC_CRON_SECOND_CHECK);
	EnableWindow(IDC_CRON_SECOND_ALL_RADIO, bSecond);
	EnableWindow(IDC_CRON_SECOND_EXPR_RADIO, bSecond);
	EnableWindow(IDC_CRON_SECOND_EDIT, bSecond && ButtonIsChecked(IDC_CRON_SECOND_EXPR_RADIO));
}


void CronDialogBox::OnSecondChanged()
{
	m_cron.SecondEnabled = ButtonIsChecked(IDC_CRON_SECOND_CHECK);
	ShowSecondControls();
	if (ButtonIsChecked(IDC_CRON_EXPR_RADIO))
	{
		UpdateIndividualControls();
	}
	m_bFormat = CRON_FORMAT_ALL;
	Format();
	Parse();
	m_menuSettings
		.Modify(
			IDM_SETTINGS_USESECOND, m_cron.SecondEnabled ? MF_CHECKED : MF_UNCHECKED,
			IDM_SETTINGS_USESECOND, ResourceString(IDS_MENU_USESECOND));
}


void CronDialogBox::OnOffsetChanged()
{
	int offset1 = ComboBoxGetSelection(IDC_CRON_EXPR_COMBO);
	int offset2 = ComboBoxGetSelection(IDC_CRON_OFFSET_COMBO);
	m_offset = offset2 - offset1;
	if (ButtonIsChecked(IDC_CRON_EXPR_RADIO))
	{
		Parse();
		UpdateIndividualControls();
	}
	else
	{
		m_bFormat = CRON_FORMAT_ALL;
		Format();
		Parse();
	}
}


void CronDialogBox::OnCopy()
{
	if (!Clipboard::Write(hwnd, hwnd, IDC_CRON_EXPR_EDIT))
	{
		MessageBoxW(hwnd, ResourceString(IDS_MSG_CLIPBOARD_COPY_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
	}
}


void CronDialogBox::OnPaste()
{
	if (!ButtonIsChecked(IDC_CRON_EXPR_RADIO))
	{
		ButtonCheck(IDC_CRON_EXPR_RADIO);
		ButtonUncheck(IDC_CRON_INDI_RADIO);
		OnSourceSelection(IDC_CRON_EXPR_RADIO);
	}
	String szText = Clipboard::Read(hwnd);
	if (szText)
	{
		SetText(IDC_CRON_EXPR_EDIT, szText);
	}
	else
	{
		MessageBoxW(hwnd, ResourceString(IDS_MSG_CLIPBOARD_COPY_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
	}
}


void CronDialogBox::OnSettingChanged(UINT uId)
{
	switch (uId)
	{
	case IDM_SETTINGS_USESECOND:
		ButtonCheck(IDC_CRON_SECOND_CHECK, !ButtonIsChecked(IDC_CRON_SECOND_CHECK));
		OnSecondChanged();
		break;
	default:
		break;
	}
}


void CronDialogBox::Parse()
{
	try
	{
		m_bParse = false;
		m_cron.Parse(GetText(IDC_CRON_EXPR_EDIT));
		m_bParseSuccessful = true;
		SetText(IDC_CRON_EXPR_STATIC, L"OK");
	}
	catch (CronError e)
	{
		m_bParseSuccessful = false;
		SetText(IDC_CRON_EXPR_STATIC, GetCronErrorText(e, true));
	}
}


void CronDialogBox::Format()
{
	ClearExpression();

	String szSecond = GetText(IDC_CRON_SECOND_EDIT).Trim();
	if ((m_bFormat & CRON_FORMAT_SECOND))
	{
		try
		{
			m_cron.ParseSecond(szSecond);
			m_bFormatSuccessful |= CRON_FORMAT_SECOND;
			SetEvalText(IDC_CRON_SECOND_EVAL_STATIC, m_cron.Second);
		}
		catch (CronError e)
		{
			m_bFormatSuccessful &= ~CRON_FORMAT_SECOND;
			SetText(IDC_CRON_SECOND_EVAL_STATIC, GetCronErrorText(e));
		}
	}
	if (ButtonIsChecked(IDC_CRON_SECOND_CHECK))
	{
		AppendToExpression(szSecond);
	}

	String szMinute = GetText(IDC_CRON_MINUTE_EDIT).Trim();
	if ((m_bFormat & CRON_FORMAT_MINUTE))
	{
		try
		{
			m_cron.ParseMinute(szMinute);
			m_bFormatSuccessful |= CRON_FORMAT_MINUTE;
			SetEvalText(IDC_CRON_MINUTE_EVAL_STATIC, m_cron.Minute);
		}
		catch (CronError e)
		{
			m_bFormatSuccessful &= ~CRON_FORMAT_MINUTE;
			SetText(IDC_CRON_MINUTE_EVAL_STATIC, GetCronErrorText(e));
		}
	}
	AppendToExpression(szMinute);

	String szHour = GetText(IDC_CRON_HOUR_EDIT).Trim();
	if ((m_bFormat & CRON_FORMAT_HOUR))
	{
		try
		{
			m_cron.ParseHour(szHour);
			m_bFormatSuccessful |= CRON_FORMAT_HOUR;
			SetEvalText(IDC_CRON_HOUR_EVAL_STATIC, m_cron.Hour);
		}
		catch (CronError e)
		{
			m_bFormatSuccessful &= ~CRON_FORMAT_HOUR;
			SetText(IDC_CRON_HOUR_EVAL_STATIC, GetCronErrorText(e));
		}
	}
	AppendToExpression(szHour);

	String szDay = GetText(IDC_CRON_DAY_EDIT).Trim();
	if ((m_bFormat & CRON_FORMAT_DAYOFMONTH))
	{
		try
		{
			m_cron.ParseDayOfMonth(szDay);
			m_bFormatSuccessful |= CRON_FORMAT_DAYOFMONTH;
			SetEvalText(IDC_CRON_DAY_EVAL_STATIC, m_cron.DayOfMonth);
		}
		catch (CronError e)
		{
			m_bFormatSuccessful &= ~CRON_FORMAT_DAYOFMONTH;
			SetText(IDC_CRON_DAY_EVAL_STATIC, GetCronErrorText(e));
		}
	}
	AppendToExpression(szDay);

	String szMonth = GetText(IDC_CRON_MONTH_EDIT).Trim();
	if ((m_bFormat & CRON_FORMAT_MONTH))
	{
		try
		{
			m_cron.ParseMonth(szMonth);
			m_bFormatSuccessful |= CRON_FORMAT_MONTH;
			SetEvalText(IDC_CRON_MONTH_EVAL_STATIC, m_cron.Month);
		}
		catch (CronError e)
		{
			m_bFormatSuccessful &= ~CRON_FORMAT_MONTH;
			SetText(IDC_CRON_MONTH_EVAL_STATIC, GetCronErrorText(e));
		}
	}
	AppendToExpression(szMonth);

	String szDayOfWeek = GetText(IDC_CRON_DOW_EDIT).Trim();
	if ((m_bFormat & CRON_FORMAT_DAYOFWEEK))
	{
		try
		{
			m_cron.ParseDayOfWeek(szDayOfWeek);
			m_bFormatSuccessful |= CRON_FORMAT_DAYOFWEEK;
			SetEvalText(IDC_CRON_DOW_EVAL_STATIC, m_cron.DayOfWeek);
		}
		catch (CronError e)
		{
			m_bFormatSuccessful &= ~CRON_FORMAT_DAYOFWEEK;
			SetText(IDC_CRON_DOW_EVAL_STATIC, GetCronErrorText(e));
		}
	}
	AppendToExpression(szDayOfWeek);

	String szYear = GetText(IDC_CRON_YEAR_EDIT).Trim();
	if ((m_bFormat & CRON_FORMAT_YEAR))
	{
		try
		{
			m_cron.ParseYear(szYear);
			m_bFormatSuccessful |= CRON_FORMAT_YEAR;
			SetEvalText(IDC_CRON_YEAR_EVAL_STATIC, m_cron.Year);
		}
		catch (CronError e)
		{
			m_bFormatSuccessful &= ~CRON_FORMAT_YEAR;
			SetText(IDC_CRON_YEAR_EVAL_STATIC, GetCronErrorText(e));
		}
	}
	AppendToExpression(szYear);

	m_bFormat = 0;
}


void CronDialogBox::ClearExpression() const
{
	SetText(IDC_CRON_EXPR_EDIT);
}


void CronDialogBox::AppendToExpression(const String& sz) const
{
	if (sz.Len)
	{
		SetText(IDC_CRON_EXPR_EDIT, String(PRINTF, L"%s %s", GetText(IDC_CRON_EXPR_EDIT), sz));
	}
}


void CronDialogBox::UpdateIndividualControls()
{
	UpdateValueControls(m_cron.Year, IDC_CRON_YEAR_ALL_RADIO, 0, 0, 0, IDC_CRON_YEAR_EXPR_RADIO, IDC_CRON_YEAR_EDIT, IDC_CRON_YEAR_EVAL_STATIC);
	UpdateValueControls(m_cron.Month, IDC_CRON_MONTH_ALL_RADIO, 0, 0, 0, IDC_CRON_MONTH_EXPR_RADIO, IDC_CRON_MONTH_EDIT, IDC_CRON_MONTH_EVAL_STATIC);
	UpdateValueControls(m_cron.DayOfMonth, IDC_CRON_DAY_ALL_RADIO, IDC_CRON_DAY_ANY_RADIO, IDC_CRON_DAY_LASTDAY_RADIO, IDC_CRON_DAY_WEEKDAY_RADIO, IDC_CRON_DAY_EXPR_RADIO, IDC_CRON_DAY_EDIT, IDC_CRON_DAY_EVAL_STATIC);
	UpdateValueControls(m_cron.DayOfWeek, IDC_CRON_DOW_ALL_RADIO, IDC_CRON_DOW_ANY_RADIO, 0, 0, IDC_CRON_DOW_EXPR_RADIO, IDC_CRON_DOW_EDIT, IDC_CRON_DOW_EVAL_STATIC);
	UpdateValueControls(m_cron.Hour, IDC_CRON_HOUR_ALL_RADIO, 0, 0, 0, IDC_CRON_HOUR_EXPR_RADIO, IDC_CRON_HOUR_EDIT, IDC_CRON_HOUR_EVAL_STATIC);
	UpdateValueControls(m_cron.Minute, IDC_CRON_MINUTE_ALL_RADIO, 0, 0, 0, IDC_CRON_MINUTE_EXPR_RADIO, IDC_CRON_MINUTE_EDIT, IDC_CRON_MINUTE_EVAL_STATIC);
	UpdateValueControls(m_cron.Second, IDC_CRON_SECOND_ALL_RADIO, 0, 0, 0, IDC_CRON_SECOND_EXPR_RADIO, IDC_CRON_SECOND_EDIT, IDC_CRON_SECOND_EVAL_STATIC);
}


void CronDialogBox::UpdateValueControls(const CronValue& value, int idAll, int idAny, int idLast, int idWeek, int idExpr, int idEdit, int idStatic)
{
	ButtonCheck(idAll, value.Type == CRON_ALL);
	if (idAny)
	{
		ButtonCheck(idAny, value.Type == CRON_ANY);
	}
	if (idLast)
	{
		ButtonCheck(idLast, value.Type == CRON_LASTDAY);
	}
	if (idWeek)
	{
		ButtonCheck(idWeek, value.Type == CRON_WEEKDAY);
	}
	ButtonCheck(idExpr, value.Type >= CRON_SINGLE);
	SetText(idEdit, value.ToString());
	SetEvalText(idStatic, value);
}


void CronDialogBox::SetEvalText(int id, const CronValue& value)
{
	if (value.Type == CRON_INVALID_VALUE)
	{
		m_bFormatSuccessful &= ~GetFormatFlag(id);
		SetText(id);
	}
	else
	{
		m_bFormatSuccessful |= GetFormatFlag(id);
		std::vector<int> ee = value.Enumerate(m_offset);
		StringBuffer buf(260);
		for (std::vector<int>::const_iterator iter = ee.cbegin(); iter != ee.cend(); iter++)
		{
			buf.AppendFormat(L" %d", *iter);
		}
		SetText(id, &buf[1]);
	}
}


void CronDialogBox::ClearEvalStatics(int id) const
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


void CronDialogBox::ShowSecondControls()
{
	int nCmdShow = m_cron.SecondEnabled ? SW_SHOW : SW_HIDE;
	ShowWindow(GetDlgItem(hwnd, IDC_CRON_SECOND_STATIC), nCmdShow);
	ShowWindow(GetDlgItem(hwnd, IDC_CRON_SECOND_ALL_RADIO), nCmdShow);
	ShowWindow(GetDlgItem(hwnd, IDC_CRON_SECOND_EXPR_RADIO), nCmdShow);
	ShowWindow(GetDlgItem(hwnd, IDC_CRON_SECOND_EDIT), nCmdShow);
	ShowWindow(GetDlgItem(hwnd, IDC_CRON_SECOND_EVAL_STATIC), nCmdShow);
}


void CronDialogBox::InitializeDescriptionStatic() const
{
	StringBuffer desc(260);
	desc.AppendFormat(L"Year: %d-%d ( , - * / )", CronValue::Min(CRON_YEAR), CronValue::Max(CRON_YEAR));
	desc.AppendFormat(L"  Month: %d-%d or %s-%s ( , - * / )", CronValue::Min(CRON_MONTH), CronValue::Max(CRON_MONTH), CronMonthWords[0], CronMonthWords[11]);
	desc.AppendFormat(L"  Day: %d-%d ( , - * / ? L W )", CronValue::Min(CRON_DAYOFMONTH), CronValue::Max(CRON_DAYOFMONTH));
	desc.AppendFormat(L"\nDay Of the Week: %d-%d or %s-%s ( , - * / ? L # )", CronValue::Min(CRON_DAYOFWEEK), CronValue::Max(CRON_DAYOFWEEK), CronDayOfWeekWords[0], CronDayOfWeekWords[6]);
	desc.AppendFormat(L"\nHour: %d-%d ( , - * / )", CronValue::Min(CRON_HOUR), CronValue::Max(CRON_HOUR));
	desc.AppendFormat(L"  Minute: %d-%d ( , - * / )", CronValue::Min(CRON_MINUTE), CronValue::Max(CRON_MINUTE));
	desc.AppendFormat(L"  Second: %d-%d ( , - * / )", CronValue::Min(CRON_SECOND), CronValue::Max(CRON_SECOND));
	desc.AppendFormat(L"\nAsterisk means every year|month|day|hour|minute|second.");
	desc.AppendFormat(L"  Question mark matches any.");
	desc.AppendFormat(L"  Comma separates numbers or expressions.");
	desc.AppendFormat(L"  Hyphen specifies a range;");
	desc.AppendFormat(L" X-Y is equivalent to X-Y/1.");
	desc.AppendFormat(L"  Slash followed by a number specifies a step;");
	desc.AppendFormat(L" X/N starts from X and increments by N.");
	desc.AppendFormat(L" X-Y/N starts from X and increments by N until Y.");
	SetText(IDC_CRON_DESC_STATIC, desc);
}
