#include "pch.h"
#include "hnrt/SyntheticInput.h"
#include "hnrt/Exception.h"
#include "hnrt/Debug.h"


using namespace hnrt;


static LONG s_CXSCREEN = 0;
static LONG s_CYSCREEN = 0;
static LONG s_CXVIRTUALSCREEN = 0;
static LONG s_CYVIRTUALSCREEN = 0;


void SyntheticInput::CheckSystemMetrics()
{
    s_CXSCREEN = GetSystemMetrics(SM_CXSCREEN);
    s_CYSCREEN = GetSystemMetrics(SM_CYSCREEN);
    s_CXVIRTUALSCREEN = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    s_CYVIRTUALSCREEN = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (s_CXSCREEN > 0)
    {
        Debug::Put(L"SyntheticInput: SM_CXSCREEN=%ld", s_CXSCREEN);
    }
    else
    {
        Debug::Put(L"SyntheticInput: Unable to determine the primary desktop width.");
        s_CXSCREEN = 1980;
    }
    if (s_CXVIRTUALSCREEN > 0)
    {
        Debug::Put(L"SyntheticInput: SM_CXVIRTUALSCREEN=%ld", s_CXVIRTUALSCREEN);
    }
    else
    {
        Debug::Put(L"SyntheticInput: Unable to determine the virtual desktop width.");
        s_CXVIRTUALSCREEN = s_CXSCREEN;
    }
    if (s_CYSCREEN > 0)
    {
        Debug::Put(L"SyntheticInput: SM_CYSCREEN=%ld", s_CYSCREEN);
    }
    else
    {
        Debug::Put(L"SyntheticInput: Unable to determine the primary desktop height.");
        s_CYSCREEN = 1080;
    }
    if (s_CYVIRTUALSCREEN > 0)
    {
        Debug::Put(L"SyntheticInput: SM_CYVIRTUALSCREEN=%ld", s_CYVIRTUALSCREEN);
    }
    else
    {
        Debug::Put(L"SyntheticInput: Unable to determine the virtual desktop height.");
        s_CYVIRTUALSCREEN = s_CYSCREEN;
    }
}


LONG SyntheticInput::NormalizeX(DWORD dwFlags, LONG x)
{
    if (!(dwFlags & MOUSEEVENTF_ABSOLUTE))
    {
        return x;
    }
    if (!s_CXSCREEN)
    {
        CheckSystemMetrics();
    }
    if ((dwFlags & MOUSEEVENTF_VIRTUALDESK))
    {
        if (x < 0) x = 0;
        else if (x >= s_CXVIRTUALSCREEN) x = s_CXVIRTUALSCREEN - 1;
        return (((655360 * x) / s_CXVIRTUALSCREEN) + 5) / 10;
    }
    else
    {
        if (x < 0) x = 0;
        else if (x >= s_CXSCREEN) x = s_CXSCREEN - 1;
        return (((655360 * x) / s_CXSCREEN) + 5) / 10;
    }
}


LONG SyntheticInput::NormalizeY(DWORD dwFlags, LONG y)
{
    if (!(dwFlags & MOUSEEVENTF_ABSOLUTE))
    {
        return y;
    }
    if (!s_CYSCREEN)
    {
        CheckSystemMetrics();
    }
    if ((dwFlags & MOUSEEVENTF_VIRTUALDESK))
    {
        if (y < 0) y = 0;
        else if (y >= s_CYVIRTUALSCREEN) y = s_CYVIRTUALSCREEN - 1;
        return (((655360 * y) / s_CYVIRTUALSCREEN) + 5) / 10;
    }
    else
    {
        if (y < 0) y = 0;
        else if (y >= s_CYSCREEN) y = s_CYSCREEN - 1;
        return (((655360 * y) / s_CYSCREEN) + 5) / 10;
    }
}


INPUT* SyntheticInput::Write(INPUT* pInput) const
{
    switch (m_dwType)
    {
    case INPUT_MOUSE:
        pInput->type = INPUT_MOUSE;
        pInput->mi.dx = NormalizeX(m_mi.dwFlags, m_mi.dx);
        pInput->mi.dy = NormalizeY(m_mi.dwFlags, m_mi.dy);
        pInput->mi.mouseData = m_mi.dwData;
        pInput->mi.dwFlags = m_mi.dwFlags;
        pInput->mi.time = 0;
        pInput->mi.dwExtraInfo = GetMessageExtraInfo();
        break;
    case INPUT_KEYBOARD:
        pInput->type = INPUT_KEYBOARD;
        pInput->ki.wVk = (m_ki.dwFlags & (KEYEVENTF_UNICODE | KEYEVENTF_SCANCODE)) == 0 ? m_ki.wData : 0;
        pInput->ki.wScan = (m_ki.dwFlags & (KEYEVENTF_UNICODE | KEYEVENTF_SCANCODE)) != 0 ? m_ki.wData : 0;
        pInput->ki.dwFlags = m_ki.dwFlags;
        pInput->ki.time = 0;
        pInput->ki.dwExtraInfo = GetMessageExtraInfo();
        break;
    default:
        memset(pInput, 0, sizeof(INPUT));
        pInput->type = ~0UL;
        break;
    }
    return pInput;
}
