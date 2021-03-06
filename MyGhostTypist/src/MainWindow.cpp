#include "MainWindow.h"
#include <CommCtrl.h>
#include "Configuration.h"
#include "Action.h"
#include "InputManager.h"
#include "ConfigurationDialogBox.h"
#include "hnrt/Debug.h"
#include "hnrt/ResourceString.h"
#include "hnrt/String.h"
#include "hnrt/VirtualKey.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/Heap.h"
#include "hnrt/Buffer.h"
#include "hnrt/VersionInfo.h"
#include "hnrt/WindowsPlatform.h"
#include "hnrt/UiAutomationFactory.h"
#include "resource.h"


using namespace hnrt;


static long s_RefCnt = 0;
static ATOM s_ClassAtom = 0;
static const WCHAR s_szClassName[] = { L"HNRT_GhostTypist" };


#define TIMERID_KEYSTROKE 1
#define TIMERID_FINDWINDOW 2
#define BUTTONID_BASE 10000
#define IDM_FILE_EXIT 1000
#define IDM_EDIT_CONFIGURE 1199
#define IDM_HELP_ABOUT 1300
#define IDM_VIEW_TARGET_BASE 2000


MainWindow::MainWindow(HINSTANCE hInstance)
    : ComLibrary(COINIT_APARTMENTTHREADED)
    , m_hInstance(hInstance)
    , m_pCfg(Configuration::Create())
    , m_cButtons(0)
    , m_hButtons(NULL)
    , m_bMaximized(false)
    , m_PreferredHeight(-1)
    , m_bSizing(false)
    , m_Ghost()
{
    if (InterlockedIncrement(&s_RefCnt) == 1)
    {
        WNDCLASSEXW wc;
        ZeroMemory(&wc, sizeof(wc));
        wc.cbSize = sizeof(wc);
        wc.style = 0;
        wc.lpfnWndProc = MessageCallback;
        wc.hInstance = hInstance;
        wc.hIcon = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_ICON1));
        wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wc.lpszClassName = s_szClassName;
        s_ClassAtom = RegisterClassExW(&wc);
        if (!s_ClassAtom)
        {
            throw Win32Exception(GetLastError(), L"MainWindow::ctor failed to register a window class.");
        }
    }
}


MainWindow::~MainWindow()
{
    if (InterlockedDecrement(&s_RefCnt) == 0)
    {
        UnregisterClassW(s_szClassName, m_hInstance);
    }
}


HWND MainWindow::Initialize(int nCmdShow)
{
    m_pCfg->Load();
    HWND hwnd = CreateWindowExW(0, s_szClassName, String::Format(ResourceString(IDS_CAPTION_ARG), m_pCfg->CredentialsList.DefaultCredentials->Username), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, m_pCfg->Width, CW_USEDEFAULT, NULL, CreateMenuBar(), m_hInstance, this);
    if (!hwnd)
    {
        throw Win32Exception(GetLastError(), L"MainWindow::Initialize failed to create a window.");
    }
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    if (!wcslen(m_pCfg->CredentialsList.DefaultCredentials->Username))
    {
        Configure(hwnd);
    }
    return hwnd;
}


int MainWindow::Run()
{
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return msg.message == WM_QUIT ? static_cast<int>(msg.wParam) : EXIT_FAILURE;
}


