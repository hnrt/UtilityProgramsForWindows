#include "InputManager.h"
#include "hnrt/Path.h"
#include "hnrt/String.h"
#include "hnrt/KeystrokeMessageFlags.h"
#include "hnrt/Debug.h"
#include "hnrt/WindowsMessage.h"
#include "hnrt/VirtualKey.h"
#include "hnrt/Exception.h"
#include "hnrt/ResourceString.h"
#include "hnrt/Exception.h"
#include "resource.h"


using namespace hnrt;


#define FILENAME L"layout%04X.xml"


RefPtr<InputManager> InputManager::Create(PCWSTR pszAppDir)
{
    InputManager* km = new InputManager();
    HKL hkl = GetKeyboardLayout(GetCurrentThreadId());
    UINT id = static_cast<UINT>(reinterpret_cast<UINT_PTR>(hkl) & 0xFFFF);
    PCWSTR pszFileName = Path::Combine(pszAppDir, String::Format(FILENAME, id));
    if (!Path::Exists(pszFileName))
    {
        pszFileName = Path::Combine(pszAppDir, String::Format(FILENAME, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)));
        if (!Path::Exists(pszFileName))
        {
            throw Exception(ResourceString(IDS_KEYSTROKE_MANAGER_UNAVAILABLE));
        }
    }
    km->LoadKeyboardLayoutFile(pszFileName);
    return RefPtr<InputManager>(km);
}


InputManager::InputManager()
    : m_Interval(USER_TIMER_MINIMUM)
    , m_Delay(0)
    , m_Queue()
{
    memset(m_map, 0, sizeof(m_map));
}


InputManager::~InputManager()
{
}


void InputManager::set_Interval(LONG value)
{
    if (value < USER_TIMER_MINIMUM)
    {
        value = USER_TIMER_MINIMUM;
    }

    m_Interval = value;
}


InputManager& InputManager::AddMouseMove(LONG x, LONG y)
{
    m_Queue.push_back(SyntheticMouseInput(x, y, MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK));
    return *this;
}


InputManager& InputManager::AddMouseClickLeft()
{
    m_Queue.push_back(SyntheticMouseInput(MOUSEEVENTF_LEFTDOWN));
    m_Queue.push_back(SyntheticMouseInput(MOUSEEVENTF_LEFTUP));
    return *this;
}


static bool IsExtendedKey(WORD wVK)
{
    switch (wVK)
    {
    case VK_SNAPSHOT:
    case VK_HOME:
    case VK_END:
    case VK_INSERT:
    case VK_DELETE:
    case VK_PRIOR:
    case VK_NEXT:
    case VK_UP:
    case VK_RIGHT:
    case VK_DOWN:
    case VK_LEFT:
    case VK_NUMLOCK:
    case VK_DIVIDE:
    case VK_RETURN:
        return true;
    default:
        return false;
    }
}


InputManager& InputManager::AddKeyboardInput(PCWSTR psz)
{
    for (const WCHAR* pCur = psz; *pCur; pCur++)
    {
        WCHAR c = *pCur;
        if (0x0020 <= c && c <= 0x007E)
        {
            WORD vk = m_map[c] & IM_MASK_VIRTUALKEY;
            DWORD dwExtended = IsExtendedKey(vk) ? KEYEVENTF_EXTENDEDKEY : 0;
            if ((m_map[c] & IM_FLAG_SHIFT))
            {
                m_Queue.push_back(SyntheticKeyboardInput(VK_LSHIFT, 0));
                m_Queue.push_back(SyntheticKeyboardInput(vk, dwExtended));
                m_Queue.push_back(SyntheticKeyboardInput(vk, dwExtended | KEYEVENTF_KEYUP));
                m_Queue.push_back(SyntheticKeyboardInput(VK_LSHIFT, KEYEVENTF_KEYUP));
            }
            else
            {
                m_Queue.push_back(SyntheticKeyboardInput(vk, dwExtended));
                m_Queue.push_back(SyntheticKeyboardInput(vk, dwExtended | KEYEVENTF_KEYUP));
            }
        }
        else
        {
            m_Queue.push_back(SyntheticKeyboardInput(c, KEYEVENTF_UNICODE));
            m_Queue.push_back(SyntheticKeyboardInput(c, KEYEVENTF_UNICODE | KEYEVENTF_KEYUP));
        }
    }
    return *this;
}


InputManager& InputManager::AddKeyboardInput(WORD wVK)
{
    DWORD dwExtended = IsExtendedKey(wVK) ? KEYEVENTF_EXTENDEDKEY : 0;
    m_Queue.push_back(SyntheticKeyboardInput(wVK, dwExtended));
    m_Queue.push_back(SyntheticKeyboardInput(wVK, dwExtended | KEYEVENTF_KEYUP));
    return *this;
}


InputManager& InputManager::AddKeyboardInput(WORD wVK, DWORD dwFlags)
{
    DWORD dwExtended = IsExtendedKey(wVK) ? KEYEVENTF_EXTENDEDKEY : 0;
    if ((dwFlags & KEYEVENTF_KEYUP) == 0)
    {
        m_Queue.push_back(SyntheticKeyboardInput(wVK, dwExtended)); // KEYDOWN
    }
    else
    {
        m_Queue.push_back(SyntheticKeyboardInput(wVK, dwExtended | KEYEVENTF_KEYUP));
    }
    return *this;
}


bool InputManager::SendNext()
{
    if (m_Delay > 0)
    {
        m_Delay -= m_Interval;
        return true;
    }
    if (m_Queue.size())
    {
        SyntheticInput si = m_Queue.front();
        m_Queue.pop_front();
        INPUT input;
        SendInput(1, si.Write(&input), sizeof(INPUT));
        if (input.type == INPUT_MOUSE)
        {
            if ((input.mi.dwFlags & MOUSEEVENTF_MOVE))
            {
                DBGPUT(L"MOUSE MOVE %s(%d,%d)", (input.mi.dwFlags & MOUSEEVENTF_ABSOLUTE) ? L"ABS" : L"REL", input.mi.dx, input.mi.dy);
            }
            else if ((input.mi.dwFlags & MOUSEEVENTF_LEFTDOWN))
            {
                DBGPUT(L"MOUSE LEFTDOWN");
            }
            else if ((input.mi.dwFlags & MOUSEEVENTF_LEFTUP))
            {
                DBGPUT(L"MOUSE LEFTUP");
                m_Delay = 500;
            }
        }
        else if (input.type == INPUT_KEYBOARD)
        {
            if ((input.ki.dwFlags & KEYEVENTF_UNICODE))
            {
                DBGPUT(L"%s %c", (input.ki.dwFlags & KEYEVENTF_KEYUP) == 0 ? L"KEYDOWN" : L"KEYUP", input.ki.wScan);
            }
            else
            {
                DBGPUT(L"%s %s", (input.ki.dwFlags & KEYEVENTF_KEYUP) == 0 ? L"KEYDOWN" : L"KEYUP", VirtualKey(input.ki.wVk).Name);
            }
        }
        return true;
    }
    else
    {
        return false;
    }
}
