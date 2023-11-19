#include "Action.h"
#include <string.h>
#include "hnrt/VirtualKey.h"
#include "hnrt/WindowHelper.h"


using namespace hnrt;


RefPtr<Action> Action::SetForegroundWindow(PCWSTR pszClassName, PCWSTR pszWindowText)
{
    return RefPtr<Action>(new SetForegroundWindowAction(pszClassName, pszWindowText));
}


RefPtr<Action> Action::TypeKey(PCWSTR psz)
{
    return RefPtr<Action>(new TypeKeyAction(psz));
}


RefPtr<Action> Action::TypeUnicode(PCWSTR psz)
{
    return RefPtr<Action>(new TypeUnicodeAction(psz));
}


RefPtr<Action> Action::TypeUsername(PCWSTR psz)
{
    return RefPtr<Action>(new TypeUsernameAction(psz));
}


RefPtr<Action> Action::TypePassword(PCWSTR psz)
{
    return RefPtr<Action>(new TypePasswordAction(psz));
}


RefPtr<Action> Action::TypeDeleteSequence()
{
    return RefPtr<Action>(new TypeDeleteSequenceAction());
}


RefPtr<Action> Action::LeftClick()
{
    return RefPtr<Action>(new LeftClickAction());
}


SetForegroundWindowAction::SetForegroundWindowAction(PCWSTR pszClassName, PCWSTR pszWindowText)
    : Action(AC_SETFOREGROUNDWINDOW)
    , m_Stack()
    , m_szAccName()
    , m_AccRole(-1)
{
    m_Stack.push_back(std::pair<String, String>(String(pszClassName), String(pszWindowText)));
}


SetForegroundWindowAction::SetForegroundWindowAction(const SetForegroundWindowAction& src)
    : Action(src)
    , m_Stack(src.m_Stack)
    , m_szAccName(src.m_szAccName)
    , m_AccRole(src.m_AccRole)
{
}


SetForegroundWindowAction& SetForegroundWindowAction::operator =(const SetForegroundWindowAction & src)
{
    Action::operator =(src);
    m_Stack = src.m_Stack;
    m_szAccName = src.m_szAccName;
    m_AccRole = src.m_AccRole;
    return *this;
}


void SetForegroundWindowAction::Prepend(PCWSTR pszClassName, PCWSTR pszWindowText)
{
    m_Stack.insert(m_Stack.begin(), std::pair<String, String>(String(pszClassName), String(pszWindowText)));
}


void SetForegroundWindowAction::Append(PCWSTR pszClassName, PCWSTR pszWindowText)
{
    m_Stack.push_back(std::pair<String, String>(String(pszClassName), String(pszWindowText)));
}


bool SetForegroundWindowAction::Find(HWND* phwnd1, HWND* phwnd2) const
{
    HWND hwnd2 = nullptr;
    HWND hwnd1 = FindWindowW(ClassName, WindowText);
    if (hwnd1)
    {
        if (!String::Compare(ClassName, L"ConsoleWindowClass"))
        {
            hwnd2 = WindowHelper(hwnd1).FindImeWindow();
        }
        else
        {
            hwnd2 = hwnd1;
            for (size_t index = 1; index < m_Stack.size(); index++)
            {
                Entry entry = m_Stack.at(index);
                HWND hwnd3 = WindowHelper(hwnd2).FindChildWindow(entry.first, entry.second);
                if (!hwnd3)
                {
                    break;
                }
                hwnd2 = hwnd3;
            }
        }
    }
    if (phwnd1)
    {
        *phwnd1 = hwnd1;
    }
    if (phwnd2)
    {
        *phwnd2 = hwnd2;
    }
    return hwnd1 && hwnd2;
}


void SetForegroundWindowAction::SetActiveAccessibility(PCWSTR pszName, LONG lRole)
{
    m_szAccName = pszName;
    m_AccRole = lRole;
}


void SetForegroundWindowAction::SetActiveAccessibility(PCWSTR pszName)
{
    m_szAccName = pszName;
}


void SetForegroundWindowAction::SetActiveAccessibility(LONG lRole)
{
    m_AccRole = lRole;
}


PCWSTR SetForegroundWindowAction::get_ClassName() const
{
    return m_Stack.at(0).first;

}


PCWSTR SetForegroundWindowAction::get_WindowText() const
{
    return m_Stack.at(0).second;
}


TypeKeyAction::TypeKeyAction(PCWSTR psz)
    : Action(AC_TYPEKEY)
    , m_sz(psz)
{
}


TypeKeyAction::TypeKeyAction(const TypeKeyAction& src)
    : Action(src)
    , m_sz(src.m_sz)
{
}


TypeKeyAction& TypeKeyAction::operator =(const TypeKeyAction& src)
{
    Action::operator =(src);
    m_sz = src.m_sz;
    return *this;
}


TypeUnicodeAction::TypeUnicodeAction(PCWSTR psz)
    : Action(AC_TYPEUNICODE)
    , m_sz(psz)
{
}


TypeUnicodeAction::TypeUnicodeAction(const TypeUnicodeAction& src)
    : Action(src)
    , m_sz(src.m_sz)
{
}


TypeUnicodeAction& TypeUnicodeAction::operator =(const TypeUnicodeAction& src)
{
    Action::operator =(src);
    m_sz = src.m_sz;
    return *this;
}


TypeUsernameAction::TypeUsernameAction(PCWSTR psz)
    : Action(AC_TYPEUSERNAME)
    , m_sz(psz)
{
}


TypeUsernameAction::TypeUsernameAction(const TypeUsernameAction& src)
    : Action(src)
    , m_sz(src.m_sz)
{
}


TypeUsernameAction& TypeUsernameAction::operator =(const TypeUsernameAction& src)
{
    Action::operator =(src);
    m_sz = src.m_sz;
    return *this;
}


TypePasswordAction::TypePasswordAction(PCWSTR psz)
    : Action(AC_TYPEPASSWORD)
    , m_sz(psz)
{
}


TypePasswordAction::TypePasswordAction(const TypePasswordAction& src)
    : Action(src)
    , m_sz(src.m_sz)
{
}


TypePasswordAction& TypePasswordAction::operator =(const TypePasswordAction& src)
{
    Action::operator =(src);
    m_sz = src.m_sz;
    return *this;
}
