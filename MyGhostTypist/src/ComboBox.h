#pragma once


#include "hnrt/String.h"


namespace hnrt
{
    class ComboBox
    {
    public:

        ComboBox();
        ComboBox(const ComboBox&) = delete;
        ~ComboBox() = default;
        void operator =(const ComboBox&) = delete;
        void Init(HWND hwnd);
        void Clear();
        void Add(PCWSTR psz);
        bool Select(PCWSTR psz);
        void Set(PCWSTR psz);
        String get_Selected() const;
        __declspec(property(get = get_Selected)) String Selected;

    protected:

        HWND m_hwnd;
    };
}
