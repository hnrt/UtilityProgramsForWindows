#pragma once


#include "hnrt/String.h"


namespace hnrt
{
    struct ICabinetExtractCallbacks
    {
        virtual String CabinetExtractGetPath(PCWSTR pszFile) = 0;
        virtual void CabinetExtractOnStart(PCWSTR pszFile, PCWSTR pszPath) = 0;
        virtual bool CabinetExtractOnEnd(PCWSTR pszFile, PCWSTR pszPath) = 0;
        virtual bool CabinetExtractOnError(PCWSTR pszFile, PCWSTR pszPath, unsigned long error) = 0;
    };

    struct ICabinet
    {
        virtual long AddRef() = 0;
        virtual long Release() = 0;
        virtual PCWSTR get_Path() const = 0;
        virtual void set_Path(PCWSTR pszPath) = 0;
        virtual void SetData(const void* pData, size_t cbSize) = 0;
        virtual const BYTE* get_Ptr() const = 0;
        virtual size_t get_Len() const = 0;
        virtual void Extract(ICabinetExtractCallbacks& callbacks) = 0;

        __declspec(property(get = get_Path, put = set_Path)) PCWSTR Path;
        __declspec(property(get = get_Ptr)) const BYTE* Ptr;
        __declspec(property(get = get_Len)) size_t Len;
    };
}
