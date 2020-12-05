#include "ConfigurationImpl.h"
#include "FindWindowTarget.h"
#include "Action.h"
#include "hnrt/Debug.h"
#include "hnrt/Path.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/ResourceString.h"
#include "hnrt/VirtualKey.h"
#include "hnrt/WhileInScope.h"
#include "resource.h"


using namespace hnrt;


RefPtr<Configuration> Configuration::Create()
{
    return RefPtr<Configuration>(new ConfigurationImpl());
}


static const WCHAR s_szDefaultConfiguration[] =
{
    L"<?xml version=\"1.0\"?>\r\n"
    L"<configuration>\r\n"
    L"  <ui width=\"400\" padding=\"0,10,0,5\">\r\n"
    L"    <font name=\"ƒƒCƒŠƒI\" size=\"12\"/>\r\n"
    L"    <button height=\"32\" margin=\"10,0,10,5\"/>\r\n"
    L"  </ui>\r\n"
    L"  <credentials-list>\r\n"
    L"    <credentials>\r\n"
    L"      <username></username>\r\n"
    L"      <password></password>\r\n"
    L"    </credentials>\r\n"
    L"  </credentials-list>\r\n"
    L"  <targets interval=\"10\" delay=\"1000\">\r\n"
    L"  </targets>\r\n"
    L"</configuration>\r\n"
};


ConfigurationImpl::ConfigurationImpl()
    : Configuration()
    , m_pszFileName(Path::Combine(m_pszAppDir, CFG_FILENAME))
    , m_bSave(false)
    , m_bLoading(false)
{
    DBGFNC(L"ConfigurationImpl::ctor");
    PCWSTR pszDir = Path::GetDirectoryName(m_pszFileName);
    if (!Path::Exists(pszDir))
    {
        throw Exception(L"\"%s\" doesn't exist.", pszDir);
    }
    m_TargetList.Callback = this;

}


ConfigurationImpl::~ConfigurationImpl()
{
    DBGFNC(L"ConfigurationImpl::dtor");
    if (m_bSave)
    {
        Save();
    }
}


void ConfigurationImpl::Load()
{
    DBGFNC(L"ConfigurationImpl::Load");
    WhileInScope<bool> wis(m_bLoading, true, false);

    XmlDocument document;
    if (Path::Exists(m_pszFileName))
    {
        document.Load(m_pszFileName);
    }
    else
    {
        document.LoadXML(s_szDefaultConfiguration);
        m_bSave = true;
    }

    XmlDocument::LoadChildren<ConfigurationImpl, void (ConfigurationImpl::*)(MSXML2::IXMLDOMNode*)>(document.DocumentElement, this,
        L"ui", &ConfigurationImpl::LoadUI,
        L"credentials-list", &ConfigurationImpl::LoadCredentialsList,
        L"target-list", &ConfigurationImpl::LoadTargetList,
        nullptr);

    if (m_bSave)
    {
        Save();
    }
}


void ConfigurationImpl::LoadUI(MSXML2::IXMLDOMNode* pNode)
{
    XmlDocument::GetAttribute(pNode, L"width", m_Window.Width);
    XmlDocument::GetAttribute(pNode, L"padding", m_Window.Padding);
    XmlDocument::LoadChildren<ConfigurationImpl, void (ConfigurationImpl::*)(MSXML2::IXMLDOMNode*)>(pNode, this,
        L"font", &ConfigurationImpl::LoadFont,
        L"button", &ConfigurationImpl::LoadButton,
        nullptr);
}


void ConfigurationImpl::LoadFont(MSXML2::IXMLDOMNode* pNode)
{
    XmlDocument::GetAttribute(pNode, L"name", m_Window.pszFontName);
    XmlDocument::GetAttribute(pNode, L"size", m_Window.FontSize);
}


void ConfigurationImpl::LoadButton(MSXML2::IXMLDOMNode* pNode)
{
    XmlDocument::GetAttribute(pNode, L"height", m_Button.Height);
    XmlDocument::GetAttribute(pNode, L"margin", m_Button.Margin);
}


void ConfigurationImpl::LoadCredentialsList(MSXML2::IXMLDOMNode* pNode)
{
    XmlDocument::LoadChildren<ConfigurationImpl, void (ConfigurationImpl::*)(MSXML2::IXMLDOMNode*)>(pNode, this,
        L"credentials", &ConfigurationImpl::LoadCredentials,
        nullptr);
}


