#pragma once


#include <Windows.h>
#include <atlbase.h>
#pragma warning(push)
#pragma warning(disable:4192)
#import <msxml6.dll> named_guids raw_interfaces_only
#pragma warning(pop)
#include <map>
#include "hnrt/ComException.h"
#include "hnrt/String.h"
#include "WindowMetrics.h"


namespace hnrt
{
    class XmlDocument
    {
    public:

        XmlDocument();
        XmlDocument(const XmlDocument&) = delete;
        ~XmlDocument() = default;
        void operator =(const XmlDocument&) = delete;
        void Load(PCWSTR pszFileName);
        void LoadXML(PCWSTR psz);
        template<typename T, typename U>
        static void LoadChildren(MSXML2::IXMLDOMNode* pParent, T* pInstance, ...);
        void Save(PCWSTR pszFileName);
        void Indent();
        MSXML2::IXMLDOMElement* BuildDocumentRoot(PCWSTR pszName);
        MSXML2::IXMLDOMElementPtr AppendElement(PCWSTR pszName, MSXML2::IXMLDOMNode* pParent);
        MSXML2::IXMLDOMTextPtr InsertTextNodeBefore(PCWSTR pszData, MSXML2::IXMLDOMNode* pChild, MSXML2::IXMLDOMNode* pParent);
        MSXML2::IXMLDOMTextPtr AppendTextNode(PCWSTR pszData, MSXML2::IXMLDOMNode* pParent);
        static void AppendChild(MSXML2::IXMLDOMNode* pParent, MSXML2::IXMLDOMNode* pChild);
        static bool GetAttribute(MSXML2::IXMLDOMNode* pNode, PCWSTR pszName, PCWSTR& pszValue);
        static bool GetAttribute(MSXML2::IXMLDOMNode* pNode, PCWSTR pszName, long& lValue);
        static bool GetAttribute(MSXML2::IXMLDOMNode* pNode, PCWSTR pszName, LTRB& value);
        static bool GetAttribute(MSXML2::IXMLDOMNode* pNode, PCWSTR pszName, bool& bValue);
        static void SetAttribute(MSXML2::IXMLDOMElement* pElement, PCWSTR pszName, PCWSTR pszValue);
        static void SetAttribute(MSXML2::IXMLDOMElement* pElement, PCWSTR pszName, long lValue);
        static void SetAttribute(MSXML2::IXMLDOMElement* pElement, PCWSTR pszName, const LTRB& value);
        static void SetAttribute(MSXML2::IXMLDOMElement* pElement, PCWSTR pszName, bool bValue);
        static PCWSTR GetText(MSXML2::IXMLDOMNode* pNode);
        static void SetText(MSXML2::IXMLDOMElement* pElement, PCWSTR pszFormat, ...);
        MSXML2::IXMLDOMElement* get_DocumentElement();
        __declspec(property(get = get_DocumentElement)) MSXML2::IXMLDOMElement* DocumentElement;

    private:

        void Indent(MSXML2::IXMLDOMNode* pParent, int level);

        MSXML2::IXMLDOMDocument2Ptr m_pDocument;
        MSXML2::IXMLDOMElementPtr m_pDocumentElement;
    };

    template<typename T, typename U>
    void XmlDocument::LoadChildren(MSXML2::IXMLDOMNode* pParent, T* pInstance, ...)
    {
        std::map<PCWSTR, U, StringLessThan> map;
        va_list argList;
        va_start(argList, pInstance);
        PCWSTR pszName;
        while ((pszName = va_arg(argList, PCWSTR)))
        {
            U pCallback = va_arg(argList, U);
            map.insert(std::pair<PCWSTR, U>(pszName, pCallback));
        }
        va_end(argList);
        MSXML2::IXMLDOMNodeListPtr pChildren;
        HRESULT hr = pParent->get_childNodes(&pChildren);
        if (FAILED(hr))
        {
            throw ComException(hr, L"Failed to get children.");
        }
        while (true)
        {
            MSXML2::IXMLDOMNodePtr pChild;
            hr = pChildren->nextNode(&pChild);
            if (FAILED(hr))
            {
                throw ComException(hr, L"Failed to get child.");
            }
            else if (hr == S_FALSE)
            {
                break;
            }
#pragma warning(push)
#pragma warning(disable:26812)
            MSXML2::DOMNodeType type;
#pragma warning(pop)
            hr = pChild->get_nodeType(&type);
            if (FAILED(hr))
            {
                throw ComException(hr, L"Failed to get child type.");
            }
            else if (type != MSXML2::DOMNodeType::NODE_ELEMENT)
            {
                continue;
            }
            CComBSTR strName;
            hr = pChild->get_nodeName(&strName);
            if (FAILED(hr))
            {
                throw ComException(hr, L"Failed to get child name.");
            }
            typename std::map<PCWSTR, U, StringLessThan>::iterator iter = map.find(strName);
            if (iter != map.end())
            {
                (pInstance->*(iter->second))(pChild);
            }
        }
    }

    inline MSXML2::IXMLDOMElement* XmlDocument::get_DocumentElement()
    {
        return m_pDocumentElement;
    }
}
