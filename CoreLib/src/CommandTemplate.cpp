#include "pch.h"
#include "hnrt/CommandTemplate.h"
#include "hnrt/Heap.h"


using namespace hnrt;


CommandTemplate::CommandTemplate()
    : m_pszTemplate(NULL)
    , m_pszCommandLine(NULL)
{
    ZeroMemory(&m_si, sizeof(m_si));
    ZeroMemory(&m_pi, sizeof(m_pi));
}


CommandTemplate::~CommandTemplate()
{
    free(m_pszTemplate);
    free(m_pszCommandLine);
}


void CommandTemplate::Append(PCWSTR psz)
{
    SIZE_T cch1 = m_pszTemplate ? wcslen(m_pszTemplate) : 0;
    SIZE_T cch2 = wcslen(psz);
    if (psz[0] != L'\"' && wcspbrk(psz, L" "))
    {
        if (cch1)
        {
            m_pszTemplate = Allocate(m_pszTemplate, cch1 + cch2 + 4);
            _snwprintf_s(m_pszTemplate + cch1, cch2 + 4, _TRUNCATE, L" \"%s\"", psz);
        }
        else
        {
            m_pszTemplate = Allocate<WCHAR>(cch2 + 3);
            _snwprintf_s(m_pszTemplate, cch2 + 3, _TRUNCATE, L"\"%s\"", psz);
        }
    }
    else if (cch1)
    {
        m_pszTemplate = Allocate(m_pszTemplate, cch1 + cch2 + 2);
        _snwprintf_s(m_pszTemplate + cch1, cch2 + 2, _TRUNCATE, L" %s", psz);
    }
    else
    {
        m_pszTemplate = Allocate<WCHAR>(cch2 + 1);
        _snwprintf_s(m_pszTemplate, cch2 + 1, _TRUNCATE, L"%s", psz);
    }
}


bool CommandTemplate::HasPlaceholder() const
{
    return wcsstr(m_pszTemplate, L"{}") ? true : false;
}


void CommandTemplate::Set(PCWSTR pszInput)
{
    free(m_pszCommandLine);
    PWCHAR pPlaceholder = wcsstr(m_pszTemplate, L"{}");
    if (pPlaceholder)
    {
        SIZE_T cch2 = wcslen(pszInput);
        SIZE_T cch1 = wcslen(m_pszTemplate);
        SIZE_T cch0 = cch1 - 2 + cch2 + 1;
        m_pszCommandLine = Allocate<WCHAR>(cch0);
        SIZE_T cch3 = pPlaceholder - m_pszTemplate;
        wcsncpy_s(m_pszCommandLine, cch0, m_pszTemplate, cch3);
        wcscpy_s(m_pszCommandLine + cch3, cch0 - cch3, pszInput);
        wcscpy_s(m_pszCommandLine + cch3 + cch2, cch0 - (cch3 + cch2), m_pszTemplate + cch3 + 2);
    }
    else
    {
        m_pszCommandLine = Clone(m_pszTemplate);
    }
}


bool CommandTemplate::Run()
{
    ZeroMemory(&m_si, sizeof(m_si));
    ZeroMemory(&m_pi, sizeof(m_pi));
    m_si.cb = sizeof(m_si);
    return CreateProcessW(NULL, m_pszCommandLine, NULL, NULL, FALSE, 0, NULL, NULL, &m_si, &m_pi) ? true : false;
}


void CommandTemplate::Wait()
{
    WaitForSingleObject(m_pi.hProcess, INFINITE);
    CloseHandle(m_pi.hProcess);
    CloseHandle(m_pi.hThread);
}
