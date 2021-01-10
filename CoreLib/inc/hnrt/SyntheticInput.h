#pragma once


#include <WinUser.h>


#define KEYEVENTF_KEYDOWN 0


namespace hnrt
{
    struct KeyboardInput
    {
        DWORD dwFlags;
        WORD wData;
    };

    struct MouseInput
    {
        DWORD dwFlags;
        LONG dx;
        LONG dy;
        DWORD dwData;
    };

    class SyntheticInput
    {
    public:

        static void CheckSystemMetrics();
        static LONG NormalizeX(DWORD dwFlags, LONG x);
        static LONG NormalizeY(DWORD dwFlags, LONG y);

        SyntheticInput(const SyntheticInput& src);
        ~SyntheticInput() = default;
        void operator =(const SyntheticInput& src);
        INPUT* Write(INPUT* pInput) const;

    protected:

        SyntheticInput(DWORD dwType);

        DWORD m_dwType;
        union
        {
            KeyboardInput m_ki;
            MouseInput m_mi;
        };
    };

#pragma warning(push)
#pragma warning(disable:26495)

    inline SyntheticInput::SyntheticInput(DWORD dwType)
        : m_dwType(dwType)
    {
    }

    inline SyntheticInput::SyntheticInput(const SyntheticInput& src)
        : m_dwType(src.m_dwType)
        , m_ki()
    {
        switch (m_dwType)
        {
        case INPUT_MOUSE:
            m_mi = src.m_mi;
            break;
        case INPUT_KEYBOARD:
            m_ki = src.m_ki;
            break;
        default:
            break;
        }
    }

#pragma warning(pop)

    inline void SyntheticInput::operator =(const SyntheticInput& src)
    {
        switch ((m_dwType = src.m_dwType))
        {
        case INPUT_MOUSE:
            m_mi = src.m_mi;
            break;
        case INPUT_KEYBOARD:
            m_ki = src.m_ki;
            break;
        default:
            break;
        }
    }

    class SyntheticMouseInput
        : public SyntheticInput
    {
    public:

        SyntheticMouseInput(DWORD dwFlags);
        SyntheticMouseInput(DWORD dwData, DWORD dwFlags);
        SyntheticMouseInput(LONG x, LONG y, DWORD dwFlags);
        SyntheticMouseInput(const SyntheticMouseInput& src);
        ~SyntheticMouseInput() = default;
        void operator =(const SyntheticMouseInput& src);
    };

    inline SyntheticMouseInput::SyntheticMouseInput(DWORD dwFlags)
        : SyntheticInput(INPUT_MOUSE)
    {
        m_mi.dx = 0;
        m_mi.dy = 0;
        m_mi.dwData = 0;
        m_mi.dwFlags = dwFlags;
    }

    inline SyntheticMouseInput::SyntheticMouseInput(DWORD dwData, DWORD dwFlags)
        : SyntheticInput(INPUT_MOUSE)
    {
        m_mi.dx = 0;
        m_mi.dy = 0;
        m_mi.dwData = dwData;
        m_mi.dwFlags = dwFlags;
    }

    inline SyntheticMouseInput::SyntheticMouseInput(LONG x, LONG y, DWORD dwFlags)
        : SyntheticInput(INPUT_MOUSE)
    {
        m_mi.dx = x;
        m_mi.dy = y;
        m_mi.dwData = 0;
        m_mi.dwFlags = MOUSEEVENTF_MOVE | dwFlags;
    }

    inline SyntheticMouseInput::SyntheticMouseInput(const SyntheticMouseInput& src)
        : SyntheticInput(INPUT_MOUSE)
    {
        m_mi = src.m_mi;
    }

    inline void SyntheticMouseInput::operator =(const SyntheticMouseInput& src)
    {
        m_mi = src.m_mi;
    }

    class SyntheticKeyboardInput
        : public SyntheticInput
    {
    public:

        SyntheticKeyboardInput(WORD wData, DWORD dwFlags);
        SyntheticKeyboardInput(const SyntheticKeyboardInput& src);
        ~SyntheticKeyboardInput() = default;
        void operator =(const SyntheticKeyboardInput& src);
    };

    inline SyntheticKeyboardInput::SyntheticKeyboardInput(WORD wData, DWORD dwFlags)
        : SyntheticInput(INPUT_KEYBOARD)
    {
        m_ki.wData = wData;
        m_ki.dwFlags = dwFlags;
    }

    inline SyntheticKeyboardInput::SyntheticKeyboardInput(const SyntheticKeyboardInput& src)
        : SyntheticInput(INPUT_KEYBOARD)
    {
        m_ki = src.m_ki;
    }

    inline void SyntheticKeyboardInput::operator =(const SyntheticKeyboardInput& src)
    {
        m_ki = src.m_ki;
    }
}
