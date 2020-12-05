#include "ConfigurationTreeView.h"
#include "ActionTarget.h"
#include "Action.h"
#include "FindWindowTarget.h"
#include "hnrt/Exception.h"


using namespace hnrt;


#define INVALID_HTREEITEM reinterpret_cast<HTREEITEM>(-1)


ConfigurationTreeView::ConfigurationTreeView(ConfigurationTreeViewCallbacks& cbs)
    : m_hwnd(nullptr)
    , m_cc()
    , m_tt()
    , m_cbs(cbs)
    , m_hCC(INVALID_HTREEITEM)
    , m_hTT(INVALID_HTREEITEM)
    , m_Selected(-1)
{
    m_cc.Callback = this;
    m_tt.Callback = this;
}


void ConfigurationTreeView::Init(HWND hwnd)
{
    m_hwnd = hwnd;

    WCHAR szText[MAX_PATH] = { 0 };
    TVINSERTSTRUCT tvins = { 0 };
    tvins.hParent = TVI_ROOT;
    tvins.hInsertAfter = TVI_FIRST;
    tvins.item.pszText = szText;
    tvins.item.cchTextMax = _countof(szText);
    tvins.item.mask = TVIF_TEXT | TVIF_PARAM;
    tvins.item.lParam = CTV_CLIST_INDEX;
    _snwprintf_s(szText, _TRUNCATE, L"credentials-list");
    m_hCC = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins)));
    tvins.hParent = m_hCC;
    tvins.hInsertAfter = TVI_FIRST;
    tvins.item.mask = TVIF_PARAM;
    tvins.item.lParam = ~0;
    for (size_t index = 0; index < m_cc.Count; index++)
    {
        tvins.hInsertAfter = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins)));
    }
    SendMessageW(m_hwnd, TVM_EXPAND, TVE_EXPAND, reinterpret_cast<LPARAM>(m_hCC));

    tvins.hParent = TVI_ROOT;
    tvins.hInsertAfter = m_hCC;
    tvins.item.mask = TVIF_TEXT | TVIF_PARAM;
    tvins.item.lParam = CTV_TLIST_INDEX;
    _snwprintf_s(szText, _TRUNCATE, L"target-list");
    m_hTT = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins)));
    tvins.hInsertAfter = TVI_FIRST;
    tvins.item.mask = TVIF_PARAM;
    tvins.item.lParam = ~0;
    for (size_t index = 0; index < m_tt.Count; index++)
    {
        RefPtr<Target> pT = m_tt[index];
        tvins.hParent = m_hTT;
        HTREEITEM hT = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins)));
        if (pT->IsTypeAction)
        {
            tvins.hParent = hT;
            tvins.hInsertAfter = TVI_FIRST;
            tvins.hInsertAfter = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins)));
            size_t count = pT->ActionTargetPtr->Count;
            for (size_t index2 = 0; index2 < count; index2++)
            {
                tvins.hInsertAfter = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins)));
            }
        }
        SendMessageW(m_hwnd, TVM_EXPAND, TVE_EXPAND, reinterpret_cast<LPARAM>(hT));
        tvins.hInsertAfter = hT;
    }
    SendMessageW(m_hwnd, TVM_EXPAND, TVE_EXPAND, reinterpret_cast<LPARAM>(m_hTT));
    Refresh();
}


void ConfigurationTreeView::OnNotify(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    LPNMTREEVIEWW pntv = reinterpret_cast<LPNMTREEVIEWW>(lParam);
    switch (pntv->hdr.code)
    {
    case TVN_SELCHANGED:
        if (pntv->itemNew.hItem)
        {
            TVITEM item = { 0 };
            item.hItem = pntv->itemNew.hItem;
            item.mask = TVIF_PARAM;
            SendMessageW(m_hwnd, TVM_GETITEM, 0, reinterpret_cast<LPARAM>(&item));
            m_Selected = static_cast<LONG>(item.lParam);
            if (IsCredentialsListSelected)
            {
                m_cbs.OnCredentialsListSelected(hwnd);
            }
            else if (IsTargetListSelected)
            {
                m_cbs.OnTargetListSelected(hwnd);
            }
            else if (IsTargetSelected)
            {
                m_cbs.OnTargetSelected(hwnd, SelectedTarget);
            }
            else if (IsCredentialsSelected)
            {
                m_cbs.OnCredentialsSelected(hwnd, SelectedCredentials);
            }
            else if (IsFindWindowSelected)
            {
                m_cbs.OnFindWindowSelected(hwnd, reinterpret_cast<FindWindowTarget*>(SelectedTarget.Ptr));
            }
            else if (IsActionItemSelected)
            {
                m_cbs.OnActionItemSelected(hwnd, reinterpret_cast<ActionTarget*>(SelectedTarget.Ptr), SelectedActionItemIndex);
            }
        }
        else
        {
            m_Selected = -1;
            m_cbs.OnTreeViewUnselected(hwnd);
        }
        break;
    default:
        break;
    }
}


