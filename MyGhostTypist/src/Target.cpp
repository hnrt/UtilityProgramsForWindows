#include "Target.h"
#include "TargetConstants.h"
#include "hnrt/String.h"
#include "hnrt/Exception.h"


using namespace hnrt;


RefPtr<Target> Target::Create(PCWSTR pszName, bool bIsVisible)
{
    return RefPtr<Target>(new Target(pszName, bIsVisible));
}


Target::Target(PCWSTR pszName, bool bIsVisible)
    : m_pszName(String::Copy(pszName))
    , m_bIsVisible(bIsVisible)
    , m_Actions()
    , m_pCallback(nullptr)
{
}


void Target::set_Name(PCWSTR pszName)
{
    m_pszName = String::Copy(pszName);
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


RefPtr<Target> Target::Clone() const
{
    Target* pCloned = new Target(m_pszName, m_bIsVisible);
    for (std::vector<RefPtr<Action>>::const_iterator iter = m_Actions.begin(); iter != m_Actions.end(); iter++)
    {
        pCloned->m_Actions.push_back((*iter)->Clone());
    }
    return RefPtr<Target>(pCloned);
}


const RefPtr<Action>& Target::operator [](ULONG index) const
{
    if (index < Count)
    {
        return m_Actions[index];
    }
    else
    {
        throw Exception(L"Target::operator [](ULONG) const: Index out of range.");
    }
}


RefPtr<Action>& Target::operator [](ULONG index)
{
    if (index < Count)
    {
        return m_Actions[index];
    }
    else
    {
        throw Exception(L"Target::operator [](ULONG): Index out of range.");
    }
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
    m_Actions.push_back(pAction);
    InvokeCallback();
}


void Target::Delete(ULONG index)
{
    if (index < Count)
    {
        m_Actions.erase(m_Actions.begin() + index);
        InvokeCallback();
    }
}


void Target::Insert(ULONG index, RefPtr<Action> pAction)
{
    if (index < Count)
    {
        m_Actions.insert(m_Actions.begin() + index, pAction);
    }
    else
    {
        m_Actions.push_back(pAction);
    }
    InvokeCallback();
}


void Target::Move(ULONG from, ULONG to)
{
    if (from < Count)
    {
        RefPtr<Action> pAction = m_Actions[from];
        m_Actions.erase(m_Actions.begin() + from);
        if (to < Count)
        {
            m_Actions.insert(m_Actions.begin() + to, pAction);
        }
        else
        {
            m_Actions.push_back(pAction);
        }
        InvokeCallback();
    }
}
