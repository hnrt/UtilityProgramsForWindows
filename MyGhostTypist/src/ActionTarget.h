#pragma once


#include <vector>
#include "Target.h"


namespace hnrt
{
    typedef std::vector<PCWSTR>::const_iterator ActionListConstIter;

    class ActionTarget
        : public Target
    {
    protected:

        ActionTarget(PCWSTR pszType, PCWSTR pszName = nullptr);

    public:

        ActionTarget(const ActionTarget&) = delete;
        virtual ~ActionTarget() = default;
        void operator =(const ActionTarget&) = delete;
        PCWSTR operator[](size_t index) const;
        PCWSTR operator[](size_t index);
        ActionTarget& Assign(size_t index, PCWSTR psz);
        ActionTarget& Append(PCWSTR psz);
        ActionTarget& Delete(size_t index);
        ActionTarget& Insert(size_t index, PCWSTR psz);
        ActionTarget& Move(size_t from, size_t to);
        size_t get_Count() const;
        ActionListConstIter Begin() const;
        ActionListConstIter End() const;

        __declspec(property(get = get_Count)) size_t Count;

    protected:

        std::vector<PCWSTR> m_ActionList;
    };

    inline PCWSTR ActionTarget::operator[](size_t index) const
    {
        return index < m_ActionList.size() ? m_ActionList[index] : nullptr;
    }

    inline PCWSTR ActionTarget::operator[](size_t index)
    {
        return index < m_ActionList.size() ? m_ActionList[index] : nullptr;
    }

    inline size_t ActionTarget::get_Count() const
    {
        return m_ActionList.size();
    }

    inline ActionListConstIter ActionTarget::Begin() const
    {
        return m_ActionList.begin();
    }

    inline ActionListConstIter ActionTarget::End() const
    {
        return m_ActionList.end();
    }
}
