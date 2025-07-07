#pragma once

#include <set>
#include "hnrt/SpinLock.h"

namespace hnrt
{
    class String;
    class MultibyteString;

    typedef std::set<String> UcsStringSet;
    typedef std::set<MultibyteString> AcpStringSet;

    class StringStoreInternal
    {
    public:

        static StringStoreInternal& Instance();

        ~StringStoreInternal();
        StringStoreInternal(const StringStoreInternal&) = delete;
        void operator =(const StringStoreInternal&) = delete;
        String Get(PCWSTR psz);
        MultibyteString Get(PCSTR psz);

    private:

        StringStoreInternal();

        SpinLock::Type m_lockUcs;
        UcsStringSet m_setUcs;
        SpinLock::Type m_lockAcp;
        AcpStringSet m_setAcp;
    };
}
