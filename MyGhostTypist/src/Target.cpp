#include "Target.h"
#include "TargetConstants.h"
#include "NullTarget.h"
#include "FindWindowTarget.h"
#include "hnrt/StringStore.h"


using namespace hnrt;


RefPtr<Target> Target::CreateNull(PCWSTR pszName)
{
    return RefPtr<Target>(new NullTarget(pszName));
}


RefPtr<Target> Target::CreateFindWindow(PCWSTR pszName, PCWSTR pszClassName, PCWSTR pszWindowText)
{
    return RefPtr<Target>(new FindWindowTarget(pszName, pszClassName, pszWindowText));
}


Target::Target(PCWSTR pszType, PCWSTR pszName)
    : m_pszType(pszType)
    , m_pszName(StringStore::Get(pszName))
    , m_bIsVisible(true)
    , m_pCallback(nullptr)
{
}


void Target::set_Name(PCWSTR pszName)
{
    m_pszName = StringStore::Get(pszName);
    if (m_pCallback)
    {
        m_pCallback->OnTargetUpdate(*this);
    }
}


void Target::set_IsVisible(bool value)
{
    m_bIsVisible = value;
    if (m_pCallback)
    {
        m_pCallback->OnTargetUpdate(*this);
    }
}


bool Target::get_IsTypeNull() const
{
    return !wcscmp(m_pszType, NULL_TARGET_TYPE);
}


bool Target::get_IsTypeAction() const
{
    return get_IsTypeFindWindow();
}


bool Target::get_IsTypeFindWindow() const
{
    return !wcscmp(m_pszType, FINDWINDOW_TARGET_TYPE);
}


const ActionTarget* Target::get_ActionTargetPtr() const
{
    return dynamic_cast<const ActionTarget*>(this);
}


ActionTarget* Target::get_ActionTargetPtr()
{
    return dynamic_cast<ActionTarget*>(this);
}


const FindWindowTarget* Target::get_FindWindowTargetPtr() const
{
    return dynamic_cast<const FindWindowTarget*>(this);
}


FindWindowTarget* Target::get_FindWindowTargetPtr()
{
    return dynamic_cast<FindWindowTarget*>(this);
}
