#pragma once


#include "hnrt/String.h"


namespace hnrt
{
    class ResourceString
        : public String
    {
    public:

        static HINSTANCE m_hInstance;

        ResourceString(UINT uId);
        ResourceString(const ResourceString&);
        ~ResourceString();
        ResourceString& operator =(const ResourceString&);
    };
}
