#include "pch.h"
#include "framework.h"
#include "MyToolbox.h"
#include "hnrt/Debug.h"
#include "hnrt/ResourceString.h"
#include "hnrt/Menu.h"
#include "hnrt/TabControlItem.h"
#include "hnrt/WindowHandle.h"
#include "hnrt/WindowDesign.h"


using namespace hnrt;


static const WCHAR s_szClassName[] = { L"HNRT_MyToolbox" };


MyToolbox::MyToolbox()
    : WindowApp(s_szClassName)
    , m_about()
    , m_tabs()
    , m_hashTab()
    , m_guidTab()
{
    INITCOMMONCONTROLSEX iccx = { sizeof(iccx), ICC_TAB_CLASSES };
    InitCommonControlsEx(&iccx);
}


void MyToolbox::Open(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    static WCHAR szWindowName[MAX_PATH] = { 0 };
    wcscpy_s(szWindowName, ResourceString(IDS_APP_TITLE));
    P.SetMenu(CreateMenuBar())
        .SetWindowName(szWindowName);
    WindowApp::Open(hInstance, lpCmdLine, nCmdShow);
    SetAccelerators(hInstance, IDC_MYTOOLBOX);
}


HMENU MyToolbox::CreateMenuBar()
{
    return Menu()
        .Add(ResourceString(IDS_FILE),
            Menu()
            .Add(ResourceString(IDS_EXIT), IDM_FILE_EXIT))
        .Add(ResourceString(IDS_EDIT),
            Menu()
            .Add(ResourceString(IDS_COPY), IDM_EDIT_COPY))
        .Add(ResourceString(IDS_VIEW),
            Menu()
            .Add(ResourceString(IDS_HASH_TABLABEL), IDM_VIEW_HASH)
            .Add(ResourceString(IDS_GUID_TABLABEL), IDM_VIEW_GUID))
        .Add(ResourceString(IDS_HELP),
            Menu()
            .Add(ResourceString(IDS_ABOUT), IDM_HELP_ABOUT));
}


void MyToolbox::OnCreate()
{
    m_about.Open(GetInstanceHandle(hwnd));
    m_tabs.Open(this);
    CreateChildren();
    m_tabs.LoadFromRegistry();
    m_tabs.OnTabSelectionChanged();
    SetMinimumSize();
    SetWindowSize(hwnd, MinimumWidth, MinimumHeight);
}


void MyToolbox::CreateChildren()
{
    TabControlItem().SetText(ResourceString(IDS_HASH_TABLABEL)).InsertInto(m_tabs);
    m_hashTab.DialogBox::Open(m_tabs);
    TabControlItem().SetText(ResourceString(IDS_GUID_TABLABEL)).InsertInto(m_tabs);
    m_guidTab.DialogBox::Open(m_tabs);
}


void MyToolbox::SetMinimumSize()
{
    RectangleMetrics rect1;
    rect1.FromClient(hwnd);
    RectangleMetrics rect2;
    m_tabs.GetViewRect(&rect2);
    LONG bcx = rect1.cx - rect2.cx + LeftFrameThickness + RightFrameThickness;
    LONG bcy = rect1.cy - rect2.cy + TopFrameThickness + BottomFrameThickness;
    LONG cxMin = 0;
    cxMin = cxMin > m_hashTab.MinimumWidth ? cxMin : m_hashTab.MinimumWidth;
    cxMin = cxMin > m_guidTab.MinimumWidth ? cxMin : m_guidTab.MinimumWidth;
    LONG cyMin = 0;
    cyMin = cyMin > m_hashTab.MinimumHeight ? cyMin : m_hashTab.MinimumHeight;
    cyMin = cyMin > m_guidTab.MinimumHeight ? cyMin : m_guidTab.MinimumHeight;
    WindowSize::SetMinimumSize(cxMin + bcx, cyMin + bcy);
}


void MyToolbox::ProcessMessage(MSG* pMsg)
{
    if (m_hAccelTable && TranslateAcceleratorW(hwnd, m_hAccelTable, pMsg))
    {
        // OK
    }
    else if (IsDialogMessageW(GetChild(m_tabs.CurrentItem), pMsg))
    {
        // OK
    }
    else
    {
        TranslateMessage(pMsg);
        DispatchMessageW(pMsg);
    }
}


LRESULT MyToolbox::OnCommand(WPARAM wParam, LPARAM lParam)
{
    DWORD wControlId = LOWORD(wParam);
    switch (wControlId)
    {
    case IDM_FILE_EXIT:
        OnClose();
        break;
    case IDM_EDIT_COPY:
        switch (m_tabs.CurrentItem)
        {
        case 0:
            m_hashTab.OnCopy();
            break;
        case 1:
            m_guidTab.OnCopy();
            break;
        default:
            break;
        }
        break;
    case IDM_VIEW_HASH:
        m_tabs.CurrentItem = 0;
        break;
    case IDM_VIEW_GUID:
        m_tabs.CurrentItem = 1;
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
    m_tabs.OnNotify(wParam, lParam);
    return FALSE;
}


void MyToolbox::UpdateLayout(HWND hwnd, LONG cxDelta, LONG cyDelta)
{
    WindowLayout::UpdateLayout(hwnd, m_tabs, 0, 0, cxDelta, cyDelta);
    RectangleMetrics rect;
    m_tabs.GetViewRect(&rect);
    SetWindowGeometry(m_hashTab, rect);
    SetWindowGeometry(m_guidTab, rect);
}


bool MyToolbox::OnFeederNotify(ULONGLONG cbTotalLength)
{
    switch (m_tabs.CurrentItem)
    {
    case 0:
        m_hashTab.SetResultHeader(cbTotalLength);
        break;
    default:
        break;
    }
    while (1)
    {
        int rc = TryProcessMessage();
        if (rc < 0)
        {
            return false;
        }
        else if (!rc)
        {
            break;
        }
    }
    return true;
}
