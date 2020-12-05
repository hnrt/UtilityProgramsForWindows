#include "ActionTarget.h"
#include "hnrt/StringStore.h"


using namespace hnrt;


ActionTarget::ActionTarget(PCWSTR pszType, PCWSTR pszName)
    : Target(pszType, pszName)
    , m_ActionList()
{
}


ActionTarget& ActionTarget::Assign(size_t index, PCWSTR psz)
{
    if (index < Count)
    {
        m_ActionList[index] = StringStore::Get(psz);
        if (m_pCallback)
        {
            m_pCallback->OnTargetUpdate(*this);
        }
    }
    return *this;
}


ActionTarget& ActionTarget::Append(PCWSTR psz)
{
    m_ActionList.push_back(StringStore::Get(psz));
    if (m_pCallback)
    {
        m_pCallback->OnTargetUpdate(*this);
    }
    return *this;
}


ActionTarget& ActionTarget::Delete(size_t index)
{
    if (index < Count)
    {
        m_ActionList.erase(m_ActionList.begin() + index);
        if (m_pCallback)
        {
            m_pCallback->OnTargetUpdate(*this);
        }
    }
    return *this;
}


ActionTarget& ActionTarget::Insert(size_t index, PCWSTR psz)
{
    if (index < Count)
    {
        m_ActionList.insert(m_ActionList.begin() + index, StringStore::Get(psz));
    }
    else
    {
        m_ActionList.push_back(StringStore::Get(psz));
    }
    if (m_pCallback)
    {
        m_pCallback->OnTargetUpdate(*this);
    }
    return *this;
}


ActionTarget& ActionTarget::Move(size_t from, size_t to)
{
    if (from < Count)
    {
        PCWSTR psz = m_ActionList[from];
        m_ActionList.erase(m_ActionList.begin() + from);
        if (to < Count)
        {
            m_ActionList.insert(m_ActionList.begin() + to, psz);
        }
        else
        {
            m_ActionList.push_back(psz);
        }
        if (m_pCallback)
        {
            m_pCallback->OnTargetUpdate(*this);
        }
    }
    return *this;
}
