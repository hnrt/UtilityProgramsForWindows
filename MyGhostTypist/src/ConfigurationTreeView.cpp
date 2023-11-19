#include "ConfigurationTreeView.h"
#include "Target.h"
#include "hnrt/Debug.h"
#include "hnrt/Exception.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/UiAutomation.h"


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
    for (ULONG index = 0; index < m_cc.Count; index++)
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
    for (ULONG index = 0; index < m_tt.Count; index++)
    {
        RefPtr<Target> pT = m_tt[index];
        tvins.hParent = m_hTT;
        HTREEITEM hT = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins)));
        tvins.hParent = hT;
        tvins.hInsertAfter = TVI_FIRST;
        for (ULONG index2 = 0; index2 < pT->Count; index2++)
        {
            tvins.hInsertAfter = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins)));
            RefPtr<Action> pA = (*pT.Ptr)[index2];
            if (pA->Type == AC_SETFOREGROUNDWINDOW)
            {
                HTREEITEM hA0 = tvins.hInsertAfter;
                tvins.hParent = hA0;
                tvins.hInsertAfter = TVI_FIRST;
                auto pAx = dynamic_cast<SetForegroundWindowAction*>(pA.Ptr);
                for (auto iter = pAx->Begin; iter != pAx->End; iter++)
                {
                    tvins.hParent = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins)));
                }
                tvins.hParent = hT;
                tvins.hInsertAfter = hA0;
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
            else if (IsActionItemSelected)
            {
                m_cbs.OnActionItemSelected(hwnd, SelectedTarget, SelectedActionItemIndex);
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
        ULONG index = SelectedCredentialsIndex;
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
        ULONG index = SelectedCredentialsIndex;
        if (index > 0)
        {
            m_cc.Move(index, index - 1);
            std::vector<HTREEITEM> hChildren;
            GetItemsFromCredentialsList(hChildren);
            SendMessageW(m_hwnd, TVM_SELECTITEM, TVGN_CARET, reinterpret_cast<LPARAM>(hChildren[static_cast<size_t>(index) - 1]));
        }
    }
}


void ConfigurationTreeView::MoveDownSelectedCredentials()
{
    if (IsCredentialsSelected)
    {
        ULONG index = SelectedCredentialsIndex;
        if (index + 1 < m_cc.Count)
        {
            m_cc.Move(index, index + 1);
            std::vector<HTREEITEM> hChildren;
            GetItemsFromCredentialsList(hChildren);
            SendMessageW(m_hwnd, TVM_SELECTITEM, TVGN_CARET, reinterpret_cast<LPARAM>(hChildren[static_cast<size_t>(index) + 1]));
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
    m_tt.Append(Target::Create(L"(new)"));
    SendMessageW(m_hwnd, TVM_SELECTITEM, TVGN_CARET, reinterpret_cast<LPARAM>(hItem));
}


void ConfigurationTreeView::AddActionItem(RefPtr<Action> pAction)
{
    if (IsActionItemSelected || IsTargetSelected)
    {
        RefPtr<Target> pTarget = SelectedTarget;
        std::vector<HTREEITEM> hChildren;
        GetItemsFromTargetList(hChildren);
        TVINSERTSTRUCT tvins = { 0 };
        tvins.hParent = hChildren[SelectedTargetIndex];
        tvins.hInsertAfter = TVI_LAST;
        tvins.item.mask = TVIF_PARAM;
        tvins.item.lParam = ~0;
        HTREEITEM hItem = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins)));
        pTarget->Append(pAction);
        SendMessageW(m_hwnd, TVM_SELECTITEM, TVGN_CARET, reinterpret_cast<LPARAM>(hItem));
    }
}


void ConfigurationTreeView::RemoveSelectedTarget()
{
    if (IsTargetSelected)
    {
        ULONG index = SelectedTargetIndex;
        std::vector<HTREEITEM> hChildren;
        GetItemsFromTargetList(hChildren);
        SendMessageW(m_hwnd, TVM_SELECTITEM, TVGN_CARET, 0);
        SendMessageW(m_hwnd, TVM_DELETEITEM, 0, reinterpret_cast<LPARAM>(hChildren[index]));
        m_tt.Delete(index);
    }
}


