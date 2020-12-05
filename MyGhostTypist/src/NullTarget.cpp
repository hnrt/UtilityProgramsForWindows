#include "NullTarget.h"
#include "TargetConstants.h"


using namespace hnrt;


NullTarget::NullTarget(PCWSTR pszName)
    : Target(NULL_TARGET_TYPE, pszName)
{
}


RefPtr<Target> NullTarget::Clone() const
{
    NullTarget* pCloned = new NullTarget(m_pszName);
    pCloned->m_bIsVisible = m_bIsVisible;
    return RefPtr<Target>(pCloned);
}
