#include "pch.h"
#include "CronDialogBox.h"
#include "resource.h"
#include "MyToolbox.h"
#include "hnrt/CronError.h"
#include "hnrt/CronTokenizer.h"
#include "hnrt/Menu.h"
#include "hnrt/WindowLayoutSnapshot.h"
#include "hnrt/WindowDesign.h"
#include "hnrt/WhileInScope.h"
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


using namespace hnrt;


constexpr auto REGVAL_BASEOFFSET = L"BaseOffset";
constexpr auto REGVAL_DISPLAYOFFSET = L"DisplayOffset";
constexpr auto REGVAL_SECOND = L"Second";
constexpr auto REGVAL_EXPRESSION = L"Expression";


constexpr auto FLAG_EXPRESSION = 1UL << 0;
constexpr auto FLAG_SECOND = 1UL << 1;
constexpr auto FLAG_MINUTE = 1UL << 2;
constexpr auto FLAG_HOUR = 1UL << 3;
constexpr auto FLAG_DAYOFMONTH = 1UL << 4;
constexpr auto FLAG_MONTH = 1UL << 5;
constexpr auto FLAG_DAYOFWEEK = 1UL << 6;
constexpr auto FLAG_YEAR = 1UL << 7;
constexpr auto FLAG_ALL = (1UL << 8) - 1;
constexpr auto FLAG_ELEMENT = FLAG_ALL & ~FLAG_EXPRESSION;


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
	, m_cron()
	, m_offset(0)
	, m_dwChanged(0)
	, m_dwSuccessful(0)
{
	m_LastModified.CursorChange = true;
	m_cron.SetAll();
}


void CronDialogBox::OnCreate()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	MyDialogBox::OnCreate();
	HFONT hFont = GetApp<MyToolbox>().GetFontForData();
	SetFont(IDC_CRON_EXPR_EDIT, hFont);
	SetFont(IDC_CRON_YEAR_EDIT, hFont);
	SetFont(IDC_CRON_MONTH_EDIT, hFont);
	SetFont(IDC_CRON_DAY_EDIT, hFont);
	SetFont(IDC_CRON_DOW_EDIT, hFont);
	SetFont(IDC_CRON_HOUR_EDIT, hFont);
	SetFont(IDC_CRON_MINUTE_EDIT, hFont);
	SetFont(IDC_CRON_SECOND_EDIT, hFont);
	DWORD dwBaseOffset = UINT_MAX;
	DWORD dwDisplayOffset = UINT_MAX;
	String szExpression = String::Empty;
	RegistryKey hKey;
	LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, m_szRegistryKeyName);
	if (rc == ERROR_SUCCESS)
	{
		m_cron.SecondEnabled = RegistryValue::GetDWORD(hKey, REGVAL_SECOND, 1) != 0;
		dwBaseOffset = RegistryValue::GetDWORD(hKey, REGVAL_BASEOFFSET, UINT_MAX);
		dwDisplayOffset = RegistryValue::GetDWORD(hKey, REGVAL_DISPLAYOFFSET, UINT_MAX);
		szExpression = RegistryValue::GetSZ(hKey, REGVAL_EXPRESSION);
	}
	ClearEvalStatics();
	InitializeOffsetComboBox(IDC_CRON_EXPR_COMBO);
	InitializeOffsetComboBox(IDC_CRON_OFFSET_COMBO);
	InitializeDescriptionStatic();
	ShowSecondControls();
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
	UpdateAllElements();
	EnableControls(FLAG_ALL);
	m_menuView
		.Add(ResourceString(IDS_MENU_CRON), IDM_VIEW_CRON);
}


void CronDialogBox::OnDestroy()
{
	RegistryKey hKey;
	LSTATUS rc = hKey.Create(HKEY_CURRENT_USER, m_szRegistryKeyName);
	if (rc == ERROR_SUCCESS)
	{
		RegistryValue::SetDWORD(hKey, REGVAL_SECOND, m_cron.SecondEnabled ? 1 : 0);
		RegistryValue::SetDWORD(hKey, REGVAL_BASEOFFSET, ComboBoxGetSelection(IDC_CRON_EXPR_COMBO));
		RegistryValue::SetDWORD(hKey, REGVAL_DISPLAYOFFSET, ComboBoxGetSelection(IDC_CRON_OFFSET_COMBO));
		RegistryValue::SetSZ(hKey, REGVAL_EXPRESSION, GetText(IDC_CRON_EXPR_EDIT));
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
	}

	// TOP ROW
	{
		after[IDC_CRON_EXPR_EDIT].cx += cxDelta;
		MoveHorizontally(after[IDC_CRON_EXPR_COMBO], cxDelta);
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
		after[IDC_CRON_DESC_EDIT].right = after[IDC_CRON_MONTH_STATIC].right + before[IDC_CRON_DESC_EDIT].right - before[IDC_CRON_MONTH_STATIC].right;
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
		MoveVerticallyAndExtend(after[IDC_CRON_DESC_EDIT], dy, dcy);
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
		RepositionLeft(after[IDC_CRON_SECOND_CHECK], after[IDC_CRON_SECOND_STATIC].left + before[IDC_CRON_SECOND_CHECK].left - before[IDC_CRON_SECOND_STATIC].left);
		RepositionTop(after[IDC_CRON_SECOND_CHECK], after[IDC_CRON_SECOND_STATIC].top);
		MoveVertically(after[IDC_CRON_SECOND_ALL_RADIO], dy);
		MoveVertically(after[IDC_CRON_SECOND_EXPR_RADIO], dy);
		MoveVertically(after[IDC_CRON_SECOND_EDIT], dy);
		MoveVerticallyAndExtend(after[IDC_CRON_SECOND_EVAL_STATIC], dy, dcy);
	}

	// FOOTER
	{
		after[IDC_CRON_EXPR_STATIC].cx += cxDelta;
		MoveVertically(after[IDC_CRON_EXPR_STATIC], cyDelta);
		MoveHorizontally(after[IDC_CRON_OFFSET_COMBO], cxDelta);
		MoveVertically(after[IDC_CRON_OFFSET_COMBO], cyDelta);
	}

	after.Apply();
}


