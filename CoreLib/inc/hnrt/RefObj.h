#pragma once


#include <Windows.h>
#include <intrin.h>


namespace hnrt
{
    class RefObj
    {
    public:

        RefObj();
        RefObj(const RefObj& src);
        virtual ~RefObj() = default;
        void operator =(const RefObj& src);
        virtual ULONG AddRef();
        virtual ULONG Release();
        unsigned long get_RefCnt() const;
        __declspec(property(get = get_RefCnt)) ULONG RefCnt;

    protected:

        LONG m_RefCnt;
    };

    inline RefObj::RefObj()
        : m_RefCnt(1L)
    {
    }

    inline RefObj::RefObj(const RefObj& src)
        : m_RefCnt(1L)
    {
    }

    inline void RefObj::operator =(const RefObj& src)
    {
    }

    inline ULONG RefObj::AddRef()
    {
        LONG count = _InterlockedIncrement(&m_RefCnt);
        return count > 0 ? count : 0;
    }

    inline ULONG RefObj::Release()
    {
        LONG count = _InterlockedDecrement(&m_RefCnt);
        if (!count)
        {
            delete this;
        }
        return count > 0 ? count : 0;
    }

    inline ULONG RefObj::get_RefCnt() const
    {
        LONG count = m_RefCnt;
        return count > 0 ? count : 0;
    }
}
