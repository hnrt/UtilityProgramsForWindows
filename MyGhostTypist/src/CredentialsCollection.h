#pragma once


#include <vector>
#include "Credentials.h"


namespace hnrt
{
    class Credentials;
    class CredentialsCollection;

    class CredentialsCollectionCallback
    {
    public:

        virtual void OnCredentialsCollectionUpdate(CredentialsCollection&) = 0;
    };

    class CredentialsCollection
    {
    public:

        CredentialsCollection();
        CredentialsCollection(const CredentialsCollection&) = delete;
        ~CredentialsCollection() = default;
        void operator =(const CredentialsCollection&) = delete;
        const RefPtr<Credentials> operator[](size_t index) const;
        RefPtr<Credentials> operator[](size_t index);
        const RefPtr<Credentials> operator[](PCWSTR pszKey) const;
        RefPtr<Credentials> operator[](PCWSTR pszKey);
        CredentialsCollection& Append(RefPtr<Credentials> pCredentials);
        CredentialsCollection& Delete(size_t index);
        CredentialsCollection& Insert(size_t index, RefPtr<Credentials> pCredentials);
        CredentialsCollection& Move(size_t from, size_t to);
        CredentialsCollection& Clear();
        size_t get_Count() const;
        void set_Callback(CredentialsCollectionCallback* pCallback);
        RefPtr<Credentials> get_DefaultCredentials() const;
        __declspec(property(get = get_Count)) size_t Count;
        __declspec(property(put = set_Callback)) CredentialsCollectionCallback* Callback;
        __declspec(property(get = get_DefaultCredentials)) RefPtr<Credentials> DefaultCredentials;

    protected:

        std::vector<RefPtr<Credentials>> m_collection;
        CredentialsCollectionCallback* m_pCallback;
    };

    inline CredentialsCollection::CredentialsCollection()
        : m_collection()
        , m_pCallback(nullptr)
    {
    }

    inline const RefPtr<Credentials> CredentialsCollection::operator[](size_t index) const
    {
        return index < Count ? m_collection[index] : RefPtr<Credentials>();
    }

    inline RefPtr<Credentials> CredentialsCollection::operator[](size_t index)
    {
        return index < Count ? m_collection[index] : RefPtr<Credentials>();
    }

    inline size_t CredentialsCollection::get_Count() const
    {
        return m_collection.size();
    }

    inline void CredentialsCollection::set_Callback(CredentialsCollectionCallback* pCallback)
    {
        m_pCallback = pCallback;
    }

    inline RefPtr<Credentials> CredentialsCollection::get_DefaultCredentials() const
    {
        if (Count == 0)
        {
            RefPtr<Credentials> pC = Credentials::Create();
            pC->Username = L"";
            pC->Password = L"";
            return pC;
        }
        size_t index = 0;
        while (true)
        {
            RefPtr<Credentials> pC = m_collection[index];
            if (!pC->Key || !*pC->Key)
            {
                return pC;
            }
            else if (++index == Count)
            {
                return m_collection[0];
            }
        }
    }
}