void ConfigurationTreeView::AddCredentials()
{
    TVINSERTSTRUCT tvins = { 0 };
    tvins.hParent = m_hCC;
    tvins.hInsertAfter = TVI_LAST;
    tvins.item.mask = TVIF_PARAM;
    tvins.item.lParam = ~0;
    HTREEITEM hItem = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins)));
    SendMessageW(m_hwnd, TVM_EXPAND, TVE_EXPAND, reinterpret_cast<LPARAM>(m_hCC));
    RefPtr<Credentials> pC = Credentials::Create();
    pC->Username = L"(new)";
    pC->Password = L"";
    pC->ClearPlainText();
    m_cc.Append(pC);
    SendMessageW(m_hwnd, TVM_SELECTITEM, TVGN_CARET, reinterpret_cast<LPARAM>(hItem));
}


void ConfigurationTreeView::RemoveSelectedCredentials()
{
    if (IsCredentialsSelected)
    {
        size_t index = SelectedCredentialsIndex;
        std::vector<HTREEITEM> hChildren;
        GetItemsFromCredentialsList(hChildren);
        SendMessageW(m_hwnd, TVM_DELETEITEM, 0, reinterpret_cast<LPARAM>(hChildren[index]));
        SendMessageW(m_hwnd, TVM_SELECTITEM, TVGN_CARET, 0);
        m_cc.Delete(index);
    }
}


void ConfigurationTreeView::MoveUpSelectedCredentials()
{
    if (IsCredentialsSelected)
    {
        size_t index = SelectedCredentialsIndex;
        if (index > 0)
        {
            m_cc.Move(index, index - 1);
            std::vector<HTREEITEM> hChildren;
            GetItemsFromCredentialsList(hChildren);
            SendMessageW(m_hwnd, TVM_SELECTITEM, TVGN_CARET, reinterpret_cast<LPARAM>(hChildren[index - 1]));
        }
    }
}


void ConfigurationTreeView::MoveDownSelectedCredentials()
{
    if (IsCredentialsSelected)
    {
        size_t index = SelectedCredentialsIndex;
        if (index + 1 < m_cc.Count)
        {
            m_cc.Move(index, index + 1);
            std::vector<HTREEITEM> hChildren;
            GetItemsFromCredentialsList(hChildren);
            SendMessageW(m_hwnd, TVM_SELECTITEM, TVGN_CARET, reinterpret_cast<LPARAM>(hChildren[index + 1]));
        }
    }
}


void ConfigurationTreeView::AddTarget()
{
    TVINSERTSTRUCT tvins = { 0 };
    tvins.hParent = m_hTT;
    tvins.hInsertAfter = TVI_LAST;
    tvins.item.mask = TVIF_PARAM;
    tvins.item.lParam = ~0;
    HTREEITEM hItem = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins)));
    SendMessageW(m_hwnd, TVM_EXPAND, TVE_EXPAND, reinterpret_cast<LPARAM>(m_hTT));
    m_tt.Append(Target::CreateNull(L"(new)"));
    SendMessageW(m_hwnd, TVM_SELECTITEM, TVGN_CARET, reinterpret_cast<LPARAM>(hItem));
}


