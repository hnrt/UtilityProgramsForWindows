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
        virtual ULONG AddRef();
        virtual ULONG Release();

        void operator =(const RefObj& src);

    private:

        LONG m_RefCnt;

    public:

        ULONG get_RefCnt() const;

        __declspec(property(get = get_RefCnt)) ULONG RefCnt;
    };

    inline RefObj::RefObj()
        : m_RefCnt(1L)
    {
    }

    inline RefObj::RefObj(const RefObj& src)
        : m_RefCnt(src.m_RefCnt)
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

    inline void RefObj::operator =(const RefObj& src)
    {
        m_RefCnt = src.m_RefCnt;
    }

    inline ULONG RefObj::get_RefCnt() const
    {
        LONG count = m_RefCnt;
        return count > 0 ? count : 0;
    }
}
