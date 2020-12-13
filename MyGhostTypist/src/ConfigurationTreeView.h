#pragma once


#include <Windows.h>
#include <commctrl.h>
#include <vector>
#include <stdarg.h>
#include <stdio.h>
#include "CredentialsCollection.h"
#include "TargetCollection.h"


namespace hnrt
{
    class ConfigurationTreeViewCallbacks
    {
    public:

        virtual void OnTreeViewUnselected(HWND hwnd) = 0;
        virtual void OnCredentialsListSelected(HWND hwnd) = 0;
        virtual void OnCredentialsSelected(HWND hwnd, RefPtr<Credentials>) = 0;
        virtual void OnTargetListSelected(HWND hwnd) = 0;
        virtual void OnTargetSelected(HWND hwnd, RefPtr<Target>) = 0;
        virtual void OnActionItemSelected(HWND hwnd, RefPtr<Target>, ULONG) = 0;
    };

    class ConfigurationTreeView
        : public CredentialsCollectionCallback
        , public CredentialsCallback
        , public TargetCollectionCallback
        , public TargetCallback
    {
    public:

        ConfigurationTreeView(ConfigurationTreeViewCallbacks& cbs);
        ConfigurationTreeView(const ConfigurationTreeView&) = delete;
        ~ConfigurationTreeView() = default;
        void operator =(const ConfigurationTreeView&) = delete;
        void Init(HWND hwnd);
        void OnNotify(HWND hwnd, WPARAM wParam, LPARAM lParam);
        void AddCredentials();
        void RemoveSelectedCredentials();
        void MoveUpSelectedCredentials();
        void MoveDownSelectedCredentials();
        void AddTarget();
        void AddActionItem(RefPtr<Action> pAction);
        void RemoveSelectedTarget();
        void RemoveSelectedActionItem();
        void MoveUpSelectedTarget();
        void MoveUpSelectedActionItem();
        void MoveDownSelectedTarget();
        void MoveDownSelectedActionItem();

        CredentialsCollection& get_cc();
        TargetCollection& get_tt();

        __declspec(property(get = get_cc)) CredentialsCollection& CredentialsList;
        __declspec(property(get = get_tt)) TargetCollection& TargetList;

        bool get_IsCredentialsListSelected() const;
        bool get_IsCredentialsSelected() const;
        bool get_IsTargetListSelected() const;
        bool get_IsTargetSelected() const;
        bool get_IsActionItemSelected() const;
        ULONG get_SelectedCredentialsIndex() const;
        RefPtr<Credentials> get_SelectedCredentials() const;
        ULONG get_SelectedTargetIndex() const;
        RefPtr<Target> get_SelectedTarget() const;
        ULONG get_SelectedActionItemIndex() const;
        RefPtr<Action> get_SelectedActionItem() const;
        void set_SelectedActionItem(RefPtr<Action>);

        __declspec(property(get = get_IsCredentialsListSelected)) bool IsCredentialsListSelected;
        __declspec(property(get = get_IsCredentialsSelected)) bool IsCredentialsSelected;
        __declspec(property(get = get_IsTargetListSelected)) bool IsTargetListSelected;
        __declspec(property(get = get_IsTargetSelected)) bool IsTargetSelected;
        __declspec(property(get = get_IsActionItemSelected)) bool IsActionItemSelected;
        __declspec(property(get = get_SelectedCredentialsIndex)) ULONG SelectedCredentialsIndex;
        __declspec(property(get = get_SelectedCredentials)) RefPtr<Credentials> SelectedCredentials;
        __declspec(property(get = get_SelectedTargetIndex)) ULONG SelectedTargetIndex;
        __declspec(property(get = get_SelectedTarget)) RefPtr<Target> SelectedTarget;
        __declspec(property(get = get_SelectedActionItemIndex)) ULONG SelectedActionItemIndex;
        __declspec(property(get = get_SelectedActionItem, put = set_SelectedActionItem)) RefPtr<Action> SelectedActionItem;

        virtual void OnCredentialsCollectionUpdate(CredentialsCollection&);
        virtual void OnCredentialsUpdate(Credentials&);
        virtual void OnTargetCollectionUpdate(TargetCollection&);
        virtual void OnTargetUpdate(Target&);

    private:

        void Refresh();
        void RefreshCredentialsList();
        void RefreshCredentials(ULONG, HTREEITEM);
        void RefreshTargetList();
        void RefreshTarget(ULONG, HTREEITEM);
        void GetItemsFromCredentialsList(std::vector<HTREEITEM>&);
        void GetItemsFromTargetList(std::vector<HTREEITEM>&);
        void GetItemsFromTarget(RefPtr<Target>, HTREEITEM, std::vector<HTREEITEM>&);

        HWND m_hwnd;
        CredentialsCollection m_cc;
        TargetCollection m_tt;
        ConfigurationTreeViewCallbacks& m_cbs;
        HTREEITEM m_hCC;
        HTREEITEM m_hTT;
        ULONG m_Selected;
    };

