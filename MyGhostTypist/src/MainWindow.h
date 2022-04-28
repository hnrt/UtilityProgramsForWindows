#pragma once


#include <Windows.h>
#include "hnrt/WindowApp.h"
#include "hnrt/FontHandle.h"
#include "hnrt/ComLibrary.h"
#include "hnrt/KeyboardMouseBridge.h"
#include "hnrt/UiAutomation.h"
#include "ButtonCollection.h"
#include "Configuration.h"
#include "Ghost.h"
#include "Target.h"


namespace hnrt
{
    class InputManager;

    class MainWindow
        : public WindowApp
        , public ComLibrary
    {
    public:

        MainWindow();
        MainWindow(const MainWindow&) = delete;
        virtual ~MainWindow() = default;
        void operator =(const MainWindow&) = delete;
        virtual void Open(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow);

    private:

        HMENU CreateMenuBar();
        virtual void OnCreate(HWND hwnd);
        virtual void OnDestroy(HWND hwnd);
        virtual void OnSize(HWND hDlg, WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnTimer(HWND hwnd, WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnCommand(HWND hwnd, WPARAM wParam, LPARAM lParam);
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

        RefPtr<Configuration> m_pCfg;
        ButtonCollection m_Buttons;
        FontHandle m_hFont;
        bool m_bMaximized;
        LONG m_PreferredHeight;
        bool m_bSizing;
        Ghost m_Ghost;
    };
}
