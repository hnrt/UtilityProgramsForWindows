#pragma once


#include <Windows.h>
#include <vector>
#include <utility>
#include "hnrt/RefObj.h"
#include "hnrt/RefPtr.h"
#include "hnrt/String.h"


#define AC_UNKNOWN 0
#define AC_SETFOREGROUNDWINDOW 1
#define AC_TYPEKEY 2
#define AC_TYPEUNICODE 3
#define AC_TYPEUSERNAME 4
#define AC_TYPEPASSWORD 5
#define AC_TYPEDELETESEQUENCE 6
#define AC_LEFTCLICK 7


#define AC_FLAG_AA (1UL<<0)


namespace hnrt
{
    class Action
        : public RefObj
    {
    public:

        static RefPtr<Action> SetForegroundWindow(PCWSTR pszClassName, PCWSTR pszWindowText);
        static RefPtr<Action> TypeKey(PCWSTR psz);
        static RefPtr<Action> TypeUnicode(PCWSTR psz);
        static RefPtr<Action> TypeUsername(PCWSTR psz = nullptr);
        static RefPtr<Action> TypePassword(PCWSTR psz = nullptr);
        static RefPtr<Action> TypeDeleteSequence();
        static RefPtr<Action> LeftClick();

        Action(const Action& src);
        virtual ~Action() = default;
        virtual Action& operator =(const Action& src);
        virtual RefPtr<Action> Clone() const = 0;
        LONG get_Type() const;
        DWORD get_Flags() const;
        void set_Flags(DWORD flags);
        __declspec(property(get = get_Type)) LONG Type;
        __declspec(property(get = get_Flags, put = set_Flags)) DWORD Flags;

    protected:

        Action(LONG type);

        LONG m_Type;
        DWORD m_Flags;
    };

    inline Action::Action(LONG type)
        : RefObj()
        , m_Type(type)
        , m_Flags(0UL)
    {
    }

    inline Action::Action(const Action& src)
        : RefObj()
        , m_Type(src.m_Type)
        , m_Flags(src.m_Flags)
    {
    }

    inline Action& Action::operator =(const Action& src)
    {
        m_Type = src.m_Type;
        m_Flags = src.m_Flags;
        return *this;
    }

    inline LONG Action::get_Type() const
    {
        return m_Type;
    }

    inline DWORD Action::get_Flags() const
    {
        return m_Flags;
    }

    inline void Action::set_Flags(DWORD flags)
    {
        m_Flags = flags;
    }

    class SetForegroundWindowAction
        : public Action
    {
    public:

        typedef std::vector<std::pair<String, String>>::const_iterator ConstIter;
        typedef std::pair<String, String> Entry;

        SetForegroundWindowAction(PCWSTR pszClassName, PCWSTR pszWindowText);
        SetForegroundWindowAction(const SetForegroundWindowAction& src);
        virtual ~SetForegroundWindowAction() = default;
        virtual SetForegroundWindowAction& operator =(const SetForegroundWindowAction& src);
        virtual RefPtr<Action> Clone() const;
        void Prepend(PCWSTR pszClassName, PCWSTR pszWindowText);
        void Append(PCWSTR pszClassName, PCWSTR pszWindowText);
        bool Find(HWND* phwnd1 = nullptr, HWND* phwnd2 = nullptr) const;
        void SetActiveAccessibility(PCWSTR pszName, LONG lRole);
        void SetActiveAccessibility(PCWSTR pszName);
        void SetActiveAccessibility(LONG lRole);
        PCWSTR get_ClassName() const;
        PCWSTR get_WindowText() const;
        ConstIter get_Begin() const;
        ConstIter get_End() const;
        PCWSTR get_AccName() const;
        LONG get_AccRole() const;
        __declspec(property(get = get_ClassName)) PCWSTR ClassName;
        __declspec(property(get = get_WindowText)) PCWSTR WindowText;
        __declspec(property(get = get_Begin)) ConstIter Begin;
        __declspec(property(get = get_End)) ConstIter End;
        __declspec(property(get = get_AccName)) PCWSTR AccName;
        __declspec(property(get = get_AccRole)) LONG AccRole;

    protected:

        std::vector<std::pair<String, String>> m_Stack;
        String m_szAccName;
        LONG m_AccRole;
    };

    inline RefPtr<Action> SetForegroundWindowAction::Clone() const
    {
        return RefPtr<Action>(new SetForegroundWindowAction(*this));
    }

    inline SetForegroundWindowAction::ConstIter SetForegroundWindowAction::get_Begin() const
    {
        return m_Stack.begin() + 1;
    }

    inline SetForegroundWindowAction::ConstIter SetForegroundWindowAction::get_End() const
    {
        return m_Stack.end();
    }

    inline PCWSTR SetForegroundWindowAction::get_AccName() const
    {
        return m_szAccName;
    }

    inline LONG SetForegroundWindowAction::get_AccRole() const
    {
        return m_AccRole;
    }

