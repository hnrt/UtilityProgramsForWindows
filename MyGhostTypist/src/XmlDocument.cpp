#include "XmlDocument.h"
#include "hnrt/ComException.h"
#include "hnrt/String.h"


using namespace hnrt;


XmlDocument::XmlDocument()
    : m_pDocument()
    , m_pDocumentElement()
{
    HRESULT hr = m_pDocument.CreateInstance(__uuidof(MSXML2::DOMDocument60));
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to create MSXML2::DOMDocument60 instance.");
    }

    hr = m_pDocument->put_async(VARIANT_FALSE);
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to set Async to FALSE.");
    }

    hr = m_pDocument->put_preserveWhiteSpace(VARIANT_TRUE);
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to set PreserveWhiteSpace to TRUE.");
    }

    hr = m_pDocument->setProperty(CComBSTR(L"SelectionLanguage"), CComVariant(CComBSTR(L"XPath")));
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to get configuration element.");
    }
}


void XmlDocument::Load(PCWSTR pszFileName)
{
    VARIANT_BOOL result = 0;
    HRESULT hr = m_pDocument->load(CComVariant(pszFileName), &result);
    if (FAILED(hr) || !result)
    {
        throw ComException(hr, L"Failed to load the document from file: %s", pszFileName);
    }

    hr = m_pDocument->get_documentElement(&m_pDocumentElement);
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to get document element.");
    }
}


void XmlDocument::LoadXML(PCWSTR psz)
{
    VARIANT_BOOL result = 0;
    HRESULT hr = m_pDocument->loadXML(CComBSTR(psz), &result);
    if (FAILED(hr) || !result)
    {
        throw ComException(hr, L"Failed to load the document from string.");
    }

    hr = m_pDocument->get_documentElement(&m_pDocumentElement);
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to get document element.");
    }
}


void XmlDocument::Save(PCWSTR pszFileName)
{
    HRESULT hr = m_pDocument->save(CComVariant(pszFileName));
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to save the document to file: %s", pszFileName);
    }
}


void XmlDocument::Indent()
{
    Indent(m_pDocumentElement, 1);
}


static PCWSTR GetIndentation(int level)
{
    PCWSTR psz = String::Format(L"\r\n");
    while (level > 0)
    {
        psz = String::Format(L"%s  ", psz);
        level--;
    }
    return psz;
}


void  XmlDocument::Indent(MSXML2::IXMLDOMNode* pParent, int level)
{
    MSXML2::IXMLDOMNodeListPtr pChildren;
    HRESULT hr = pParent->get_childNodes(&pChildren);
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to get children.");
    }
    PCWSTR pszSP = nullptr;
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
        else if (type == MSXML2::DOMNodeType::NODE_ELEMENT)
        {
            if (!pszSP)
            {
                pszSP = GetIndentation(level);
            }
            InsertTextNodeBefore(pszSP, pChild, pParent);
            Indent(pChild, level + 1);
        }
    }
    if (pszSP)
    {
        AppendTextNode(GetIndentation(level - 1), pParent);
    }
}


MSXML2::IXMLDOMElement* XmlDocument::BuildDocumentRoot(PCWSTR pszName)
{
    MSXML2::IXMLDOMProcessingInstructionPtr pXML;
    HRESULT hr = m_pDocument->createProcessingInstruction(CComBSTR(L"xml"), CComBSTR(L"version=\"1.0\" encoding=\"UTF-8\""), &pXML);
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to create xml processing instruction.");
    }
    AppendChild(m_pDocument, pXML);

    hr = m_pDocument->createElement(CComBSTR(pszName), &m_pDocumentElement);
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to get document element.");
    }

    hr = m_pDocument->putref_documentElement(m_pDocumentElement);
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to put document element.");
    }

    return m_pDocumentElement;
}


MSXML2::IXMLDOMElementPtr XmlDocument::AppendElement(PCWSTR pszName, MSXML2::IXMLDOMNode* pParent)
{
    MSXML2::IXMLDOMElementPtr pElement;
    HRESULT hr = m_pDocument->createElement(CComBSTR(pszName), &pElement);
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to create element.");
    }

    AppendChild(pParent, pElement);

    return pElement;
}


MSXML2::IXMLDOMTextPtr XmlDocument::InsertTextNodeBefore(PCWSTR pszData, MSXML2::IXMLDOMNode* pChild, MSXML2::IXMLDOMNode* pParent)
{
    MSXML2::IXMLDOMTextPtr pTextNode;
    HRESULT hr = m_pDocument->createTextNode(CComBSTR(pszData), &pTextNode);
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to create text node.");
    }

    pParent->insertBefore(pTextNode, CComVariant(pChild), nullptr);
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to insert text node.");
    }

    return pTextNode;
}


MSXML2::IXMLDOMTextPtr XmlDocument::AppendTextNode(PCWSTR pszData, MSXML2::IXMLDOMNode* pParent)
{
    MSXML2::IXMLDOMTextPtr pTextNode;
    HRESULT hr = m_pDocument->createTextNode(CComBSTR(pszData), &pTextNode);
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to create text node.");
    }

    AppendChild(pParent, pTextNode);

    return pTextNode;
}


void XmlDocument::AppendChild(MSXML2::IXMLDOMNode* pParent, MSXML2::IXMLDOMNode* pChild)
{
    HRESULT hr = pParent->appendChild(pChild, nullptr);
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to append child.");
    }
}