LRESULT CALLBACK MainWindow::MessageCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    MainWindow* pThis = reinterpret_cast<MainWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (pThis)
    {
        switch (uMsg)
        {
        case WM_CREATE:
            pThis->OnCreate(hwnd);
            return 0;
        case WM_DESTROY:
            pThis->OnDestroy(hwnd);
            return 0;
        case WM_SIZE:
            pThis->OnSize(hwnd, static_cast<UINT>(wParam));
            return 0;
        case WM_TIMER:
            return pThis->OnTimer(hwnd, wParam, lParam);
        case WM_COMMAND:
            return pThis->OnCommand(hwnd, wParam, lParam);
        default:
            break;
        }
    }
    else if (uMsg == WM_NCCREATE)
    {
        pThis = reinterpret_cast<MainWindow*>(reinterpret_cast<CREATESTRUCTW*>(lParam)->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}


HFONT MainWindow::CreateFontByNameAndSize(HWND hwnd, PCWSTR pszName, long size)
{
    HDC hDC = GetDC(hwnd);
    LOGFONTW lf = { 0 };
    lf.lfHeight = -MulDiv(size, GetDeviceCaps(hDC, LOGPIXELSY), 72);
    wcscpy_s(lf.lfFaceName, pszName);
    lf.lfCharSet = SHIFTJIS_CHARSET;
    return CreateFontIndirectW(&lf);
}


void MainWindow::OnCreate(HWND hwnd)
{
    DBGFNC(L"MainWindow::OnCreate");
    m_Ghost.Initialize(m_pCfg);
    RecreateViewMenus(hwnd);
    RecreateButtons(hwnd);
    CheckButtonStatus();
    SetTimer(hwnd, TIMERID_KEYSTROKE, m_pCfg->TypingInterval > USER_TIMER_MINIMUM ? m_pCfg->TypingInterval : USER_TIMER_MINIMUM, NULL);
    SetTimer(hwnd, TIMERID_FINDWINDOW, 3000, NULL);
}


void MainWindow::OnDestroy(HWND hwnd)
{
    DBGFNC(L"MainWindow::OnDestroy");
    m_Ghost.Uninitialize();
    PostQuitMessage(EXIT_SUCCESS);
}


void MainWindow::OnSize(HWND hwnd, UINT uHint)
{
    DBGFNC(L"MainWindow::OnSize");
    if (uHint == SIZE_MINIMIZED || m_bSizing)
    {
        return;
    }
    m_bSizing = true;
    DoLayout(hwnd, uHint);
    m_bSizing = false;
}


LRESULT MainWindow::OnTimer(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case TIMERID_KEYSTROKE:
        LetGhostPlay(hwnd);
        return 0;
    case TIMERID_FINDWINDOW:
        CheckButtonStatus();
        return 0;
    default:
        break;
    }
    return DefWindowProcW(hwnd, WM_TIMER, wParam, lParam);
}


LRESULT MainWindow::OnCommand(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    DWORD wControlId = LOWORD(wParam);
    switch (wControlId)
    {
    case IDM_FILE_EXIT:
        DestroyWindow(hwnd);
        return 0;
    case IDM_EDIT_CONFIGURE:
        Configure(hwnd);
        return 0;
    case IDM_HELP_ABOUT:
        OnAbout(hwnd);
        return 0;
    default:
        if (BUTTONID_BASE <= wControlId && wControlId < BUTTONID_BASE + m_cButtons)
        {
            switch (HIWORD(wParam))
            {
            case BN_CLICKED:
                if (!m_Ghost.IsProcessing())
                {
                    OnButtonClicked(hwnd, wControlId - BUTTONID_BASE);
                }
                return 0;
            default:
                break;
            }
        }
        else if (IDM_VIEW_TARGET_BASE <= wControlId && wControlId < IDM_VIEW_TARGET_BASE + m_cButtons)
        {
            ToggleButtonVisibility(hwnd, wControlId - IDM_VIEW_TARGET_BASE);
        }
        break;
    }
    return DefWindowProcW(hwnd, WM_COMMAND, wParam, lParam);
}


void MainWindow::OnButtonClicked(HWND hwnd, DWORD index)
{
    DBGFNC(L"MainWindow::OnButtonClicked");
    ShowWindow(hwnd, SW_MINIMIZE);
    if (!m_Ghost.Start(m_pCfg->TargetList[index]))
    {
        ShowWindow(hwnd, SW_NORMAL);
    }
}


