#include "pch.h"
#include "hnrt/TabControlItem.h"
#include "hnrt/TabControl.h"


using namespace hnrt;


TabControlItem::TabControlItem()
{
	memset(this, 0, sizeof(*this));
}


TabControlItem::TabControlItem(const TabControlItem& src)
{
    mask = src.mask;
    if ((src.mask & TCIF_STATE) != 0)
    {
        dwState = src.dwState;
        dwStateMask = src.dwStateMask;
    }
    else
    {
        dwState = 0;
        dwStateMask = 0;
    }
    if ((src.mask & TCIF_TEXT) != 0)
    {
        pszText = new WCHAR[src.cchTextMax];
        cchTextMax = src.cchTextMax;
        wcscpy_s(pszText, cchTextMax, src.pszText);
    }
    else
    {
        pszText = 0;
        cchTextMax = 0;
    }
    iImage = ((src.mask & TCIF_IMAGE) != 0) ? src.iImage : 0;
    lParam = ((src.mask & TCIF_PARAM) != 0) ? src.lParam : 0;
}


TabControlItem::~TabControlItem()
{
    delete pszText;
    memset(this, 0, sizeof(*this));
}


TabControlItem& TabControlItem::operator =(const TabControlItem& src)
{
    mask = src.mask;
    if ((src.mask & TCIF_STATE) != 0)
    {
        dwState = src.dwState;
        dwStateMask = src.dwStateMask;
    }
    else
    {
        dwState = 0;
        dwStateMask = 0;
    }
    delete pszText;
    if ((src.mask & TCIF_TEXT) != 0)
    {
        pszText = new WCHAR[src.cchTextMax];
        cchTextMax = src.cchTextMax;
        wcscpy_s(pszText, cchTextMax, src.pszText);
    }
    else
    {
        pszText = 0;
        cchTextMax = 0;
    }
    iImage = ((src.mask & TCIF_IMAGE) != 0) ? src.iImage : 0;
    lParam = ((src.mask & TCIF_PARAM) != 0) ? src.lParam : 0;
    return *this;
}


TabControlItem& TabControlItem::SetState(DWORD dwState, DWORD dwStateMask)
{
    this->mask |= TCIF_STATE;
    this->dwState = dwState;
    this->dwStateMask = dwStateMask == ~0UL ? dwState : dwStateMask;
    return *this;
}


TabControlItem& TabControlItem::SetText(PCWSTR pszText)
{
    if ((this->mask & TCIF_TEXT) != 0)
    {
        delete this->pszText;
    }
    this->mask |= TCIF_TEXT;
    int cchTextMax = static_cast<int>(wcslen(pszText) + 1);
    this->pszText = new WCHAR[cchTextMax];
    this->cchTextMax = cchTextMax;
    wcscpy_s(this->pszText, this->cchTextMax, pszText);
    return *this;
}


TabControlItem& TabControlItem::SetImage(INT iImage)
{
    this->mask |= TCIF_IMAGE;
    this->iImage = iImage;
    return *this;
}


TabControlItem& TabControlItem::SetParam(LPARAM lParam)
{
    this->mask |= TCIF_PARAM;
    this->lParam = lParam;
    return *this;
}


INT TabControlItem::InsertInto(HWND hwnd, INT iIndex)
{
    if (iIndex < 0)
    {
        iIndex = TabControl(hwnd).ItemCount;
        if (iIndex < 0)
        {
            return -1;
        }
    }
    return static_cast<INT>(SendMessageW(hwnd, TCM_INSERTITEMW, iIndex, reinterpret_cast<LPARAM>(this)));
}


TabControlItem& TabControlItem::GetFrom(HWND hwnd, INT iIndex)
{
    INT iCount = TabControl(hwnd).GetItemCount();
    if (iIndex >= 0 && iIndex < iCount)
    {
        delete pszText;
        memset(this, 0, sizeof(*this));
        WCHAR sz[1024] = { 0 };
        pszText = sz;
        cchTextMax = _countof(sz);
        if (SendMessageW(hwnd, TCM_GETITEM, iIndex, reinterpret_cast<LPARAM>(this)))
        {
            if ((mask & TCIF_TEXT) != 0)
            {
                cchTextMax = static_cast<int>(wcslen(sz)) + 1;
                pszText = new WCHAR[cchTextMax];
                wcscpy_s(pszText, cchTextMax, sz);
            }
            else
            {
                pszText = 0;
                cchTextMax = 0;
            }
        }
        else
        {
            pszText = 0;
            cchTextMax = 0;
        }
        return *this;
    }
    else if (iIndex < 0 && iCount + iIndex >= 0)
    {
        return GetFrom(hwnd, iCount + iIndex);
    }
    else
    {
        delete pszText;
        memset(this, 0, sizeof(*this));
        return *this;
    }
}
