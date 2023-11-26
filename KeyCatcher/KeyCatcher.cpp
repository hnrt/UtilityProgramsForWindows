#include "KeyCatcher.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hnrt/Debug.h"
#include "hnrt/WindowsMessage.h"
#include "hnrt/KeystrokeMessageFlags.h"
#include "hnrt/VirtualKey.h"
#include "hnrt/FileWriter.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/Unicode.h"
#include "hnrt/Exception.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/Heap.h"


using namespace hnrt;


ATOM KeyCatcher::m_ClassAtom = 0;
LONG KeyCatcher::m_RefCount = 0;


static const WCHAR s_szClassName[] = { L"HnrtKeyCatcher" };


static PWSTR GetExePath()
{
    static WCHAR szPath[MAX_PATH];
    if (!szPath[0])
    {
        if (!GetModuleFileNameW(NULL, szPath, MAX_PATH))
        {
            DWORD dwError = GetLastError();
            throw Exception(L"KeyCatcher::ctor failed to get the module path. (Error %lu %s)", dwError, ErrorMessage::Get(dwError));
        }
    }
    return Clone(szPath);
}


KeyCatcher::KeyCatcher(HINSTANCE hInstance)
    : m_pszExePath(GetExePath())
    , m_hInstance(hInstance)
    , m_KeyInput()
    , m_pszFileName(nullptr)
{
    if (InterlockedIncrement(&m_RefCount) == 1)
    {
        WNDCLASSEXW wc;
        ZeroMemory(&wc, sizeof(wc));
        wc.cbSize = sizeof(wc);
        wc.style = 0;
        wc.lpfnWndProc = MessageCallback;
        wc.hInstance = hInstance;
        wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wc.lpszClassName = s_szClassName;
        m_ClassAtom = RegisterClassExW(&wc);
        if (!m_ClassAtom)
        {
            DWORD dwError = GetLastError();
            throw Exception(L"KeyCatcher::ctor failed to register a window class. (Error %lu %s)", dwError, ErrorMessage::Get(dwError));
        }
    }
}


KeyCatcher::~KeyCatcher()
{
    if (InterlockedDecrement(&m_RefCount) == 0)
    {
        UnregisterClassW(s_szClassName, m_hInstance);
    }
    free(m_pszFileName);
    free(m_pszExePath);
}


void KeyCatcher::Initialize(int nCmdShow)
{
    HWND hwnd = CreateWindowExW(0, s_szClassName, L"KeyCatcher", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, m_hInstance, this);
    if (!hwnd)
    {
        DWORD dwError = GetLastError();
        throw Exception(L"KeyCatcher::Initialize failed to create a window. (Error %lu %s)", dwError, ErrorMessage::Get(dwError));
    }

    ShowWindow(hwnd, nCmdShow);
}


