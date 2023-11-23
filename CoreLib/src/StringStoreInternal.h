#pragma once


#include <set>
#include "hnrt/String.h"
#include "hnrt/StringAcp.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/SpinLock.h"


namespace hnrt
{
    typedef std::set<String> UcsStringSet;
    typedef std::set<StringAcp> AcpStringSet;

    class StringStoreInternal
    {
    public:

        static StringStoreInternal& Instance();

        ~StringStoreInternal();
        StringStoreInternal(const StringStoreInternal&) = delete;
        void operator =(const StringStoreInternal&) = delete;
        PCWSTR Get(PCWSTR psz);
        PCSTR Get(PCSTR psz);

    private:

        StringStoreInternal();

        SpinLock::Type m_lockUcs;
        UcsStringSet m_setUcs;
        SpinLock::Type m_lockAcp;
        AcpStringSet m_setAcp;
    };
}
