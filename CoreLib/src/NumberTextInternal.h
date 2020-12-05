#pragma once


#include <Windows.h>


namespace hnrt
{
    class NumberFormatter
    {
    public:

        NumberFormatter();
        NumberFormatter(const NumberFormatter&) = delete;
        void operator =(const NumberFormatter&) = delete;
        const NUMBERFMTW* operator &() const { return &m_numfmt; }

    private:

        NUMBERFMTW m_numfmt;
        WCHAR m_szGrouping[10];
        WCHAR m_szDecimal[4];
        WCHAR m_szThousand[4];
    };
}
