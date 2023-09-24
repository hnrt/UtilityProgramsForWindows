#include "pch.h"
#include "AboutBox.h"
#include "resource.h"


using namespace hnrt;


AboutBox::AboutBox()
	: DialogBox(IDD_ABOUTBOX)
{
}


void AboutBox::Show()
{
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
