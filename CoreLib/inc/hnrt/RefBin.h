#pragma once

#include "hnrt/RefObj.h"

namespace hnrt
{
	class RefBin
		: public RefObj
	{
    private:

        size_t m_Len;

        RefBin(size_t);
        RefBin(const void*, size_t);

    public:

        RefBin(const RefBin&) = delete;
        virtual ~RefBin();

        void* operator new(size_t, size_t);
        void operator delete(void*);
        void operator delete(void*, size_t);
        void operator =(const RefBin&) = delete;

    public:

        size_t get_Len() const;

        __declspec(property(get = get_Len)) size_t Len;

    public:

        static RefBin& Get(void*);
        static void* Get(RefBin&);
        static void* Create(size_t);
        static void* Create(const void*, size_t);
    };

    inline size_t RefBin::get_Len() const
    {
        return m_Len;
    }

    inline void* RefBin::Get(RefBin& bin)
    {
        return &bin + 1;
    }

    inline RefBin& RefBin::Get(void* ptr)
    {
        return *(reinterpret_cast<RefBin*>(ptr) - 1);
    }

    inline void* RefBin::Create(size_t len)
    {
        return len ? Get(*(new(len) RefBin(len))) : nullptr;
    }

    inline void* RefBin::Create(const void* ptr, size_t len)
    {
        return (ptr && len) ? Get(*(new(len) RefBin(ptr, len))) : nullptr;
    }

    inline void* BinAddRef(void* ptr)
    {
        if (ptr)
        {
            RefBin::Get(ptr).AddRef();
        }
        return ptr;
    }

    inline void BinRelease(void* ptr)
    {
        if (ptr)
        {
            RefBin::Get(ptr).Release();
        }
    }
}