void ConfigurationTreeView::RemoveSelectedActionItem()
{
    if (IsActionItemSelected)
    {
        ULONG index1 = SelectedTargetIndex;
        ULONG index2 = SelectedActionItemIndex;
        std::vector<HTREEITEM> hChildren1;
        GetItemsFromTargetList(hChildren1);
        std::vector<HTREEITEM> hChildren2;
        GetItemsFromTarget(m_tt[index1], hChildren1[index1], hChildren2);
        SendMessageW(m_hwnd, TVM_SELECTITEM, TVGN_CARET, 0);
        if (index2)
        {
            SendMessageW(m_hwnd, TVM_DELETEITEM, 0, reinterpret_cast<LPARAM>(hChildren2[index2]));
            m_tt[index1]->Delete(index2);
        }
        else
        {
            while (m_tt[index1]->Count)
            {
                index2 = m_tt[index1]->Count - 1;
                SendMessageW(m_hwnd, TVM_DELETEITEM, 0, reinterpret_cast<LPARAM>(hChildren2[index2]));
                m_tt[index1]->Delete(index2);
            }
        }
    }
}


void ConfigurationTreeView::MoveUpSelectedTarget()
{
    if (IsTargetSelected)
    {
        ULONG index = SelectedTargetIndex;
        if (index > 0)
        {
            RefPtr<Target> pTarget = m_tt[index];
            std::vector<HTREEITEM> hChildren;
            GetItemsFromTargetList(hChildren);
            SendMessageW(m_hwnd, TVM_DELETEITEM, 0, reinterpret_cast<LPARAM>(hChildren[index]));
            TVINSERTSTRUCT tvins = { 0 };
            tvins.hParent = m_hTT;
            tvins.hInsertAfter = index > 1 ? hChildren[static_cast<size_t>(index) - 2] : TVI_FIRST;
            tvins.item.mask = TVIF_PARAM;
            tvins.item.lParam = ~0;
            HTREEITEM hItem = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins)));
            for (ULONG index2 = 0; index2 < pTarget->Count; index2++)
            {
                tvins.hParent = hItem;
                tvins.hInsertAfter = TVI_LAST;
                HTREEITEM hItem2 = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins)));
                RefPtr<Action> pAction = (*pTarget.Ptr)[index2];
                if (pAction->Type == AC_SETFOREGROUNDWINDOW)
                {
                    auto pActionEx = dynamic_cast<SetForegroundWindowAction*>(pAction.Ptr);
                    for (auto iter = pActionEx->Begin; iter != pActionEx->End; iter++)
                    {
                        tvins.hParent = hItem2;
                        tvins.hInsertAfter = TVI_LAST;
                        hItem2 = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins)));
                    }
                }
            }
            SendMessageW(m_hwnd, TVM_EXPAND, TVE_EXPAND, reinterpret_cast<LPARAM>(hItem));
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
        ULONG from = SelectedActionItemIndex;
        ULONG to = from - 1;
        if (from > 1)
        {
            pTarget->Move(from, to);
            std::vector<HTREEITEM> hTT;
            GetItemsFromTargetList(hTT);
            std::vector<HTREEITEM> hAA;
            GetItemsFromTarget(pTarget, hTT[SelectedTargetIndex], hAA);
            SendMessageW(m_hwnd, TVM_SELECTITEM, TVGN_CARET, reinterpret_cast<LPARAM>(hAA[to]));
        }
    }
}


