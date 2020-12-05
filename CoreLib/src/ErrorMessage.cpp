#include "pch.h"
#include <map>
#include "hnrt/ErrorMessage.h"
#include "hnrt/String.h"
#include "hnrt/SpinLock.h"


using namespace hnrt;


typedef std::map<unsigned long, PCWSTR> MessageMap;
typedef std::pair<unsigned long, PCWSTR> MessageMapEntry;


PCWSTR ErrorMessage::Get(unsigned long code)
{
    static SpinLock::Type lockValue = SPIN_LOCK_INITIALIZER;
    static MessageMap msgMap;
    SpinLock lock(lockValue);
    MessageMap::const_iterator iter = msgMap.find(code);
    if (iter != msgMap.end())
    {
        return iter->second;
    }
    wchar_t* psz = nullptr;
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
    PCWSTR psz2 = String::Copy(psz);
    msgMap.insert(MessageMapEntry(code, psz2));
    LocalFree(reinterpret_cast<HLOCAL>(psz));
    return psz2;
}
