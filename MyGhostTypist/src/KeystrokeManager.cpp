#include "KeystrokeManager.h"
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


RefPtr<KeystrokeManager> KeystrokeManager::Create(PCWSTR pszAppDir)
{
    KeystrokeManager* km = new KeystrokeManager();
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
    km->LoadLayout(pszFileName);
    return RefPtr<KeystrokeManager>(km);
}


KeystrokeManager::KeystrokeManager()
    : m_Interval(USER_TIMER_MINIMUM)
    , m_Delay(0)
    , m_Queue()
    , m_hwnd(NULL)
    , m_uTimerId(0)
{
    memset(m_map, 0, sizeof(m_map));
}


KeystrokeManager::~KeystrokeManager()
{
}


void KeystrokeManager::set_Interval(long value)
{
    if (m_uTimerId)
    {
        KillTimer(m_hwnd, m_uTimerId);
    }

    if (value < USER_TIMER_MINIMUM)
    {
        value = USER_TIMER_MINIMUM;
    }

    m_Interval = value;

    if (m_uTimerId)
    {
        SetTimer(m_hwnd, m_uTimerId, m_Interval, NULL);
    }
}


void KeystrokeManager::Start(HWND hwnd, UINT_PTR uId)
{
    if (m_uTimerId)
    {
        KillTimer(m_hwnd, m_uTimerId);
    }

    m_uTimerId = SetTimer(hwnd, uId, m_Interval, NULL);
    m_hwnd = hwnd;
}


void KeystrokeManager::Stop()
{
    KillTimer(m_hwnd, m_uTimerId);
    m_hwnd = NULL;
    m_uTimerId = 0;
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


KeystrokeManager& KeystrokeManager::Add(PCWSTR psz)
{
    for (const WCHAR* pCur = psz; *pCur; pCur++)
    {
        WCHAR c = *pCur;
        if (0x0020 <= c && c <= 0x007E)
        {
            WORD vk = m_map[c] & KM_MASK_VIRTUALKEY;
            DWORD dwExtended = IsExtendedKey(vk) ? KEYEVENTF_EXTENDEDKEY : 0;
            if ((m_map[c] & KM_FLAG_SHIFT))
            {
                m_Queue.push_back(Keystroke(VK_LSHIFT, 0));
                m_Queue.push_back(Keystroke(vk, dwExtended));
                m_Queue.push_back(Keystroke(vk, dwExtended | KEYEVENTF_KEYUP));
                m_Queue.push_back(Keystroke(VK_LSHIFT, KEYEVENTF_KEYUP));
            }
            else
            {
                m_Queue.push_back(Keystroke(vk, dwExtended));
                m_Queue.push_back(Keystroke(vk, dwExtended | KEYEVENTF_KEYUP));
            }
        }
        else
        {
            m_Queue.push_back(Keystroke(c, KEYEVENTF_UNICODE));
            m_Queue.push_back(Keystroke(c, KEYEVENTF_UNICODE | KEYEVENTF_KEYUP));
        }
    }
    return *this;
}


KeystrokeManager& KeystrokeManager::Add(WORD wVK)
{
    DWORD dwExtended = IsExtendedKey(wVK) ? KEYEVENTF_EXTENDEDKEY : 0;
    m_Queue.push_back(Keystroke(wVK, dwExtended));
    m_Queue.push_back(Keystroke(wVK, dwExtended | KEYEVENTF_KEYUP));
    return *this;
}


KeystrokeManager& KeystrokeManager::Add(WORD wVK, DWORD dwFlags)
{
    DWORD dwExtended = IsExtendedKey(wVK) ? KEYEVENTF_EXTENDEDKEY : 0;
    if ((dwFlags & KEYEVENTF_KEYUP) == 0)
    {
        m_Queue.push_back(Keystroke(wVK, dwExtended));
    }
    else
    {
        m_Queue.push_back(Keystroke(wVK, dwExtended | KEYEVENTF_KEYUP));
    }
    return *this;
}


bool KeystrokeManager::TypeNext()
{
    if (m_Delay > 0)
    {
        m_Delay -= m_Interval;
        return true;
    }
    if (m_Queue.size())
    {
        Keystroke ks = m_Queue.front();
        m_Queue.pop_front();
#ifdef _DEBUG
        if ((ks.Flags & KEYEVENTF_UNICODE))
        {
            DBGPUT(L"%s %c", (ks.Flags & KEYEVENTF_KEYUP) == 0 ? L"KEYDOWN" : L"KEYUP", ks.ScanCode);
        }
        else
        {
            DBGPUT(L"%s %s", (ks.Flags & KEYEVENTF_KEYUP) == 0 ? L"KEYDOWN" : L"KEYUP", VirtualKey(ks.VirtualKey).Name);
        }
#endif
        INPUT si;
        si.type = INPUT_KEYBOARD;
        si.ki.wVk = ks.VirtualKey;
        si.ki.wScan = ks.ScanCode;
        si.ki.dwFlags = ks.Flags;
        si.ki.time = 0;
        si.ki.dwExtraInfo = GetMessageExtraInfo();
        SendInput(1, &si, sizeof(INPUT));
        return true;
    }
    else
    {
        return false;
    }
}
