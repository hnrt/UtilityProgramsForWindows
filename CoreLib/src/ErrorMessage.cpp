#include "pch.h"
#include <map>
#include "hnrt/ErrorMessage.h"
#include "hnrt/String.h"
#include "hnrt/SpinLock.h"


using namespace hnrt;


typedef std::map<DWORD, PCWSTR> MessageMap;
typedef std::pair<DWORD, PCWSTR> MessageMapEntry;


PCWSTR ErrorMessage::Get(DWORD code)
{
    static SpinLock::Type lockValue = SPIN_LOCK_INITIALIZER;
    static MessageMap msgMap;
    SpinLock lock(lockValue);
    MessageMap::const_iterator iter = msgMap.find(code);
    if (iter != msgMap.end())
    {
        return iter->second;
    }
    PWSTR psz = nullptr;
    if (!FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, code, 0, reinterpret_cast<LPWSTR>(&psz), 260, NULL))
    {
        return String::Format(L"Error %lu", code);
    }
    PWCHAR pCur = psz + wcslen(psz);
    while (psz < pCur && pCur[-1] == L'\n')
    {
        pCur--;
        if (psz < pCur && pCur[-1] == L'\r')
        {
            pCur--;
        }
    }
    pCur[0] = L'\0';
    msgMap.insert(MessageMapEntry(code, psz));
    return psz;
}
