#include "Ghost.h"
#include "hnrt/UiAutomationFactory.h"
#include "hnrt/VirtualKey.h"
#include "hnrt/Exception.h"
#include "hnrt/Debug.h"


using namespace hnrt;


Ghost::Ghost()
    : m_pConfiguration()
    , m_pInputManager()
    , m_KeyboardMouseBridge()
    , m_pTarget()
    , m_ActionIter()
    , m_hwndTarget(nullptr)
    , m_PreviousKeyboardState(0)
    , m_CurrentKeyboardState(0)
    , m_pAutomation()
{
}


void Ghost::Initialize(RefPtr<Configuration> pConfiguration)
{
    m_pConfiguration = pConfiguration;

    m_pInputManager = InputManager::Create(m_pConfiguration->AppDir, m_pConfiguration->InstallDir);
    m_pInputManager->Interval = m_pConfiguration->TypingInterval > USER_TIMER_MINIMUM ? m_pConfiguration->TypingInterval : USER_TIMER_MINIMUM;

    m_KeyboardMouseBridge.StartServer();
    if (!m_KeyboardMouseBridge.SetToggleKeySequence(L"LCONTROL+LSHIFT+ESCAPE"))
    {
        throw Exception(L"SetToggleKeySequence failed.");
    }
}


void Ghost::Uninitialize()
{
    m_KeyboardMouseBridge.StopServer();
}


bool Ghost::IsProcessing() const
{
    return m_pTarget;
}


bool Ghost::Start(RefPtr<Target> pTarget)
{
    DBGFNC(L"Ghost::Start");

    m_ActionIter = pTarget->Begin;
    if (m_ActionIter == pTarget->End)
    {
        DBGPUT(L"No action.");
        return false;
    }

    auto pAction = dynamic_cast<const SetForegroundWindowAction*>(m_ActionIter->Ptr);
    m_ActionIter++;
    if (!pAction)
    {
        DBGPUT(L"Action#1 is not of SetForegroundWindowAction.");
        return false;
    }
    DBGPUT(L"class=\"%s\" text=\"%s\"", pAction->ClassName, pAction->WindowText);

    HWND hwndTopLevel;
    HWND hwndTarget;
    if (!pAction->Find(&hwndTopLevel, &hwndTarget))
    {
        DBGPUT(L"Window not found.");
        return false;
    }
    DBGPUT(L"toplevel=%p target=%p", hwndTopLevel, hwndTarget);

    BOOL bRet;

    bRet = IsIconic(hwndTopLevel);
    DBGPUT(L"IsIconic returned %s.", bRet ? L"TRUE" : L"FALSE");
    if (bRet)
    {
        bRet = ShowWindow(hwndTopLevel, SW_SHOWNORMAL);
        DBGPUT(L"ShowWindow(SHOWNORMAL) returned %s.", bRet ? L"TRUE" : L"FALSE");
    }

    bRet = SetForegroundWindow(hwndTopLevel);
    DBGPUT(L"SetForegroundWindow returned %s.", bRet ? L"TRUE" : L"FALSE");
    if (!bRet)
    {
        return false;
    }

    m_pTarget = pTarget;
    m_hwndTarget = hwndTarget;

    if (m_pTarget->BlockKeybd || m_pTarget->BlockMouse)
    {
        DWORD dwFlags = 0;
        if (m_pTarget->BlockKeybd)
        {
            dwFlags |= KEYBOARD_FLAG_BLOCK;
        }
        if (m_pTarget->BlockMouse)
        {
            dwFlags |= MOUSE_FLAG_BLOCK;
        }
        m_KeyboardMouseBridge.SetBlockInputState(dwFlags);
    }

    if (m_KeyboardMouseBridge.StartAgent(hwndTarget))
    {
        m_PreviousKeyboardState = m_KeyboardMouseBridge.GetKeyboardState(hwndTarget, KEYBOARD_FLAG_OPENCLOSE);
        if (KEYBOARD_STATE_UNPACK_OPENCLOSE(m_PreviousKeyboardState))
        {
            m_CurrentKeyboardState = m_KeyboardMouseBridge.SetKeyboardState(hwndTarget, KEYBOARD_STATE_PACK(DWORD, 0, 0, 0), KEYBOARD_FLAG_OPENCLOSE);
        }
        else
        {
            m_CurrentKeyboardState = m_PreviousKeyboardState;
        }
    }
    else
    {
        m_CurrentKeyboardState = m_PreviousKeyboardState = 0;
    }

    m_pAutomation = UiAutomationFactory::Create(hwndTarget, pAction->AccName, pAction->AccRole);
    if (m_pAutomation)
    {
        DBGPUT(L"AA: DefaultAction=%s", m_pAutomation->DefaultAction);
    }
    else
    {
        m_pAutomation = UiAutomationFactory::Create(hwndTopLevel, pAction->AccName, pAction->AccRole);
        if (m_pAutomation)
        {
            DBGPUT(L"AA: Alternative: DefaultAction=%s", m_pAutomation->DefaultAction);
        }
    }

    m_pInputManager->Delay = m_pConfiguration->TypingDelay;

    return true;
}


