#pragma once


#include "hnrt/VersionInfo.h"


namespace hnrt
{
    class VersionInfoInternal
        : public VersionInfo
    {
    public:

        VersionInfoInternal(PCWSTR pszFileName);
        VersionInfoInternal(const VersionInfoInternal&) = delete;
        virtual ~VersionInfoInternal();
        void operator =(const VersionInfoInternal&) = delete;
        virtual bool Select(unsigned short wLanguage);

    private:

        void QueryString(WORD wLanguage, WORD wCodePage, PCWSTR pszName, wchar_t*& pszValue);

        PVOID m_pData;
    };
}
