#pragma once


#include <Windows.h>
#include "hnrt/ComLibrary.h"
#include "hnrt/KeyboardMouseBridge.h"
#include "Configuration.h"
#include "Target.h"


namespace hnrt
{
    class KeystrokeManager;

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
        HMENU CreateMenuBar();
        void OnCreate(HWND hwnd);
        void OnDestroy(HWND hwnd);
        void OnSize(HWND hwnd, UINT uHint);
        LRESULT OnTimer(HWND hwnd, WPARAM wParam, LPARAM lParam);
        LRESULT OnCommand(HWND hwnd, WPARAM wParam, LPARAM lParam);
        void OnButtonClicked(HWND hwnd, DWORD index);
        void OnAbout(HWND hwnd);
        void RecreateViewMenus(HWND hwnd);
        void RecreateButtons(HWND hwnd);
        void ToggleButtonVisibility(HWND hwnd, UINT uIndex);
        void DoLayout(HWND hwnd, UINT uHint = ~0U);
        void ForceLayout(HWND hwnd);
        void CheckButtonStatus();
        bool StartProcess();
        bool ContinueProcess();
        void EndProcess();
        void Configure(HWND hwnd);

        HINSTANCE m_hInstance;
        RefPtr<Configuration> m_pCfg;
        ULONG m_cButtons;
        HWND* m_hButtons;
        bool m_bMaximized;
        LONG m_PreferredHeight;
        bool m_bSizing;
        RefPtr<KeystrokeManager> m_pKeystrokeManager;
        bool m_bProcessing;
        RefPtr<Target> m_pTarget;
        Target::ActionIter m_ActionIter;
        KeyboardMouseBridge m_KeyboardMouseBridge;
        HWND m_hwndTarget;
        DWORD m_PreviousKeyboardState;
        DWORD m_CurrentKeyboardState;
    };
}
