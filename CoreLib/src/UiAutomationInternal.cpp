#include "pch.h"
#include <atlbase.h>
#include <list>
#include "UiAutomationInternal.h"
#include "hnrt/String.h"


#pragma comment(lib,"oleacc")


using namespace hnrt;


UiAutomation::UiAutomation()
    : RefObj()
{
}


UiAutomationInternal::UiAutomationInternal(HWND hwnd)
    : UiAutomation()
    , m_pAccessible()
    , m_hr(AccessibleObjectFromWindow(hwnd, OBJID_CLIENT, IID_IAccessible, (void**)&m_pAccessible))
{
}


UiAutomationInternal::UiAutomationInternal(RefPtr<IAccessible> pAccessible)
    : UiAutomation()
    , m_pAccessible(pAccessible)
    , m_hr(S_OK)
{
}


UiAutomationInternal::~UiAutomationInternal()
{
}


RefPtr<UiAutomation> UiAutomationInternal::HitTest(POINT pt)
{
    RefPtr<IAccessible> pAccessible1;
    m_hr = HitTest(m_pAccessible, pt, pAccessible1);
    if (m_hr != S_OK)
    {
        return RefPtr<UiAutomation>();
    }
    else if (pAccessible1 != m_pAccessible)
    {
        for (int safetyCount = 0; safetyCount < 100; safetyCount++)
        {
            RefPtr<IAccessible> pAccessible2;
            m_hr = HitTest(pAccessible1, pt, pAccessible2);
            if (m_hr != S_OK || pAccessible2 == pAccessible1)
            {
                break;
            }
            pAccessible1 = pAccessible2;
        }
    }
    return RefPtr<UiAutomation>(new UiAutomationInternal(pAccessible1));
}


HRESULT UiAutomationInternal::HitTest(RefPtr<IAccessible> pAccessible, POINT pt, RefPtr<IAccessible>& pAccessibleOut)
{
    CComVariant varChild;
    HRESULT hr = pAccessible->accHitTest(pt.x, pt.y, &varChild);
    if (hr == S_OK)
    {
        if (varChild.vt == VT_DISPATCH)
        {
            if (varChild.pdispVal)
            {
                hr = varChild.pdispVal->QueryInterface(IID_IAccessible, (void**)&pAccessibleOut);
                if (hr == S_OK)
                {
                    if (!pAccessibleOut)
                    {
                        hr = E_FAIL;
                    }
                }
            }
            else
            {
                hr = E_FAIL;
            }
        }
        else if (varChild.vt == VT_I4)
        {
            if (varChild.lVal == CHILDID_SELF)
            {
                pAccessibleOut = pAccessible;
            }
            else
            {
                RefPtr<IDispatch> pDispatch;
                hr = pAccessible->get_accChild(varChild, &pDispatch);
                if (hr == S_OK)
                {
                    hr = pDispatch->QueryInterface(IID_IAccessible, (void**)&pAccessibleOut);
                    if (hr == S_OK)
                    {
                        if (!pAccessibleOut)
                        {
                            hr = E_FAIL;
                        }
                    }
                }
            }
        }
        else
        {
            hr = E_FAIL;
        }
    }
    return hr;
}


bool UiAutomationInternal::get_HasName() const
{
    if (m_pAccessible)
    {
        CComVariant varSelf;
        varSelf.vt = VT_I4;
        varSelf.lVal = CHILDID_SELF;
        CComBSTR bstr;
        m_hr = m_pAccessible->get_accName(varSelf, &bstr);
        return m_hr == S_OK && bstr;
    }
    return false;
}


PCWSTR UiAutomationInternal::get_Name() const
{
    if (m_pAccessible)
    {
        CComVariant varSelf;
        varSelf.vt = VT_I4;
        varSelf.lVal = CHILDID_SELF;
        CComBSTR bstr;
        m_hr = m_pAccessible->get_accName(varSelf, &bstr);
        if (m_hr == S_OK)
        {
            if (bstr)
            {
                return String::Copy(bstr);
            }
            else
            {
                m_hr = E_FAIL;
            }
        }
    }
    return nullptr;
}


