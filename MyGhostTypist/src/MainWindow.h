#pragma once


#include <Windows.h>
#include "hnrt/ComLibrary.h"
#include "hnrt/KeyboardMouseBridge.h"
#include "hnrt/UiAutomation.h"
#include "Configuration.h"
#include "Ghost.h"
#include "Target.h"


namespace hnrt
{
    class InputManager;

    class MainWindow
        : public ComLibrary
    {
    public:

        MainWindow(HINSTANCE hInstance);
        MainWindow(const MainWindow&) = delete;
        ~MainWindow();
        void operator =(const MainWindow&) = delete;
        HWND Initialize(int nCmdShow);
        int Run();

    private:

        static LRESULT CALLBACK MessageCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static HFONT CreateFontByNameAndSize(HWND hwnd, PCWSTR pszName, long size);
        static void DeleteMenuItems(HMENU hMenu);
        HMENU CreateMenuBar();
        void OnCreate(HWND hwnd);
        void OnDestroy(HWND hwnd);
        void OnSize(HWND hwnd, UINT uHint);
        LRESULT OnTimer(HWND hwnd, WPARAM wParam, LPARAM lParam);
        LRESULT OnCommand(HWND hwnd, WPARAM wParam, LPARAM lParam);
        void OnButtonClicked(HWND hwnd, DWORD index);
        void OnAbout(HWND hwnd);
        void RecreateEditMenus(HWND hwnd);
        void RecreateViewMenus(HWND hwnd);
        void RecreateButtons(HWND hwnd);
        void CopyToClipboard(HWND hwnd, PCWSTR psz);
        void ToggleButtonVisibility(HWND hwnd, UINT uIndex);
        void DoLayout(HWND hwnd, UINT uHint = ~0U);
        void ForceLayout(HWND hwnd);
        void LetGhostPlay(HWND hwnd);
        void CheckButtonStatus();
        void Configure(HWND hwnd);

        HINSTANCE m_hInstance;
        RefPtr<Configuration> m_pCfg;
        ULONG m_cButtons;
        HWND* m_hButtons;
        bool m_bMaximized;
        LONG m_PreferredHeight;
        bool m_bSizing;
        Ghost m_Ghost;
    };
}
