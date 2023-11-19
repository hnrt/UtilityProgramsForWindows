#include "ConfigurationImpl.h"
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
    L"      <username>anonymous</username>\r\n"
    L"      <password>qwerty12345</password>\r\n"
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
    , m_Version(0)
    , m_pTarget()
{
    DBGFNC(L"ConfigurationImpl::ctor");
    PCWSTR pszDir = Path::GetDirectoryName(m_pszFileName);
    if (!Path::Exists(pszDir))
    {
        PCWSTR pszDir2 = Path::GetDirectoryName(pszDir);
        if (!Path::Exists(pszDir2))
        {
            PCWSTR pszDir3 = Path::GetDirectoryName(pszDir2);
            if (!Path::Exists(pszDir3))
            {
                throw Exception(L"\"%s\" doesn't exist.", pszDir);
            }
            if (!CreateDirectoryW(pszDir2, NULL))
            {
                throw Win32Exception(GetLastError(), L"Unable to create \"%s\".", pszDir2);
            }
        }
        if (!CreateDirectoryW(pszDir, NULL))
        {
            throw Win32Exception(GetLastError(), L"Unable to create \"%s\".", pszDir);
        }
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

    if (!XmlDocument::GetAttribute(document.DocumentElement, L"version", m_Version))
    {
        m_Version = 0;
    }

    XmlElementLoader()
        .Add(L"ui", new ConfigurationElementLoadAction(this, &ConfigurationImpl::LoadUI))
        .Add(L"credentials-list", new ConfigurationElementLoadAction(this, &ConfigurationImpl::LoadCredentialsList))
        .Add(L"target-list", new ConfigurationElementLoadAction(this, &ConfigurationImpl::LoadTargetList))
        .Load(document.DocumentElement);

    if (m_bSave)
    {
        Save();
    }
}


void ConfigurationImpl::LoadUI(MSXML2::IXMLDOMNode* pNode)
{
    XmlDocument::GetAttribute(pNode, L"width", m_Window.Width);
    XmlDocument::GetAttribute(pNode, L"padding", m_Window.Padding);
    XmlElementLoader()
        .Add(L"font", new ConfigurationElementLoadAction(this, &ConfigurationImpl::LoadFont))
        .Add(L"button", new ConfigurationElementLoadAction(this, &ConfigurationImpl::LoadButton))
        .Load(pNode);
}


void ConfigurationImpl::LoadFont(MSXML2::IXMLDOMNode* pNode)
{
    XmlDocument::GetAttribute(pNode, L"name", m_Window.szFontName);
    XmlDocument::GetAttribute(pNode, L"size", m_Window.FontSize);
}


void ConfigurationImpl::LoadButton(MSXML2::IXMLDOMNode* pNode)
{
    XmlDocument::GetAttribute(pNode, L"height", m_Button.Height);
    XmlDocument::GetAttribute(pNode, L"margin", m_Button.Margin);
}


void ConfigurationImpl::LoadCredentialsList(MSXML2::IXMLDOMNode* pNode)
{
    XmlElementLoader()
        .Add(L"credentials", new ConfigurationElementLoadAction(this, &ConfigurationImpl::LoadCredentials))
        .Load(pNode);
}


void ConfigurationImpl::LoadCredentials(MSXML2::IXMLDOMNode* pNode)
{
    m_CredentialsList.Append(Credentials::Create());
    String szName;
    if (XmlDocument::GetAttribute(pNode, L"name", szName))
    {
        m_CredentialsList[m_CredentialsList.Count - 1]->Key = szName;
    }
    XmlElementLoader()
        .Add(L"username", new ConfigurationElementLoadAction(this, &ConfigurationImpl::LoadUsername))
        .Add(L"password", new ConfigurationElementLoadAction(this, &ConfigurationImpl::LoadPassword))
        .Load(pNode);
    m_CredentialsList[m_CredentialsList.Count - 1]->Callback = this;
}


void ConfigurationImpl::LoadUsername(MSXML2::IXMLDOMNode* pNode)
{
    m_CredentialsList[m_CredentialsList.Count - 1]->Username = XmlDocument::GetText(pNode);
}


void ConfigurationImpl::LoadPassword(MSXML2::IXMLDOMNode* pNode)
{
    bool bEncrypted = false;
    String szFormat;
    if (XmlDocument::GetAttribute(pNode, L"format", szFormat))
    {
        if (!String::Compare(szFormat, L"plaintext"))
        {
            bEncrypted = false;
        }
        else if (!String::Compare(szFormat, L"encrypted"))
        {
            bEncrypted = true;
        }
        else
        {
            throw Exception(L"Invalid password format attribute value: %s", szFormat.Ptr);
        }
    }
    PCWSTR pszValue = XmlDocument::GetText(pNode);
    if (bEncrypted)
    {
        if (m_Version < 2L)
        {
            RefPtr<Credentials> oldCred = Credentials::Create(m_Version);
            oldCred->EncryptedPassword = pszValue;
            m_CredentialsList[m_CredentialsList.Count - 1]->Password = oldCred->Password;
        }
        else
        {
            m_CredentialsList[m_CredentialsList.Count - 1]->EncryptedPassword = pszValue;
        }
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
    XmlElementLoader()
        .Add(L"target", new ConfigurationElementLoadAction(this, &ConfigurationImpl::LoadTarget))
        .Load(pNode);
}


void ConfigurationImpl::LoadTarget(MSXML2::IXMLDOMNode* pNode)
{
    String szName;
    XmlDocument::GetAttribute(pNode, L"name", szName);
    bool bVisible = true;
    XmlDocument::GetAttribute(pNode, L"visible", bVisible);
    m_pTarget = Target::Create(szName, bVisible);
    m_TargetList.Append(m_pTarget);
    bool bBlockKeybd;
    if (XmlDocument::GetAttribute(pNode, L"block_keybd", bBlockKeybd))
    {
        m_pTarget->BlockKeybd = bBlockKeybd;
    }
    bool bBlockMouse;
    if (XmlDocument::GetAttribute(pNode, L"block_mouse", bBlockMouse))
    {
        m_pTarget->BlockMouse = bBlockMouse;
    }
    if (m_Version == 0)
    {
        XmlElementLoader()
            .Add(L"FindWindow", new ConfigurationElementLoadAction(this, &ConfigurationImpl::LoadFindWindowV0))
            .Load(pNode);
    }
    else
    {
        XmlElementLoader()
            .Add(L"SetForegroundWindow", new ConfigurationElementLoadAction(this, &ConfigurationImpl::LoadSetForegroundWindow))
            .Add(L"TypeUsername", new ConfigurationElementLoadAction(this, &ConfigurationImpl::LoadTypeUsername))
            .Add(L"TypePassword", new ConfigurationElementLoadAction(this, &ConfigurationImpl::LoadTypePassword))
            .Add(L"TypeDeleteSequence", new ConfigurationElementLoadAction(this, &ConfigurationImpl::LoadTypeDeleteSequence))
            .Add(L"Type", new ConfigurationElementLoadAction(this, &ConfigurationImpl::LoadType))
            .Add(L"LeftClick", new ConfigurationElementLoadAction(this, &ConfigurationImpl::LoadLeftClick))
            .Load(pNode);
    }
}


void ConfigurationImpl::LoadFindWindowV0(MSXML2::IXMLDOMNode* pNode)
{
    String szClassName;
    XmlDocument::GetAttribute(pNode, L"class", szClassName);
    String szWindowText;
    XmlDocument::GetAttribute(pNode, L"text", szWindowText);
    RefPtr<Action> pAction;
    static const WCHAR pszSeparator[] = { L"@@@###@@@" };
    static size_t cchSp = wcslen(pszSeparator);
    PCWSTR pCnSp = wcsstr(szClassName, pszSeparator);
    PCWSTR pWtSp = wcsstr(szWindowText, pszSeparator);
    if (pCnSp && pWtSp)
    {
        String szCn(szClassName, pCnSp - szClassName.Ptr);
        String szWt(szWindowText, pWtSp - szWindowText.Ptr);
        pAction = Action::SetForegroundWindow(szCn, szWt);
        dynamic_cast<SetForegroundWindowAction*>(pAction.Ptr)->Append(pCnSp + cchSp, pWtSp + cchSp);
    }
    else
    {
        pAction = Action::SetForegroundWindow(szClassName, szWindowText);
    }
    m_pTarget->Append(pAction);
    XmlElementLoader()
        .Add(L"TypeUsername", new ConfigurationElementLoadAction(this, &ConfigurationImpl::LoadTypeUsername))
        .Add(L"TypePassword", new ConfigurationElementLoadAction(this, &ConfigurationImpl::LoadTypePassword))
        .Add(L"TypeDeleteSequence", new ConfigurationElementLoadAction(this, &ConfigurationImpl::LoadTypeDeleteSequence))
        .Add(L"Type", new ConfigurationElementLoadAction(this, &ConfigurationImpl::LoadType))
        .Load(pNode);
}


void ConfigurationImpl::LoadSetForegroundWindow(MSXML2::IXMLDOMNode* pNode)
{
    String szClassName;
    XmlDocument::GetAttribute(pNode, L"class", szClassName);
    String szWindowText;
    XmlDocument::GetAttribute(pNode, L"text", szWindowText);
    auto pAction = Action::SetForegroundWindow(szClassName, szWindowText);
    m_pTarget->Append(pAction);
    String szAccName;
    if (XmlDocument::GetAttribute(pNode, L"accName", szAccName))
    {
        dynamic_cast<SetForegroundWindowAction*>(pAction.Ptr)->SetActiveAccessibility(szAccName.Ptr);
    }
    LONG accRole;
    if (XmlDocument::GetAttribute(pNode, L"accRole", accRole))
    {
        dynamic_cast<SetForegroundWindowAction*>(pAction.Ptr)->SetActiveAccessibility(accRole);
    }
    XmlElementLoader()
        .Add(L"FindWindow", new ConfigurationElementLoadAction(this, &ConfigurationImpl::LoadFindWindow))
        .Load(pNode);
}


void ConfigurationImpl::LoadFindWindow(MSXML2::IXMLDOMNode* pNode)
{
    String szClassName;
    XmlDocument::GetAttribute(pNode, L"class", szClassName);
    String szWindowText;
    XmlDocument::GetAttribute(pNode, L"text", szWindowText);
    dynamic_cast<SetForegroundWindowAction*>((*m_pTarget.Ptr)[m_pTarget->Count - 1].Ptr)->Append(szClassName, szWindowText);
    XmlElementLoader()
        .Add(L"FindWindow", new ConfigurationElementLoadAction(this, &ConfigurationImpl::LoadFindWindow))
        .Load(pNode);
}


void ConfigurationImpl::LoadTypeUsername(MSXML2::IXMLDOMNode* pNode)
{
    String szName;
    if (!XmlDocument::GetAttribute(pNode, L"name", szName))
    {
        szName = nullptr;
    }
    auto pAction = Action::TypeUsername(szName);
    bool bAA;
    if (XmlDocument::GetAttribute(pNode, L"aa", bAA))
    {
        if (bAA)
        {
            pAction->Flags = pAction->Flags | AC_FLAG_AA;
        }
    }
    m_pTarget->Append(pAction);
}


void ConfigurationImpl::LoadTypePassword(MSXML2::IXMLDOMNode* pNode)
{
    String szName;
    if (!XmlDocument::GetAttribute(pNode, L"name", szName))
    {
        szName = nullptr;
    }
    auto pAction = Action::TypePassword(szName);
    bool bAA;
    if (XmlDocument::GetAttribute(pNode, L"aa", bAA))
    {
        if (bAA)
        {
            pAction->Flags = pAction->Flags | AC_FLAG_AA;
        }
    }
    m_pTarget->Append(pAction);
}


void ConfigurationImpl::LoadTypeDeleteSequence(MSXML2::IXMLDOMNode* pNode)
{
    m_pTarget->Append(Action::TypeDeleteSequence());
}


void ConfigurationImpl::LoadType(MSXML2::IXMLDOMNode* pNode)
{
    RefPtr<Action> pAction;
    String szKey;
    if (XmlDocument::GetAttribute(pNode, L"key", szKey))
    {
        pAction = Action::TypeKey(szKey);
    }
    else
    {        
        pAction = Action::TypeUnicode(XmlDocument::GetText(pNode));
    }
    bool bAA;
    if (XmlDocument::GetAttribute(pNode, L"aa", bAA))
    {
        if (bAA)
        {
            pAction->Flags = pAction->Flags | AC_FLAG_AA;
        }
    }
    m_pTarget->Append(pAction);
}


void ConfigurationImpl::LoadLeftClick(MSXML2::IXMLDOMNode* pNode)
{
    auto pAction = Action::LeftClick();
    bool bAA;
    if (XmlDocument::GetAttribute(pNode, L"aa", bAA))
    {
        if (bAA)
        {
            pAction->Flags = pAction->Flags | AC_FLAG_AA;
        }
    }
    m_pTarget->Append(pAction);
}


void ConfigurationImpl::Save()
{
    DBGFNC(L"ConfigurationImpl::Save");

    m_bSave = false;

    XmlDocument document;
    MSXML2::IXMLDOMElement* pRoot = document.BuildDocumentRoot(L"configuration");
    XmlDocument::SetAttribute(pRoot, L"version", 2L);
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
    XmlDocument::SetAttribute(pFont, L"name", m_Window.szFontName.Ptr);
    XmlDocument::SetAttribute(pFont, L"size", m_Window.FontSize);
    MSXML2::IXMLDOMElementPtr pButton = document.AppendElement(L"button", pUI);
    XmlDocument::SetAttribute(pButton, L"height", m_Button.Height);
    XmlDocument::SetAttribute(pButton, L"margin", m_Button.Margin);
}


void ConfigurationImpl::BuildCredentialsList(XmlDocument& document, MSXML2::IXMLDOMElement* pParent)
{
    MSXML2::IXMLDOMElementPtr pCC = document.AppendElement(L"credentials-list", pParent);
    for (ULONG index = 0; index < m_CredentialsList.Count; index++)
    {
        BuildCredentials(document, pCC, index);
    }
}


void ConfigurationImpl::BuildCredentials(XmlDocument& document, MSXML2::IXMLDOMElement* pParent, ULONG index)
{
    MSXML2::IXMLDOMElementPtr pC = document.AppendElement(L"credentials", pParent);
    if (m_CredentialsList[index]->Key.Len > 0)
    {
        XmlDocument::SetAttribute(pC, L"name", m_CredentialsList[index]->Key.Ptr);
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
    for (ULONG index = 0; index < m_TargetList.Count; index++)
    {
        BuildTarget(document, pTT, index);
    }
}


void ConfigurationImpl::BuildTarget(XmlDocument& document, MSXML2::IXMLDOMElement* pParent, ULONG index)
{
    m_pTarget = m_TargetList[index];
    MSXML2::IXMLDOMElementPtr pT = document.AppendElement(L"target", pParent);
    XmlDocument::SetAttribute(pT, L"name", m_pTarget->Name.Ptr);
    XmlDocument::SetAttribute(pT, L"visible", m_pTarget->IsVisible);
    if (m_pTarget->BlockKeybd)
    {
        XmlDocument::SetAttribute(pT, L"block_keybd", true);
    }
    if (m_pTarget->BlockMouse)
    {
        XmlDocument::SetAttribute(pT, L"block_mouse", true);
    }
    for (ULONG index = 0; index < m_pTarget->Count; index++)
    {
        RefPtr<Action> pAction = (*m_pTarget.Ptr)[index];
        switch (pAction->Type)
        {
        case AC_SETFOREGROUNDWINDOW:
        {
            auto pX = document.AppendElement(L"SetForegroundWindow", pT);
            auto pActionX = dynamic_cast<SetForegroundWindowAction*>(pAction.Ptr);
            XmlDocument::SetAttribute(pX, L"class", pActionX->ClassName);
            XmlDocument::SetAttribute(pX, L"text", pActionX->WindowText);
            if (pActionX->AccName)
            {
                XmlDocument::SetAttribute(pX, L"accName", pActionX->AccName);
            }
            if (pActionX->AccRole > 0)
            {
                XmlDocument::SetAttribute(pX, L"accRole", pActionX->AccRole);
            }
            for (SetForegroundWindowAction::ConstIter iter = pActionX->Begin; iter != pActionX->End; iter++)
            {
                pX = document.AppendElement(L"FindWindow", pX);
                XmlDocument::SetAttribute(pX, L"class", iter->first.Ptr);
                XmlDocument::SetAttribute(pX, L"text", iter->second.Ptr);
            }
            break;
        }
        case AC_TYPEKEY:
        {
            auto pX = document.AppendElement(L"Type", pT);
            XmlDocument::SetAttribute(pX, L"key", dynamic_cast<TypeKeyAction*>(pAction.Ptr)->Key);
            if ((pAction->Flags & AC_FLAG_AA))
            {
                XmlDocument::SetAttribute(pX, L"aa", true);
            }
            break;
        }
        case AC_TYPEUNICODE:
        {
            auto pX = document.AppendElement(L"Type", pT);
            if ((pAction->Flags & AC_FLAG_AA))
            {
                XmlDocument::SetAttribute(pX, L"aa", true);
            }
            XmlDocument::SetText(pX, L"%s", dynamic_cast<TypeUnicodeAction*>(pAction.Ptr)->Text);
            break;
        }
        case AC_TYPEUSERNAME:
        {
            auto pX = document.AppendElement(L"TypeUsername", pT);
            auto pszName = dynamic_cast<TypeUsernameAction*>(pAction.Ptr)->Name;
            if (pszName && pszName[0])
            {
                XmlDocument::SetAttribute(pX, L"name", pszName);
            }
            if ((pAction->Flags & AC_FLAG_AA))
            {
                XmlDocument::SetAttribute(pX, L"aa", true);
            }
            break;
        }
        case AC_TYPEPASSWORD:
        {
            auto pX = document.AppendElement(L"TypePassword", pT);
            auto pszName = dynamic_cast<TypePasswordAction*>(pAction.Ptr)->Name;
            if (pszName && pszName[0])
            {
                XmlDocument::SetAttribute(pX, L"name", pszName);
            }
            if ((pAction->Flags & AC_FLAG_AA))
            {
                XmlDocument::SetAttribute(pX, L"aa", true);
            }
            break;
        }
        case AC_TYPEDELETESEQUENCE:
            document.AppendElement(L"TypeDeleteSequence", pT);
            break;
        case AC_LEFTCLICK:
        {
            auto pX = document.AppendElement(L"LeftClick", pT);
            if ((pAction->Flags & AC_FLAG_AA))
            {
                XmlDocument::SetAttribute(pX, L"aa", true);
            }
            break;
        }
        default:
            break;
        }
    }
}


void ConfigurationImpl::OnCredentialsCollectionUpdate(CredentialsCollection& collection)
{
    for (ULONG index = 0; index < collection.Count; index++)
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
    for (ULONG index = 0; index < collection.Count; index++)
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
