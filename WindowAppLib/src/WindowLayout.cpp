#include "pch.h"
#include "hnrt/WindowLayout.h"


using namespace hnrt;


void WindowLayout::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
}


void WindowLayout::UpdateLayout(HWND hwndParent, HWND hwnd, LONG dx, LONG dy, LONG dcx, LONG dcy, BOOL bInvalidate)
{
    UINT uFlags = SWP_NOZORDER;
    LONG x, y, cx, cy;
    RECT rect = { 0, 0, 0, 0 };
    GetWindowRect(hwnd, &rect);
    if (dx || dy)
    {
        POINT pt = { rect.left, rect.top };
        ScreenToClient(hwndParent, &pt);
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
    SetWindowPos(hwnd, NULL, x, y, cx, cy, uFlags);
    if (bInvalidate)
    {
        InvalidateRect(hwnd, NULL, TRUE);
    }
}


void WindowLayout::UpdateLayout(HWND hwndDialog, UINT id, LONG dx, LONG dy, LONG dcx, LONG dcy, BOOL bInvalidate)
{
    UpdateLayout(hwndDialog, GetDlgItem(hwndDialog, id), dx, dy, dcx, dcy, bInvalidate);
}
