#pragma once


#include <Windows.h>


namespace hnrt
{
    struct KeystrokeMessageFlags
    {
        unsigned RepeatCount : 16;
        unsigned ScanCode : 8;
        unsigned ExtendedKey : 1;
        unsigned Reserved : 4;
        unsigned ContextCode : 1;
        unsigned PreviousKeyState : 1;
        unsigned TransitionState : 1;
    };

    union KeystrokeMessage
    {
        KeystrokeMessageFlags flags;
        LPARAM param;
    };

    class KeystrokeMessageBuilder
    {
    public:

        KeystrokeMessageBuilder(unsigned int uRepeatCount, unsigned int uScanCode, unsigned int uExtendedKey = 0, unsigned int uContextCode = 0, unsigned int uPreviousKeyState = 0, unsigned int uTransitionState = 0);
        KeystrokeMessageBuilder(const KeystrokeMessageBuilder&) = delete;
        ~KeystrokeMessageBuilder() = default;
        void operator =(const KeystrokeMessageBuilder&) = delete;
        LPARAM get_LParam() const;
        operator LPARAM() const;
        KeystrokeMessageFlags get_Flags() const;
        __declspec(property(get = get_LParam)) LPARAM LParam;
        __declspec(property(get = get_Flags)) KeystrokeMessageFlags Flags;

    private:

        KeystrokeMessage m_value;
    };

    inline KeystrokeMessageBuilder::KeystrokeMessageBuilder(unsigned int uRepeatCount, unsigned int uScanCode, unsigned int uExtendedKey, unsigned int uContextCode, unsigned int uPreviousKeyState, unsigned int uTransitionState)
    {
        m_value.param = 0;
        m_value.flags.RepeatCount = uRepeatCount;
        m_value.flags.ScanCode = uScanCode;
        m_value.flags.ExtendedKey = uExtendedKey;
        m_value.flags.ContextCode = uContextCode;
        m_value.flags.PreviousKeyState = uPreviousKeyState;
        m_value.flags.TransitionState = uTransitionState;
    }

    inline LPARAM KeystrokeMessageBuilder::get_LParam() const
    {
        return m_value.param;
    }

    inline KeystrokeMessageBuilder::operator LPARAM() const
    {
        return get_LParam();
    }

    inline KeystrokeMessageFlags KeystrokeMessageBuilder::get_Flags() const
    {
        return m_value.flags;
    }
}
