#include "pch.h"
#include "hnrt/WindowLayout.h"
#include "hnrt/WindowDesign.h"


using namespace hnrt;


void WindowLayout::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
}


void WindowLayout::UpdateLayout(HWND hwndParent, HWND hwnd, LONG dx, LONG dy, LONG dcx, LONG dcy, BOOL bInvalidate)
{
    UINT uFlags = SWP_NOZORDER;
    LONG x, y, cx, cy;
    RectangleMetrics rect;
    rect.FromWindow(hwnd);
    rect.ToClient(hwndParent);
    if (dx || dy)
    {
        x = rect.x + dx;
        y = rect.y + dy;
    }
    else
    {
        x = 0;
        y = 0;
        uFlags |= SWP_NOMOVE;
    }
    if (dcx || dcy)
    {
        cx = rect.cx + dcx;
        cy = rect.cy + dcy;
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
