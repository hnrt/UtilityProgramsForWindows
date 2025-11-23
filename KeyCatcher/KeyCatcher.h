#pragma once

#include <list>
#include <Windows.h>
#include "hnrt/String.h"

namespace hnrt
{
    class KeyCatcher
    {
    public:

        KeyCatcher(HINSTANCE hInstance);
        KeyCatcher(const KeyCatcher&) = delete;
        ~KeyCatcher();
        void operator =(const KeyCatcher&) = delete;
        void Initialize(int nCmdShow);
        int Run();
        String get_Name() const;
        String get_FileName() const;
        void set_FileName(String pszFileName);

        __declspec(property(get = get_Name)) String Name;
        __declspec(property(get = get_FileName, put = set_FileName)) String FileName;

    private:

        static ATOM m_ClassAtom;
        static LONG m_RefCount;

        static LRESULT CALLBACK MessageCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        void PushBack(UINT uMsg, WPARAM wParam, LPARAM lParam);
        void Save();

        struct KeyMessage
        {
            UINT uMessage;
            WPARAM wParam;
            LPARAM lParam;

            KeyMessage(UINT, WPARAM, LPARAM);
            KeyMessage(const KeyMessage&);
            ~KeyMessage() = default;
            void operator =(const KeyMessage&);
        };

        String m_ExePath;
        HINSTANCE m_hInstance;
        std::list<KeyMessage> m_KeyInput;
        String m_FileName;
    };
}
