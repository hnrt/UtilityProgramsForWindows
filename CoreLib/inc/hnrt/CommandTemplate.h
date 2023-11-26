#pragma once


#include "hnrt/StringBuffer.h"


namespace hnrt
{
    class CommandTemplate
    {
    public:

        CommandTemplate();
        ~CommandTemplate() = default;
        PCWSTR Template() const { return m_Template; }
        PCWSTR CommandLine() const { return m_CommandLine; }
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

        StringBuffer m_Template;
        StringBuffer m_CommandLine;
        STARTUPINFO m_si;
        PROCESS_INFORMATION m_pi;
    };
}
