#include "MainWindow.h"
#include <CommCtrl.h>
#include "hnrt/Debug.h"
#include "hnrt/WindowClass.h"
#include "hnrt/ResourceString.h"
#include "hnrt/String.h"
#include "hnrt/VirtualKey.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/Heap.h"
#include "hnrt/Buffer.h"
#include "hnrt/VersionInfo.h"
#include "hnrt/WindowsPlatform.h"
#include "hnrt/UiAutomationFactory.h"
#include "hnrt/Menu.h"
#include "hnrt/WindowStyle.h"
#include "hnrt/LogicalFont.h"
#include "hnrt/Clipboard.h"
#include "Configuration.h"
#include "Action.h"
#include "InputManager.h"
#include "ConfigurationDialogBox.h"
#include "resource.h"


using namespace hnrt;


static const WCHAR s_szClassName[] = { L"HNRT_GhostTypist" };


#define TIMERID_KEYSTROKE 1
#define TIMERID_FINDWINDOW 2
#define BUTTONID_BASE 10000
#define IDM_FILE_EXIT 1000
#define IDM_EDIT_CONFIGURE 1199
#define IDM_HELP_ABOUT 1300
#define IDM_EDIT_USERNAME_BASE 2000
#define IDM_EDIT_PASSWORD_BASE 3000
#define IDM_VIEW_TARGET_BASE 4000


MainWindow::MainWindow()
    : WindowApp(s_szClassName)
    , ComLibrary(COINIT_APARTMENTTHREADED)
    , m_pCfg(Configuration::Create())
    , m_Buttons()
    , m_hFont()
    , m_bMaximized(false)
    , m_PreferredHeight(-1)
    , m_bSizing(false)
    , m_Ghost()
{
}


void MainWindow::Open(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    m_pCfg->Load();
    C.SetIcon(LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_ICON1)))
        .SetBackground(WHITE_BRUSH);
    P.SetWindowName(String(PRINTF, ResourceString(IDS_CAPTION_ARG), m_pCfg->CredentialsList.DefaultCredentials->Username))
        .SetWidth(m_pCfg->Width)
        .SetMenu(CreateMenuBar());
    WindowApp::Open(hInstance, lpCmdLine, nCmdShow);
    if (!wcslen(m_pCfg->CredentialsList.DefaultCredentials->Username))
    {
        Configure(hwnd);
    }
}


void MainWindow::OnCreate()
{
    DBGFNC(L"MainWindow::OnCreate");
    m_Ghost.Initialize(m_pCfg);
    m_Buttons.Padding = m_pCfg->Padding;
    m_Buttons.Margin = m_pCfg->ButtonMargin;
    m_hFont = LogicalFont()
        .SetFaceName(m_pCfg->FontName)
        .SetHeight(m_pCfg->FontSize, hwnd)
        .SetJapaneseCharSet()
        .Create();
    RecreateEditMenus(hwnd);
    RecreateViewMenus(hwnd);
    RecreateButtons(hwnd);
    CheckButtonStatus();
    SetTimer(hwnd, TIMERID_KEYSTROKE, m_pCfg->TypingInterval > USER_TIMER_MINIMUM ? m_pCfg->TypingInterval : USER_TIMER_MINIMUM, NULL);
    SetTimer(hwnd, TIMERID_FINDWINDOW, 3000, NULL);
}


void MainWindow::OnDestroy()
{
    DBGFNC(L"MainWindow::OnDestroy");
    m_hFont = nullptr;
    m_Ghost.Uninitialize();
}


void MainWindow::OnSize(WPARAM wParam, LPARAM lParam)
{
    DBGFNC(L"MainWindow::OnSize");
    if (wParam == SIZE_MINIMIZED || m_bSizing)
    {
        return;
    }
    m_bSizing = true;
    DoLayout(hwnd, static_cast<UINT>(wParam));
    m_bSizing = false;
}


LRESULT MainWindow::OnTimer(WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case TIMERID_KEYSTROKE:
        LetGhostPlay(hwnd);
        break;
    case TIMERID_FINDWINDOW:
        CheckButtonStatus();
        break;
    default:
        return DefWindowProcW(hwnd, WM_TIMER, wParam, lParam);
    }
    return 0;
}