void MainWindow::OnAbout(HWND hwnd)
{
    RefPtr<VersionInfo> pV = VersionInfo::Create();
    MessageBoxW(hwnd,
        String::Format(ResourceString(IDS_VERSIONINFO),
            pV->ProductName,
            pV->FileVersionMajor, pV->FileVersionMinor, pV->FileVersionPatch, pV->FileVersionBuild,
            pV->LegalCopyright ? pV->LegalCopyright : L""),
        ResourceString(IDS_CAPTION),
        MB_OK);
}


HMENU MainWindow::CreateMenuBar()
{
    HMENU hFileMenu = CreateMenu();
    AppendMenuW(hFileMenu, MF_STRING, IDM_FILE_EXIT, ResourceString(IDS_EXIT));
    HMENU hEditMenu = CreateMenu();
    AppendMenuW(hEditMenu, MF_STRING, IDM_EDIT_CONFIGURE, ResourceString(IDS_CONFIGURE));
    HMENU hViewMenu = CreateMenu();
    HMENU hHelpMenu = CreateMenu();
    AppendMenuW(hHelpMenu, MF_STRING, IDM_HELP_ABOUT, ResourceString(IDS_ABOUT));
    HMENU hMenuBar = CreateMenu();
    AppendMenuW(hMenuBar, MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(hFileMenu), ResourceString(IDS_FILE));
    AppendMenuW(hMenuBar, MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(hEditMenu), ResourceString(IDS_EDIT));
    AppendMenuW(hMenuBar, MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(hViewMenu), ResourceString(IDS_VIEW));
    AppendMenuW(hMenuBar, MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(hHelpMenu), ResourceString(IDS_HELP));
    return hMenuBar;
}


void MainWindow::RecreateViewMenus(HWND hwnd)
{
    HMENU hMenuBar = GetMenu(hwnd);
    HMENU hViewMenu = GetSubMenu(hMenuBar, 2);
    int n = GetMenuItemCount(hViewMenu);
    while (n > 0)
    {
        n--;
        DeleteMenu(hViewMenu, n, MF_BYPOSITION);
    }
    for (UINT_PTR index = 0; index < m_pCfg->TargetList.Count; index++)
    {
        RefPtr<Target> pTarget = m_pCfg->TargetList[index];
        AppendMenuW(hViewMenu, MF_STRING | (pTarget->IsVisible ? MF_CHECKED : MF_UNCHECKED), IDM_VIEW_TARGET_BASE + index, pTarget->Name);
    }
}