bool XmlDocument::GetAttribute(MSXML2::IXMLDOMNode* pNode, PCWSTR pszName, PCWSTR& pszValue)
{
    MSXML2::IXMLDOMNamedNodeMapPtr pMap;
    HRESULT hr = pNode->get_attributes(&pMap);
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to get attributes.");
    }
    MSXML2::IXMLDOMNodePtr pItem;
    hr = pMap->getNamedItem(CComBSTR(pszName), &pItem);
    if (hr == S_OK)
    {
        CComVariant varValue;
        hr = pItem->get_nodeValue(&varValue);
        if (FAILED(hr))
        {
            throw ComException(hr, L"Failed to get node value.");
        }
        else if (varValue.vt == VT_BSTR)
        {
            pszValue = String::Copy(varValue.bstrVal);
            return true;
        }
        else
        {
            throw Exception(L"Failed to get %s attribute; Invalid type.", pszName);
        }
    }
    else if (hr == S_FALSE)
    {
        return false;
    }
    else
    {
        throw ComException(hr, L"Failed to get %s attribute.", pszName);
    }
}


bool XmlDocument::GetAttribute(MSXML2::IXMLDOMNode* pNode, PCWSTR pszName, long& lValue)
{
    MSXML2::IXMLDOMNamedNodeMapPtr pMap;
    HRESULT hr = pNode->get_attributes(&pMap);
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to get attributes.");
    }
    MSXML2::IXMLDOMNodePtr pItem;
    hr = pMap->getNamedItem(CComBSTR(pszName), &pItem);
    if (hr == S_OK)
    {
        CComVariant varValue;
        hr = pItem->get_nodeValue(&varValue);
        if (FAILED(hr))
        {
            throw ComException(hr, L"Failed to get node value.");
        }
        else if (varValue.vt == VT_BSTR)
        {
            lValue = wcstol(varValue.bstrVal, nullptr, 0);
            return true;
        }
        else if (varValue.vt == VT_I4)
        {
            lValue = varValue.lVal;
            return true;
        }
        else if (varValue.vt == VT_I8)
        {
            lValue = static_cast<long>(varValue.llVal);
            return true;
        }
        else if (varValue.vt == VT_I2)
        {
            lValue = static_cast<long>(varValue.iVal);
            return true;
        }
        else
        {
            throw Exception(L"Failed to get %s attribute; Invalid type.", pszName);
        }
    }
    else if (hr == S_FALSE)
    {
        return false;
    }
    else
    {
        throw ComException(hr, L"Failed to get %s attribute.", pszName);
    }
}


bool XmlDocument::GetAttribute(MSXML2::IXMLDOMNode* pNode, PCWSTR pszName, RectangleMetrics& value)
{
    PCWSTR psz;
    if (GetAttribute(pNode, pszName, psz))
    {
        return value.Parse(psz);
    }
    else
    {
        return false;
    }
}


bool XmlDocument::GetAttribute(MSXML2::IXMLDOMNode* pNode, PCWSTR pszName, bool& bValue)
{
    PCWSTR psz;
    if (GetAttribute(pNode, pszName, psz))
    {
        if (!String::CaseCompare(psz, L"true") || !String::CaseCompare(psz, L"t"))
        {
            bValue = true;
            return true;
        }
        else if (!String::CaseCompare(psz, L"false") || !String::CaseCompare(psz, L"f"))
        {
            bValue = false;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}


void XmlDocument::SetAttribute(MSXML2::IXMLDOMElement* pElement, PCWSTR pszName, PCWSTR pszValue)
{
    HRESULT hr = pElement->setAttribute(CComBSTR(pszName), CComVariant(CComBSTR(pszValue)));
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to set attribute %s.", pszName);
    }
}


void XmlDocument::SetAttribute(MSXML2::IXMLDOMElement* pElement, PCWSTR pszName, long lValue)
{
    WCHAR szValue[MAX_PATH];
    _snwprintf_s(szValue, _TRUNCATE, L"%ld", lValue);
    SetAttribute(pElement, pszName, szValue);
}


void XmlDocument::SetAttribute(MSXML2::IXMLDOMElement* pElement, PCWSTR pszName, const RectangleMetrics& value)
{
    WCHAR szValue[MAX_PATH];
    _snwprintf_s(szValue, _TRUNCATE, L"%ld,%ld,%ld,%ld", value.left, value.top, value.right, value.bottom);
    SetAttribute(pElement, pszName, szValue);
}


void XmlDocument::SetAttribute(MSXML2::IXMLDOMElement* pElement, PCWSTR pszName, bool bValue)
{
    SetAttribute(pElement, pszName, bValue ? L"true" : L"false");
}


PCWSTR XmlDocument::GetText(MSXML2::IXMLDOMNode* pNode)
{
    CComBSTR strValue;
    HRESULT hr = pNode->get_text(&strValue);
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to get text.");
    }
    return String::Copy(strValue);
}


void XmlDocument::SetText(MSXML2::IXMLDOMElement* pElement, PCWSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    HRESULT hr = pElement->put_text(CComBSTR(String::VaFormat(pszFormat, argList)));
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to set text.");
    }
    va_end(argList);
}


XmlElementLoader::XmlElementLoader()
    : m_map()
{
}


XmlElementLoader::~XmlElementLoader()
{
    for (Map::iterator iter = m_map.begin(); iter != m_map.end(); iter++)
    {
        delete iter->second;
        iter->second = nullptr;
    }
}


XmlElementLoader& XmlElementLoader::Add(PCWSTR pszName, XmlElementLoadAction* pAction)
{
    m_map.insert(Entry(pszName, pAction));
    return *this;
}


XmlElementLoader& XmlElementLoader::Load(MSXML2::IXMLDOMNode* pParent)
{
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
        Map::iterator iter = m_map.find(strName);
        if (iter != m_map.end())
        {
            iter->second->Invoke(pChild);
        }
    }
    return *this;
}
