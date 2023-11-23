#include "CredentialsCollection.h"


using namespace hnrt;


const RefPtr<Credentials> CredentialsCollection::operator[](PCWSTR pszKey) const
{
    for (ULONG index = 0; index < Count; index++)
    {
        if (!String::Compare(pszKey, m_collection[index]->Key))
        {
            return m_collection[index];
        }
    }
    return RefPtr<Credentials>();
}


RefPtr<Credentials> CredentialsCollection::operator[](PCWSTR pszKey)
{
    for (ULONG index = 0; index < Count; index++)
    {
        if (!String::Compare(pszKey, m_collection[index]->Key))
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


CredentialsCollection& CredentialsCollection::Delete(ULONG index)
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


CredentialsCollection& CredentialsCollection::Insert(ULONG index, RefPtr<Credentials> pCredentials)
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


CredentialsCollection& CredentialsCollection::Move(ULONG from, ULONG to)
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
