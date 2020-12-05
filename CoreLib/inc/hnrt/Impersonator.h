#pragma once


#include "hnrt/WindowsHandle.h"


namespace hnrt
{
    class Impersonator
    {
    public:

        Impersonator(DWORD dwAttributes, PSID psid);
        Impersonator(const Impersonator&) = delete;
        virtual ~Impersonator();
        void operator =(const Impersonator&) = delete;

    protected:

        WindowsHandle m_hToken1;
        WindowsHandle m_hToken2;
    };

    class LowIntegrityImpersonator
        : public Impersonator
    {
    public:

        LowIntegrityImpersonator();
        LowIntegrityImpersonator(const Impersonator&) = delete;
        virtual ~LowIntegrityImpersonator() = default;
        void operator =(const LowIntegrityImpersonator&) = delete;
    };
}
