#include "Target.h"
#include "TargetConstants.h"
#include "hnrt/String.h"


using namespace hnrt;


RefPtr<Target> Target::Create(PCWSTR pszName, bool bIsVisible)
{
    return RefPtr<Target>(new Target(pszName, bIsVisible));
}


Target::Target(const String& szName, bool bIsVisible)
    : m_szName(szName)
    , m_bIsVisible(bIsVisible)
    , m_bBlockKeybd(false)
    , m_bBlockMouse(false)
    , m_Actions()
    , m_pCallback(nullptr)
{
}


void Target::set_Name(const String& szName)
{
    m_szName = szName;
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


void Target::set_BlockKeybd(bool value)
{
    m_bBlockKeybd = value;
    if (m_pCallback)
    {
        m_pCallback->OnTargetUpdate(*this);
    }
}


void Target::set_BlockMouse(bool value)
{
    m_bBlockMouse = value;
    if (m_pCallback)
    {
        m_pCallback->OnTargetUpdate(*this);
    }
}


RefPtr<Target> Target::Clone() const
{
    Target* pCloned = new Target(m_szName, m_bIsVisible);
    pCloned->m_bBlockKeybd = m_bBlockKeybd;
    pCloned->m_bBlockMouse = m_bBlockMouse;
    for (auto iter = m_Actions.CBegin; iter != m_Actions.CEnd; iter++)
    {
        pCloned->m_Actions.Append((*iter)->Clone());
    }
    return RefPtr<Target>(pCloned);
}


const RefPtr<Action>& Target::operator [](ULONG index) const
{
    return m_Actions[index];
}


RefPtr<Action>& Target::operator [](ULONG index)
{
    return m_Actions[index];
}


void Target::InvokeCallback()
{
    if (m_pCallback)
    {
        m_pCallback->OnTargetUpdate(*this);
    }
}


void Target::Append(RefPtr<Action> pAction)
{
    m_Actions.Append(pAction);
    InvokeCallback();
}


void Target::Delete(ULONG index)
{
    if (index < Count)
    {
        m_Actions.Delete(index);
        InvokeCallback();
    }
}


void Target::Insert(ULONG index, RefPtr<Action> pAction)
{
    m_Actions.Insert(index, pAction);
    InvokeCallback();
}


void Target::Move(ULONG from, ULONG to)
{
    if (from < Count)
    {
        m_Actions.Move(from, to);
        InvokeCallback();
    }
}