void ConfigurationTreeView::AddFindWindow(PCWSTR pszClassName, PCWSTR pszWindowText)
{
    if (IsTargetSelected)
    {
        RefPtr<Target> pTarget = SelectedTarget;
        if (pTarget->IsTypeNull)
        {
            size_t index = SelectedTargetIndex;
            std::vector<HTREEITEM> hChildren;
            GetItemsFromTargetList(hChildren);
            TVINSERTSTRUCT tvins = { 0 };
            tvins.hParent = hChildren[index];
            tvins.hInsertAfter = TVI_FIRST;
            tvins.item.mask = TVIF_PARAM;
            tvins.item.lParam = ~0;
            SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins));
            SendMessageW(m_hwnd, TVM_EXPAND, TVE_EXPAND, reinterpret_cast<LPARAM>(hChildren[index]));
            m_tt.Assign(index, Target::CreateFindWindow(pTarget->Name, pszClassName, pszWindowText));
        }
    }
}


void ConfigurationTreeView::AddActionItem(PCWSTR pszAction)
{
    if (IsActionItemSelected || IsFindWindowSelected || IsTargetSelected)
    {
        RefPtr<Target> pTarget = SelectedTarget;
        if (pTarget->IsTypeAction)
        {
            std::vector<HTREEITEM> hChildren;
            GetItemsFromTargetList(hChildren);
            TVINSERTSTRUCT tvins = { 0 };
            tvins.hParent = hChildren[SelectedTargetIndex];
            tvins.hInsertAfter = TVI_LAST;
            tvins.item.mask = TVIF_PARAM;
            tvins.item.lParam = ~0;
            HTREEITEM hItem = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins)));
            pTarget->ActionTargetPtr->Append(pszAction);
            SendMessageW(m_hwnd, TVM_SELECTITEM, TVGN_CARET, reinterpret_cast<LPARAM>(hItem));
        }
    }
}


void ConfigurationTreeView::RemoveSelectedTarget()
{
    if (IsTargetSelected || IsFindWindowSelected)
    {
        size_t index = SelectedTargetIndex;
        std::vector<HTREEITEM> hChildren;
        GetItemsFromTargetList(hChildren);
        SendMessageW(m_hwnd, TVM_DELETEITEM, 0, reinterpret_cast<LPARAM>(hChildren[index]));
        SendMessageW(m_hwnd, TVM_SELECTITEM, TVGN_CARET, 0);
        m_tt.Delete(index);
    }
}


void ConfigurationTreeView::RemoveSelectedActionItem()
{
    if (IsActionItemSelected)
    {
        size_t index1 = SelectedTargetIndex;
        size_t index2 = SelectedActionItemIndex;
        std::vector<HTREEITEM> hChildren1;
        GetItemsFromTargetList(hChildren1);
        std::vector<HTREEITEM> hChildren2;
        GetItemsFromTarget(m_tt[index1], hChildren1[index1], hChildren2);
        SendMessageW(m_hwnd, TVM_DELETEITEM, 0, reinterpret_cast<LPARAM>(hChildren2[index2]));
        SendMessageW(m_hwnd, TVM_SELECTITEM, TVGN_CARET, 0);
        m_tt[index1]->ActionTargetPtr->Delete(index2);
    }
}


void ConfigurationTreeView::MoveUpSelectedTarget()
{
    if (IsTargetSelected || IsFindWindowSelected)
    {
        size_t index = SelectedTargetIndex;
        if (index > 0)
        {
            RefPtr<Target> pTarget = m_tt[index];
            std::vector<HTREEITEM> hChildren;
            GetItemsFromTargetList(hChildren);
            SendMessageW(m_hwnd, TVM_DELETEITEM, 0, reinterpret_cast<LPARAM>(hChildren[index]));
            TVINSERTSTRUCT tvins = { 0 };
            tvins.hParent = m_hTT;
            tvins.hInsertAfter = index > 1 ? hChildren[index - 2] : TVI_FIRST;
            tvins.item.mask = TVIF_PARAM;
            tvins.item.lParam = ~0;
            HTREEITEM hItem = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins)));
            if (pTarget->IsTypeAction)
            {
                tvins.hParent = hItem;
                tvins.hInsertAfter = TVI_LAST;
                SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins));
                for (ActionListConstIter iter = pTarget->ActionTargetPtr->Begin(); iter != pTarget->ActionTargetPtr->End(); iter++)
                {
                    SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins));
                }
                SendMessageW(m_hwnd, TVM_EXPAND, TVE_EXPAND, reinterpret_cast<LPARAM>(hItem));
            }
            m_tt.Move(index, index - 1);
            SendMessageW(m_hwnd, TVM_SELECTITEM, TVGN_CARET, reinterpret_cast<LPARAM>(hItem));
        }
    }
}


