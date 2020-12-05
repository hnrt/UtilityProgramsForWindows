#pragma once


#include <Windows.h>
#include <list>


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
        PCWSTR get_Name() const;
        PCWSTR get_FileName() const;
        void set_FileName(PCWSTR pszFileName);

        __declspec(property(get = get_Name)) PCWSTR Name;
        __declspec(property(get = get_FileName, put = set_FileName)) PCWSTR FileName;

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

        PWSTR m_pszExePath;
        HINSTANCE m_hInstance;
        std::list<KeyMessage> m_KeyInput;
        PWSTR m_pszFileName;
    };
}
