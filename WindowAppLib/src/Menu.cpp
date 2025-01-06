#include "pch.h"
#include "hnrt/Buffer.h"
#include "hnrt/Menu.h"


using namespace hnrt;


Menu::Menu()
	: m_h(CreateMenu())
{
}


Menu::Menu(HMENU h)
    : m_h(h)
{
}


Menu::Menu(HWND hwnd)
    : m_h(GetMenu(hwnd))
{
}


Menu::Menu(const Menu& src)
    : m_h(src.m_h)
{
}


Menu Menu::operator [](size_t index) const
{
    return Menu(GetSubMenu(m_h, static_cast<int>(index)));
}


Menu Menu::operator [](PCWSTR psz) const
{
    return Menu(FindMenu(m_h, psz));
}


Menu& Menu::Set(HMENU h)
{
    m_h = h;
    return *this;
}


Menu& Menu::Add(PCWSTR psz, HMENU h)
{
	AppendMenuW(m_h, MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(h), psz);
	return *this;
}


Menu& Menu::Add(PCWSTR psz, UINT id, UINT flags)
{
    AppendMenuW(m_h, MF_STRING | flags, id, psz);
    return *this;
}


Menu& Menu::AddSeparator()
{
    AppendMenuW(m_h, MF_SEPARATOR, 0, NULL);
    return *this;
}


Menu& Menu::Insert(UINT position, PCWSTR psz, UINT id, UINT flags)
{
    MENUITEMINFOW info = { 0 };
    info.cbSize = sizeof(info);
    info.fMask = MIIM_ID | MIIM_FTYPE | MIIM_STRING | MIIM_STATE;
    info.wID = id;
    info.fType = MFT_STRING;
    info.dwTypeData = const_cast<LPWSTR>(psz);
    info.fState = flags;
    InsertMenuItemW(m_h, position, TRUE, &info);
    return *this;
}


Menu& Menu::InsertSeparator(UINT position)
{
    MENUITEMINFOW info = { 0 };
    info.cbSize = sizeof(info);
    info.fMask = MIIM_FTYPE;
    info.fType = MFT_SEPARATOR;
    InsertMenuItemW(m_h, position, TRUE, &info);
    return *this;
}


Menu& Menu::Modify(UINT uPosition, UINT uFlags, UINT_PTR id, PCWSTR psz)
{
    ModifyMenuW(m_h, uPosition, MF_STRING | uFlags, id, psz);
    return *this;
}


Menu& Menu::Enable(UINT uItem, UINT uEnable)
{
    EnableMenuItem(m_h, uItem, uEnable);
    return *this;
}


Menu& Menu::RemoveAll()
{
    int n = GetMenuItemCount(m_h);
    while (n > 0)
    {
        n--;
        DeleteMenu(m_h, n, MF_BYPOSITION);
    }
    return *this;
}


HMENU Menu::FindMenu(HMENU h, PCWSTR psz)
{
    int n = GetMenuItemCount(h);
    while (n > 0)
    {
        n--;
        MENUITEMINFOW info = { 0 };
        info.cbSize = sizeof(info);
        info.fMask = MIIM_STRING;
        if (GetMenuItemInfoW(h, n, TRUE, &info))
        {
            if (info.cch)
            {
                info.cch++;
                Buffer<WCHAR> buf(info.cch);
                info.dwTypeData = &buf;
                info.fMask = MIIM_STRING | MIIM_SUBMENU;
                if (GetMenuItemInfoW(h, n, TRUE, &info))
                {
                    if (!info.dwTypeData)
                    {
                        continue;
                    }
                    else if (!wcscmp(info.dwTypeData, psz))
                    {
                        return info.hSubMenu;
                    }
                }
            }
        }
    }
    return nullptr;
}
