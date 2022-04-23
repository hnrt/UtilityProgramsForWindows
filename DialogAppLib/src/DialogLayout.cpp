#include "pch.h"
#include "hnrt/DialogLayout.h"


using namespace hnrt;


void DialogLayout::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
}


void DialogLayout::UpdateLayout(HWND hDlg, UINT id, LONG dx, LONG dy, LONG dcx, LONG dcy, BOOL bInvalidate)
{
    HWND hwndChild = GetDlgItem(hDlg, id);
    UINT uFlags = SWP_NOZORDER;
    LONG x, y, cx, cy;
    RECT rect = { 0, 0, 0, 0 };
    GetWindowRect(hwndChild, &rect);
    if (dx || dy)
    {
        POINT pt = { rect.left, rect.top };
        ScreenToClient(hDlg, &pt);
        x = pt.x + dx;
        y = pt.y + dy;
    }
    else
    {
        x = 0;
        y = 0;
        uFlags |= SWP_NOMOVE;
    }
    if (dcx || dcy)
    {
        cx = rect.right - rect.left + dcx;
        cy = rect.bottom - rect.top + dcy;
    }
    else
    {
        cx = 0;
        cy = 0;
        uFlags |= SWP_NOSIZE;
    }
    SetWindowPos(hwndChild, NULL, x, y, cx, cy, uFlags);
    if (bInvalidate)
    {
        InvalidateRect(hwndChild, NULL, TRUE);
    }
}
