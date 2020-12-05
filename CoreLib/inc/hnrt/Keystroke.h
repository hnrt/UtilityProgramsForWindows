#pragma once


#include <Windows.h>


namespace hnrt
{
    class Keystroke
    {
    public:

        Keystroke(WORD wData, DWORD dwFlags);
        Keystroke(const Keystroke& src);
        ~Keystroke() = default;
        Keystroke& operator =(const Keystroke& src);
        WORD get_VirtualKey() const;
        WORD get_ScanCode() const;
        DWORD get_Flags() const;
        __declspec(property(get = get_VirtualKey)) WORD VirtualKey;
        __declspec(property(get = get_ScanCode)) WORD ScanCode;
        __declspec(property(get = get_Flags)) DWORD Flags;

    private:

        WORD m_wData;
        DWORD m_dwFlags;
    };

    inline Keystroke::Keystroke(WORD wData, DWORD dwFlags)
        : m_wData(wData)
        , m_dwFlags(dwFlags)
    {
    }

    inline Keystroke::Keystroke(const Keystroke& src)
        : m_wData(src.m_wData)
        , m_dwFlags(src.m_dwFlags)
    {
    }

    inline Keystroke& Keystroke::operator =(const Keystroke& src)
    {
        m_wData = src.m_wData;
        m_dwFlags = src.m_dwFlags;
        return *this;
    }

    inline WORD Keystroke::get_VirtualKey() const
    {
        return (m_dwFlags & (KEYEVENTF_UNICODE | KEYEVENTF_SCANCODE)) == 0 ? m_wData : 0;
    }

    inline WORD Keystroke::get_ScanCode() const
    {
        return (m_dwFlags & (KEYEVENTF_UNICODE | KEYEVENTF_SCANCODE)) != 0 ? m_wData : 0;
    }

    inline DWORD Keystroke::get_Flags() const
    {
        return m_dwFlags;
    }
}
