#pragma once


#include <intrin.h>


namespace hnrt
{
    class SpinLock
    {
    public:

        typedef long Type;

        SpinLock(Type& value);
        SpinLock(const SpinLock&) = delete;
        ~SpinLock();
        void operator =(const SpinLock&) = delete;

    private:

        Type& m_value;
    };

    inline SpinLock::SpinLock(Type& value)
        : m_value(value)
    {
        while (_InterlockedCompareExchange(&m_value, 1L, 0L))
        {
            continue;
        }
    }

    inline SpinLock::~SpinLock()
    {
        _InterlockedExchange(&m_value, 0L);
    }

#define SPIN_LOCK_INITIALIZER 0L
}
