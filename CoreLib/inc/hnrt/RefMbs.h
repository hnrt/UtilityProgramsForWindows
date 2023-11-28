#pragma once


#include "hnrt/RefObj.h"
#include "hnrt/StringOptions.h"


namespace hnrt
{
    class RefMbs
        : public RefObj
    {
    public:

        void* operator new(size_t, size_t);
        void operator delete(void*);
        RefMbs(const RefMbs&) = delete;
        virtual ~RefMbs();
        void operator =(const RefMbs&) = delete;
        void ZeroFill();
        size_t get_len() const;

        __declspec(property(get = get_len)) size_t Len;

    private:

        RefMbs(PCSTR, size_t);
        RefMbs(PCSTR, va_list, size_t);
        RefMbs(PCSTR, size_t, PCSTR, size_t);
        RefMbs(PCSTR, size_t, PCSTR, va_list, size_t);
        RefMbs(UINT, PCWSTR, int, int);
        RefMbs(StringOptions, PCSTR, size_t);
        RefMbs(StringOptions, PCSTR, va_list, size_t);

        size_t m_len;
        CHAR m_buf[1];

    public:

        static RefMbs& Get(PCSTR);
        static PCSTR Get(RefMbs*);
        static PCSTR Create(PCSTR, INT_PTR = -1);
        static PCSTR Create(PCSTR, va_list);
        static PCSTR Create(PCSTR, PCSTR, INT_PTR = -1);
        static PCSTR Create(PCSTR, PCSTR, va_list);
        static PCSTR Create(UINT, PCWSTR, INT_PTR = -1);
        static PCSTR Create(StringOptions, PCSTR);
        static PCSTR Create(StringOptions, PCSTR, va_list);
    };

    inline size_t RefMbs::get_len() const
    {
        return m_len;
    }
}
