#pragma once


#include <Windows.h>
#include <string.h>
#include "hnrt/WindowDesign.h"
#include "hnrt/String.h"


namespace hnrt
{
    struct WindowMetrics
    {
        LONG Width;
        RectangleMetrics Padding;
        String szFontName;
        LONG FontSize;
        WindowMetrics();
    };

    inline WindowMetrics::WindowMetrics()
        : Width(400)
        , Padding()
        , szFontName(L"Segoe UI")
        , FontSize(12)
    {
    }

    struct ButtonMetrics
    {
        LONG Height;
        RectangleMetrics Margin;
        ButtonMetrics();
    };

    inline ButtonMetrics::ButtonMetrics()
        : Height(24)
        , Margin()
    {
    }
}