void ConfigurationImpl::LoadCredentials(MSXML2::IXMLDOMNode* pNode)
{
    m_CredentialsList.Append(Credentials::Create());
    PCWSTR pszName;
    if (XmlDocument::GetAttribute(pNode, L"name", pszName))
    {
        m_CredentialsList[m_CredentialsList.Count - 1]->Key = pszName;
    }
    XmlDocument::LoadChildren<ConfigurationImpl, void (ConfigurationImpl::*)(MSXML2::IXMLDOMNode*)>(pNode, this,
        L"username", &ConfigurationImpl::LoadUsername,
        L"password", &ConfigurationImpl::LoadPassword,
        nullptr);
    m_CredentialsList[m_CredentialsList.Count - 1]->Callback = this;
}


void ConfigurationImpl::LoadUsername(MSXML2::IXMLDOMNode* pNode)
{
    m_CredentialsList[m_CredentialsList.Count - 1]->Username = XmlDocument::GetText(pNode);
}


void ConfigurationImpl::LoadPassword(MSXML2::IXMLDOMNode* pNode)
{
    bool bEncrypted = false;
    PCWSTR pszFormat;
    if (XmlDocument::GetAttribute(pNode, L"format", pszFormat))
    {
        if (!wcscmp(pszFormat, L"plaintext"))
        {
            bEncrypted = false;
        }
        else if (!wcscmp(pszFormat, L"encrypted"))
        {
            bEncrypted = true;
        }
        else
        {
            throw Exception(L"Invalid password format attribute value: %s", pszFormat);
        }
    }
    PCWSTR pszValue = XmlDocument::GetText(pNode);
    if (bEncrypted)
    {
        m_CredentialsList[m_CredentialsList.Count - 1]->EncryptedPassword = pszValue;
    }
    else
    {
        m_CredentialsList[m_CredentialsList.Count - 1]->Password = pszValue;
    }
}


void ConfigurationImpl::LoadTargetList(MSXML2::IXMLDOMNode* pNode)
{
    XmlDocument::GetAttribute(pNode, L"interval", m_TypingInterval);
    XmlDocument::GetAttribute(pNode, L"delay", m_TypingDelay);
    XmlDocument::LoadChildren<ConfigurationImpl, void(ConfigurationImpl::*)(MSXML2::IXMLDOMNode*)>(pNode, this,
        L"target", &ConfigurationImpl::LoadTarget,
        nullptr);
}


void ConfigurationImpl::LoadTarget(MSXML2::IXMLDOMNode* pNode)
{
    size_t index = m_TargetList.Count;
    PCWSTR pszName;
    XmlDocument::GetAttribute(pNode, L"name", pszName);
    bool bVisible = true;
    XmlDocument::GetAttribute(pNode, L"visible", bVisible);
    XmlDocument::LoadChildren<ConfigurationImpl, void(ConfigurationImpl::*)(MSXML2::IXMLDOMNode*)>(pNode, this,
        L"FindWindow", &ConfigurationImpl::LoadFindWindow,
        nullptr);
    if (index == m_TargetList.Count)
    {
        RefPtr<Target> pTarget = Target::CreateNull(pszName);
        pTarget->IsVisible = bVisible;
        pTarget->Callback = this;
        m_TargetList.Append(pTarget);
    }
    else
    {
        while (index < m_TargetList.Count)
        {
            m_TargetList[index]->Name = pszName;
            m_TargetList[index]->IsVisible = bVisible;
            m_TargetList[index]->Callback = this;
            index++;
        }
    }
}


void ConfigurationImpl::LoadFindWindow(MSXML2::IXMLDOMNode* pNode)
{
    PCWSTR pszClassName;
    XmlDocument::GetAttribute(pNode, L"class", pszClassName);
    PCWSTR pszWindowText;
    XmlDocument::GetAttribute(pNode, L"text", pszWindowText);
    RefPtr<Target> pTarget = Target::CreateFindWindow(nullptr, pszClassName, pszWindowText);
    m_TargetList.Append(pTarget);
    XmlDocument::LoadChildren<ConfigurationImpl, void(ConfigurationImpl::*)(MSXML2::IXMLDOMNode*)>(pNode, this,
        L"TypeUsername", &ConfigurationImpl::LoadTypeUsername,
        L"TypePassword", &ConfigurationImpl::LoadTypePassword,
        L"TypeDeleteSequence", &ConfigurationImpl::LoadTypeDeleteSequence,
        L"Type", &ConfigurationImpl::LoadType,
        nullptr);
}