void CronDialogBox::OnTabSelectionChanging()
{
	MyDialogBox::OnTabSelectionChanging();
	m_menuView
		.Enable(IDM_VIEW_CRON, MF_ENABLED);
}


void CronDialogBox::OnTabSelectionChanged()
{
	MyDialogBox::OnTabSelectionChanged();
	SetTimer(100);
	m_menuFile
		.Insert(0, ResourceString(IDS_MENU_NEW), IDM_FILE_NEW)
		.InsertSeparator(1);
	m_menuEdit
		.Insert(0, ResourceString(IDS_MENU_COPYRESULT), IDM_EDIT_COPYRESULT)
		.InsertSeparator(1);
	m_menuView
		.Enable(IDM_VIEW_CRON, MF_DISABLED);
	m_menuSettings
		.Add(ResourceString(IDS_MENU_USESECOND), IDM_SETTINGS_USESECOND, m_cron.SecondEnabled ? MF_CHECKED : MF_UNCHECKED);
}


INT_PTR CronDialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (m_cProcessing)
	{
		return TRUE;
	}
	if (MyDialogBox::OnCommand(wParam, lParam))
	{
		return TRUE;
	}
	UINT idChild = LOWORD(wParam);
	UINT idNotif = HIWORD(wParam);
	switch (idChild)
	{
	case IDC_CRON_COPY_BUTTON:
		if (idNotif == BN_CLICKED)
		{
			OnCopyResult();
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRON_EXPR_EDIT:
	case IDC_CRON_YEAR_EDIT:
	case IDC_CRON_MONTH_EDIT:
	case IDC_CRON_DAY_EDIT:
	case IDC_CRON_DOW_EDIT:
	case IDC_CRON_HOUR_EDIT:
	case IDC_CRON_MINUTE_EDIT:
	case IDC_CRON_SECOND_EDIT:
		return OnEditCommand(wParam, lParam);
	case IDC_CRON_YEAR_ALL_RADIO:
		if (idNotif == BN_CLICKED)
		{
			DisableWindow(IDC_CRON_YEAR_EDIT);
			SetText(IDC_CRON_YEAR_EDIT, L"*");
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRON_YEAR_EXPR_RADIO:
		if (idNotif == BN_CLICKED)
		{
			EnableWindow(IDC_CRON_YEAR_EDIT);
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRON_MONTH_ALL_RADIO:
		if (idNotif == BN_CLICKED)
		{
			DisableWindow(IDC_CRON_MONTH_EDIT);
			SetText(IDC_CRON_MONTH_EDIT, L"*");
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRON_MONTH_EXPR_RADIO:
		if (idNotif == BN_CLICKED)
		{
			EnableWindow(IDC_CRON_MONTH_EDIT);
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRON_DAY_ALL_RADIO:
		if (idNotif == BN_CLICKED)
		{
			DisableWindow(IDC_CRON_DAY_EDIT);
			SetText(IDC_CRON_DAY_EDIT, L"*");
			if (!ButtonIsChecked(IDC_CRON_DOW_ANY_RADIO))
			{
				WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
				ButtonUncheck(IDC_CRON_DOW_ALL_RADIO);
				ButtonCheck(IDC_CRON_DOW_ANY_RADIO);
				ButtonUncheck(IDC_CRON_DOW_EXPR_RADIO);
				DisableWindow(IDC_CRON_DOW_EDIT);
				SetText(IDC_CRON_DOW_EDIT, L"?");
			}
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRON_DAY_ANY_RADIO:
		if (idNotif == BN_CLICKED)
		{
			DisableWindow(IDC_CRON_DAY_EDIT);
			SetText(IDC_CRON_DAY_EDIT, L"?");
			if (!ButtonIsChecked(IDC_CRON_DOW_ALL_RADIO))
			{
				WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
				ButtonCheck(IDC_CRON_DOW_ALL_RADIO);
				ButtonUncheck(IDC_CRON_DOW_ANY_RADIO);
				ButtonUncheck(IDC_CRON_DOW_EXPR_RADIO);
				DisableWindow(IDC_CRON_DOW_EDIT);
				SetText(IDC_CRON_DOW_EDIT, L"*");
			}
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRON_DAY_LASTDAY_RADIO:
		if (idNotif == BN_CLICKED)
		{
			DisableWindow(IDC_CRON_DAY_EDIT);
			SetText(IDC_CRON_DAY_EDIT, L"L");
			if (!ButtonIsChecked(IDC_CRON_DOW_ANY_RADIO))
			{
				WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
				ButtonUncheck(IDC_CRON_DOW_ALL_RADIO);
				ButtonCheck(IDC_CRON_DOW_ANY_RADIO);
				ButtonUncheck(IDC_CRON_DOW_EXPR_RADIO);
				DisableWindow(IDC_CRON_DOW_EDIT);
				SetText(IDC_CRON_DOW_EDIT, L"?");
			}
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRON_DAY_WEEKDAY_RADIO:
		if (idNotif == BN_CLICKED)
		{
			DisableWindow(IDC_CRON_DAY_EDIT);
			SetText(IDC_CRON_DAY_EDIT, L"W");
			if (!ButtonIsChecked(IDC_CRON_DOW_ANY_RADIO))
			{
				WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
				ButtonUncheck(IDC_CRON_DOW_ALL_RADIO);
				ButtonCheck(IDC_CRON_DOW_ANY_RADIO);
				ButtonUncheck(IDC_CRON_DOW_EXPR_RADIO);
				DisableWindow(IDC_CRON_DOW_EDIT);
				SetText(IDC_CRON_DOW_EDIT, L"?");
			}
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRON_DAY_EXPR_RADIO:
		if (idNotif == BN_CLICKED)
		{
			EnableWindow(IDC_CRON_DAY_EDIT);
			if (!ButtonIsChecked(IDC_CRON_DOW_ANY_RADIO))
			{
				WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
				ButtonUncheck(IDC_CRON_DOW_ALL_RADIO);
				ButtonCheck(IDC_CRON_DOW_ANY_RADIO);
				ButtonUncheck(IDC_CRON_DOW_EXPR_RADIO);
				DisableWindow(IDC_CRON_DOW_EDIT);
				SetText(IDC_CRON_DOW_EDIT, L"?");
			}
			if (GetText(IDC_CRON_DAY_EDIT).Trim() == String(L"?"))
			{
				SetText(IDC_CRON_DAY_EDIT, L"*");
			}
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRON_DOW_ALL_RADIO:
		if (idNotif == BN_CLICKED)
		{
			DisableWindow(IDC_CRON_DOW_EDIT);
			SetText(IDC_CRON_DOW_EDIT, L"*");
			if (!ButtonIsChecked(IDC_CRON_DAY_ANY_RADIO))
			{
				WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
				ButtonUncheck(IDC_CRON_DAY_ALL_RADIO);
				ButtonCheck(IDC_CRON_DAY_ANY_RADIO);
				ButtonUncheck(IDC_CRON_DAY_LASTDAY_RADIO);
				ButtonUncheck(IDC_CRON_DAY_WEEKDAY_RADIO);
				ButtonUncheck(IDC_CRON_DAY_EXPR_RADIO);
				DisableWindow(IDC_CRON_DAY_EDIT);
				SetText(IDC_CRON_DAY_EDIT, L"?");
			}
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRON_DOW_ANY_RADIO:
		if (idNotif == BN_CLICKED)
		{
			DisableWindow(IDC_CRON_DOW_EDIT);
			SetText(IDC_CRON_DOW_EDIT, L"?");
			if (!ButtonIsChecked(IDC_CRON_DAY_ALL_RADIO))
			{
				WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
				ButtonCheck(IDC_CRON_DAY_ALL_RADIO);
				ButtonUncheck(IDC_CRON_DAY_ANY_RADIO);
				ButtonUncheck(IDC_CRON_DAY_LASTDAY_RADIO);
				ButtonUncheck(IDC_CRON_DAY_WEEKDAY_RADIO);
				ButtonUncheck(IDC_CRON_DAY_EXPR_RADIO);
				DisableWindow(IDC_CRON_DAY_EDIT);
				SetText(IDC_CRON_DAY_EDIT, L"*");
			}
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRON_DOW_EXPR_RADIO:
		if (idNotif == BN_CLICKED)
		{
			EnableWindow(IDC_CRON_DOW_EDIT);
			if (!ButtonIsChecked(IDC_CRON_DAY_ANY_RADIO))
			{
				WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
				ButtonUncheck(IDC_CRON_DAY_ALL_RADIO);
				ButtonCheck(IDC_CRON_DAY_ANY_RADIO);
				ButtonUncheck(IDC_CRON_DAY_LASTDAY_RADIO);
				ButtonUncheck(IDC_CRON_DAY_WEEKDAY_RADIO);
				ButtonUncheck(IDC_CRON_DAY_EXPR_RADIO);
				DisableWindow(IDC_CRON_DAY_EDIT);
				SetText(IDC_CRON_DAY_EDIT, L"?");
			}
			if (GetText(IDC_CRON_DOW_EDIT).Trim() == String(L"?"))
			{
				SetText(IDC_CRON_DOW_EDIT, L"*");
			}
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRON_HOUR_ALL_RADIO:
		if (idNotif == BN_CLICKED)
		{
			DisableWindow(IDC_CRON_HOUR_EDIT);
			SetText(IDC_CRON_HOUR_EDIT, L"*");
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRON_HOUR_EXPR_RADIO:
		if (idNotif == BN_CLICKED)
		{
			EnableWindow(IDC_CRON_HOUR_EDIT);
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRON_MINUTE_ALL_RADIO:
		if (idNotif == BN_CLICKED)
		{
			DisableWindow(IDC_CRON_MINUTE_EDIT);
			SetText(IDC_CRON_MINUTE_EDIT, L"*");
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRON_MINUTE_EXPR_RADIO:
		if (idNotif == BN_CLICKED)
		{
			EnableWindow(IDC_CRON_MINUTE_EDIT);
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRON_SECOND_ALL_RADIO:
		if (idNotif == BN_CLICKED)
		{
			DisableWindow(IDC_CRON_SECOND_EDIT);
			SetText(IDC_CRON_SECOND_EDIT, L"*");
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRON_SECOND_EXPR_RADIO:
		if (idNotif == BN_CLICKED)
		{
			EnableWindow(IDC_CRON_SECOND_EDIT);
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRON_SECOND_CHECK:
		if (idNotif == BN_CLICKED)
		{
			OnSecondChanged();
		}
		else
		{
			return FALSE;
		}
		break;
	case IDC_CRON_EXPR_COMBO:
	case IDC_CRON_OFFSET_COMBO:
		if (idNotif == CBN_SELCHANGE)
		{
			OnOffsetChanged();
		}
		else
		{
			return FALSE;
		}
		break;
	case IDM_SETTINGS_USESECOND:
	{
		WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
		ButtonCheck(IDC_CRON_SECOND_CHECK, !ButtonIsChecked(IDC_CRON_SECOND_CHECK));
		OnSecondChanged();
		break;
	}
	default:
		return FALSE;
	}
	return TRUE;
}


INT_PTR CronDialogBox::OnTimer(WPARAM wParam, LPARAM lParam)
{
	MyDialogBox::OnTimer(wParam, lParam);
	if (wParam == TIMERID(Id, 100))
	{
		if ((m_dwChanged & FLAG_EXPRESSION))
		{
			if (m_LastModified.IsUpdateRequired)
			{
				WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
				Parse();
				UpdateAllElements();
				EnableControls(FLAG_ALL);
				m_LastModified.By = 0;
			}
		}
		else if ((m_dwChanged & FLAG_ELEMENT))
		{
			if (m_LastModified.IsUpdateRequired)
			{
				WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
				Format();
				Parse();
				EnableControls(FLAG_ALL);
				m_LastModified.By = 0;
			}
		}
	}
	else if (wParam == TIMERID(Id, 1000))
	{
		if (!m_dwChanged && (m_dwSuccessful & FLAG_EXPRESSION))
		{
			WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
			String status;
			int offset = ComboBoxGetSelection(IDC_CRON_EXPR_COMBO);
			SYSTEMTIME st = { 0 };
			FileTime().AddMinutes(offset).ToSystemTime(st);
			if (m_cron.GetNextTime(st, st))
			{
				FileTime(st).AddMinutes(m_offset).ToSystemTime(st);
				status.Format(L"OK [ %04d-%02d-%02d %02d:%02d:%02d ]", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			}
			else
			{
				status.Format(L"OK [ No more scheduled time ]");
			}
			if (GetText(IDC_CRON_EXPR_STATIC) != status)
			{
				SetText(IDC_CRON_EXPR_STATIC, status);
				DBGPUT(L"Cron: %s", status);
			}
		}
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
	case IDC_CRON_SECOND_EVAL_STATIC:
	case IDC_CRON_MINUTE_EVAL_STATIC:
	case IDC_CRON_HOUR_EVAL_STATIC:
	case IDC_CRON_DAY_EVAL_STATIC:
	case IDC_CRON_MONTH_EVAL_STATIC:
	case IDC_CRON_DOW_EVAL_STATIC:
	case IDC_CRON_YEAR_EVAL_STATIC:
		SetTextColor(hdc, (m_dwSuccessful & ControlIdToFlag(id)) != 0 ? RGB_SUCCESSFUL : RGB_ERROR);
		SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
		return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_3DFACE));
	default:
		break;
	}
	return 0;
}


void CronDialogBox::OnEditChanged(int id)
{
	MyDialogBox::OnEditChanged(id);
	switch (id)
	{
	case IDC_CRON_EXPR_EDIT:
		if (!(m_dwChanged & FLAG_EXPRESSION))
		{
			m_dwChanged |= FLAG_EXPRESSION;
			EnableControls(FLAG_EXPRESSION);
			ClearEvalStatics();
		}
		break;
	case IDC_CRON_YEAR_EDIT:
	case IDC_CRON_MONTH_EDIT:
	case IDC_CRON_DAY_EDIT:
	case IDC_CRON_DOW_EDIT:
	case IDC_CRON_HOUR_EDIT:
	case IDC_CRON_MINUTE_EDIT:
	case IDC_CRON_SECOND_EDIT:
		if (!(m_dwChanged & FLAG_ELEMENT))
		{
			EnableControls(FLAG_ELEMENT);
		}
		if (!(m_dwChanged & ControlIdToFlag(id)))
		{
			m_dwChanged |= ControlIdToFlag(id);
			ClearEvalStatics(GetEvalStatic(id));
		}
		break;
	default:
		break;
	}
}


void CronDialogBox::OnSecondChanged()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	m_cron.SecondEnabled = ButtonIsChecked(IDC_CRON_SECOND_CHECK);
	ShowSecondControls();
	m_dwChanged |= FLAG_ALL;
	Format();
	Parse();
	EnableControls(FLAG_ALL);
	m_menuSettings
		.Modify(
			IDM_SETTINGS_USESECOND, m_cron.SecondEnabled ? MF_CHECKED : MF_UNCHECKED,
			IDM_SETTINGS_USESECOND, ResourceString(IDS_MENU_USESECOND));
}


void CronDialogBox::OnOffsetChanged()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
	int offset1 = ComboBoxGetSelection(IDC_CRON_EXPR_COMBO);
	int offset2 = ComboBoxGetSelection(IDC_CRON_OFFSET_COMBO);
	m_offset = offset2 - offset1;
	Parse();
	UpdateAllElements();
	EnableControls(FLAG_ALL);
}


void CronDialogBox::OnNew()
{
	SetText(IDC_CRON_EXPR_EDIT, m_cron.SecondEnabled ? L"0 * * * * ? *" : L"* * * * ? *");
}


void CronDialogBox::OnCopyResult()
{
	CopyAllText(IDC_CRON_EXPR_EDIT);
}


void CronDialogBox::Parse()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);

	try
	{
		m_cron.Parse(GetText(IDC_CRON_EXPR_EDIT));
		m_dwSuccessful |= FLAG_EXPRESSION;
		SetText(IDC_CRON_EXPR_STATIC, L"OK");
	}
	catch (CronError e)
	{
		m_dwSuccessful &= ~FLAG_EXPRESSION;
		SetText(IDC_CRON_EXPR_STATIC, GetCronErrorText(e, true));
	}

	m_dwChanged &= ~FLAG_EXPRESSION;
}


void CronDialogBox::Format()
{
	WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);

	ClearExpression();

	String szSecond = GetText(IDC_CRON_SECOND_EDIT).Trim();
	if ((m_dwChanged & FLAG_SECOND))
	{
		try
		{
			m_cron.ParseSecond(szSecond);
			m_dwSuccessful |= FLAG_SECOND;
			SetEvalText(IDC_CRON_SECOND_EVAL_STATIC, m_cron.Second);
		}
		catch (CronError e)
		{
			m_dwSuccessful &= ~FLAG_SECOND;
			SetText(IDC_CRON_SECOND_EVAL_STATIC, GetCronErrorText(e));
		}
	}
	if (ButtonIsChecked(IDC_CRON_SECOND_CHECK))
	{
		AppendToExpression(szSecond);
	}

	String szMinute = GetText(IDC_CRON_MINUTE_EDIT).Trim();
	if ((m_dwChanged & FLAG_MINUTE))
	{
		try
		{
			m_cron.ParseMinute(szMinute);
			m_dwSuccessful |= FLAG_MINUTE;
			SetEvalText(IDC_CRON_MINUTE_EVAL_STATIC, m_cron.Minute);
		}
		catch (CronError e)
		{
			m_dwSuccessful &= ~FLAG_MINUTE;
			SetText(IDC_CRON_MINUTE_EVAL_STATIC, GetCronErrorText(e));
		}
	}
	AppendToExpression(szMinute);

	String szHour = GetText(IDC_CRON_HOUR_EDIT).Trim();
	if ((m_dwChanged & FLAG_HOUR))
	{
		try
		{
			m_cron.ParseHour(szHour);
			m_dwSuccessful |= FLAG_HOUR;
			SetEvalText(IDC_CRON_HOUR_EVAL_STATIC, m_cron.Hour);
		}
		catch (CronError e)
		{
			m_dwSuccessful &= ~FLAG_HOUR;
			SetText(IDC_CRON_HOUR_EVAL_STATIC, GetCronErrorText(e));
		}
	}
	AppendToExpression(szHour);

	String szDay = GetText(IDC_CRON_DAY_EDIT).Trim();
	if ((m_dwChanged & FLAG_DAYOFMONTH))
	{
		try
		{
			m_cron.ParseDayOfMonth(szDay);
			m_dwSuccessful |= FLAG_DAYOFMONTH;
			SetEvalText(IDC_CRON_DAY_EVAL_STATIC, m_cron.DayOfMonth);
		}
		catch (CronError e)
		{
			m_dwSuccessful &= ~FLAG_DAYOFMONTH;
			SetText(IDC_CRON_DAY_EVAL_STATIC, GetCronErrorText(e));
		}
	}
	AppendToExpression(szDay);

	String szMonth = GetText(IDC_CRON_MONTH_EDIT).Trim();
	if ((m_dwChanged & FLAG_MONTH))
	{
		try
		{
			m_cron.ParseMonth(szMonth);
			m_dwSuccessful |= FLAG_MONTH;
			SetEvalText(IDC_CRON_MONTH_EVAL_STATIC, m_cron.Month);
		}
		catch (CronError e)
		{
			m_dwSuccessful &= ~FLAG_MONTH;
			SetText(IDC_CRON_MONTH_EVAL_STATIC, GetCronErrorText(e));
		}
	}
	AppendToExpression(szMonth);

	String szDayOfWeek = GetText(IDC_CRON_DOW_EDIT).Trim();
	if ((m_dwChanged & FLAG_DAYOFWEEK))
	{
		try
		{
			m_cron.ParseDayOfWeek(szDayOfWeek);
			m_dwSuccessful |= FLAG_DAYOFWEEK;
			SetEvalText(IDC_CRON_DOW_EVAL_STATIC, m_cron.DayOfWeek);
		}
		catch (CronError e)
		{
			m_dwSuccessful &= ~FLAG_DAYOFWEEK;
			SetText(IDC_CRON_DOW_EVAL_STATIC, GetCronErrorText(e));
		}
	}
	AppendToExpression(szDayOfWeek);

	String szYear = GetText(IDC_CRON_YEAR_EDIT).Trim();
	if ((m_dwChanged & FLAG_YEAR))
	{
		try
		{
			m_cron.ParseYear(szYear);
			m_dwSuccessful |= FLAG_YEAR;
			SetEvalText(IDC_CRON_YEAR_EVAL_STATIC, m_cron.Year);
		}
		catch (CronError e)
		{
			m_dwSuccessful &= ~FLAG_YEAR;
			SetText(IDC_CRON_YEAR_EVAL_STATIC, GetCronErrorText(e));
		}
	}
	AppendToExpression(szYear);

	m_dwChanged &= ~FLAG_ELEMENT;
}


void CronDialogBox::ClearExpression() const
{
	SetText(IDC_CRON_EXPR_EDIT);
}


void CronDialogBox::AppendToExpression(const String& sz) const
{
	if (sz.Len)
	{
		if (GetTextLength(IDC_CRON_EXPR_EDIT) > 0)
		{
			SetText(IDC_CRON_EXPR_EDIT, String(PRINTF, L"%s %s", GetText(IDC_CRON_EXPR_EDIT), sz));
		}
		else
		{
			SetText(IDC_CRON_EXPR_EDIT, sz);
		}
	}
}


void CronDialogBox::UpdateAllElements()
{
	UpdateElement(m_cron.Year, IDC_CRON_YEAR_ALL_RADIO, 0, 0, 0, IDC_CRON_YEAR_EXPR_RADIO, IDC_CRON_YEAR_EDIT, IDC_CRON_YEAR_EVAL_STATIC);
	UpdateElement(m_cron.Month, IDC_CRON_MONTH_ALL_RADIO, 0, 0, 0, IDC_CRON_MONTH_EXPR_RADIO, IDC_CRON_MONTH_EDIT, IDC_CRON_MONTH_EVAL_STATIC);
	UpdateElement(m_cron.DayOfMonth, IDC_CRON_DAY_ALL_RADIO, IDC_CRON_DAY_ANY_RADIO, IDC_CRON_DAY_LASTDAY_RADIO, IDC_CRON_DAY_WEEKDAY_RADIO, IDC_CRON_DAY_EXPR_RADIO, IDC_CRON_DAY_EDIT, IDC_CRON_DAY_EVAL_STATIC);
	UpdateElement(m_cron.DayOfWeek, IDC_CRON_DOW_ALL_RADIO, IDC_CRON_DOW_ANY_RADIO, 0, 0, IDC_CRON_DOW_EXPR_RADIO, IDC_CRON_DOW_EDIT, IDC_CRON_DOW_EVAL_STATIC);
	UpdateElement(m_cron.Hour, IDC_CRON_HOUR_ALL_RADIO, 0, 0, 0, IDC_CRON_HOUR_EXPR_RADIO, IDC_CRON_HOUR_EDIT, IDC_CRON_HOUR_EVAL_STATIC);
	UpdateElement(m_cron.Minute, IDC_CRON_MINUTE_ALL_RADIO, 0, 0, 0, IDC_CRON_MINUTE_EXPR_RADIO, IDC_CRON_MINUTE_EDIT, IDC_CRON_MINUTE_EVAL_STATIC);
	UpdateElement(m_cron.Second, IDC_CRON_SECOND_ALL_RADIO, 0, 0, 0, IDC_CRON_SECOND_EXPR_RADIO, IDC_CRON_SECOND_EDIT, IDC_CRON_SECOND_EVAL_STATIC);
}


void CronDialogBox::UpdateElement(const CronValue& value, int idAll, int idAny, int idLast, int idWeek, int idExpr, int idEdit, int idStatic)
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
	EnableWindow(idEdit, value.Type >= CRON_SINGLE);
	SetText(idEdit, value.ToString());
	SetEvalText(idStatic, value);
}


void CronDialogBox::SetEvalText(int id, const CronValue& value)
{
	if (value.Type == CRON_INVALID_VALUE)
	{
		m_dwSuccessful &= ~ControlIdToFlag(id);
		SetText(id);
	}
	else
	{
		m_dwSuccessful |= ControlIdToFlag(id);
		std::vector<int> ee = value.Enumerate(m_offset);
		StringBuffer buf(260);
		for (std::vector<int>::const_iterator iter = ee.cbegin(); iter != ee.cend(); iter++)
		{
			buf.AppendFormat(L" %d", *iter);
		}
		SetText(id, buf.Len > 0 ? &buf[1] : L"");
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


void CronDialogBox::EnableControls(DWORD dwFlags) const
{
	EnableWindow(IDC_CRON_EXPR_EDIT, !!(dwFlags & FLAG_EXPRESSION));
	EnableWindow(IDC_CRON_EXPR_COMBO, !!(dwFlags & FLAG_EXPRESSION));
	EnableWindow(IDC_CRON_YEAR_ALL_RADIO, !!(dwFlags & FLAG_YEAR));
	EnableWindow(IDC_CRON_YEAR_EXPR_RADIO, !!(dwFlags & FLAG_YEAR));
	EnableWindow(IDC_CRON_YEAR_EDIT, !!(dwFlags & FLAG_YEAR) && ButtonIsChecked(IDC_CRON_YEAR_EXPR_RADIO));
	EnableWindow(IDC_CRON_MONTH_ALL_RADIO, !!(dwFlags & FLAG_MONTH));
	EnableWindow(IDC_CRON_MONTH_EXPR_RADIO, !!(dwFlags & FLAG_MONTH));
	EnableWindow(IDC_CRON_MONTH_EDIT, !!(dwFlags & FLAG_MONTH) && ButtonIsChecked(IDC_CRON_MONTH_EXPR_RADIO));
	EnableWindow(IDC_CRON_DAY_ALL_RADIO, !!(dwFlags & FLAG_DAYOFMONTH));
	EnableWindow(IDC_CRON_DAY_ANY_RADIO, !!(dwFlags & FLAG_DAYOFMONTH));
	EnableWindow(IDC_CRON_DAY_LASTDAY_RADIO, !!(dwFlags & FLAG_DAYOFMONTH));
	EnableWindow(IDC_CRON_DAY_WEEKDAY_RADIO, !!(dwFlags & FLAG_DAYOFMONTH));
	EnableWindow(IDC_CRON_DAY_EXPR_RADIO, !!(dwFlags & FLAG_DAYOFMONTH));
	EnableWindow(IDC_CRON_DAY_EDIT, !!(dwFlags & FLAG_DAYOFMONTH) && ButtonIsChecked(IDC_CRON_DAY_EXPR_RADIO));
	EnableWindow(IDC_CRON_DOW_ALL_RADIO, !!(dwFlags & FLAG_DAYOFWEEK));
	EnableWindow(IDC_CRON_DOW_ANY_RADIO, !!(dwFlags & FLAG_DAYOFWEEK));
	EnableWindow(IDC_CRON_DOW_EXPR_RADIO, !!(dwFlags & FLAG_DAYOFWEEK));
	EnableWindow(IDC_CRON_DOW_EDIT, !!(dwFlags & FLAG_DAYOFWEEK) && ButtonIsChecked(IDC_CRON_DOW_EXPR_RADIO));
	EnableWindow(IDC_CRON_HOUR_ALL_RADIO, !!(dwFlags & FLAG_HOUR));
	EnableWindow(IDC_CRON_HOUR_EXPR_RADIO, !!(dwFlags & FLAG_HOUR));
	EnableWindow(IDC_CRON_HOUR_EDIT, !!(dwFlags & FLAG_HOUR) && ButtonIsChecked(IDC_CRON_HOUR_EXPR_RADIO));
	EnableWindow(IDC_CRON_MINUTE_ALL_RADIO, !!(dwFlags & FLAG_MINUTE));
	EnableWindow(IDC_CRON_MINUTE_EXPR_RADIO, !!(dwFlags & FLAG_MINUTE));
	EnableWindow(IDC_CRON_MINUTE_EDIT, !!(dwFlags & FLAG_MINUTE) && ButtonIsChecked(IDC_CRON_MINUTE_EXPR_RADIO));
	EnableWindow(IDC_CRON_SECOND_ALL_RADIO, !!(dwFlags & FLAG_SECOND));
	EnableWindow(IDC_CRON_SECOND_EXPR_RADIO, !!(dwFlags & FLAG_SECOND));
	EnableWindow(IDC_CRON_SECOND_EDIT, !!(dwFlags & FLAG_SECOND) && ButtonIsChecked(IDC_CRON_SECOND_EXPR_RADIO));
	EnableWindow(IDC_CRON_SECOND_CHECK, !!(dwFlags & FLAG_SECOND));
}


void CronDialogBox::ShowSecondControls() const
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
	desc.AppendFormat(L"\r\nMonth: %d-%d or %s-%s ( , - * / )", CronValue::Min(CRON_MONTH), CronValue::Max(CRON_MONTH), CronMonthWords[0], CronMonthWords[11]);
	desc.AppendFormat(L"\r\nDay: %d-%d ( , - * / ? L W )", CronValue::Min(CRON_DAYOFMONTH), CronValue::Max(CRON_DAYOFMONTH));
	desc.AppendFormat(L"\r\n    L means the last day of the month.");
	desc.AppendFormat(L"\r\n    nW means the weekday nearest day n of the month; e.g., 3W means the weekday nearest day 3 of the month.");
	desc.Append(L"\r\n");
	desc.AppendFormat(L"\r\nDay Of the Week: %d-%d or %s-%s ( , - * / ? L # )", CronValue::Min(CRON_DAYOFWEEK), CronValue::Max(CRON_DAYOFWEEK), CronDayOfWeekWords[0], CronDayOfWeekWords[6]);
	desc.AppendFormat(L"\r\n    wL means the last w-day of the month; e.g., 1L means the last Sunday of the month.");
	desc.AppendFormat(L"\r\n    w#n means the n-th w-day of the month; e.g., 7#1 means the first Saturday of the month.");
	desc.Append(L"\r\n");
	desc.AppendFormat(L"\r\nHour: %d-%d ( , - * / )", CronValue::Min(CRON_HOUR), CronValue::Max(CRON_HOUR));
	desc.AppendFormat(L"\r\nMinute: %d-%d ( , - * / )", CronValue::Min(CRON_MINUTE), CronValue::Max(CRON_MINUTE));
	desc.AppendFormat(L"\r\nSecond: %d-%d ( , - * / )", CronValue::Min(CRON_SECOND), CronValue::Max(CRON_SECOND));
	desc.Append(L"\r\n");
	desc.AppendFormat(L"\r\nAsterisk means every year|month|day|hour|minute|second.");
	desc.AppendFormat(L"\r\nQuestion mark matches any.");
	desc.AppendFormat(L"\r\nComma separates numbers or expressions.");
	desc.AppendFormat(L"\r\nHyphen specifies a range;");
	desc.AppendFormat(L"\r\n    X-Y is equivalent to X-Y/1.");
	desc.AppendFormat(L"\r\nSlash followed by a number specifies a step;");
	desc.AppendFormat(L"\r\n    X/N starts from X and increments by N.");
	desc.AppendFormat(L"\r\n    X-Y/N starts from X and increments by N until Y.");
	SetText(IDC_CRON_DESC_EDIT, desc);
}


DWORD CronDialogBox::ControlIdToFlag(int id)
{
	switch (id)
	{
	case IDC_CRON_EXPR_EDIT:
	case IDC_CRON_EXPR_STATIC:
		return FLAG_EXPRESSION;
	case IDC_CRON_YEAR_EDIT:
	case IDC_CRON_YEAR_EVAL_STATIC:
		return FLAG_YEAR;
	case IDC_CRON_MONTH_EDIT:
	case IDC_CRON_MONTH_EVAL_STATIC:
		return FLAG_MONTH;
	case IDC_CRON_DAY_EDIT:
	case IDC_CRON_DAY_EVAL_STATIC:
		return FLAG_DAYOFMONTH;
	case IDC_CRON_DOW_EDIT:
	case IDC_CRON_DOW_EVAL_STATIC:
		return FLAG_DAYOFWEEK;
	case IDC_CRON_HOUR_EDIT:
	case IDC_CRON_HOUR_EVAL_STATIC:
		return FLAG_HOUR;
	case IDC_CRON_MINUTE_EDIT:
	case IDC_CRON_MINUTE_EVAL_STATIC:
		return FLAG_MINUTE;
	case IDC_CRON_SECOND_EDIT:
	case IDC_CRON_SECOND_EVAL_STATIC:
		return FLAG_SECOND;
	default:
		return 0;
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
