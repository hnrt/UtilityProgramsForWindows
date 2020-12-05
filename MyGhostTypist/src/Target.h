#pragma once


#include "hnrt/RefObj.h"
#include "hnrt/RefPtr.h"


namespace hnrt
{
    class Target;
    class ActionTarget;
    class FindWindowTarget;

    class TargetCallback
    {
    public:

        virtual void OnTargetUpdate(Target&) = 0;
    };

    class Target
        : public RefObj
    {
    public:

        static RefPtr<Target> CreateNull(PCWSTR pszName = nullptr);
        static RefPtr<Target> CreateFindWindow(PCWSTR pszName, PCWSTR pszClassName, PCWSTR pszWindowText);

    protected:

        Target(PCWSTR pszType, PCWSTR pszName = nullptr);

    public:

        Target(const Target&) = delete;
        virtual ~Target() = default;
        void operator =(const Target&) = delete;
        virtual RefPtr<Target> Clone() const = 0;
        PCWSTR get_Type() const;
        PCWSTR get_Name() const;
        void set_Name(PCWSTR pszName);
        bool get_IsVisible() const;
        void set_IsVisible(bool value);
        void set_Callback(TargetCallback* pCallbak);
        bool get_IsTypeNull() const;
        bool get_IsTypeAction() const;
        bool get_IsTypeFindWindow() const;
        const ActionTarget* get_ActionTargetPtr() const;
        ActionTarget* get_ActionTargetPtr();
        const FindWindowTarget* get_FindWindowTargetPtr() const;
        FindWindowTarget* get_FindWindowTargetPtr();
        __declspec(property(get = get_Type)) PCWSTR Type;
        __declspec(property(get = get_Name, put = set_Name)) PCWSTR Name;
        __declspec(property(get = get_IsVisible, put = set_IsVisible)) bool IsVisible;
        __declspec(property(put = set_Callback)) TargetCallback* Callback;
        __declspec(property(get = get_IsTypeNull)) bool IsTypeNull;
        __declspec(property(get = get_IsTypeAction)) bool IsTypeAction;
        __declspec(property(get = get_IsTypeFindWindow)) bool IsTypeFindWindow;
        __declspec(property(get = get_ActionTargetPtr)) ActionTarget* ActionTargetPtr;
        __declspec(property(get = get_FindWindowTargetPtr)) FindWindowTarget* FindWindowTargetPtr;

    protected:

        PCWSTR m_pszType;
        PCWSTR m_pszName;
        bool m_bIsVisible;
        TargetCallback* m_pCallback;
    };

    inline PCWSTR Target::get_Type() const
    {
        return m_pszType;
    }

    inline PCWSTR Target::get_Name() const
    {
        return m_pszName;
    }

    inline bool Target::get_IsVisible() const
    {
        return m_bIsVisible;
    }

    inline void Target::set_Callback(TargetCallback* pCallbak)
    {
        m_pCallback = pCallbak;
    }
}
