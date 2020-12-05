#pragma once


#include <string.h>


namespace hnrt
{
    struct LTRB
    {
        long Left;
        long Top;
        long Right;
        long Bottom;

        inline LTRB();
        inline LTRB(long left, long top, long right, long bottom);
        inline LTRB(const LTRB& src);
        ~LTRB() = default;
        inline void operator =(const LTRB& src);
        inline bool Parse(PCWSTR psz);
    };

    inline LTRB::LTRB()
        : Left(0)
        , Top(0)
        , Right(0)
        , Bottom(0)
    {
    }

    inline LTRB::LTRB(long left, long top, long right, long bottom)
        : Left(left)
        , Top(top)
        , Right(right)
        , Bottom(bottom)
    {
    }

    inline LTRB::LTRB(const LTRB& src)
        : Left(src.Left)
        , Top(src.Top)
        , Right(src.Right)
        , Bottom(src.Bottom)
    {
    }

    inline void LTRB::operator =(const LTRB& src)
    {
        Left = src.Left;
        Top = src.Top;
        Right = src.Right;
        Bottom = src.Bottom;
    }

    inline bool LTRB::Parse(PCWSTR psz)
    {
        wchar_t* pEnd;
        long n1 = wcstol(psz, &pEnd, 10);
        if (*pEnd == L'\0')
        {
            Left = n1;
            Top = n1;
            Right = n1;
            Bottom = n1;
            return true;
        }
        else if (*pEnd == L',')
        {
            long n2 = wcstol(++pEnd, &pEnd, 10);
            if (*pEnd == L'\0')
            {
                Left = n1;
                Top = n2;
                Right = n1;
                Bottom = n2;
                return true;
            }
            else if (*pEnd == L',')
            {
                long n3 = wcstol(++pEnd, &pEnd, 10);
                if (*pEnd == L',')
                {
                    long n4 = wcstol(++pEnd, &pEnd, 10);
                    if (*pEnd == L'\0')
                    {
                        Left = n1;
                        Top = n2;
                        Right = n3;
                        Bottom = n4;
                        return true;
                    }
                }
            }
        }
        return false;
    }

    typedef LTRB Margin;
    typedef LTRB Padding;

    struct WindowMetrics
    {
        long Width;
        Padding Padding;
        PCWSTR pszFontName;
        long FontSize;
        WindowMetrics();
    };

    inline WindowMetrics::WindowMetrics()
        : Width(400)
        , Padding()
        , pszFontName(L"Segoe UI")
        , FontSize(12)
    {
    }

    struct ButtonMetrics
    {
        long Height;
        Margin Margin;
        ButtonMetrics();
    };

    inline ButtonMetrics::ButtonMetrics()
        : Height(24)
        , Margin()
    {
    }
}