void MainWindow::RecreateButtons(HWND hwnd)
{
    while (m_cButtons > 0)
    {
        m_cButtons--;
        HWND hwndButton = m_hButtons[m_cButtons];
        if (hwndButton)
        {
            DestroyWindow(hwndButton);
        }
    }
    HFONT hFont = CreateFontByNameAndSize(hwnd, m_pCfg->FontName, m_pCfg->FontSize);
    m_hButtons = Allocate(m_hButtons, m_pCfg->TargetList.Count);
    for (UINT_PTR index = 0; index < m_pCfg->TargetList.Count; index++)
    {
        RefPtr<Target> pTarget = m_pCfg->TargetList[index];
        HWND hwndButton = CreateWindowExW(0, L"Button", pTarget->Name, WS_CHILD | (pTarget->IsVisible ? WS_VISIBLE : 0) | WS_DISABLED | BS_CENTER | BS_VCENTER, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, NULL, m_hInstance, NULL);
        SetWindowLongW(hwndButton, GWL_ID, BUTTONID_BASE + m_cButtons);
        SendMessageW(hwndButton, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
        m_hButtons[m_cButtons++] = hwndButton;
    }
    m_PreferredHeight = -1;
}


void MainWindow::ToggleButtonVisibility(HWND hwnd, UINT uIndex)
{
    HMENU hMenuBar = GetMenu(hwnd);
    HMENU hViewMenu = GetSubMenu(hMenuBar, 2);
    HWND hwndButton = m_hButtons[uIndex];
    RefPtr<Target> pTarget = m_pCfg->TargetList[uIndex];
    pTarget->IsVisible = !pTarget->IsVisible;
    DWORD dwStyle = GetWindowLongW(hwndButton, GWL_STYLE);
    if (pTarget->IsVisible)
    {
        dwStyle |= WS_VISIBLE;
    }
    else
    {
        dwStyle &= ~WS_VISIBLE;
    }
    SetWindowLongW(hwndButton, GWL_STYLE, dwStyle);
    ForceLayout(hwnd);
    InvalidateRect(hwnd, NULL, TRUE);
    if (!ModifyMenuW(hViewMenu, IDM_VIEW_TARGET_BASE + uIndex, MF_STRING | (pTarget->IsVisible ? MF_CHECKED : MF_UNCHECKED), IDM_VIEW_TARGET_BASE + (UINT_PTR)uIndex, pTarget->Name))
    {
        DBGPUT(L"ModifyMenu=%lu", GetLastError());
    }
}


void MainWindow::DoLayout(HWND hwnd, UINT uHint)
{
    DBGFNC(L"MainWindow::DoLayout");
    if (uHint == SIZE_MAXIMIZED)
    {
        m_bMaximized = true;
    }
    else if (uHint != ~0U)
    {
        m_bMaximized = false;
    }
    RECT rectWnd = { 0 };
    GetWindowRect(hwnd, &rectWnd);
    LONG cxWnd = rectWnd.right - rectWnd.left;
    LONG cyWnd = rectWnd.bottom - rectWnd.top;
    DBGPUT(L"wnd={%ld,%ld,%ld,%ld} (%ldx%ld)", rectWnd.left, rectWnd.top, rectWnd.right, rectWnd.bottom, cxWnd, cyWnd);
    RECT rectClt = { 0 };
    GetClientRect(hwnd, &rectClt);
    LONG cxClt = rectClt.right - rectClt.left;
    LONG cyClt = rectClt.bottom - rectClt.top;
    DBGPUT(L"clt={%ld,%ld,%ld,%ld} (%ldx%ld)", rectClt.left, rectClt.top, rectClt.right, rectClt.bottom, cxClt, cyClt);
    POINT pt0 = { 0, 0 };
    ClientToScreen(hwnd, &pt0);
    LONG cxFrame = pt0.x - rectWnd.left;
    LONG cyTopFrame = pt0.y - rectWnd.top;
    POINT pt9 = { rectClt.right, rectClt.bottom };
    ClientToScreen(hwnd, &pt9);
    LONG cyBottomFrame = rectWnd.bottom - pt9.y;
    if (cyBottomFrame < cxFrame)
    {
        cyBottomFrame = cxFrame;
    }
    DBGPUT(L"cxFrame=%ld cyTopFrame=%ld cyBottomFrame=%ld", cxFrame, cyTopFrame, cyBottomFrame);
    LONG cxButton = cxClt - (m_pCfg->PaddingLeft + m_pCfg->PaddingRight + m_pCfg->ButtonMarginLeft + m_pCfg->ButtonMarginRight);
    DBGPUT(L"cxButton=%ld", cxButton);
    // in client coordinates
    LONG x = m_pCfg->PaddingLeft + m_pCfg->ButtonMarginLeft;
    LONG y = m_pCfg->PaddingTop;
    for (ULONG i = 0; i < m_cButtons; i++)
    {
        RefPtr<Target> pTarget = m_pCfg->TargetList[i];
        if (!pTarget->IsVisible)
        {
            continue;
        }
        y += m_pCfg->ButtonMarginTop;
        SetWindowPos(m_hButtons[i], NULL, x, y, cxButton, m_pCfg->ButtonHeight, SWP_NOZORDER);
        y += m_pCfg->ButtonHeight + m_pCfg->ButtonMarginBottom;
    }
    y += m_pCfg->PaddingBottom;
    if (!m_bMaximized)
    {
        LONG cx = cxWnd;
        LONG cy = cyTopFrame + y + cyBottomFrame;
        if (m_PreferredHeight < 0)
        {
            m_PreferredHeight = cy;
        }
        else if (cyWnd < m_PreferredHeight)
        {
            cy = m_PreferredHeight;
        }
        if (cx != cxWnd || cy != cyWnd)
        {
            DBGPUT(L"cx=%ld cy=%ld", cx, cy);
            SetWindowPos(hwnd, NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER);
        }
    }
}


void MainWindow::ForceLayout(HWND hwnd)
{
    m_PreferredHeight = -1;
    DoLayout(hwnd);
}


void MainWindow::LetGhostPlay(HWND hwnd)
{
    if (m_Ghost.IsProcessing())
    {
        int rc = m_Ghost.Process();
        if (rc != GHOST_PROCESSED)
        {
            switch (rc)
            {
            case GHOST_COMPLETED:
                break;
            case GHOST_ERROR_INVALID_VIRTUAL_KEY_NAME:
                MessageBoxW(hwnd, L"Invalid virtual key name.", ResourceString(IDS_CAPTION), MB_OK | MB_ICONWARNING);
                break;
            case GHOST_ERROR_ACTIVE_ACCESSIBILITY_UNAVAILABLE:
                MessageBoxW(hwnd, L"Active Accessibility not available.", ResourceString(IDS_CAPTION), MB_OK | MB_ICONWARNING);
                break;
            case GHOST_ERROR_ACTIVE_ACCESSIBILITY_LOCATE_PART:
                MessageBoxW(hwnd, L"Active Accessibility failed to locate the UI part.", ResourceString(IDS_CAPTION), MB_OK | MB_ICONWARNING);
                break;
            case GHOST_ERROR_LOCATE_WINDOW:
                MessageBoxW(hwnd, L"Failed to locate the window.", ResourceString(IDS_CAPTION), MB_OK | MB_ICONWARNING);
                break;
            default:
                break;
            }
            m_Ghost.End();
            //ShowWindow(hwnd, SW_NORMAL);
        }
    }
}


void MainWindow::CheckButtonStatus()
{
    for (size_t index = 0; index < m_pCfg->TargetList.Count; index++)
    {
        RefPtr<Target> pTarget = m_pCfg->TargetList[index];
        if (pTarget->Count)
        {
            auto pAction = dynamic_cast<const SetForegroundWindowAction*>(pTarget->Begin->Ptr);
            if (pAction)
            {
                EnableWindow(m_hButtons[index], pAction->Find() ? TRUE : FALSE);
            }
        }
    }
}


void MainWindow::Configure(HWND hwnd)
{
    ConfigurationDialogBox cd(m_hInstance);
    for (size_t index = 0; index < m_pCfg->CredentialsList.Count; index++)
    {
        cd.CredentialsList.Append(m_pCfg->CredentialsList[index]->Clone());
    }
    for (size_t index = 0; index < m_pCfg->TargetList.Count; index++)
    {
        cd.TargetList.Append(m_pCfg->TargetList[index]->Clone());
    }
    if (cd.Run(hwnd))
    {
        m_pCfg->CredentialsList.Clear();
        for (size_t index = 0; index < cd.CredentialsList.Count; index++)
        {
            m_pCfg->CredentialsList.Append(cd.CredentialsList[index]->Clone());
        }
        m_pCfg->TargetList.Clear();
        for (size_t index = 0; index < cd.TargetList.Count; index++)
        {
            m_pCfg->TargetList.Append(cd.TargetList[index]->Clone());
        }
        m_pCfg->Save();
        SetWindowTextW(hwnd, String::Format(ResourceString(IDS_CAPTION_ARG), m_pCfg->CredentialsList.DefaultCredentials->Username));
        RecreateViewMenus(hwnd);
        RecreateButtons(hwnd);
        ForceLayout(hwnd);
    }
}