void ConfigurationTreeView::MoveUpSelectedActionItem()
{
    if (IsActionItemSelected)
    {
        RefPtr<Target> pTarget = SelectedTarget;
        size_t index = SelectedActionItemIndex;
        if (index > 0)
        {
            pTarget->ActionTargetPtr->Move(index, index - 1);
            std::vector<HTREEITEM> hTT;
            GetItemsFromTargetList(hTT);
            std::vector<HTREEITEM> hAA;
            GetItemsFromTarget(pTarget, hTT[SelectedTargetIndex], hAA);
            SendMessageW(m_hwnd, TVM_SELECTITEM, TVGN_CARET, reinterpret_cast<LPARAM>(hAA[1 + index - 1]));
        }
    }
}


void ConfigurationTreeView::MoveDownSelectedTarget()
{
    if (IsTargetSelected || IsFindWindowSelected)
    {
        size_t index = SelectedTargetIndex;
        if (index + 1 < m_tt.Count)
        {
            RefPtr<Target> pTarget = m_tt[index];
            std::vector<HTREEITEM> hChildren;
            GetItemsFromTargetList(hChildren);
            SendMessageW(m_hwnd, TVM_DELETEITEM, 0, reinterpret_cast<LPARAM>(hChildren[index]));
            TVINSERTSTRUCT tvins = { 0 };
            tvins.hParent = m_hTT;
            tvins.hInsertAfter = hChildren[index + 1];
            tvins.item.mask = TVIF_PARAM;
            tvins.item.lParam = ~0;
            HTREEITEM hItem = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins)));
            if (pTarget->IsTypeAction)
            {
                tvins.hParent = hItem;
                tvins.hInsertAfter = TVI_LAST;
                SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins));
                for (ActionListConstIter iter = pTarget->ActionTargetPtr->Begin(); iter != pTarget->ActionTargetPtr->End(); iter++)
                {
                    SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins));
                }
                SendMessageW(m_hwnd, TVM_EXPAND, TVE_EXPAND, reinterpret_cast<LPARAM>(hItem));
            }
            m_tt.Move(index, index + 1);
            SendMessageW(m_hwnd, TVM_SELECTITEM, TVGN_CARET, reinterpret_cast<LPARAM>(hItem));
        }
    }
}


void ConfigurationTreeView::MoveDownSelectedActionItem()
{
    if (IsActionItemSelected)
    {
        RefPtr<Target> pTarget = SelectedTarget;
        size_t index = SelectedActionItemIndex;
        if (index + 1 < pTarget->ActionTargetPtr->Count)
        {
            pTarget->ActionTargetPtr->Move(index, index + 1);
            std::vector<HTREEITEM> hTT;
            GetItemsFromTargetList(hTT);
            std::vector<HTREEITEM> hAA;
            GetItemsFromTarget(pTarget, hTT[SelectedTargetIndex], hAA);
            SendMessageW(m_hwnd, TVM_SELECTITEM, TVGN_CARET, reinterpret_cast<LPARAM>(hAA[1 + index + 1]));
        }
    }
}


void ConfigurationTreeView::OnCredentialsCollectionUpdate(CredentialsCollection& collection)
{
    if (m_hwnd)
    {
        RefreshCredentialsList();
    }
}


void ConfigurationTreeView::OnCredentialsUpdate(Credentials& credentials)
{
    if (m_hwnd && m_cc.Count)
    {
        std::vector<HTREEITEM> hChildren;
        GetItemsFromCredentialsList(hChildren);
        for (size_t index = 0; index < m_cc.Count; index++)
        {
            if (m_cc[index].Ptr == &credentials)
            {
                RefreshCredentials(index, hChildren[index]);
                return;
            }
        }
    }
}


void ConfigurationTreeView::OnTargetCollectionUpdate(TargetCollection& collection)
{
    if (m_hwnd)
    {
        RefreshTargetList();
    }
}