void ConfigurationTreeView::MoveDownSelectedTarget()
{
    if (IsTargetSelected)
    {
        ULONG index = SelectedTargetIndex;
        if (index + 1 < m_tt.Count)
        {
            RefPtr<Target> pTarget = m_tt[index];
            std::vector<HTREEITEM> hChildren;
            GetItemsFromTargetList(hChildren);
            SendMessageW(m_hwnd, TVM_DELETEITEM, 0, reinterpret_cast<LPARAM>(hChildren[index]));
            TVINSERTSTRUCT tvins = { 0 };
            tvins.hParent = m_hTT;
            tvins.hInsertAfter = hChildren[static_cast<size_t>(index) + 1];
            tvins.item.mask = TVIF_PARAM;
            tvins.item.lParam = ~0;
            HTREEITEM hItem = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins)));
            for (ULONG index2 = 0; index2 < pTarget->Count; index2++)
            {
                tvins.hParent = hItem;
                tvins.hInsertAfter = TVI_LAST;
                HTREEITEM hItem2 = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins)));
                RefPtr<Action> pAction = (*pTarget.Ptr)[index2];
                if (pAction->Type == AC_SETFOREGROUNDWINDOW)
                {
                    auto pActionEx = dynamic_cast<SetForegroundWindowAction*>(pAction.Ptr);
                    for (auto iter = pActionEx->Begin; iter != pActionEx->End; iter++)
                    {
                        tvins.hParent = hItem2;
                        tvins.hInsertAfter = TVI_LAST;
                        hItem2 = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins)));
                    }
                }
            }
            SendMessageW(m_hwnd, TVM_EXPAND, TVE_EXPAND, reinterpret_cast<LPARAM>(hItem));
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
        ULONG from = SelectedActionItemIndex;
        ULONG to = from + 1;
        if (0 < from && to < pTarget->Count)
        {
            std::vector<HTREEITEM> hTT;
            GetItemsFromTargetList(hTT);
            std::vector<HTREEITEM> hAA;
            GetItemsFromTarget(pTarget, hTT[SelectedTargetIndex], hAA);
            pTarget->Move(from, to);
            SendMessageW(m_hwnd, TVM_SELECTITEM, TVGN_CARET, reinterpret_cast<LPARAM>(hAA[to]));
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
        for (ULONG index = 0; index < m_cc.Count; index++)
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
        for (ULONG index = 0; index < m_tt.Count; index++)
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
        for (ULONG index = 0; index < m_cc.Count; index++)
        {
            RefreshCredentials(index, hChildren[index]);
        }
    }
}


void ConfigurationTreeView::RefreshCredentials(ULONG index, HTREEITEM hItem)
{
    StringBuffer szText(260);
    if (m_cc[index]->Key.Len > 0)
    {
        szText.AppendFormat(L"%s [%s]", m_cc[index]->Username.Ptr, m_cc[index]->Key.Ptr);
    }
    else
    {
        szText.Append(m_cc[index]->Username);
    }
    TVITEM item = { 0 };
    item.mask = TVIF_TEXT | TVIF_PARAM;
    item.pszText = &szText;
    item.cchTextMax = static_cast<int>(szText.Cap);
    item.hItem = hItem;
    item.lParam = CTV_INDEX(CTV_CITEM_LEVEL1, CTV_CITEM_LEVEL2_MIN + static_cast<size_t>(index));
    SendMessageW(m_hwnd, TVM_SETITEM, 0, reinterpret_cast<LPARAM>(&item));
    m_cc[index]->Callback = this;
}


void ConfigurationTreeView::RefreshTargetList()
{
    if (m_tt.Count)
    {
        std::vector<HTREEITEM> hChildren;
        GetItemsFromTargetList(hChildren);
        for (ULONG index = 0; index < m_tt.Count; index++)
        {
            RefreshTarget(index, hChildren[index]);
        }
    }
}


