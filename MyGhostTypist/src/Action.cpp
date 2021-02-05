#include "Action.h"
#include <string.h>
#include "hnrt/String.h"
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
    , m_pszAccName(nullptr)
    , m_AccRole(-1)
{
    m_Stack.push_back(std::pair<PCWSTR, PCWSTR>(String::Copy(pszClassName), String::Copy(pszWindowText)));
}


SetForegroundWindowAction::SetForegroundWindowAction(const SetForegroundWindowAction& src)
    : Action(src)
    , m_Stack(src.m_Stack)
    , m_pszAccName(src.m_pszAccName)
    , m_AccRole(src.m_AccRole)
{
}


SetForegroundWindowAction& SetForegroundWindowAction::operator =(const SetForegroundWindowAction & src)
{
    Action::operator =(src);
    m_Stack = src.m_Stack;
    m_pszAccName = src.m_pszAccName;
    m_AccRole = src.m_AccRole;
    return *this;
}


void SetForegroundWindowAction::Prepend(PCWSTR pszClassName, PCWSTR pszWindowText)
{
    m_Stack.insert(m_Stack.begin(), std::pair<PCWSTR, PCWSTR>(String::Copy(pszClassName), String::Copy(pszWindowText)));
}


void SetForegroundWindowAction::Append(PCWSTR pszClassName, PCWSTR pszWindowText)
{
    m_Stack.push_back(std::pair<PCWSTR, PCWSTR>(String::Copy(pszClassName), String::Copy(pszWindowText)));
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
                hwnd2 = WindowHelper(hwnd2).FindChildWindow(entry.first, entry.second);
                if (!hwnd2)
                {
                    entry = m_Stack.at(m_Stack.size() - 1);
                    hwnd2 = WindowHelper(hwnd1).FindChildWindow2(entry.first, entry.second);
                    break;
                }
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
    m_pszAccName = pszName ? String::Copy(pszName) : nullptr;
    m_AccRole = lRole;
}


void SetForegroundWindowAction::SetActiveAccessibility(PCWSTR pszName)
{
    m_pszAccName = pszName ? String::Copy(pszName) : nullptr;
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
    , m_psz(String::Copy(psz))
{
}


TypeKeyAction::TypeKeyAction(const TypeKeyAction& src)
    : Action(src)
    , m_psz(src.m_psz)
{
}


TypeKeyAction& TypeKeyAction::operator =(const TypeKeyAction& src)
{
    Action::operator =(src);
    m_psz = src.m_psz;
    return *this;
}


TypeUnicodeAction::TypeUnicodeAction(PCWSTR psz)
    : Action(AC_TYPEUNICODE)
    , m_psz(String::Copy(psz))
{
}


TypeUnicodeAction::TypeUnicodeAction(const TypeUnicodeAction& src)
    : Action(src)
    , m_psz(src.m_psz)
{
}


TypeUnicodeAction& TypeUnicodeAction::operator =(const TypeUnicodeAction& src)
{
    Action::operator =(src);
    m_psz = src.m_psz;
    return *this;
}


TypeUsernameAction::TypeUsernameAction(PCWSTR psz)
    : Action(AC_TYPEUSERNAME)
    , m_psz(String::Copy(psz))
{
}


TypeUsernameAction::TypeUsernameAction(const TypeUsernameAction& src)
    : Action(src)
    , m_psz(src.m_psz)
{
}


TypeUsernameAction& TypeUsernameAction::operator =(const TypeUsernameAction& src)
{
    Action::operator =(src);
    m_psz = src.m_psz;
    return *this;
}


TypePasswordAction::TypePasswordAction(PCWSTR psz)
    : Action(AC_TYPEPASSWORD)
    , m_psz(String::Copy(psz))
{
}


TypePasswordAction::TypePasswordAction(const TypePasswordAction& src)
    : Action(src)
    , m_psz(src.m_psz)
{
}


TypePasswordAction& TypePasswordAction::operator =(const TypePasswordAction& src)
{
    Action::operator =(src);
    m_psz = src.m_psz;
    return *this;
}