DWORD UiAutomationInternal::get_Role() const
{
    if (m_pAccessible)
    {
        CComVariant varSelf;
        varSelf.vt = VT_I4;
        varSelf.lVal = CHILDID_SELF;
        CComVariant varRole;
        m_hr = m_pAccessible->get_accRole(varSelf, &varRole);
        if (m_hr == S_OK)
        {
            if (varRole.vt == VT_I4)
            {
                return varRole.lVal;
            }
            else
            {
                m_hr = E_FAIL;
            }
        }
    }
    return -1;
}


PCWSTR UiAutomation::GetRoleName(DWORD dwRole)
{
    switch (dwRole)
    {
    case (DWORD)-1: return L"ROLE_UNDEFINED";
#define ROLETEXT(x) case x: return L#x
        ROLETEXT(ROLE_SYSTEM_TITLEBAR);
        ROLETEXT(ROLE_SYSTEM_MENUBAR);
        ROLETEXT(ROLE_SYSTEM_SCROLLBAR);
        ROLETEXT(ROLE_SYSTEM_GRIP);
        ROLETEXT(ROLE_SYSTEM_SOUND);
        ROLETEXT(ROLE_SYSTEM_CURSOR);
        ROLETEXT(ROLE_SYSTEM_CARET);
        ROLETEXT(ROLE_SYSTEM_ALERT);
        ROLETEXT(ROLE_SYSTEM_WINDOW);
        ROLETEXT(ROLE_SYSTEM_CLIENT);
        ROLETEXT(ROLE_SYSTEM_MENUPOPUP);
        ROLETEXT(ROLE_SYSTEM_MENUITEM);
        ROLETEXT(ROLE_SYSTEM_TOOLTIP);
        ROLETEXT(ROLE_SYSTEM_APPLICATION);
        ROLETEXT(ROLE_SYSTEM_DOCUMENT);
        ROLETEXT(ROLE_SYSTEM_PANE);
        ROLETEXT(ROLE_SYSTEM_CHART);
        ROLETEXT(ROLE_SYSTEM_DIALOG);
        ROLETEXT(ROLE_SYSTEM_BORDER);
        ROLETEXT(ROLE_SYSTEM_GROUPING);
        ROLETEXT(ROLE_SYSTEM_SEPARATOR);
        ROLETEXT(ROLE_SYSTEM_TOOLBAR);
        ROLETEXT(ROLE_SYSTEM_STATUSBAR);
        ROLETEXT(ROLE_SYSTEM_TABLE);
        ROLETEXT(ROLE_SYSTEM_COLUMNHEADER);
        ROLETEXT(ROLE_SYSTEM_ROWHEADER);
        ROLETEXT(ROLE_SYSTEM_COLUMN);
        ROLETEXT(ROLE_SYSTEM_ROW);
        ROLETEXT(ROLE_SYSTEM_CELL);
        ROLETEXT(ROLE_SYSTEM_LINK);
        ROLETEXT(ROLE_SYSTEM_HELPBALLOON);
        ROLETEXT(ROLE_SYSTEM_CHARACTER);
        ROLETEXT(ROLE_SYSTEM_LIST);
        ROLETEXT(ROLE_SYSTEM_LISTITEM);
        ROLETEXT(ROLE_SYSTEM_OUTLINE);
        ROLETEXT(ROLE_SYSTEM_OUTLINEITEM);
        ROLETEXT(ROLE_SYSTEM_PAGETAB);
        ROLETEXT(ROLE_SYSTEM_PROPERTYPAGE);
        ROLETEXT(ROLE_SYSTEM_INDICATOR);
        ROLETEXT(ROLE_SYSTEM_GRAPHIC);
        ROLETEXT(ROLE_SYSTEM_STATICTEXT);
        ROLETEXT(ROLE_SYSTEM_TEXT);
        ROLETEXT(ROLE_SYSTEM_PUSHBUTTON);
        ROLETEXT(ROLE_SYSTEM_CHECKBUTTON);
        ROLETEXT(ROLE_SYSTEM_RADIOBUTTON);
        ROLETEXT(ROLE_SYSTEM_COMBOBOX);
        ROLETEXT(ROLE_SYSTEM_DROPLIST);
        ROLETEXT(ROLE_SYSTEM_PROGRESSBAR);
        ROLETEXT(ROLE_SYSTEM_DIAL);
        ROLETEXT(ROLE_SYSTEM_HOTKEYFIELD);
        ROLETEXT(ROLE_SYSTEM_SLIDER);
        ROLETEXT(ROLE_SYSTEM_SPINBUTTON);
        ROLETEXT(ROLE_SYSTEM_DIAGRAM);
        ROLETEXT(ROLE_SYSTEM_ANIMATION);
        ROLETEXT(ROLE_SYSTEM_EQUATION);
        ROLETEXT(ROLE_SYSTEM_BUTTONDROPDOWN);
        ROLETEXT(ROLE_SYSTEM_BUTTONMENU);
        ROLETEXT(ROLE_SYSTEM_BUTTONDROPDOWNGRID);
        ROLETEXT(ROLE_SYSTEM_WHITESPACE);
        ROLETEXT(ROLE_SYSTEM_PAGETABLIST);
        ROLETEXT(ROLE_SYSTEM_CLOCK);
        ROLETEXT(ROLE_SYSTEM_SPLITBUTTON);
        ROLETEXT(ROLE_SYSTEM_IPADDRESS);
        ROLETEXT(ROLE_SYSTEM_OUTLINEBUTTON);
#undef ROLETEXT
    default:
        return String::Format(L"ROLE_%ld", dwRole);
    }
}


