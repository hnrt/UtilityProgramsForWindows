#pragma once


#include <Windows.h>


namespace hnrt
{
    class CommandTemplate
    {
    public:

        CommandTemplate();
        ~CommandTemplate();
        PCWSTR Template() const { return m_pszTemplate; }
        PCWSTR CommandLine() const { return m_pszCommandLine; }
        DWORD ProcessId() const { return m_pi.dwProcessId; }
        DWORD ThreadId() const { return m_pi.dwThreadId; }
        void Append(PCWSTR);
        bool HasPlaceholder() const;
        void Set(PCWSTR);
        bool Run();
        void Wait();

        CommandTemplate(const CommandTemplate&) = delete;
        void operator =(const CommandTemplate&) = delete;

    private:

        PWSTR m_pszTemplate;
        PWSTR m_pszCommandLine;
        STARTUPINFO m_si;
        PROCESS_INFORMATION m_pi;
    };
}
