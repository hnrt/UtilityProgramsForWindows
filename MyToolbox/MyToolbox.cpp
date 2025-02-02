﻿#include "pch.h"
#include "MyToolbox.h"
#include "resource.h"
#include "hnrt/Debug.h"
#include "hnrt/ResourceString.h"
#include "hnrt/Menu.h"
#include "hnrt/LogicalFont.h"
#include "hnrt/WindowHandle.h"
#include "hnrt/WindowDesign.h"
#include "hnrt/RegistryKey.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/Path.h"
#include "hnrt/Interlocked.h"


using namespace hnrt;


static const WCHAR s_szRegValFaceName[] = L"FaceName";
static const WCHAR s_szRegValFaceNameForTab[] = L"FaceNameForTab";
static const WCHAR s_szRegValFaceNameForData[] = L"FaceNameForData";


static const WCHAR s_szClassName[] = { L"HNRT_MyToolbox" };


MyToolbox::MyToolbox()
    : WindowApp(s_szClassName)
    , m_about()
    , m_tabs()
    , m_hashTab()
    , m_crptTab()
    , m_guidTab()
    , m_pctcTab()
    , m_cronTab()
    , m_ntoaTab()
    , m_dttmTab()
    , m_sfidTab()
    , m_gtinTab()
    , m_bs64Tab()
    , m_clipTab()
    , m_hFont(NULL)
    , m_hFontForTab(NULL)
    , m_hFontForData(NULL)
{
    INITCOMMONCONTROLSEX iccx = { sizeof(iccx), ICC_TAB_CLASSES };
    InitCommonControlsEx(&iccx);
    RegistryKey hKey;
    LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, REGKEY_ROOT);
    if (rc == ERROR_SUCCESS)
    {
    }
}


MyToolbox::~MyToolbox()
{
    RegistryKey hKey;
    LSTATUS rc = hKey.Create(HKEY_CURRENT_USER, REGKEY_ROOT);
    if (rc == ERROR_SUCCESS)
    {
    }
}


String MyToolbox::GetRegistryKeyName(PCWSTR pszName) const
{
    String szKeyName(REGKEY_ROOT);
    if (pszName)
    {
        szKeyName.AppendFormat(L"\\%s", pszName);
    }
    return szKeyName;
}


String MyToolbox::GetDirectoryPath(PCWSTR pszName) const
{
    String szDirectoryPath = Path::Combine(Path::GetKnownFolder(FOLDERID_LocalAppData), APP_GROUP, APP_NAME);
    if (pszName)
    {
        szDirectoryPath = Path::Combine(szDirectoryPath, pszName);
    }
    return szDirectoryPath;
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
        .Add(ResourceString(IDS_MENU_FILE),
            Menu()
            .Add(ResourceString(IDS_MENU_EXIT), IDM_FILE_EXIT))
        .Add(ResourceString(IDS_MENU_EDIT),
            Menu())
        .Add(ResourceString(IDS_MENU_VIEW),
            Menu())
        .Add(ResourceString(IDS_MENU_SETTINGS),
            Menu())
        .Add(ResourceString(IDS_MENU_HELP),
            Menu()
            .Add(ResourceString(IDS_MENU_ABOUT), IDM_HELP_ABOUT));
}


void MyToolbox::OnCreate()
{
    String szFaceName(FACENAME);
    String szFaceNameForTab(FACENAME_TAB);
    String szFaceNameForData(FACENAME_DATA);
    RegistryKey hKey;
    LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, REGKEY_ROOT);
    if (rc == ERROR_SUCCESS)
    {
        szFaceName = RegistryValue::GetSZ(hKey, s_szRegValFaceName, szFaceName);
        szFaceNameForTab = RegistryValue::GetSZ(hKey, s_szRegValFaceNameForTab, szFaceNameForTab);
        szFaceNameForData = RegistryValue::GetSZ(hKey, s_szRegValFaceNameForData, szFaceNameForData);
    }
    m_hFont = LogicalFont()
        .SetFaceName(szFaceName)
        .SetHeight(POINTSIZE, hwnd)
        .SetJapaneseCharSet()
        .Create();
    m_hFontForTab = LogicalFont()
        .SetFaceName(szFaceNameForTab)
        .SetHeight(POINTSIZE_TAB, hwnd)
        .SetJapaneseCharSet()
        .Create();
    m_hFontForData = LogicalFont()
        .SetFaceName(szFaceNameForData)
        .SetHeight(POINTSIZE_DATA, hwnd)
        .SetPitchAndFamily(FIXED_PITCH | FF_DONTCARE)
        .SetJapaneseCharSet()
        .Create();
    SetFont(hwnd, m_hFont);
    m_about.Open(GetInstanceHandle(hwnd));
    m_tabs.Open(hwnd);
    CreateChildren();
    m_tabs.LoadFromRegistry();
    m_tabs.OnTabSelectionChanging();
    m_tabs.OnTabSelectionChanged();
    SetMinimumSize();
    SetWindowSize(hwnd, MinimumWidth, MinimumHeight);
}


void MyToolbox::OnDestroy()
{
    SetFont(hwnd, NULL);
    DeleteObject(Interlocked<HFONT>::ExchangePointer(&m_hFont, NULL));
    DeleteObject(Interlocked<HFONT>::ExchangePointer(&m_hFontForTab, NULL));
    DeleteObject(Interlocked<HFONT>::ExchangePointer(&m_hFontForData, NULL));
}


