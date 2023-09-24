#include "pch.h"
#include "framework.h"
#include "MyToolbox.h"
#include "hnrt/Debug.h"
#include "hnrt/ResourceString.h"
#include "hnrt/Menu.h"
#include "hnrt/TabControlItem.h"
#include "hnrt/WindowDesign.h"


using namespace hnrt;


static const WCHAR s_szClassName[] = { L"HNRT_GhostTypist" };


MyToolbox* MyToolbox::m_pInstance = nullptr;


MyToolbox& MyToolbox::GetInstance()
{
    return *m_pInstance;
}


MyToolbox::MyToolbox()
    : WindowApp(s_szClassName)
    , m_about()
    , m_tabs()
    , m_hashTab()
    , m_bSizing(false)
{
    INITCOMMONCONTROLSEX iccx = { sizeof(iccx), ICC_TAB_CLASSES };
    InitCommonControlsEx(&iccx);
    m_pInstance = this;
}


void MyToolbox::Open(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    static WCHAR szWindowName[MAX_PATH] = { 0 };
    wcscpy_s(szWindowName, ResourceString(IDS_APP_TITLE));
    P.SetMenu(CreateMenuBar())
        .SetWindowName(szWindowName);
    WindowApp::Open(hInstance, lpCmdLine, nCmdShow);
}


HMENU MyToolbox::CreateMenuBar()
{
    return Menu()
        .Add(ResourceString(IDS_FILE),
            Menu()
            .Add(ResourceString(IDS_EXIT), IDM_FILE_EXIT))
        .Add(ResourceString(IDS_HELP),
            Menu()
            .Add(ResourceString(IDS_ABOUT), IDM_HELP_ABOUT));
}


void MyToolbox::OnCreate()
{
    HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(hwnd, GWLP_HINSTANCE));
    m_about.Open(hInstance);
    HFONT hFont = reinterpret_cast<HFONT>(::SendMessageW(m_about, WM_GETFONT, 0, 0));
    m_tabs.Open(hwnd);
    ::SendMessageW(m_tabs, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), FALSE);
    TabControlItem().SetText(ResourceString(IDS_HASH_TABLABEL)).InsertInto(m_tabs);
    m_hashTab.DialogBox::Open(m_tabs);
    ShowWindow(m_hashTab, SW_SHOW);
    TabControlItem().SetText(L"bar").InsertInto(m_tabs);
    TabControlItem().SetText(L"baz").InsertInto(m_tabs);
    m_tabs.CurrentItem = 0;
    RectangleMetrics rect1;
    rect1.FromClient(hwnd);
    RectangleMetrics rect2;
    m_tabs.GetViewRect(&rect2);
    LONG bcx = rect1.cx - rect2.cx + LeftFrameThickness + RightFrameThickness;
    LONG bcy = rect1.cy - rect2.cy + TopFrameThickness + BottomFrameThickness;
    LONG cxMin = 0;
    cxMin = cxMin > m_hashTab.MinimumWidth ? cxMin : m_hashTab.MinimumWidth;
    LONG cyMin = 0;
    cyMin = cyMin > m_hashTab.MinimumHeight ? cyMin : m_hashTab.MinimumHeight;
    SetMinimumSize(cxMin + bcx, cyMin + bcy);
    SetWindowPos(hwnd, NULL, 0, 0, MinimumWidth, MinimumHeight, SWP_NOMOVE | SWP_NOZORDER);
}


LRESULT MyToolbox::OnCommand(WPARAM wParam, LPARAM lParam)
{
    DWORD wControlId = LOWORD(wParam);
    switch (wControlId)
    {
    case IDM_FILE_EXIT:
        OnClose();
        break;
    case IDM_HELP_ABOUT:
        m_about.Show();
        break;
    default:
        break;
    }
    return 0;
}


LRESULT MyToolbox::OnNotify(WPARAM wParam, LPARAM lParam)
{
    LPNMHDR p = reinterpret_cast<LPNMHDR>(lParam);
    if (p->hwndFrom == m_tabs)
    {
        INT selected = m_tabs.CurrentItem;
#pragma warning(disable: 26454)
        if (p->code == TCN_SELCHANGING)
#pragma warning(default: 26454)
        {
            DBGPUT(L"TCN_SELCHANGING selected=%d", selected);
            switch (selected)
            {
            case 0:
                ShowWindow(m_hashTab, SW_HIDE);
                break;
            default:
                break;
            }
        }
#pragma warning(disable: 26454)
        else if (p->code == TCN_SELCHANGE)
#pragma warning(default: 26454)
        {
            DBGPUT(L"TCN_SELCHANGE selected=%d", selected);
            switch (selected)
            {
            case 0:
                ShowWindow(m_hashTab, SW_SHOW);
                break;
            default:
                break;
            }
        }
    }
    return FALSE;
}


void MyToolbox::UpdateLayout(HWND hwnd, LONG cxDelta, LONG cyDelta)
{
    WindowLayout::UpdateLayout(hwnd, m_tabs, 0, 0, cxDelta, cyDelta);
    RectangleMetrics rect;
    m_tabs.GetViewRect(&rect);
    SetWindowPos(m_hashTab, NULL, rect.x, rect.y, rect.cx, rect.cy, SWP_NOZORDER);
}
