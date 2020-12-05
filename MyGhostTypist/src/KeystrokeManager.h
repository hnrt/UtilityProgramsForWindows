#pragma once


#include "hnrt/RefObj.h"
#include "hnrt/RefPtr.h"
#include "hnrt/Keystroke.h"
#include <list>


namespace hnrt
{
    class KeystrokeManager
        : public RefObj
    {
    public:

        static RefPtr<KeystrokeManager> Create(PCWSTR pszAppDir);

        KeystrokeManager(const KeystrokeManager&) = delete;
        ~KeystrokeManager();
        void operator =(const KeystrokeManager&) = delete;
        void Start(HWND hwnd, UINT_PTR uId);
        void Stop();
        KeystrokeManager& Add(PCWSTR psz);
        KeystrokeManager& Add(WORD wVK);
        KeystrokeManager& Add(WORD wVK, DWORD dwFlags);
        #define KM_KEYDOWN 0
        #define KM_KEYUP KEYEVENTF_KEYUP
        bool TypeNext();
        long get_Interval() const;
        void set_Interval(long value);
        long get_Delay() const;
        void set_Delay(long value);
        __declspec(property(get = get_Interval, put = set_Interval)) long Interval;
        __declspec(property(get = get_Delay, put = set_Delay)) long Delay;

    private:

        KeystrokeManager();
        void LoadLayout(PCWSTR pszFileName);

        unsigned short m_map[128];
        long m_Interval;
        long m_Delay;
        std::list<Keystroke> m_Queue;
        HWND m_hwnd;
        UINT_PTR m_uTimerId;
    };

    inline long KeystrokeManager::get_Interval() const
    {
        return m_Interval;
    }

    inline long KeystrokeManager::get_Delay() const
    {
        return m_Delay;
    }

    inline void KeystrokeManager::set_Delay(long value)
    {
        m_Delay = value;
    }
}


#define KM_FLAG_SHIFT 0x8000
#define KM_MASK_VIRTUALKEY 0x00FF
