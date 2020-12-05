#pragma once


#include <vector>
#include "Target.h"


namespace hnrt
{
    class TargetCollection;

    class TargetCollectionCallback
    {
    public:

        virtual void OnTargetCollectionUpdate(TargetCollection&) = 0;
    };

    class TargetCollection
    {
    public:

        TargetCollection();
        TargetCollection(const TargetCollection&) = delete;
        ~TargetCollection() = default;
        void operator =(const TargetCollection&) = delete;
        const RefPtr<Target> operator[](size_t index) const;
        RefPtr<Target> operator[](size_t index);
        TargetCollection& Append(RefPtr<Target> pTarget);
        TargetCollection& Delete(size_t index);
        TargetCollection& Insert(size_t index, RefPtr<Target> pTarget);
        TargetCollection& Move(size_t from, size_t to);
        TargetCollection& Assign(size_t index, RefPtr<Target> pTarget);
        TargetCollection& Clear();
        size_t get_Count() const;
        void set_Callback(TargetCollectionCallback* pCallbak);
        __declspec(property(get = get_Count)) size_t Count;
        __declspec(property(put = set_Callback)) TargetCollectionCallback* Callback;

    private:

        std::vector<RefPtr<Target>> m_collection;
        TargetCollectionCallback* m_pCallback;
    };

    inline const RefPtr<Target> TargetCollection::operator[](size_t index) const
    {
        return index < Count ? m_collection[index] : RefPtr<Target>();
    }

    inline RefPtr<Target> TargetCollection::operator[](size_t index)
    {
        return index < Count ? m_collection[index] : RefPtr<Target>();
    }

    inline size_t TargetCollection::get_Count() const
    {
        return m_collection.size();
    }

    inline void TargetCollection::set_Callback(TargetCollectionCallback* pCallbak)
    {
        m_pCallback = pCallbak;
    }
}