void MyToolbox::CreateChildren()
{
    m_tabs.Add(ResourceString(IDS_TAB_HASH), &m_hashTab);
    m_tabs.Add(ResourceString(IDS_TAB_CRPT), &m_crptTab);
    m_tabs.Add(ResourceString(IDS_TAB_GUID), &m_guidTab);
    m_tabs.Add(ResourceString(IDS_TAB_PCTC), &m_pctcTab);
    m_tabs.Add(ResourceString(IDS_TAB_CRON), &m_cronTab);
    m_tabs.Add(ResourceString(IDS_TAB_NTOA), &m_ntoaTab);
    m_tabs.Add(ResourceString(IDS_TAB_DTTM), &m_dttmTab);
    m_tabs.Add(ResourceString(IDS_TAB_SFID), &m_sfidTab);
    m_tabs.Add(ResourceString(IDS_TAB_GTIN), &m_gtinTab);
    m_tabs.Add(ResourceString(IDS_TAB_BS64), &m_bs64Tab);
    m_tabs.Add(ResourceString(IDS_TAB_CLIP), &m_clipTab);
    for (int index = 0; index < m_tabs.ItemCount; index++)
    {
        m_tabs[index].Open(m_tabs);
    }
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
    LONG cyMin = 0;
    for (int index = 0; index < m_tabs.ItemCount; index++)
    {
        LONG cx = m_tabs[index].MinimumWidth;
        LONG cy = m_tabs[index].MinimumHeight;
        cxMin = cxMin > cx ? cxMin : cx;
        cyMin = cyMin > cy ? cyMin : cy;
    }
    WindowSize::SetMinimumSize(cxMin + bcx, cyMin + bcy);
}


void MyToolbox::ProcessMessage(MSG* pMsg)
{
    if (m_hAccelTable && TranslateAcceleratorW(hwnd, m_hAccelTable, pMsg))
    {
        // OK
    }
    else if (IsDialogMessageW(m_tabs[m_tabs.CurrentItem], pMsg))
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
    case IDM_FILE_NEW:
        m_tabs[m_tabs.CurrentItem].OnNew();
        break;
    case IDM_FILE_LOADFROM:
        m_tabs[m_tabs.CurrentItem].OnLoadFrom();
        break;
    case IDM_FILE_SAVEAS:
        m_tabs[m_tabs.CurrentItem].OnSaveAs();
        break;
    case IDM_FILE_LOAD1FROM:
        m_tabs[m_tabs.CurrentItem].OnLoad1From();
        break;
    case IDM_FILE_SAVE1AS:
        m_tabs[m_tabs.CurrentItem].OnSave1As();
        break;
    case IDM_FILE_LOAD2FROM:
        m_tabs[m_tabs.CurrentItem].OnLoad2From();
        break;
    case IDM_FILE_SAVE2AS:
        m_tabs[m_tabs.CurrentItem].OnSave2As();
        break;
    case IDM_EDIT_CUT:
        m_tabs[m_tabs.CurrentItem].OnCut();
        break;
    case IDM_EDIT_COPY:
        m_tabs[m_tabs.CurrentItem].OnCopy();
        break;
    case IDM_EDIT_PASTE:
        m_tabs[m_tabs.CurrentItem].OnPaste();
        break;
    case IDM_EDIT_DELETE:
        m_tabs[m_tabs.CurrentItem].OnDelete();
        break;
    case IDM_EDIT_SELECTALL:
        m_tabs[m_tabs.CurrentItem].OnSelectAll();
        break;
    case IDM_EDIT_COPYALL:
        m_tabs[m_tabs.CurrentItem].OnCopyAll();
        break;
    case IDM_EDIT_CLEAR:
        m_tabs[m_tabs.CurrentItem].OnClear();
        break;
    case IDM_EDIT_EXECUTE:
        m_tabs[m_tabs.CurrentItem].OnExecute();
        break;
    case IDM_EDIT_COPYRESULT:
        m_tabs[m_tabs.CurrentItem].OnCopyResult();
        break;
    case IDM_VIEW_HASH:
        m_tabs.CurrentItem = m_hashTab.Id;
        break;
    case IDM_VIEW_CRPT:
        m_tabs.CurrentItem = m_crptTab.Id;
        break;
    case IDM_VIEW_GUID:
        m_tabs.CurrentItem = m_guidTab.Id;
        break;
    case IDM_VIEW_PCTC:
        m_tabs.CurrentItem = m_pctcTab.Id;
        break;
    case IDM_VIEW_CRON:
        m_tabs.CurrentItem = m_cronTab.Id;
        break;
    case IDM_VIEW_NTOA:
        m_tabs.CurrentItem = m_ntoaTab.Id;
        break;
    case IDM_VIEW_DTTM:
        m_tabs.CurrentItem = m_dttmTab.Id;
        break;
    case IDM_VIEW_SFID:
        m_tabs.CurrentItem = m_sfidTab.Id;
        break;
    case IDM_VIEW_GTIN:
        m_tabs.CurrentItem = m_gtinTab.Id;
        break;
    case IDM_VIEW_BS64:
        m_tabs.CurrentItem = m_bs64Tab.Id;
        break;
    case IDM_VIEW_CLIP:
        m_tabs.CurrentItem = m_clipTab.Id;
        break;
    case IDM_HELP_ABOUT:
        m_about.Show();
        break;
    default:
        return m_tabs[m_tabs.CurrentItem].OnCommand(wParam, lParam);
    }
    return TRUE;
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
    for (int index = 0; index < m_tabs.ItemCount; index++)
    {
        SetWindowGeometry(m_tabs[index], rect);
    }
}


bool MyToolbox::OnFeederNotify(ULONGLONG cbTotalLength)
{
    m_tabs[m_tabs.CurrentItem].OnFeederNotify(cbTotalLength);
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