void ConfigurationImpl::LoadTypeUsername(MSXML2::IXMLDOMNode* pNode)
{
    PCWSTR pszName;
    if (!XmlDocument::GetAttribute(pNode, L"name", pszName))
    {
        pszName = nullptr;
    }
    m_TargetList[m_TargetList.Count - 1]->ActionTargetPtr->Append(Action::TypeUsername(pszName));
}


void ConfigurationImpl::LoadTypePassword(MSXML2::IXMLDOMNode* pNode)
{
    PCWSTR pszName;
    if (!XmlDocument::GetAttribute(pNode, L"name", pszName))
    {
        pszName = nullptr;
    }
    m_TargetList[m_TargetList.Count - 1]->ActionTargetPtr->Append(Action::TypePassword(pszName));
}


void ConfigurationImpl::LoadTypeDeleteSequence(MSXML2::IXMLDOMNode* pNode)
{
    m_TargetList[m_TargetList.Count - 1]->ActionTargetPtr->Append(Action::TypeDeleteSequence());
}


void ConfigurationImpl::LoadType(MSXML2::IXMLDOMNode* pNode)
{
    PCWSTR pszKey;
    if (XmlDocument::GetAttribute(pNode, L"key", pszKey))
    {
        m_TargetList[m_TargetList.Count - 1]->ActionTargetPtr->Append(pszKey);
    }
    else
    {
        m_TargetList[m_TargetList.Count - 1]->ActionTargetPtr->Append(Action::TypeUnicode(XmlDocument::GetText(pNode)));
    }
}


void ConfigurationImpl::Save()
{
    DBGFNC(L"ConfigurationImpl::Save");

    m_bSave = false;

    XmlDocument document;
    MSXML2::IXMLDOMElement* pRoot = document.BuildDocumentRoot(L"configuration");
    BuildUI(document, pRoot);
    BuildCredentialsList(document, pRoot);
    BuildTargetList(document, pRoot);

    document.Indent();
    document.Save(m_pszFileName);
}


void ConfigurationImpl::BuildUI(XmlDocument& document, MSXML2::IXMLDOMElement* pParent)
{
    MSXML2::IXMLDOMElementPtr pUI = document.AppendElement(L"ui", pParent);
    XmlDocument::SetAttribute(pUI, L"width", m_Window.Width);
    XmlDocument::SetAttribute(pUI, L"padding", m_Window.Padding);
    MSXML2::IXMLDOMElementPtr pFont = document.AppendElement(L"font", pUI);
    XmlDocument::SetAttribute(pFont, L"name", m_Window.pszFontName);
    XmlDocument::SetAttribute(pFont, L"size", m_Window.FontSize);
    MSXML2::IXMLDOMElementPtr pButton = document.AppendElement(L"button", pUI);
    XmlDocument::SetAttribute(pButton, L"height", m_Button.Height);
    XmlDocument::SetAttribute(pButton, L"margin", m_Button.Margin);
}


void ConfigurationImpl::BuildCredentialsList(XmlDocument& document, MSXML2::IXMLDOMElement* pParent)
{
    MSXML2::IXMLDOMElementPtr pCC = document.AppendElement(L"credentials-list", pParent);
    for (size_t index = 0; index < m_CredentialsList.Count; index++)
    {
        BuildCredentials(document, pCC, index);
    }
}


void ConfigurationImpl::BuildCredentials(XmlDocument& document, MSXML2::IXMLDOMElement* pParent, size_t index)
{
    MSXML2::IXMLDOMElementPtr pC = document.AppendElement(L"credentials", pParent);
    if (m_CredentialsList[index]->Key)
    {
        XmlDocument::SetAttribute(pC, L"name", m_CredentialsList[index]->Key);
    }
    MSXML2::IXMLDOMElementPtr pUN = document.AppendElement(L"username", pC);
    XmlDocument::SetText(pUN, m_CredentialsList[index]->Username);
    MSXML2::IXMLDOMElementPtr pPW = document.AppendElement(L"password", pC);
    XmlDocument::SetAttribute(pPW, L"format", L"encrypted");
    XmlDocument::SetText(pPW, m_CredentialsList[index]->EncryptedPassword);
}