LRESULT MainWindow::OnCommand(WPARAM wParam, LPARAM lParam)
{
    DWORD wControlId = LOWORD(wParam);
    switch (wControlId)
    {
    case IDM_FILE_EXIT:
        OnClose();
        break;
    case IDM_EDIT_CONFIGURE:
        Configure(hwnd);
        break;
    case IDM_HELP_ABOUT:
        OnAbout(hwnd);
        break;
    default:
        if (BUTTONID_BASE <= wControlId && wControlId < BUTTONID_BASE + m_Buttons.Count)
        {
            switch (HIWORD(wParam))
            {
            case BN_CLICKED:
                if (!m_Ghost.IsProcessing())
                {
                    OnButtonClicked(hwnd, wControlId - BUTTONID_BASE);
                }
                break;
            default:
                return DefWindowProcW(hwnd, WM_COMMAND, wParam, lParam);
            }
        }
        else if (IDM_EDIT_USERNAME_BASE <= wControlId && wControlId < IDM_EDIT_USERNAME_BASE + m_pCfg->CredentialsList.Count)
        {
            DWORD dwIndex = wControlId - IDM_EDIT_USERNAME_BASE;
            CopyToClipboard(hwnd, m_pCfg->CredentialsList[dwIndex]->Username);
        }
        else if (IDM_EDIT_PASSWORD_BASE <= wControlId && wControlId < IDM_EDIT_PASSWORD_BASE + m_pCfg->CredentialsList.Count)
        {
            DWORD dwIndex = wControlId - IDM_EDIT_PASSWORD_BASE;
            CopyToClipboard(hwnd, m_pCfg->CredentialsList[dwIndex]->Password);
            m_pCfg->CredentialsList[dwIndex]->ClearPlainText();
        }
        else if (IDM_VIEW_TARGET_BASE <= wControlId && wControlId < IDM_VIEW_TARGET_BASE + m_Buttons.Count)
        {
            ToggleButtonVisibility(hwnd, wControlId - IDM_VIEW_TARGET_BASE);
        }
        else
        {
            return DefWindowProcW(hwnd, WM_COMMAND, wParam, lParam);
        }
        break;
    }
    return 0;
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
        String(PRINTF, ResourceString(IDS_VERSIONINFO),
            pV->ProductName,
            pV->FileVersionMajor, pV->FileVersionMinor, pV->FileVersionPatch, pV->FileVersionBuild,
            pV->LegalCopyright ? pV->LegalCopyright : L""),
        ResourceString(IDS_CAPTION),
        MB_OK);
}


HMENU MainWindow::CreateMenuBar()
{
    return Menu()
        .Add(ResourceString(IDS_FILE),
            Menu()
            .Add(ResourceString(IDS_EXIT), IDM_FILE_EXIT))
        .Add(ResourceString(IDS_EDIT),
            Menu()
            .Add(ResourceString(IDS_COPY_MENU), Menu())
            .AddSeparator()
            .Add(ResourceString(IDS_CONFIGURE), IDM_EDIT_CONFIGURE))
        .Add(ResourceString(IDS_VIEW), Menu())
        .Add(ResourceString(IDS_HELP),
            Menu()
            .Add(ResourceString(IDS_ABOUT), IDM_HELP_ABOUT));
}


void MainWindow::RecreateEditMenus(HWND hwnd)
{
    Menu menuBar(hwnd);
    Menu menuEdit(menuBar[ResourceString(IDS_EDIT)]);
    Menu menuEditCopy(menuEdit[ResourceString(IDS_COPY_MENU)]);
    menuEditCopy.RemoveAll();
    for (UINT index = 0; index < m_pCfg->CredentialsList.Count; index++)
    {
        String strFormatU, strFormatP;
        if (index < 10)
        {
            strFormatU = String(PRINTF, L"%s (&%%d)", ResourceString(IDS_COPY_UN).Ptr);
            strFormatP = String(PRINTF, L"%s (&%%d)", ResourceString(IDS_COPY_PW).Ptr);
        }
        else
        {
            strFormatU = ResourceString(IDS_COPY_UN).Ptr;
            strFormatP = ResourceString(IDS_COPY_PW).Ptr;
        }
        RefPtr<Credentials> pCredentials = m_pCfg->CredentialsList[index];
        menuEditCopy
            .Add(String(PRINTF, strFormatU, pCredentials->Username, (index * 2 + 1) % 10), IDM_EDIT_USERNAME_BASE + index)
            .Add(String(PRINTF, strFormatP, pCredentials->Username, (index * 2 + 2) % 10), IDM_EDIT_PASSWORD_BASE + index);
    }
}