PCWSTR UiAutomationInternal::get_Value() const
{
    if (m_pAccessible)
    {
        CComVariant varSelf;
        varSelf.vt = VT_I4;
        varSelf.lVal = CHILDID_SELF;
        CComBSTR bstr;
        m_hr = m_pAccessible->get_accValue(varSelf, &bstr);
        if (m_hr == S_OK)
        {
            if (bstr)
            {
                return String::Copy(bstr);
            }
            else
            {
                m_hr = E_FAIL;
            }
        }
    }
    return nullptr;
}


void UiAutomationInternal::put_Value(PCWSTR psz)
{
    if (m_pAccessible)
    {
        CComVariant varSelf;
        varSelf.vt = VT_I4;
        varSelf.lVal = CHILDID_SELF;
        m_hr = m_pAccessible->put_accValue(varSelf, CComBSTR(psz));
    }
}


PCWSTR UiAutomationInternal::get_DefaultAction() const
{
    if (m_pAccessible)
    {
        CComVariant varSelf;
        varSelf.vt = VT_I4;
        varSelf.lVal = CHILDID_SELF;
        CComBSTR bstr;
        HRESULT hr = m_pAccessible->get_accDefaultAction(varSelf, &bstr);
        if (hr == S_OK)
        {
            if (bstr)
            {
                return String::Copy(bstr);
            }
            else
            {
                m_hr = E_FAIL;
            }
        }
    }
    return nullptr;
}


bool UiAutomationInternal::DoDefaultAction() const
{
    if (m_pAccessible)
    {
        CComVariant varSelf;
        varSelf.vt = VT_I4;
        varSelf.lVal = CHILDID_SELF;
        CComBSTR bstr;
        m_hr = m_pAccessible->accDoDefaultAction(varSelf);
        return m_hr == S_OK;
    }
    return false;
}


bool UiAutomationInternal::Locate(RECT& rect) const
{
    if (m_pAccessible)
    {
        CComVariant varSelf;
        varSelf.vt = VT_I4;
        varSelf.lVal = CHILDID_SELF;
        LONG x = 0, y = 0, w = 0, h = 0;
        m_hr = m_pAccessible->accLocation(&x, &y, &w, &h, varSelf);
        if (m_hr == S_OK)
        {
            rect.left = x;
            rect.top = y;
            rect.right = x + w;
            rect.bottom = y + h;
            return true;
        }
    }
    return false;
}


bool UiAutomationInternal::Match(PCWSTR pszName, DWORD dwRole) const
{
    if (m_pAccessible)
    {
        if (Role == dwRole)
        {
            if (pszName)
            {
                PCWSTR pszMyName = Name;
                if (pszMyName && !String::Compare(pszMyName, pszName))
                {
                    return true;
                }
            }
            else if (!HasName)
            {
                return true;
            }
        }
    }
    return false;
}


