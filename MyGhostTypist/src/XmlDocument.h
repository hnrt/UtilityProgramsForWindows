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
    class XmlElementLoadAction
    {
    public:

        XmlElementLoadAction() = default;
        XmlElementLoadAction(const XmlElementLoadAction&) = delete;
        virtual ~XmlElementLoadAction() = default;
        void operator =(const XmlElementLoadAction&) = delete;
        virtual void Invoke(MSXML2::IXMLDOMNode*) = 0;
    };

    class XmlElementLoader
    {
    public:

        XmlElementLoader();
        XmlElementLoader(const XmlElementLoader&) = delete;
        ~XmlElementLoader();
        void operator =(const XmlElementLoader&) = delete;
        XmlElementLoader& Add(PCWSTR pszName, XmlElementLoadAction* pAction);
        XmlElementLoader& Load(MSXML2::IXMLDOMNode* pParent);

    private:

        typedef std::map<PCWSTR, XmlElementLoadAction*, StringLessThan> Map;
        typedef std::pair<PCWSTR, XmlElementLoadAction*> Entry;

        Map m_map;
    };

    class XmlDocument
    {
    public:

        XmlDocument();
        XmlDocument(const XmlDocument&) = delete;
        ~XmlDocument() = default;
        void operator =(const XmlDocument&) = delete;
        void Load(PCWSTR pszFileName);
        void LoadXML(PCWSTR psz);
        void Save(PCWSTR pszFileName);
        void Indent();
        MSXML2::IXMLDOMElement* BuildDocumentRoot(PCWSTR pszName);
        MSXML2::IXMLDOMElementPtr AppendElement(PCWSTR pszName, MSXML2::IXMLDOMNode* pParent);
        MSXML2::IXMLDOMTextPtr InsertTextNodeBefore(PCWSTR pszData, MSXML2::IXMLDOMNode* pChild, MSXML2::IXMLDOMNode* pParent);
        MSXML2::IXMLDOMTextPtr AppendTextNode(PCWSTR pszData, MSXML2::IXMLDOMNode* pParent);
        static void AppendChild(MSXML2::IXMLDOMNode* pParent, MSXML2::IXMLDOMNode* pChild);
        static bool GetAttribute(MSXML2::IXMLDOMNode* pNode, PCWSTR pszName, PCWSTR& pszValue);
        static bool GetAttribute(MSXML2::IXMLDOMNode* pNode, PCWSTR pszName, long& lValue);
        static bool GetAttribute(MSXML2::IXMLDOMNode* pNode, PCWSTR pszName, RectangleMetrics& value);
        static bool GetAttribute(MSXML2::IXMLDOMNode* pNode, PCWSTR pszName, bool& bValue);
        static void SetAttribute(MSXML2::IXMLDOMElement* pElement, PCWSTR pszName, PCWSTR pszValue);
        static void SetAttribute(MSXML2::IXMLDOMElement* pElement, PCWSTR pszName, long lValue);
        static void SetAttribute(MSXML2::IXMLDOMElement* pElement, PCWSTR pszName, const RectangleMetrics& value);
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

    inline MSXML2::IXMLDOMElement* XmlDocument::get_DocumentElement()
    {
        return m_pDocumentElement;
    }
}