void ConfigurationTreeView::OnTargetUpdate(Target& target)
{
    if (m_hwnd && m_tt.Count)
    {
        std::vector<HTREEITEM> hChildren;
        GetItemsFromTargetList(hChildren);
        for (size_t index = 0; index < m_tt.Count; index++)
        {
            if (m_tt[index].Ptr == &target)
            {
                RefreshTarget(index, hChildren[index]);
                return;
            }
        }
    }
}


void ConfigurationTreeView::Refresh()
{
    RefreshCredentialsList();
    RefreshTargetList();
}


void ConfigurationTreeView::RefreshCredentialsList()
{
    if (m_cc.Count)
    {
        std::vector<HTREEITEM> hChildren;
        GetItemsFromCredentialsList(hChildren);
        for (size_t index = 0; index < m_cc.Count; index++)
        {
            RefreshCredentials(index, hChildren[index]);
        }
    }
}


void ConfigurationTreeView::RefreshCredentials(size_t index, HTREEITEM hItem)
{
    WCHAR szText[MAX_PATH] = { 0 };
    TVITEM item = { 0 };
    item.mask = TVIF_TEXT | TVIF_PARAM;
    item.pszText = szText;
    item.cchTextMax = _countof(szText);
    item.hItem = hItem;
    if (m_cc[index]->Key && m_cc[index]->Key[0])
    {
        _snwprintf_s(szText, _TRUNCATE, L"%s [%s]", m_cc[index]->Username, m_cc[index]->Key);
    }
    else
    {
        _snwprintf_s(szText, _TRUNCATE, L"%s", m_cc[index]->Username);
    }
    item.lParam = CTV_INDEX(CTV_CITEM_LEVEL1, CTV_CITEM_LEVEL2_MIN + index);
    SendMessageW(m_hwnd, TVM_SETITEM, 0, reinterpret_cast<LPARAM>(&item));
    m_cc[index]->Callback = this;
}


void ConfigurationTreeView::RefreshTargetList()
{
    if (m_tt.Count)
    {
        std::vector<HTREEITEM> hChildren;
        GetItemsFromTargetList(hChildren);
        for (size_t index = 0; index < m_tt.Count; index++)
        {
            RefreshTarget(index, hChildren[index]);
        }
    }
}


void ConfigurationTreeView::RefreshTarget(size_t index, HTREEITEM hT)
{
    WCHAR szText[MAX_PATH] = { 0 };
    TVITEM item = { 0 };
    item.mask = TVIF_TEXT | TVIF_PARAM;
    item.pszText = szText;
    item.cchTextMax = _countof(szText);
    item.hItem = hT;
    _snwprintf_s(szText, _TRUNCATE, L"%s", m_tt[index]->Name);
    item.lParam = CTV_INDEX(CTV_TNAME_LEVEL1, CTV_TNAME_LEVEL2_MIN + index);
    SendMessageW(m_hwnd, TVM_SETITEM, 0, reinterpret_cast<LPARAM>(&item));
    std::vector<HTREEITEM> hChildren;
    GetItemsFromTarget(m_tt[index], hT, hChildren);
    if (m_tt[index]->IsTypeFindWindow)
    {
        FindWindowTarget* pFW = m_tt[index]->FindWindowTargetPtr;
        item.hItem = hChildren[0];
        _snwprintf_s(szText, _TRUNCATE, L"FindWindow class=\"%s\" text=\"%s\"", pFW->ClassName, pFW->WindowText);
        item.lParam = CTV_INDEX(CTV_TITEM_LEVEL1_MIN + index, CTV_TITEM_LEVEL2);
        SendMessageW(m_hwnd, TVM_SETITEM, 0, reinterpret_cast<LPARAM>(&item));
        for (size_t index2 = 0; index2 < pFW->Count; index2++)
        {
            PCWSTR psz = (*pFW)[index2];
            item.hItem = hChildren[1 + index2];
            switch (Action::GetClass(psz))
            {
            case AC_TYPEUSERNAME:
                psz = Action::GetValue(psz);
                _snwprintf_s(szText, _TRUNCATE, psz ? L"TypeUsername [%s]" : L"TypeUsername", psz);
                break;
            case AC_TYPEPASSWORD:
                psz = Action::GetValue(psz);
                _snwprintf_s(szText, _TRUNCATE, psz ? L"TypePassword [%s]" : L"TypePassword", psz);
                break;
            case AC_TYPEDELETESEQUENCE:
                _snwprintf_s(szText, _TRUNCATE, L"TypeDeleteSequence");
                break;
            case AC_TYPEUNICODE:
                psz = Action::GetValue(psz);
                _snwprintf_s(szText, _TRUNCATE, L"Type \"%s\"", psz);
                break;
            case AC_TYPEKEY:
                _snwprintf_s(szText, _TRUNCATE, L"Type %s", psz);
                break;
            default:
                _snwprintf_s(szText, _TRUNCATE, L"?");
                break;
            }
            item.lParam = CTV_INDEX(CTV_AITEM_LEVEL1_MIN + index, CTV_AITEM_LEVEL2_MIN + index2);
            SendMessageW(m_hwnd, TVM_SETITEM, 0, reinterpret_cast<LPARAM>(&item));
        }
    }
    m_tt[index]->Callback = this;
}


