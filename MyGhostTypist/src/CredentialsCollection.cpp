#include "CredentialsCollection.h"
#include "hnrt/StringStore.h"


using namespace hnrt;


const RefPtr<Credentials> CredentialsCollection::operator[](PCWSTR pszKey) const
{
    for (size_t index = 0; index < Count; index++)
    {
        if (m_collection[index]->Key && !wcscmp(pszKey, m_collection[index]->Key))
        {
            return m_collection[index];
        }
    }
    return RefPtr<Credentials>();
}


RefPtr<Credentials> CredentialsCollection::operator[](PCWSTR pszKey)
{
    for (size_t index = 0; index < Count; index++)
    {
        if (m_collection[index]->Key && !wcscmp(pszKey, m_collection[index]->Key))
        {
            return m_collection[index];
        }
    }
    return RefPtr<Credentials>();
}


CredentialsCollection& CredentialsCollection::Append(RefPtr<Credentials> pCredentials)
{
    m_collection.push_back(pCredentials);
    if (m_pCallback)
    {
        m_pCallback->OnCredentialsCollectionUpdate(*this);
    }
    return *this;
}


CredentialsCollection& CredentialsCollection::Delete(size_t index)
{
    if (index < Count)
    {
        m_collection.erase(m_collection.begin() + index);
        if (m_pCallback)
        {
            m_pCallback->OnCredentialsCollectionUpdate(*this);
        }
    }
    return *this;
}


CredentialsCollection& CredentialsCollection::Insert(size_t index, RefPtr<Credentials> pCredentials)
{
    if (index < Count)
    {
        m_collection.insert(m_collection.begin() + index, pCredentials);
    }
    else
    {
        m_collection.push_back(pCredentials);
    }
    if (m_pCallback)
    {
        m_pCallback->OnCredentialsCollectionUpdate(*this);
    }
    return *this;

}


CredentialsCollection& CredentialsCollection::Move(size_t from, size_t to)
{
    if (from < Count)
    {
        RefPtr<Credentials> pCredentials = m_collection[from];
        m_collection.erase(m_collection.begin() + from);
        if (to < Count)
        {
            m_collection.insert(m_collection.begin() + to, pCredentials);
        }
        else
        {
            m_collection.push_back(pCredentials);
        }
        if (m_pCallback)
        {
            m_pCallback->OnCredentialsCollectionUpdate(*this);
        }
    }
    return *this;
}


CredentialsCollection& CredentialsCollection::Clear()
{
    m_collection.clear();
    if (m_pCallback)
    {
        m_pCallback->OnCredentialsCollectionUpdate(*this);
    }
    return *this;
}