    inline CredentialsCollection& ConfigurationTreeView::get_cc()
    {
        return m_cc;
    }

    inline TargetCollection& ConfigurationTreeView::get_tt()
    {
        return m_tt;
    }

#define CTV_LEVEL_BASE 65536
#define CTV_INDEX(L1,L2) ((L1)*CTV_LEVEL_BASE+(L2))
#define CTV_CLIST_LEVEL1 0
#define CTV_CLIST_LEVEL2 0
#define CTV_CLIST_INDEX CTV_INDEX(CTV_CLIST_LEVEL1, CTV_CLIST_LEVEL2)
#define CTV_CITEM_LEVEL1 1
#define CTV_CITEM_LEVEL2_MIN 0
#define CTV_TLIST_LEVEL1 0
#define CTV_TLIST_LEVEL2 1
#define CTV_TLIST_INDEX CTV_INDEX(CTV_TLIST_LEVEL1, CTV_TLIST_LEVEL2)
#define CTV_TNAME_LEVEL1 0
#define CTV_TNAME_LEVEL2_MIN 2
#define CTV_TITEM_LEVEL1_MIN 2
#define CTV_TITEM_LEVEL2 0
#define CTV_AITEM_LEVEL1_MIN 2
#define CTV_AITEM_LEVEL2_MIN 0
#define CTV_LEVEL1 (m_Selected / CTV_LEVEL_BASE)
#define CTV_LEVEL2 (m_Selected % CTV_LEVEL_BASE)

    inline bool ConfigurationTreeView::get_IsCredentialsListSelected() const
    {
        return m_Selected == CTV_CLIST_INDEX;
    }

    inline bool ConfigurationTreeView::get_IsCredentialsSelected() const
    {
        ULONG index1 = CTV_LEVEL1;
        return index1 == CTV_CITEM_LEVEL1;
    }

    inline bool ConfigurationTreeView::get_IsTargetListSelected() const
    {
        return m_Selected == CTV_TLIST_INDEX;
    }

    inline bool ConfigurationTreeView::get_IsTargetSelected() const
    {
        ULONG index1 = CTV_LEVEL1;
        ULONG index2 = CTV_LEVEL2 - CTV_TNAME_LEVEL2_MIN;
        return index1 == CTV_TNAME_LEVEL1 && index2 < m_tt.Count;
    }

    inline ULONG ConfigurationTreeView::get_SelectedCredentialsIndex() const
    {
        ULONG index1 = CTV_LEVEL1;
        ULONG index2 = CTV_LEVEL2 - CTV_CITEM_LEVEL2_MIN;
        return index1 == CTV_CITEM_LEVEL1 && index2 < m_cc.Count ? index2 : static_cast<ULONG>(-1);
    }

    inline RefPtr<Credentials> ConfigurationTreeView::get_SelectedCredentials() const
    {
        ULONG index1 = CTV_LEVEL1;
        ULONG index2 = CTV_LEVEL2 - CTV_CITEM_LEVEL2_MIN;
        return index1 == CTV_CITEM_LEVEL1 && index2 < m_cc.Count ? m_cc[index2] : RefPtr<Credentials>();
    }

    inline ULONG ConfigurationTreeView::get_SelectedTargetIndex() const
    {
        ULONG index1 = CTV_LEVEL1;
        if (index1 == CTV_TNAME_LEVEL1)
        {
            ULONG index2 = CTV_LEVEL2 - CTV_TNAME_LEVEL2_MIN;
            return index2 < m_tt.Count ? index2 : static_cast<ULONG>(-1);
        }
        else
        {
            index1 -= CTV_TITEM_LEVEL1_MIN;
            return index1 < m_tt.Count ? index1 : static_cast<ULONG>(-1);
        }
    }

    inline RefPtr<Target> ConfigurationTreeView::get_SelectedTarget() const
    {
        ULONG index = SelectedTargetIndex;
        return index < m_tt.Count ? m_tt[index] : RefPtr<Target>();
    }
}