void ConfigurationTreeView::GetItemsFromCredentialsList(std::vector<HTREEITEM>& hh)
{
    if (m_cc.Count)
    {
        HTREEITEM h = INVALID_HTREEITEM;
        for (size_t index = 0; index < m_cc.Count; index++)
        {
            if (index == 0)
            {
                h = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_GETNEXTITEM, TVGN_CHILD, reinterpret_cast<LPARAM>(m_hCC)));
            }
            else
            {
                h = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_GETNEXTITEM, TVGN_NEXT, reinterpret_cast<LPARAM>(h)));
            }
            hh.push_back(h);
        }
    }
}


void ConfigurationTreeView::GetItemsFromTargetList(std::vector<HTREEITEM>& hh)
{
    if (m_tt.Count)
    {
        HTREEITEM h = INVALID_HTREEITEM;
        for (size_t index = 0; index < m_tt.Count; index++)
        {
            if (index == 0)
            {
                h = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_GETNEXTITEM, TVGN_CHILD, reinterpret_cast<LPARAM>(m_hTT)));
            }
            else
            {
                h = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_GETNEXTITEM, TVGN_NEXT, reinterpret_cast<LPARAM>(h)));
            }
            hh.push_back(h);
        }
    }
}


void ConfigurationTreeView::GetItemsFromTarget(RefPtr<Target> pTarget, HTREEITEM hParent, std::vector<HTREEITEM>& hChildren)
{
    if (pTarget->IsTypeAction)
    {
        HTREEITEM hChild = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_GETNEXTITEM, TVGN_CHILD, reinterpret_cast<LPARAM>(hParent)));
        if (hChild)
        {
            hChildren.push_back(hChild);
            for (size_t index = 0; index < pTarget->ActionTargetPtr->Count; index++)
            {
                hChild = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_GETNEXTITEM, TVGN_NEXT, reinterpret_cast<LPARAM>(hChild)));
                hChildren.push_back(hChild);
            }
        }
    }
}


bool ConfigurationTreeView::get_IsActionItemSelected() const
{
    size_t index1 = CTV_LEVEL1 - CTV_AITEM_LEVEL1_MIN;
    size_t index2 = CTV_LEVEL2 - CTV_AITEM_LEVEL2_MIN;
    return index1 < m_tt.Count&& m_tt[index1]->IsTypeAction&& index2 < m_tt[index1]->ActionTargetPtr->Count;
}


size_t ConfigurationTreeView::get_SelectedActionItemIndex() const
{
    size_t index1 = CTV_LEVEL1 - CTV_AITEM_LEVEL1_MIN;
    size_t index2 = CTV_LEVEL2 - CTV_AITEM_LEVEL2_MIN;
    return index1 < m_tt.Count&& m_tt[index1]->IsTypeAction&& index2 < m_tt[index1]->ActionTargetPtr->Count ? index2 : static_cast<size_t>(-1);
}


PCWSTR ConfigurationTreeView::get_SelectedActionItem() const
{
    return IsActionItemSelected ? (*SelectedTarget->ActionTargetPtr)[SelectedActionItemIndex] : nullptr;
}


void ConfigurationTreeView::set_SelectedActionItem(PCWSTR psz)
{
    if (IsActionItemSelected)
    {
        SelectedTarget->ActionTargetPtr->Assign(SelectedActionItemIndex, psz);
    }
}