int Ghost::Process()
{
    DBGFNC(L"Ghost::Process");

    if (m_pInputManager->SendNext())
    {
        return GHOST_PROCESSED;
    }

    while (m_ActionIter != m_pTarget->End)
    {
        RefPtr<Action> pAction = *m_ActionIter;
        m_ActionIter++;
        DBGPUT(L"ActionType=%ld", pAction->Type);
        switch (pAction->Type)
        {
        case AC_TYPEKEY:
        {
            PCWSTR pszKey = dynamic_cast<TypeKeyAction*>(pAction.Ptr)->Key;
            if (pszKey)
            {
                VirtualKey vk(pszKey);
                if (vk.Value)
                {
                    m_pInputManager->AddKeyboardInput(vk);
                    m_pInputManager->SendNext();
                    return GHOST_PROCESSED;
                }
                else
                {
                    m_ActionIter = m_pTarget->End;
                    return GHOST_ERROR_INVALID_VIRTUAL_KEY_NAME;
                }
            }
            break;
        }

        case AC_TYPEUNICODE:
        {
            PCWSTR pszText = dynamic_cast<TypeUnicodeAction*>(pAction.Ptr)->Text;
            if (pszText && pszText[0])
            {
                if ((pAction->Flags & AC_FLAG_AA))
                {
                    if (m_pAutomation)
                    {
                        m_pAutomation->Value = pszText;
                    }
                    else
                    {
                        m_ActionIter = m_pTarget->End;
                        return GHOST_ERROR_ACTIVE_ACCESSIBILITY_UNAVAILABLE;
                    }
                }
                else
                {
                    m_pInputManager->AddKeyboardInput(pszText);
                    m_pInputManager->SendNext();
                }
                return GHOST_PROCESSED;
            }
            break;
        }

        case AC_TYPEUSERNAME:
        {
            PCWSTR pszName = dynamic_cast<TypeUsernameAction*>(pAction.Ptr)->Name;
            RefPtr<Credentials> pCredentials = pszName && *pszName ? m_pConfiguration->CredentialsList[pszName] : m_pConfiguration->CredentialsList.DefaultCredentials;
            if (pCredentials && pCredentials->Username && pCredentials->Username[0])
            {
                if ((pAction->Flags & AC_FLAG_AA))
                {
                    if (m_pAutomation)
                    {
                        m_pAutomation->Value = pCredentials->Username;
                    }
                    else
                    {
                        m_ActionIter = m_pTarget->End;
                        return GHOST_ERROR_ACTIVE_ACCESSIBILITY_UNAVAILABLE;
                    }
                }
                else
                {
                    m_pInputManager->AddKeyboardInput(pCredentials->Username);
                    m_pInputManager->SendNext();
                }
                return GHOST_PROCESSED;
            }
            break;
        }

        case AC_TYPEPASSWORD:
        {
            PCWSTR pszName = dynamic_cast<TypePasswordAction*>(pAction.Ptr)->Name;
            RefPtr<Credentials> pCredentials = pszName && *pszName ? m_pConfiguration->CredentialsList[pszName] : m_pConfiguration->CredentialsList.DefaultCredentials;
            if (pCredentials && pCredentials->Password && pCredentials->Password[0])
            {
                if ((pAction->Flags & AC_FLAG_AA))
                {
                    if (m_pAutomation)
                    {
                        m_pAutomation->Value = pCredentials->Password;
                    }
                    else
                    {
                        pCredentials->ClearPlainText();
                        m_ActionIter = m_pTarget->End;
                        return GHOST_ERROR_ACTIVE_ACCESSIBILITY_UNAVAILABLE;
                    }
                }
                else
                {
                    m_pInputManager->AddKeyboardInput(pCredentials->Password);
                    m_pInputManager->SendNext();
                }
                pCredentials->ClearPlainText();
                return GHOST_PROCESSED;
            }
            break;
        }

        case AC_TYPEDELETESEQUENCE:
            m_pInputManager->AddKeyboardInput(VK_END);
            m_pInputManager->AddKeyboardInput(VK_LSHIFT, KEYEVENTF_KEYDOWN);
            m_pInputManager->AddKeyboardInput(VK_HOME);
            m_pInputManager->AddKeyboardInput(VK_LSHIFT, KEYEVENTF_KEYUP);
            m_pInputManager->AddKeyboardInput(VK_DELETE);
            m_pInputManager->SendNext();
            return GHOST_PROCESSED;

        case AC_LEFTCLICK:
            if ((pAction->Flags & AC_FLAG_AA))
            {
                if (m_pAutomation)
                {
                    RECT rect = { 0 };
                    if (m_pAutomation->Locate(rect))
                    {
                        DBGPUT(L"AA: (%ld,%ld) %ldx%ld", rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
                        m_pInputManager->AddMouseMove((rect.left + rect.right) / 2, (rect.top + rect.bottom) / 2);
                        m_pInputManager->AddMouseClickLeft();
                        m_pInputManager->SendNext();
                        return GHOST_PROCESSED;
                    }
                    else
                    {
                        m_ActionIter = m_pTarget->End;
                        return GHOST_ERROR_ACTIVE_ACCESSIBILITY_LOCATE_PART;
                    }
                }
                else
                {
                    m_ActionIter = m_pTarget->End;
                    return GHOST_ERROR_ACTIVE_ACCESSIBILITY_UNAVAILABLE;
                }
            }
            else if (m_hwndTarget)
            {
                RECT rect = { 0 };
                if (GetWindowRect(m_hwndTarget, &rect))
                {
                    DBGPUT(L"GetWindowRect: (%ld,%ld) %ldx%ld", rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
                    m_pInputManager->AddMouseMove((rect.left + rect.right) / 2, (rect.top + rect.bottom) / 2);
                    m_pInputManager->AddMouseClickLeft();
                    m_pInputManager->SendNext();
                    return GHOST_PROCESSED;
                }
                else
                {
                    m_ActionIter = m_pTarget->End;
                    return GHOST_ERROR_LOCATE_WINDOW;
                }
            }
            break;

        default:
            break;
        }
    }

    return GHOST_COMPLETED;
}


void Ghost::End()
{
    DBGFNC(L"Ghost::End");
    if (m_hwndTarget)
    {
        if (m_PreviousKeyboardState != m_CurrentKeyboardState)
        {
            m_CurrentKeyboardState = m_KeyboardMouseBridge.SetKeyboardState(m_hwndTarget, m_PreviousKeyboardState, KEYBOARD_FLAG_OPENCLOSE);
        }
        m_KeyboardMouseBridge.StopAgent(m_hwndTarget);
        m_KeyboardMouseBridge.SetBlockInputState(0);
        m_hwndTarget = nullptr;
    }
    m_pTarget = RefPtr<Target>();
}
