#pragma once


#include <oleacc.h>
#include "hnrt/UiAutomation.h"


namespace hnrt
{
    class UiAutomationInternal
        : public UiAutomation
    {
    public:

        UiAutomationInternal(HWND hwnd);
        UiAutomationInternal(const UiAutomationInternal& src) = delete;
        virtual ~UiAutomationInternal();
        void operator =(const UiAutomationInternal& src) = delete;
        virtual bool get_IsValid() const override;
        virtual RefPtr<UiAutomation> HitTest(POINT pt) override;
        virtual bool get_HasName() const override;
        virtual String get_Name() const override;
        virtual DWORD get_Role() const override;
        virtual String get_Value() const override;
        virtual void put_Value(PCWSTR psz) override;
        virtual String get_DefaultAction() const override;
        virtual bool DoDefaultAction() const override;
        virtual bool Locate(RECT& rect) const override;
        virtual bool Match(PCWSTR pszName, DWORD dwRole) const override;
        virtual RefPtr<UiAutomation> Find(PCWSTR pszName, DWORD dwRole) override;

    protected:

        UiAutomationInternal(RefPtr<IAccessible> pAccessible);
        static HRESULT HitTest(RefPtr<IAccessible> pAccessible, POINT pt, RefPtr<IAccessible>& pAccessibleOut);
        RefPtr<UiAutomation> FindChild(PCWSTR pszName, DWORD dwRole, LONG lDepth);

        mutable RefPtr<IAccessible> m_pAccessible;
        mutable HRESULT m_hr;
    };

    inline bool UiAutomationInternal::get_IsValid() const
    {
        return m_pAccessible;
    }

    class VariantArray
    {
    public:

        VariantArray(ULONG capacity);
        VariantArray(const VariantArray&) = delete;
        ~VariantArray();
        void operator =(const VariantArray&) = delete;
        VARIANT& operator[](ULONG index) const;
        VARIANT* get_Ptr() const;
        __declspec(property(get = get_Ptr)) VARIANT* Ptr;
        ULONG get_Len() const;
        __declspec(property(get = get_Len)) ULONG Len;

    private:

        ULONG m_Capacity;
        VARIANT* m_pBase;
    };

    inline VariantArray::VariantArray(ULONG capacity)
        : m_Capacity(capacity)
        , m_pBase(new VARIANT[capacity])
    {
        memset(m_pBase, 0, sizeof(m_pBase[0]) * m_Capacity);
    }

    inline VariantArray::~VariantArray()
    {
        for (ULONG index = 0; index < m_Capacity; index++)
        {
            VariantClear(&m_pBase[index]);
        }
        delete[] m_pBase;
    }

    inline VARIANT& VariantArray::operator[](ULONG index) const
    {
        return m_pBase[index];
    }

    inline VARIANT* VariantArray::get_Ptr() const
    {
        return m_pBase;
    }

    inline ULONG VariantArray::get_Len() const
    {
        return m_Capacity;
    }
}
