#pragma once


#include "hnrt/RefObj.h"
#include "hnrt/RefPtr.h"
#include "Action.h"
#include <vector>


namespace hnrt
{
    class Target;

    class TargetCallback
    {
    public:

        virtual void OnTargetUpdate(Target&) = 0;
    };

    class Target
        : public RefObj
    {
    public:

        typedef std::vector<RefPtr<Action>>::iterator ActionIter;

        static RefPtr<Target> Create(PCWSTR pszName = nullptr, bool bIsVisible = true);

        Target(const Target&) = delete;
        virtual ~Target() = default;
        void operator =(const Target&) = delete;
        RefPtr<Target> Clone() const;
        const RefPtr<Action>& operator [](ULONG index) const;
        RefPtr<Action>& operator [](ULONG index);
        void InvokeCallback();
        void Append(RefPtr<Action> pAction);
        void Delete(ULONG index);
        void Insert(ULONG index, RefPtr<Action> pAction);
        void Move(ULONG from, ULONG to);
        PCWSTR get_Name() const;
        void set_Name(PCWSTR pszName);
        bool get_IsVisible() const;
        void set_IsVisible(bool value);
        ULONG get_Count() const;
        ActionIter get_Begin();
        ActionIter get_End();
        void set_Callback(TargetCallback* pCallbak);
        __declspec(property(get = get_Name, put = set_Name)) PCWSTR Name;
        __declspec(property(get = get_IsVisible, put = set_IsVisible)) bool IsVisible;
        __declspec(property(get = get_Count)) ULONG Count;
        __declspec(property(get = get_Begin)) ActionIter Begin;
        __declspec(property(get = get_End)) ActionIter End;
        __declspec(property(put = set_Callback)) TargetCallback* Callback;

    protected:

        Target(PCWSTR pszName = nullptr, bool bIsVisible = true);

        PCWSTR m_pszName;
        bool m_bIsVisible;
        std::vector<RefPtr<Action>> m_Actions;
        TargetCallback* m_pCallback;
    };

    inline PCWSTR Target::get_Name() const
    {
        return m_pszName;
    }

    inline bool Target::get_IsVisible() const
    {
        return m_bIsVisible;
    }

    inline ULONG Target::get_Count() const
    {
        return static_cast<ULONG>(m_Actions.size());
    }

    inline Target::ActionIter Target::get_Begin()
    {
        return m_Actions.begin();
    }

    inline Target::ActionIter Target::get_End()
    {
        return m_Actions.end();
    }

    inline void Target::set_Callback(TargetCallback* pCallbak)
    {
        m_pCallback = pCallbak;
    }
}
