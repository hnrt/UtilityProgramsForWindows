#pragma once


#include "Action.h"
#include <vector>


namespace hnrt
{
    class ActionCollection
    {
    public:

        typedef std::vector<RefPtr<Action>>::const_iterator ConstIter;

        ActionCollection();
        ActionCollection(const ActionCollection&) = delete;
        ~ActionCollection();
        void operator =(const ActionCollection&) = delete;
        const RefPtr<Action>& operator [](ULONG index) const;
        RefPtr<Action>& operator [](ULONG index);
        ActionCollection& Append(RefPtr<Action> pAction);
        ActionCollection& Delete(ULONG index);
        ActionCollection& Insert(ULONG index, RefPtr<Action> pAction);
        ActionCollection& Move(ULONG from, ULONG to);
        ULONG get_Count() const;
        ConstIter get_CBegin() const;
        ConstIter get_CEnd() const;

        __declspec(property(get = get_Count)) ULONG Count;
        __declspec(property(get = get_CBegin)) ConstIter CBegin;
        __declspec(property(get = get_CEnd)) ConstIter CEnd;

    private:

        std::vector<RefPtr<Action>> m_vector;
    };

    inline ActionCollection& ActionCollection::Append(RefPtr<Action> pAction)
    {
        m_vector.push_back(pAction);
        return *this;
    }

    inline ActionCollection& ActionCollection::Delete(ULONG index)
    {
        m_vector.erase(m_vector.begin() + index);
        return *this;
    }

    inline ActionCollection& ActionCollection::Insert(ULONG index, RefPtr<Action> pAction)
    {
        if (index < Count)
        {
            m_vector.insert(m_vector.begin() + index, pAction);
        }
        else
        {
            m_vector.push_back(pAction);
        }
        return *this;
    }

    inline ActionCollection& ActionCollection::Move(ULONG from, ULONG to)
    {
        if (from < Count)
        {
            RefPtr<Action> pAction = m_vector[from];
            Delete(from);
            Insert(to, pAction);
        }
        return *this;
    }

    inline ULONG ActionCollection::get_Count() const
    {
        return static_cast<ULONG>(m_vector.size());
    }

    inline ActionCollection::ConstIter ActionCollection::get_CBegin() const
    {
        return m_vector.begin();
    }

    inline ActionCollection::ConstIter ActionCollection::get_CEnd() const
    {
        return m_vector.end();
    }
}
