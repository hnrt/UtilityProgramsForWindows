#pragma once


#include <Windows.h>
#include <vector>
#include <utility>
#include "hnrt/RefObj.h"
#include "hnrt/RefPtr.h"


#define AC_UNKNOWN 0
#define AC_SETFOREGROUNDWINDOW 1
#define AC_TYPEKEY 2
#define AC_TYPEUNICODE 3
#define AC_TYPEUSERNAME 4
#define AC_TYPEPASSWORD 5
#define AC_TYPEDELETESEQUENCE 6


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

        Action(const Action& src);
        virtual ~Action() = default;
        virtual Action& operator =(const Action& src);
        virtual RefPtr<Action> Clone() const = 0;
        LONG get_Type() const;
        __declspec(property(get = get_Type)) LONG Type;

    protected:

        Action(LONG type);

        LONG m_type;
    };

    inline Action::Action(LONG type)
        : RefObj()
        , m_type(type)
    {
    }

    inline Action::Action(const Action& src)
        : RefObj()
        , m_type(src.m_type)
    {
    }

    inline Action& Action::operator =(const Action& src)
    {
        m_type = src.m_type;
        return *this;
    }

    inline LONG Action::get_Type() const
    {
        return m_type;
    }

    class SetForegroundWindowAction
        : public Action
    {
    public:

        typedef std::vector<std::pair<PCWSTR, PCWSTR>>::const_iterator ConstIter;
        typedef std::pair<PCWSTR, PCWSTR> Entry;

        SetForegroundWindowAction(PCWSTR pszClassName, PCWSTR pszWindowText);
        SetForegroundWindowAction(const SetForegroundWindowAction& src);
        virtual ~SetForegroundWindowAction() = default;
        virtual SetForegroundWindowAction& operator =(const SetForegroundWindowAction& src);
        virtual RefPtr<Action> Clone() const;
        void Prepend(PCWSTR pszClassName, PCWSTR pszWindowText);
        void Append(PCWSTR pszClassName, PCWSTR pszWindowText);
        bool Find(HWND* phwnd1 = nullptr, HWND* phwnd2 = nullptr) const;
        PCWSTR get_ClassName() const;
        PCWSTR get_WindowText() const;
        ConstIter get_Begin() const;
        ConstIter get_End() const;
        __declspec(property(get = get_ClassName)) PCWSTR ClassName;
        __declspec(property(get = get_WindowText)) PCWSTR WindowText;
        __declspec(property(get = get_Begin)) ConstIter Begin;
        __declspec(property(get = get_End)) ConstIter End;

    protected:

        std::vector<std::pair<PCWSTR, PCWSTR>> m_Stack;
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

        PCWSTR m_psz;
    };

    inline RefPtr<Action> TypeKeyAction::Clone() const
    {
        return RefPtr<Action>(new TypeKeyAction(*this));
    }

    inline PCWSTR TypeKeyAction::get_Key() const
    {
        return m_psz;
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

        PCWSTR m_psz;
    };

    inline RefPtr<Action> TypeUnicodeAction::Clone() const
    {
        return RefPtr<Action>(new TypeUnicodeAction(*this));
    }

    inline PCWSTR TypeUnicodeAction::get_Text() const
    {
        return m_psz;
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

        PCWSTR m_psz;
    };

    inline RefPtr<Action> TypeUsernameAction::Clone() const
    {
        return RefPtr<Action>(new TypeUsernameAction(*this));
    }

    inline PCWSTR TypeUsernameAction::get_Name() const
    {
        return m_psz;
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

        PCWSTR m_psz;
    };

    inline RefPtr<Action> TypePasswordAction::Clone() const
    {
        return RefPtr<Action>(new TypePasswordAction(*this));
    }

    inline PCWSTR TypePasswordAction::get_Name() const
    {
        return m_psz;
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
}
