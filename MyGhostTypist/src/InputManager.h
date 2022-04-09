#pragma once


#include "hnrt/RefObj.h"
#include "hnrt/RefPtr.h"
#include "hnrt/SyntheticInput.h"
#include <Windows.h>
#include <list>


#define IM_FLAG_SHIFT 0x8000
#define IM_MASK_VIRTUALKEY 0x00FF


namespace hnrt
{
    class InputManager
        : public RefObj
    {
    public:

        static RefPtr<InputManager> Create(PCWSTR pszAppDir, PCWSTR pszInstallDir);

        InputManager(const InputManager&) = delete;
        ~InputManager();
        void operator =(const InputManager&) = delete;
        InputManager& AddMouseMove(LONG x, LONG y);
        InputManager& AddMouseClickLeft();
        InputManager& AddKeyboardInput(PCWSTR psz);
        InputManager& AddKeyboardInput(WORD wVK);
        InputManager& AddKeyboardInput(WORD wVK, DWORD dwFlags);
        bool SendNext();
        LONG get_Interval() const;
        void set_Interval(LONG value);
        LONG get_Delay() const;
        void set_Delay(LONG value);
        __declspec(property(get = get_Interval, put = set_Interval)) LONG Interval;
        __declspec(property(get = get_Delay, put = set_Delay)) LONG Delay;

    private:

        InputManager();
        void LoadKeyboardLayoutFile(PCWSTR pszFileName);

        USHORT m_map[128];
        LONG m_Interval;
        LONG m_Delay;
        std::list<SyntheticInput> m_Queue;
    };

    inline LONG InputManager::get_Interval() const
    {
        return m_Interval;
    }

    inline LONG InputManager::get_Delay() const
    {
        return m_Delay;
    }

    inline void InputManager::set_Delay(LONG value)
    {
        m_Delay = value;
    }
}