void ConfigurationTreeView::RefreshTarget(ULONG index, HTREEITEM hT)
{
    DBGFNC(L"ConfigurationTreeView::RefreshTarget");
    DBGPUT(L"index=%lu", index);
    RefPtr<Target> pTarget = m_tt[index];
    StringBuffer szText(MAX_PATH, pTarget->Name);
    TVITEM item = { 0 };
    item.mask = TVIF_TEXT | TVIF_PARAM;
    item.pszText = &szText;
    item.cchTextMax = static_cast<int>(szText.Cap);
    item.hItem = hT;
    item.lParam = CTV_INDEX(CTV_TNAME_LEVEL1, CTV_TNAME_LEVEL2_MIN + static_cast<size_t>(index));
    SendMessageW(m_hwnd, TVM_SETITEM, 0, reinterpret_cast<LPARAM>(&item));
    std::vector<HTREEITEM> hChildren;
    GetItemsFromTarget(pTarget, hT, hChildren);
    for (ULONG index2 = 0; index2 < pTarget->Count; index2++)
    {
        RefPtr<Action> pAction = (*pTarget.Ptr)[index2];
        switch (pAction->Type)
        {
        case AC_SETFOREGROUNDWINDOW:
        {
            HTREEITEM hX = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_GETNEXTITEM, TVGN_CHILD, reinterpret_cast<LPARAM>(hChildren[index2])));
            if (hX)
            {
                SendMessageW(m_hwnd, TVM_DELETEITEM, 0, reinterpret_cast<LPARAM>(hX));
            }
            TVINSERTSTRUCT tvins = { 0 };
            tvins.hParent = hChildren[index2];
            tvins.hInsertAfter = TVI_LAST;
            tvins.item.mask = TVIF_TEXT | TVIF_PARAM;
            auto pActionEx = dynamic_cast<SetForegroundWindowAction*>(pAction.Ptr);
            for (auto iter = pActionEx->Begin; iter != pActionEx->End; iter++)
            {
                szText.Format(L"FindWindow class=\"%s\" text=\"%s\"", iter->first.Ptr, iter->second.Ptr);
                tvins.item.pszText = &szText;
                tvins.item.cchTextMax = static_cast<int>(szText.Cap);
                tvins.item.lParam = CTV_INDEX(CTV_AITEM_LEVEL1_MIN + static_cast<size_t>(index), CTV_AITEM_LEVEL2_MIN + static_cast<size_t>(index2));
                DBGPUT(L"tvins.hParent=%p item.pszText=\"%s\" item.lParam=%zu", tvins.hParent, tvins.item.pszText, tvins.item.lParam);
                tvins.hParent = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvins)));
            }
            szText.Format(L"SetForegroundWindow class=\"%s\" text=\"%s\"", pActionEx->ClassName, pActionEx->WindowText);
            if (pActionEx->AccRole > 0)
            {
                if (pActionEx->AccName)
                {
                    const size_t Threshold = 30;
                    const WCHAR* pStart = pActionEx->AccName;
                    const WCHAR* pEnd = wcspbrk(pStart, L"\r\n");
                    if (pEnd)
                    {
                        if (pEnd - pStart < Threshold)
                        {
                            szText.AppendFormat(L" name=\"%.*s...\"", (int)(pEnd - pStart), pStart);
                        }
                        else
                        {
                            szText.AppendFormat(L" name=\"%.*s...\"", (int)(Threshold - 3), pStart);
                        }
                    }
                    else if (wcslen(pStart) < Threshold)
                    {
                        szText.AppendFormat(L" name=\"%s\"", pStart);
                    }
                    else
                    {
                        szText.AppendFormat(L" name=\"%.*s...\"", (int)(Threshold - 3), pStart);
                    }
                }
                else
                {
                    szText.AppendFormat(L" name=null");
                }
                szText.AppendFormat(L" role=%s", UiAutomation::GetRoleName(pActionEx->AccRole));
            }
            item.pszText = &szText;
            item.cchTextMax = static_cast<int>(szText.Cap);
            break;
        }
        case AC_TYPEKEY:
            szText.Format(L"Type %s", dynamic_cast<TypeKeyAction*>(pAction.Ptr)->Key);
            item.pszText = &szText;
            item.cchTextMax = static_cast<int>(szText.Cap);
            break;
        case AC_TYPEUNICODE:
            szText.Format(L"Type \"%s\"", dynamic_cast<TypeUnicodeAction*>(pAction.Ptr)->Text);
            item.pszText = &szText;
            item.cchTextMax = static_cast<int>(szText.Cap);
            break;
        case AC_TYPEUSERNAME:
        {
            PCWSTR psz = dynamic_cast<TypeUsernameAction*>(pAction.Ptr)->Name;
            if (psz && *psz)
            {
                szText.Format(L"TypeUsername [%s]", psz);
            }
            else
            {
                szText = L"TypeUsername";
            }
            item.pszText = &szText;
            item.cchTextMax = static_cast<int>(szText.Cap);
            break;
        }
        case AC_TYPEPASSWORD:
        {
            PCWSTR psz = dynamic_cast<TypePasswordAction*>(pAction.Ptr)->Name;
            if (psz && *psz)
            {
                szText.Format(L"TypePassword [%s]", psz);
            }
            else
            {
                szText = L"TypePassword";
            }
            item.pszText = &szText;
            item.cchTextMax = static_cast<int>(szText.Cap);
            break;
        }
        case AC_TYPEDELETESEQUENCE:
            szText = L"TypeDeleteSequence";
            item.pszText = &szText;
            item.cchTextMax = static_cast<int>(szText.Cap);
            break;
        case AC_LEFTCLICK:
            szText = L"LeftClick";
            item.pszText = &szText;
            item.cchTextMax = static_cast<int>(szText.Cap);
            break;
        default:
            szText = L"?";
            item.pszText = &szText;
            item.cchTextMax = static_cast<int>(szText.Cap);
            break;
        }
        item.hItem = hChildren[index2];
        item.lParam = CTV_INDEX(CTV_AITEM_LEVEL1_MIN + static_cast<size_t>(index), CTV_AITEM_LEVEL2_MIN + static_cast<size_t>(index2));
        DBGPUT(L"item.hItem=%p item.pszText=\"%s\" item.lParam=%zu", item.hItem, item.pszText, item.lParam);
        SendMessageW(m_hwnd, TVM_SETITEM, 0, reinterpret_cast<LPARAM>(&item));
    }
    pTarget->Callback = this;
}


