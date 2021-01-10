#pragma once


#include <Windows.h>
#include <commctrl.h>
#include "ConfigurationTreeView.h"
#include "TargetCollection.h"
#include "ComboBox.h"


namespace hnrt
{
    class ConfigurationDialogBox
        : private ConfigurationTreeViewCallbacks
    {
    public:

        ConfigurationDialogBox(HINSTANCE hInstance);
        ConfigurationDialogBox(const ConfigurationDialogBox&) = delete;
        ~ConfigurationDialogBox();
        void operator =(const ConfigurationDialogBox&) = delete;
        bool Run(HWND hwndParent);
        CredentialsCollection& get_cc();
        TargetCollection& get_tt();
        __declspec(property(get = get_cc)) CredentialsCollection& CredentialsList;
        __declspec(property(get = get_tt)) TargetCollection& TargetList;

    private:

        static INT_PTR MessageCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        void OnCreate(HWND hwnd);
        INT_PTR OnCommand(HWND hwnd, WPARAM wParam, LPARAM lParam);
        INT_PTR OnNotify(HWND hwnd, WPARAM wParam, LPARAM lParam);
        INT_PTR OnUpButtonClicked(HWND hwnd);
        INT_PTR OnDownButtonClicked(HWND hwnd);
        INT_PTR OnAddButtonClicked(HWND hwnd);
        INT_PTR OnRemoveButtonClicked(HWND hwnd);
        INT_PTR OnEditNotified(HWND hwnd, WORD wControlId, WORD wNotification);
        INT_PTR OnComboBoxNotified(HWND hwnd, WORD wControlId, WORD wNotification);
        INT_PTR OnShowPasswordClicked(HWND hwnd);
        INT_PTR OnFindWindowButtonClicked(HWND hwnd);
        INT_PTR OnUseAaClicked(HWND hwnd);
        INT_PTR OnLeftButtonDown(HWND hwnd, WPARAM wParam, LPARAM lParam);
        INT_PTR OnLeftClickButtonClicked(HWND hwnd);
        INT_PTR OnUsernameButtonClicked(HWND hwnd);
        INT_PTR OnPasswordButtonClicked(HWND hwnd);
        INT_PTR OnDeleteSequenceButtonClicked(HWND hwnd);
        INT_PTR OnUnicodeButtonClicked(HWND hwnd);
        INT_PTR OnReturnButtonClicked(HWND hwnd);
        INT_PTR OnTabButtonClicked(HWND hwnd);
        virtual void OnTreeViewUnselected(HWND hwnd);
        virtual void OnCredentialsListSelected(HWND hwnd);
        virtual void OnCredentialsSelected(HWND hwnd, RefPtr<Credentials>);
        virtual void OnTargetListSelected(HWND hwnd);
        virtual void OnTargetSelected(HWND hwnd, RefPtr<Target>);
        virtual void OnActionItemSelected(HWND hwnd, RefPtr<Target>, ULONG);
        void DisableCredentialsGroup(HWND hwnd);
        void DisableTargetGroup(HWND hwnd);
        void UpdateMoveButtons(HWND hwnd);
        void LoadCredKeyCombo(HWND hwnd);

        HINSTANCE m_hInstance;
        HWND m_hwndParent;
        bool m_bCapturing;
        ConfigurationTreeView m_tv;
        ComboBox m_cbCredentials;
        LONG m_tvstate;
        bool m_bChanging;
    };

    inline CredentialsCollection& ConfigurationDialogBox::get_cc()
    {
        return m_tv.CredentialsList;
    }

    inline TargetCollection& ConfigurationDialogBox::get_tt()
    {
        return m_tv.TargetList;
    }
}