void ConfigurationImpl::BuildTargetList(XmlDocument& document, MSXML2::IXMLDOMElement* pParent)
{
    MSXML2::IXMLDOMElementPtr pTT = document.AppendElement(L"target-list", pParent);
    XmlDocument::SetAttribute(pTT, L"interval", m_TypingInterval);
    XmlDocument::SetAttribute(pTT, L"delay", m_TypingDelay);
    for (size_t index = 0; index < m_TargetList.Count; index++)
    {
        BuildTarget(document, pTT, index);
    }
}


void ConfigurationImpl::BuildTarget(XmlDocument& document, MSXML2::IXMLDOMElement* pParent, size_t index)
{
    RefPtr<Target> pTarget = m_TargetList[index];
    MSXML2::IXMLDOMElementPtr pT = document.AppendElement(L"target", pParent);
    XmlDocument::SetAttribute(pT, L"name", pTarget->Name);
    XmlDocument::SetAttribute(pT, L"visible", pTarget->IsVisible);
    if (pTarget->IsTypeFindWindow)
    {
        FindWindowTarget* pFindWindowTarget = pTarget->FindWindowTargetPtr;
        MSXML2::IXMLDOMElementPtr pFW = document.AppendElement(L"FindWindow", pT);
        XmlDocument::SetAttribute(pFW, L"class", pFindWindowTarget->ClassName);
        XmlDocument::SetAttribute(pFW, L"text", pFindWindowTarget->WindowText);
        for (ActionListConstIter iter = pFindWindowTarget->Begin(); iter != pFindWindowTarget->End(); iter++)
        {
            PCWSTR psz = *iter;
            switch (Action::GetClass(psz))
            {
            case AC_TYPEUSERNAME:
                if ((psz = Action::GetValue(psz)))
                {
                    MSXML2::IXMLDOMElementPtr pX = document.AppendElement(L"TypeUsername", pFW);
                    XmlDocument::SetAttribute(pX, L"name", psz);
                }
                else
                {
                    document.AppendElement(L"TypeUsername", pFW);
                }
                break;
            case AC_TYPEPASSWORD:
                if ((psz = Action::GetValue(psz)))
                {
                    MSXML2::IXMLDOMElementPtr pX = document.AppendElement(L"TypePassword", pFW);
                    XmlDocument::SetAttribute(pX, L"name", psz);
                }
                else
                {
                    document.AppendElement(L"TypePassword", pFW);
                }
                break;
            case AC_TYPEDELETESEQUENCE:
                document.AppendElement(L"TypeDeleteSequence", pFW);
                break;
            case AC_TYPEUNICODE:
            {
                psz = Action::GetValue(psz);
                MSXML2::IXMLDOMElementPtr pX = document.AppendElement(L"Type", pFW);
                XmlDocument::SetText(pX, L"%s", psz);
                break;
            }
            case AC_TYPEKEY:
            {
                MSXML2::IXMLDOMElementPtr pX = document.AppendElement(L"Type", pFW);
                XmlDocument::SetAttribute(pX, L"key", psz);
                break;
            }
            default:
                break;
            }
        }
    }
}


void ConfigurationImpl::OnCredentialsCollectionUpdate(CredentialsCollection& collection)
{
    for (size_t index = 0; index < collection.Count; index++)
    {
        collection[index]->Callback = this;
    }
    if (!m_bLoading)
    {
        m_bSave = true;
    }
}


void ConfigurationImpl::OnCredentialsUpdate(Credentials& credentials)
{
    if (!m_bLoading)
    {
        m_bSave = true;
    }
}


void ConfigurationImpl::OnTargetCollectionUpdate(TargetCollection& collection)
{
    for (size_t index = 0; index < collection.Count; index++)
    {
        collection[index]->Callback = this;
    }
    if (!m_bLoading)
    {
        m_bSave = true;
    }
}


void ConfigurationImpl::OnTargetUpdate(Target& target)
{
    if (!m_bLoading)
    {
        m_bSave = true;
    }
}
