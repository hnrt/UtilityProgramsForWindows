#pragma once


#include <new.h>


namespace hnrt
{
    class CoreLibrary
    {
    public:

        CoreLibrary();
        CoreLibrary(const CoreLibrary&) = delete;
        ~CoreLibrary();
        void operator =(const CoreLibrary&) = delete;

    private:

        static int __cdecl OutOfMemory(size_t size);

        _PNH m_pnh0;
        int m_NewMode0;
    };
}
