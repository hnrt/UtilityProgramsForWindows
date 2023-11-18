#pragma once


#include <set>
#include "hnrt/String.h"
#include "hnrt/StringAcp.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/SpinLock.h"


namespace hnrt
{
    typedef std::set<PCWSTR, StringLessThan> UcsStringSet;
    typedef std::set<PCSTR, StringAcpLessThan> AcpStringSet;

    class StringStoreInternal
    {
    public:

        static StringStoreInternal& Instance();

        ~StringStoreInternal();
        StringStoreInternal(const StringStoreInternal&) = delete;
        void operator =(const StringStoreInternal&) = delete;
        PCWSTR Get(PCWSTR psz);
        PCSTR Get(PCSTR psz);
        PCWSTR Set(PWSTR psz);
        PCSTR Set(PSTR psz);
        PCWSTR Set(StringBuffer& buf);

    private:

        StringStoreInternal();

        SpinLock::Type m_lockUcs;
        UcsStringSet m_setUcs;
        SpinLock::Type m_lockAcp;
        AcpStringSet m_setAcp;
    };
}
