#pragma once


#include "hnrt/RefObj.h"
#include "hnrt/RefPtr.h"
#include "ActionCollection.h"


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
        const String& get_Name() const;
        void set_Name(const String& szName);
        bool get_IsVisible() const;
        void set_IsVisible(bool value);
        bool get_BlockKeybd() const;
        void set_BlockKeybd(bool value);
        bool get_BlockMouse() const;
        void set_BlockMouse(bool value);
        ULONG get_Count() const;
        ActionCollection::ConstIter get_Begin();
        ActionCollection::ConstIter get_End();
        void set_Callback(TargetCallback* pCallbak);
        __declspec(property(get = get_Name, put = set_Name)) const String& Name;
        __declspec(property(get = get_IsVisible, put = set_IsVisible)) bool IsVisible;
        __declspec(property(get = get_BlockKeybd, put = set_BlockKeybd)) bool BlockKeybd;
        __declspec(property(get = get_BlockMouse, put = set_BlockMouse)) bool BlockMouse;
        __declspec(property(get = get_Count)) ULONG Count;
        __declspec(property(get = get_Begin)) ActionCollection::ConstIter Begin;
        __declspec(property(get = get_End)) ActionCollection::ConstIter End;
        __declspec(property(put = set_Callback)) TargetCallback* Callback;

    protected:

        Target(const String& szName, bool bIsVisible);

        String m_szName;
        bool m_bIsVisible;
        bool m_bBlockKeybd;
        bool m_bBlockMouse;
        ActionCollection m_Actions;
        TargetCallback* m_pCallback;
    };

    inline const String& Target::get_Name() const
    {
        return m_szName;
    }

    inline bool Target::get_IsVisible() const
    {
        return m_bIsVisible;
    }

    inline bool Target::get_BlockKeybd() const
    {
        return m_bBlockKeybd;
    }

    inline bool Target::get_BlockMouse() const
    {
        return m_bBlockMouse;
    }

    inline ULONG Target::get_Count() const
    {
        return m_Actions.Count;
    }

    inline ActionCollection::ConstIter Target::get_Begin()
    {
        return m_Actions.CBegin;
    }

    inline ActionCollection::ConstIter Target::get_End()
    {
        return m_Actions.CEnd;
    }

    inline void Target::set_Callback(TargetCallback* pCallbak)
    {
        m_pCallback = pCallbak;
    }
}
