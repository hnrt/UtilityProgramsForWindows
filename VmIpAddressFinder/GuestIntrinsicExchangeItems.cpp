#include "GuestIntrinsicExchangeItems.h"
#pragma warning(push)
#pragma warning(disable:4192)
#import <msxml6.dll> named_guids raw_interfaces_only
#pragma warning(pop)
#include <string.h>
#include "hnrt/RefPtr.h"
#include "hnrt/ComException.h"


using namespace hnrt;


class AttributeMap
    : public KeyValueMap
{
public:

    AttributeMap(RefPtr<IXMLDOMNode>& pNode);
    AttributeMap(const AttributeMap&) = delete;
    ~AttributeMap() = default;
    AttributeMap& operator =(const AttributeMap&) = delete;
};


AttributeMap::AttributeMap(RefPtr<IXMLDOMNode>& pNode)
    : KeyValueMap()
{
    RefPtr<IXMLDOMNamedNodeMap> pAttrMap;
    HRESULT hr = pNode->get_attributes(&pAttrMap);
    while (hr == S_OK)
    {
        RefPtr<IXMLDOMNode> pAttr;
        hr = pAttrMap->nextNode(&pAttr);
        if (hr == S_OK)
        {
            BSTR strName = nullptr;
            hr = pAttr->get_nodeName(&strName);
            if (hr == S_OK)
            {
                BSTR strText = nullptr;
                hr = pAttr->get_text(&strText);
                if (hr == S_OK)
                {
                    KeyValueMap::Add(String::Copy(strName), String::Copy(strText));
                    SysFreeString(strText);
                }
                SysFreeString(strName);
            }
        }
    }
}


static PCWSTR GetNodeValue(RefPtr<IXMLDOMNode>& pNode)
{
    RefPtr<IXMLDOMNodeList> pChildren;
    HRESULT hr = pNode->get_childNodes(&pChildren);
    while (hr == S_OK)
    {
        RefPtr <IXMLDOMNode> pChild;
        hr = pChildren->nextNode(&pChild);
        if (hr == S_OK)
        {
            BSTR strName = nullptr;
            hr = pChild->get_nodeName(&strName);
            if (hr == S_OK)
            {
                bool bFound = !String::Compare(strName, L"VALUE") ? true : false;
                SysFreeString(strName);
                if (bFound)
                {
                    BSTR strText = nullptr;
                    hr = pChild->get_text(&strText);
                    if (hr == S_OK)
                    {
                        PCWSTR pszValue = String::Copy(strText);
                        SysFreeString(strText);
                        return pszValue;
                    }
                    else
                    {
                        return nullptr;
                    }
                }
            }
        }
    }
    return nullptr;
}


GuestIntrinsicExchangeItems::GuestIntrinsicExchangeItems(const KeyValueMap& exchangeComponentMap)
    : KeyValueMap()
{
    PCWSTR pszCount = exchangeComponentMap.GetValue(L"GuestIntrinsicExchangeItems.Count");
    if (!pszCount)
    {
        return;
    }
    long count = wcstol(pszCount, nullptr, 10);
    if (!count)
    {
        return;
    }
    RefPtr<IXMLDOMDocument> pDomDocument;
    HRESULT hr = ::CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (LPVOID*)&pDomDocument);
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to get xml document parser.");
    }
    for (long i = 0; i < count; i++)
    {
        PCWSTR pszXml = exchangeComponentMap.GetValue(String::Format(L"GuestIntrinsicExchangeItems[%ld]", i));
        if (!pszXml)
        {
            continue;
        }
        VARIANT_BOOL bSuccess;
        hr = pDomDocument->loadXML(const_cast<BSTR>(pszXml), &bSuccess);
        if (FAILED(hr))
        {
            continue;
        }
        RefPtr<IXMLDOMNodeList> pProps;
        hr = pDomDocument->getElementsByTagName(const_cast<BSTR>(L"PROPERTY"), &pProps);
        if (FAILED(hr))
        {
            continue;
        }
        PCWSTR pszName = nullptr;
        PCWSTR pszValue = nullptr;
        while (true)
        {
            RefPtr<IXMLDOMNode> pProp;
            hr = pProps->nextNode(&pProp);
            if (hr != S_OK)
            {
                break;
            }
            AttributeMap attributes(pProp);
            PCWSTR pszPropName = attributes.GetValue(L"NAME");
            if (!pszPropName)
            {
                continue;
            }
            if (!String::Compare(pszPropName, L"Name"))
            {
                pszName = GetNodeValue(pProp);
            }
            else if (!String::Compare(pszPropName, L"Data"))
            {
                pszValue = GetNodeValue(pProp);
            }
            if (pszName && pszValue)
            {
                KeyValueMap::Add(pszName, pszValue);
                break;
            }
        }
    }
}