    class TypeKeyAction
        : public Action
    {
    public:

        TypeKeyAction(PCWSTR psz);
        TypeKeyAction(const TypeKeyAction& src);
        virtual ~TypeKeyAction() = default;
        virtual TypeKeyAction& operator =(const TypeKeyAction& src);
        virtual RefPtr<Action> Clone() const;
        PCWSTR get_Key() const;
        __declspec(property(get = get_Key)) PCWSTR Key;

    protected:

        String m_sz;
    };

    inline RefPtr<Action> TypeKeyAction::Clone() const
    {
        return RefPtr<Action>(new TypeKeyAction(*this));
    }

    inline PCWSTR TypeKeyAction::get_Key() const
    {
        return m_sz;
    }

    class TypeUnicodeAction
        : public Action
    {
    public:

        TypeUnicodeAction(PCWSTR psz);
        TypeUnicodeAction(const TypeUnicodeAction& src);
        virtual ~TypeUnicodeAction() = default;
        virtual TypeUnicodeAction& operator =(const TypeUnicodeAction& src);
        virtual RefPtr<Action> Clone() const;
        PCWSTR get_Text() const;
        __declspec(property(get = get_Text)) PCWSTR Text;

    protected:

        String m_sz;
    };

    inline RefPtr<Action> TypeUnicodeAction::Clone() const
    {
        return RefPtr<Action>(new TypeUnicodeAction(*this));
    }

    inline PCWSTR TypeUnicodeAction::get_Text() const
    {
        return m_sz;
    }

    class TypeUsernameAction
        : public Action
    {
    public:

        TypeUsernameAction(PCWSTR psz);
        TypeUsernameAction(const TypeUsernameAction& src);
        virtual ~TypeUsernameAction() = default;
        virtual TypeUsernameAction& operator =(const TypeUsernameAction& src);
        virtual RefPtr<Action> Clone() const;
        PCWSTR get_Name() const;
        __declspec(property(get = get_Name)) PCWSTR Name;

    protected:

        String m_sz;
    };

    inline RefPtr<Action> TypeUsernameAction::Clone() const
    {
        return RefPtr<Action>(new TypeUsernameAction(*this));
    }

    inline PCWSTR TypeUsernameAction::get_Name() const
    {
        return m_sz;
    }

    class TypePasswordAction
        : public Action
    {
    public:

        TypePasswordAction(PCWSTR psz);
        TypePasswordAction(const TypePasswordAction& src);
        virtual ~TypePasswordAction() = default;
        virtual TypePasswordAction& operator =(const TypePasswordAction& src);
        virtual RefPtr<Action> Clone() const;
        PCWSTR get_Name() const;
        __declspec(property(get = get_Name)) PCWSTR Name;

    protected:

        String m_sz;
    };

    inline RefPtr<Action> TypePasswordAction::Clone() const
    {
        return RefPtr<Action>(new TypePasswordAction(*this));
    }

    inline PCWSTR TypePasswordAction::get_Name() const
    {
        return m_sz;
    }

    class TypeDeleteSequenceAction
        : public Action
    {
    public:

        TypeDeleteSequenceAction();
        TypeDeleteSequenceAction(const TypeDeleteSequenceAction& src);
        virtual ~TypeDeleteSequenceAction() = default;
        virtual TypeDeleteSequenceAction& operator =(const TypeDeleteSequenceAction& src);
        virtual RefPtr<Action> Clone() const;
    };

    inline TypeDeleteSequenceAction::TypeDeleteSequenceAction()
        : Action(AC_TYPEDELETESEQUENCE)
    {
    }

    inline TypeDeleteSequenceAction::TypeDeleteSequenceAction(const TypeDeleteSequenceAction& src)
        : Action(AC_TYPEDELETESEQUENCE)
    {
    }

    inline TypeDeleteSequenceAction& TypeDeleteSequenceAction::operator =(const TypeDeleteSequenceAction& src)
    {
        return *this;
    }

    inline RefPtr<Action> TypeDeleteSequenceAction::Clone() const
    {
        return RefPtr<Action>(new TypeDeleteSequenceAction(*this));
    }

    class LeftClickAction
        : public Action
    {
    public:

        LeftClickAction();
        LeftClickAction(const LeftClickAction& src);
        virtual ~LeftClickAction() = default;
        virtual LeftClickAction& operator =(const LeftClickAction& src);
        virtual RefPtr<Action> Clone() const;
    };

    inline LeftClickAction::LeftClickAction()
        : Action(AC_LEFTCLICK)
    {
    }

    inline LeftClickAction::LeftClickAction(const LeftClickAction& src)
        : Action(src)
    {
    }

    inline LeftClickAction& LeftClickAction::operator =(const LeftClickAction& src)
    {
        Action::operator=(src);
        return *this;
    }

    inline RefPtr<Action> LeftClickAction::Clone() const
    {
        return RefPtr<Action>(new LeftClickAction(*this));
    }
}
