#include "TargetCollection.h"


using namespace hnrt;


TargetCollection::TargetCollection()
    : m_collection()
    , m_pCallback(nullptr)
{
}


TargetCollection& TargetCollection::Append(RefPtr<Target> pTarget)
{
    m_collection.push_back(pTarget);
    if (m_pCallback)
    {
        m_pCallback->OnTargetCollectionUpdate(*this);
    }
    return *this;
}


TargetCollection& TargetCollection::Delete(ULONG index)
{
    if (index < Count)
    {
        m_collection.erase(m_collection.begin() + index);
        if (m_pCallback)
        {
            m_pCallback->OnTargetCollectionUpdate(*this);
        }
    }
    return *this;
}


TargetCollection& TargetCollection::Insert(ULONG index, RefPtr<Target> pTarget)
{
    if (index < Count)
    {
        m_collection.insert(m_collection.begin() + index, pTarget);
    }
    else
    {
        m_collection.push_back(pTarget);
    }
    if (m_pCallback)
    {
        m_pCallback->OnTargetCollectionUpdate(*this);
    }
    return *this;
}


TargetCollection& TargetCollection::Move(ULONG from, ULONG to)
{
    if (from < Count)
    {
        RefPtr<Target> pTarget = m_collection[from];
        m_collection.erase(m_collection.begin() + from);
        if (to < Count)
        {
            m_collection.insert(m_collection.begin() + to, pTarget);
        }
        else
        {
            m_collection.push_back(pTarget);
        }
        if (m_pCallback)
        {
            m_pCallback->OnTargetCollectionUpdate(*this);
        }
    }
    return *this;
}


TargetCollection& TargetCollection::Clear()
{
    m_collection.clear();
    if (m_pCallback)
    {
        m_pCallback->OnTargetCollectionUpdate(*this);
    }
    return *this;
}


TargetCollection& TargetCollection::Assign(ULONG index, RefPtr<Target> pTarget)
{
    if (index < Count)
    {
        m_collection[index] = pTarget;
        if (m_pCallback)
        {
            m_pCallback->OnTargetCollectionUpdate(*this);
        }
    }
    return *this;
}
