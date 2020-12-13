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
        const RefPtr<Target> operator[](ULONG index) const;
        RefPtr<Target> operator[](ULONG index);
        TargetCollection& Append(RefPtr<Target> pTarget);
        TargetCollection& Delete(ULONG index);
        TargetCollection& Insert(ULONG index, RefPtr<Target> pTarget);
        TargetCollection& Move(ULONG from, ULONG to);
        TargetCollection& Assign(ULONG index, RefPtr<Target> pTarget);
        TargetCollection& Clear();
        ULONG get_Count() const;
        void set_Callback(TargetCollectionCallback* pCallbak);
        __declspec(property(get = get_Count)) ULONG Count;
        __declspec(property(put = set_Callback)) TargetCollectionCallback* Callback;

    private:

        std::vector<RefPtr<Target>> m_collection;
        TargetCollectionCallback* m_pCallback;
    };

    inline const RefPtr<Target> TargetCollection::operator[](ULONG index) const
    {
        return index < Count ? m_collection[index] : RefPtr<Target>();
    }

    inline RefPtr<Target> TargetCollection::operator[](ULONG index)
    {
        return index < Count ? m_collection[index] : RefPtr<Target>();
    }

    inline ULONG TargetCollection::get_Count() const
    {
        return static_cast<ULONG>(m_collection.size());
    }

    inline void TargetCollection::set_Callback(TargetCollectionCallback* pCallbak)
    {
        m_pCallback = pCallbak;
    }
}