void ConfigurationTreeView::GetItemsFromCredentialsList(std::vector<HTREEITEM>& hh)
{
    if (m_cc.Count)
    {
        HTREEITEM h = INVALID_HTREEITEM;
        for (ULONG index = 0; index < m_cc.Count; index++)
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
        HTREEITEM h = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_GETNEXTITEM, TVGN_CHILD, reinterpret_cast<LPARAM>(m_hTT)));
        hh.push_back(h);
        for (ULONG index = 1; index < m_tt.Count; index++)
        {
            h = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_GETNEXTITEM, TVGN_NEXT, reinterpret_cast<LPARAM>(h)));
            hh.push_back(h);
        }
    }
}


void ConfigurationTreeView::GetItemsFromTarget(RefPtr<Target> pTarget, HTREEITEM hParent, std::vector<HTREEITEM>& hChildren)
{
    if (pTarget->Count)
    {
        HTREEITEM hChild = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_GETNEXTITEM, TVGN_CHILD, reinterpret_cast<LPARAM>(hParent)));
        hChildren.push_back(hChild);
        for (ULONG index = 1; index < pTarget->Count; index++)
        {
            hChild = reinterpret_cast<HTREEITEM>(SendMessageW(m_hwnd, TVM_GETNEXTITEM, TVGN_NEXT, reinterpret_cast<LPARAM>(hChild)));
            hChildren.push_back(hChild);
        }
    }
}


bool ConfigurationTreeView::get_IsActionItemSelected() const
{
    ULONG index1 = CTV_LEVEL1 - CTV_AITEM_LEVEL1_MIN;
    ULONG index2 = CTV_LEVEL2 - CTV_AITEM_LEVEL2_MIN;
    return index1 < m_tt.Count && index2 < m_tt[index1]->Count;
}


ULONG ConfigurationTreeView::get_SelectedActionItemIndex() const
{
    ULONG index1 = CTV_LEVEL1 - CTV_AITEM_LEVEL1_MIN;
    ULONG index2 = CTV_LEVEL2 - CTV_AITEM_LEVEL2_MIN;
    return index1 < m_tt.Count && index2 < m_tt[index1]->Count ? static_cast<ULONG>(index2) : static_cast<ULONG>(-1);
}


RefPtr<Action> ConfigurationTreeView::get_SelectedActionItem() const
{
    return IsActionItemSelected ? (*SelectedTarget.Ptr)[SelectedActionItemIndex] : RefPtr<Action>();
}


void ConfigurationTreeView::set_SelectedActionItem(RefPtr<Action> pAction)
{
    if (IsActionItemSelected)
    {
        (*SelectedTarget.Ptr)[SelectedActionItemIndex] = pAction;
        SelectedTarget->InvokeCallback();
    }
}
