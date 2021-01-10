#pragma once


#include "hnrt/RefObj.h"
#include "hnrt/RefPtr.h"


namespace hnrt
{
    class UiAutomation
        : public RefObj
    {
    public:

        UiAutomation(const UiAutomation& src) = delete;
        virtual ~UiAutomation() = default;
        void operator =(const UiAutomation& src) = delete;
        virtual bool get_IsValid() const = 0;
        __declspec(property(get = get_IsValid)) bool IsValid;
        virtual RefPtr<UiAutomation> HitTest(POINT pt) = 0;
        virtual bool get_HasName() const = 0;
        __declspec(property(get = get_HasName)) bool HasName;
        virtual PCWSTR get_Name() const = 0;
        __declspec(property(get = get_Name)) PCWSTR Name;
        virtual DWORD get_Role() const = 0;
        __declspec(property(get = get_Role)) DWORD Role;
        virtual PCWSTR get_Value() const = 0;
        virtual void put_Value(PCWSTR psz) = 0;
        __declspec(property(get = get_Value, put = put_Value)) PCWSTR Value;
        virtual PCWSTR get_DefaultAction() const = 0;
        __declspec(property(get = get_DefaultAction)) PCWSTR DefaultAction;
        virtual bool DoDefaultAction() const = 0;
        virtual bool Locate(RECT& rect) const = 0;
        virtual bool Match(PCWSTR pszName, DWORD dwRole) const = 0;
        virtual RefPtr<UiAutomation> Find(PCWSTR pszName, DWORD dwRole) = 0;

        static PCWSTR GetRoleName(DWORD dwRole);

    protected:

        friend class UiAutomationFactory;

        UiAutomation();
    };
}