RefPtr<UiAutomation> UiAutomationInternal::Find(PCWSTR pszName, DWORD dwRole)
{
    if (m_pAccessible && dwRole > 0)
    {
        if (Match(pszName, dwRole))
        {
            return RefPtr<UiAutomation>(new UiAutomationInternal(m_pAccessible));
        }
        return FindChild(pszName, dwRole, 0);
    }
    return RefPtr<UiAutomation>();
}


RefPtr<UiAutomation> UiAutomationInternal::FindChild(PCWSTR pszName, DWORD dwRole, LONG lDepth)
{
    if (lDepth < 32)
    {
        LONG cChildren = -1;
        m_hr = m_pAccessible->get_accChildCount(&cChildren);
        if (m_hr == S_OK)
        {
            if (cChildren > 0)
            {
                VariantArray vars(cChildren);
                LONG cObtained = -1;
                m_hr = AccessibleChildren(m_pAccessible.Ptr, 0, cChildren, vars.Ptr, &cObtained);
                if (m_hr == S_OK)
                {
                    for (LONG index = 0; index < cObtained; index++)
                    {
                        if (vars[index].vt == VT_DISPATCH)
                        {
                            RefPtr<IAccessible> pAccessible;
                            m_hr = vars[index].pdispVal->QueryInterface(IID_IAccessible, (void**)&pAccessible);
                            if (m_hr == S_OK)
                            {
                                RefPtr<UiAutomation> pAutomation(new UiAutomationInternal(pAccessible));
                                if (pAutomation->Match(pszName, dwRole))
                                {
                                    return pAutomation;
                                }
                            }
                        }
                        else if (vars[index].vt == VT_I4)
                        {
                            if (vars[index].lVal == CHILDID_SELF)
                            {
                                AddRef();
                                return RefPtr<UiAutomation>(this);
                            }
                            RefPtr<IDispatch> pDispatch;
                            m_hr = m_pAccessible->get_accChild(vars[index], &pDispatch);
                            if (m_hr == S_OK)
                            {
                                if (pDispatch)
                                {
                                    RefPtr<IAccessible> pAccessible;
                                    m_hr = pDispatch->QueryInterface(IID_IAccessible, (void**)&pAccessible);
                                    if (m_hr == S_OK)
                                    {
                                        RefPtr<UiAutomation> pAutomation(new UiAutomationInternal(pAccessible));
                                        if (pAutomation->Match(pszName, dwRole))
                                        {
                                            return pAutomation;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    for (LONG index = 0; index < cObtained; index++)
                    {
                        if (vars[index].vt == VT_DISPATCH)
                        {
                            RefPtr<IAccessible> pAccessible;
                            m_hr = vars[index].pdispVal->QueryInterface(IID_IAccessible, (void**)&pAccessible);
                            if (m_hr == S_OK)
                            {
                                RefPtr<UiAutomation> pAutomation = dynamic_cast<UiAutomationInternal*>(RefPtr<UiAutomation>(new UiAutomationInternal(pAccessible)).Ptr)->FindChild(pszName, dwRole, lDepth + 1);
                                if (pAutomation)
                                {
                                    return pAutomation;
                                }
                            }
                        }
                        else if (vars[index].vt == VT_I4)
                        {
                            RefPtr<IDispatch> pDispatch;
                            m_hr = m_pAccessible->get_accChild(vars[index], &pDispatch);
                            if (m_hr == S_OK)
                            {
                                if (pDispatch)
                                {
                                    RefPtr<IAccessible> pAccessible;
                                    m_hr = pDispatch->QueryInterface(IID_IAccessible, (void**)&pAccessible);
                                    if (m_hr == S_OK)
                                    {
                                        RefPtr<UiAutomation> pAutomation = dynamic_cast<UiAutomationInternal*>(RefPtr<UiAutomation>(new UiAutomationInternal(pAccessible)).Ptr)->FindChild(pszName, dwRole, lDepth + 1);
                                        if (pAutomation)
                                        {
                                            return pAutomation;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return RefPtr<UiAutomation>();
}
