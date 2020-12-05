#pragma once


#include "Target.h"


namespace hnrt
{
    class NullTarget
        : public Target
    {
    public:

        NullTarget(PCWSTR pszName = nullptr);
        NullTarget(const NullTarget&) = delete;
        virtual ~NullTarget() = default;
        void operator =(const NullTarget&) = delete;
        virtual RefPtr<Target> Clone() const;
    };
}