void MainWindow::RecreateViewMenus(HWND hwnd)
{
    Menu menuBar(hwnd);
    Menu menuView(menuBar[ResourceString(IDS_VIEW)]);
    menuView.RemoveAll();
    for (UINT index = 0; index < m_pCfg->TargetList.Count; index++)
    {
        RefPtr<Target> pTarget = m_pCfg->TargetList[index];
        menuView.Add(pTarget->Name, IDM_VIEW_TARGET_BASE + index, pTarget->IsVisible ? MF_CHECKED : MF_UNCHECKED);
    }
}


void MainWindow::RecreateButtons(HWND hwnd)
{
    m_Buttons.RemoveAll();
    m_Buttons.Resize(m_pCfg->TargetList.Count);
    if (m_pCfg->TargetList.Count > 0)
    {
        for (ULONG index = 0; index < m_pCfg->TargetList.Count; index++)
        {
            RefPtr<Target> pTarget = m_pCfg->TargetList[index];
            m_Buttons.Add(C.hInstance, hwnd, BUTTONID_BASE + index, pTarget->Name, m_hFont, pTarget->IsVisible);
        }
    }
    m_PreferredHeight = -1;
}


void MainWindow::CopyToClipboard(HWND hwnd, PCWSTR psz)
{
    if (!Clipboard::Write(hwnd, psz))
    {
        MessageBoxW(hwnd, ResourceString(IDS_COPY_FAILURE), ResourceString(IDS_CAPTION), MB_OK | MB_ICONERROR);
        return;
    }
}


void MainWindow::ToggleButtonVisibility(HWND hwnd, UINT uIndex)
{
    RefPtr<Target> pTarget = m_pCfg->TargetList[uIndex];
    pTarget->IsVisible = !pTarget->IsVisible;
    WindowStyle(m_Buttons[uIndex]).SetVisibility(pTarget->IsVisible).Apply();
    ForceLayout(hwnd);
    InvalidateRect(hwnd, NULL, TRUE);
    Menu menuBar(hwnd);
    Menu menuView(menuBar[ResourceString(IDS_VIEW)]);
    menuView.Modify(IDM_VIEW_TARGET_BASE + uIndex, pTarget->IsVisible ? MF_CHECKED : MF_UNCHECKED, IDM_VIEW_TARGET_BASE + uIndex, pTarget->Name);
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
    LONG y = m_Buttons.ArrangePositions(0, 0, ClientWidth, m_pCfg->ButtonHeight);
    if (!m_bMaximized)
    {
        LONG cx = WindowWidth;
        LONG cy = TopFrameThickness + y + BottomFrameThickness;
        if (m_PreferredHeight < 0)
        {
            m_PreferredHeight = cy;
        }
        else if (WindowHeight < m_PreferredHeight)
        {
            cy = m_PreferredHeight;
        }
        if (cy != WindowHeight)
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
    for (ULONG index = 0; index < m_pCfg->TargetList.Count; index++)
    {
        RefPtr<Target> pTarget = m_pCfg->TargetList[index];
        if (pTarget->Count)
        {
            auto pAction = dynamic_cast<const SetForegroundWindowAction*>(pTarget->Begin->Ptr);
            if (pAction)
            {
                EnableWindow(m_Buttons[index], pAction->Find() ? TRUE : FALSE);
            }
        }
    }
}


void MainWindow::Configure(HWND hwnd)
{
    ConfigurationDialogBox cd(C.hInstance);
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
        SetWindowTextW(hwnd, String(PRINTF, ResourceString(IDS_CAPTION_ARG), m_pCfg->CredentialsList.DefaultCredentials->Username));
        RecreateEditMenus(hwnd);
        RecreateViewMenus(hwnd);
        RecreateButtons(hwnd);
        ForceLayout(hwnd);
    }
}
