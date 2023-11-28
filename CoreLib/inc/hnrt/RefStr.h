#pragma once


#include "hnrt/RefObj.h"
#include "hnrt/StringOptions.h"


namespace hnrt
{
    class RefStr
        : public RefObj
    {
    public:

        void* operator new(size_t, size_t);
        void operator delete(void*);
        RefStr(const RefStr&) = delete;
        virtual ~RefStr();
        void operator =(const RefStr&) = delete;
        void ZeroFill();
        size_t get_len() const;

        __declspec(property(get = get_len)) size_t Len;

    private:

        RefStr(PCWSTR, size_t);
        RefStr(PCWSTR, va_list, size_t);
        RefStr(PCWSTR, size_t, PCWSTR, size_t);
        RefStr(PCWSTR, size_t, PCWSTR, va_list, size_t);
        RefStr(UINT, PCSTR, int, int);
        RefStr(StringOptions, PCWSTR, size_t);
        RefStr(StringOptions, PCWSTR, va_list, size_t);

        size_t m_len;
        WCHAR m_buf[1];

    public:

        static RefStr& Get(PCWSTR);
        static PCWSTR Get(RefStr*);
        static PCWSTR Create(PCWSTR, INT_PTR = -1);
        static PCWSTR Create(PCWSTR, va_list);
        static PCWSTR Create(PCWSTR, PCWSTR, INT_PTR = -1);
        static PCWSTR Create(PCWSTR, PCWSTR, va_list);
        static PCWSTR Create(UINT, PCSTR, INT_PTR = -1);
        static PCWSTR Create(StringOptions, PCWSTR);
        static PCWSTR Create(StringOptions, PCWSTR, va_list);
    };

    inline size_t RefStr::get_len() const
    {
        return m_len;
    }
}
