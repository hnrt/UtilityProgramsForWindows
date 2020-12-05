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


TargetCollection& TargetCollection::Delete(size_t index)
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


TargetCollection& TargetCollection::Insert(size_t index, RefPtr<Target> pTarget)
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


TargetCollection& TargetCollection::Move(size_t from, size_t to)
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


TargetCollection& TargetCollection::Assign(size_t index, RefPtr<Target> pTarget)
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
