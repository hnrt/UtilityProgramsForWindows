#include "pch.h"
#include "hnrt/CommandTemplate.h"
#include "hnrt/Heap.h"


using namespace hnrt;


CommandTemplate::CommandTemplate()
    : m_Template(MAX_PATH)
    , m_CommandLine(MAX_PATH)
    , m_si()
    , m_pi()
{
}


void CommandTemplate::Append(PCWSTR psz)
{
    if (psz[0] != L'\"' && wcspbrk(psz, L" "))
    {
        if (m_Template.Len)
        {
            m_Template.AppendFormat(L" \"%s\"", psz);
        }
        else
        {
            m_Template.AppendFormat(L"\"%s\"", psz);
        }
    }
    else if (m_Template.Len)
    {
        m_Template.AppendFormat(L" %s", psz);
    }
    else
    {
        m_Template.Append(psz);
    }
}


bool CommandTemplate::HasPlaceholder() const
{
    return wcsstr(m_Template, L"{}") ? true : false;
}


void CommandTemplate::Set(PCWSTR pszInput)
{
    PWCHAR pPlaceholder = wcsstr(m_Template, L"{}");
    if (pPlaceholder)
    {
        size_t cch = pPlaceholder - m_Template;
        m_CommandLine.Assign(m_Template, cch);
        m_CommandLine.Append(pszInput);
        m_CommandLine.Append(m_Template + cch + 2);
    }
    else
    {
        m_CommandLine = m_Template;
    }
}


bool CommandTemplate::Run()
{
    ZeroMemory(&m_si, sizeof(m_si));
    ZeroMemory(&m_pi, sizeof(m_pi));
    m_si.cb = sizeof(m_si);
    return CreateProcessW(NULL, m_CommandLine, NULL, NULL, FALSE, 0, NULL, NULL, &m_si, &m_pi) ? true : false;
}


void CommandTemplate::Wait()
{
    WaitForSingleObject(m_pi.hProcess, INFINITE);
    CloseHandle(m_pi.hProcess);
    CloseHandle(m_pi.hThread);
}
