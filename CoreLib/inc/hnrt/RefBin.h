#pragma once

#include "hnrt/RefObj.h"

namespace hnrt
{
	class RefBin
		: public RefObj
	{
	public:

        void* operator new(size_t, size_t);
        void operator delete(void*);
        void operator delete(void*, size_t);
        RefBin(const RefBin&) = delete;
        virtual ~RefBin();
        void operator =(const RefBin&) = delete;
        size_t get_len() const;

        __declspec(property(get = get_len)) size_t Len;

    private:

        RefBin(size_t);
        RefBin(const void*, size_t);

        size_t m_len;
        unsigned char m_buf[8];

    public:

        static RefBin& Get(void*);
        static void* Get(RefBin*);
        static void* Create(size_t);
        static void* Create(const void*, size_t);
    };

    inline size_t RefBin::get_len() const
    {
        return m_len;
    }
}