LRESULT CALLBACK KeyCatcher::MessageCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    KeyCatcher* pThis = reinterpret_cast<KeyCatcher*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (pThis)
    {
        KeystrokeMessage km = { 0 };
        switch (uMsg)
        {
        case WM_CREATE:
            Debug::Put(L"WM_CREATE");
            break;
        case WM_DESTROY:
            Debug::Put(L"WM_DESTROY");
            pThis->Save();
            PostQuitMessage(EXIT_SUCCESS);
            return 0;
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
            km.param = lParam;
            Debug::Put(L"%s: wParam=%02X(%s) lParam=%08X RepeatCount=%u ScanCode=%u ExtendedKey=%u ContextCode=%u PreviousKeyState=%u TransitionState=%u",
                WindowsMessage(uMsg).Name,
                VirtualKey(wParam).Value, VirtualKey(wParam).Name,
                static_cast<UINT>(lParam),
                km.flags.RepeatCount, km.flags.ScanCode, km.flags.ExtendedKey, km.flags.ContextCode, km.flags.PreviousKeyState, km.flags.TransitionState);
            if (pThis->m_pszFileName)
            {
                pThis->PushBack(uMsg, wParam, lParam);
            }
            break;
        case WM_CHAR:
        case WM_SYSCHAR:
        case WM_UNICHAR:
        case WM_DEADCHAR:
        case WM_SYSDEADCHAR:
            km.param = lParam;
            Debug::Put(L"%s: wParam=%04X(%c) lParam=%08X RepeatCount=%u ScanCode=%u ExtendedKey=%u ContextCode=%u PreviousKeyState=%u TransitionState=%u",
                WindowsMessage(uMsg).Name,
                static_cast<UINT>(wParam), static_cast<UINT>(wParam),
                static_cast<UINT>(lParam),
                km.flags.RepeatCount, km.flags.ScanCode, km.flags.ExtendedKey, km.flags.ContextCode, km.flags.PreviousKeyState, km.flags.TransitionState);
            if (pThis->m_pszFileName)
            {
                pThis->PushBack(uMsg, wParam, lParam);
            }
            break;
        default:
            Debug::Put(L"%s", WindowsMessage(uMsg).Name);
            break;
        }
    }
    else if (uMsg == WM_NCCREATE)
    {
        Debug::Put(L"WM_NCCREATE");
        pThis = reinterpret_cast<KeyCatcher*>(reinterpret_cast<CREATESTRUCTW*>(lParam)->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else
    {
        Debug::Put(L"%s", WindowsMessage(uMsg).Name);
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}


int KeyCatcher::Run()
{
    MSG msg = { 0 };
    while (GetMessageW(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return msg.message == WM_QUIT ? static_cast<int>(msg.wParam) : EXIT_FAILURE;
}


PCWSTR KeyCatcher::get_Name() const
{
    return !m_pszExePath ? L"keycatcher.exe" : wcschr(m_pszExePath, L'\\') ? wcsrchr(m_pszExePath, L'\\') + 1 : m_pszExePath;
}


PCWSTR KeyCatcher::get_FileName() const
{
    return m_pszFileName;
}


void KeyCatcher::set_FileName(PCWSTR pszFileName)
{
    Deallocate(m_pszFileName);
    m_pszFileName = Clone(pszFileName);
}


void KeyCatcher::PushBack(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
    if (!m_KeyInput.empty())
    {
        const KeyMessage& last = m_KeyInput.back();
        if (uMessage == last.uMessage && wParam == last.wParam && lParam == last.lParam)
        {
            return;
        }
    }
    m_KeyInput.push_back(KeyMessage(uMessage, wParam, lParam));
}


void KeyCatcher::Save()
{
    if (!m_pszFileName)
    {
        return;
    }
    StringBuffer s(8192);
    s.AppendFormat(L"%cMessage,WPARAM,LPARAM,RepeatCount,ScanCode,ExtendedKey,ContextCode,PreviousKeyState,TransitionState\r\n", BYTE_ORDER_MARK);
    for (std::list<KeyMessage>::const_iterator iter = m_KeyInput.begin(); iter != m_KeyInput.end(); iter++)
    {
        const KeyMessage& km = *iter;
        KeystrokeMessage k = { 0 };
        k.param = km.lParam;
        switch (km.uMessage)
        {
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
            s.AppendFormat(L"%s,%s,%08X,%u,%u,%u,%u,%u,%u\r\n",
                WindowsMessage(km.uMessage).Name,
                VirtualKey(km.wParam).Name,
                static_cast<UINT>(km.lParam),
                k.flags.RepeatCount, k.flags.ScanCode, k.flags.ExtendedKey, k.flags.ContextCode, k.flags.PreviousKeyState, k.flags.TransitionState);
            break;
        case WM_CHAR:
        case WM_SYSCHAR:
        case WM_UNICHAR:
        case WM_DEADCHAR:
        case WM_SYSDEADCHAR:
            s.AppendFormat(L"%s", WindowsMessage(km.uMessage).Name);
            {
                WCHAR c = static_cast<WCHAR>(km.wParam);
                if (iswprint(c) && c != L'\"')
                {
                    s.AppendFormat(L",\"%c\"", c);
                }
                else
                {
                    s.AppendFormat(L",U+%04X", c);
                }
            }
            s.AppendFormat(L",%08X,%u,%u,%u,%u,%u,%u\r\n",
                static_cast<UINT>(km.lParam),
                k.flags.RepeatCount, k.flags.ScanCode, k.flags.ExtendedKey, k.flags.ContextCode, k.flags.PreviousKeyState, k.flags.TransitionState);
            break;
        default:
            break;
        }
    }
    FileWriter f(m_pszFileName);
    f.Write(s.Ptr, s.Len * sizeof(WCHAR));
    f.Close();
}


KeyCatcher::KeyMessage::KeyMessage(UINT uMessage_, WPARAM wParam_, LPARAM lParam_)
    : uMessage(uMessage_)
    , wParam(wParam_)
    , lParam(lParam_)
{
}


KeyCatcher::KeyMessage::KeyMessage(const KeyMessage& src)
    : uMessage(src.uMessage)
    , wParam(src.wParam)
    , lParam(src.lParam)
{
}


void KeyCatcher::KeyMessage::operator =(const KeyMessage& src)
{
    uMessage = src.uMessage;
    wParam = src.wParam;
    lParam = src.lParam;
}
