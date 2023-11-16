#include "pch.h"
#include "AboutBox.h"
#include "resource.h"
#include "hnrt/VersionInfo.h"
#include "hnrt/String.h"


using namespace hnrt;


AboutBox::AboutBox()
	: DialogBox(IDD_ABOUTBOX)
{
}


void AboutBox::Show()
{
	RefPtr<VersionInfo> vi = VersionInfo::Create();
	SetWindowTextW(hwnd, vi->ProductName);
	SetText(IDC_ABOUT_HEADER_STATIC, vi->FileDescription);
	SetText(IDC_ABOUT_VERSION_STATIC, String(PRINTF, L"Version %s", vi->ProductVersion));
	SetText(IDC_ABOUT_COPYRIGHT_STATIC, vi->LegalCopyright);
	ShowWindow(hwnd, SW_SHOW);
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}


void AboutBox::Hide()
{
	ShowWindow(hwnd, SW_HIDE);
}


void AboutBox::OnClose()
{
	Hide();
}


INT_PTR AboutBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	UINT idChild = LOWORD(wParam);
	switch (idChild)
	{
	case IDOK:
		Hide();
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
