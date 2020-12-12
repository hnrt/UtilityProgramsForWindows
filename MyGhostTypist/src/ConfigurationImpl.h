#pragma once


#include "Configuration.h"
#include "XmlDocument.h"


#define CFG_FILENAME L"configuration.xml"


namespace hnrt
{
    class ConfigurationImpl
        : public Configuration
        , public CredentialsCollectionCallback
        , public CredentialsCallback
        , public TargetCollectionCallback
        , public TargetCallback
    {
    public:

        ConfigurationImpl();
        ConfigurationImpl(const ConfigurationImpl& src) = delete;
        virtual ~ConfigurationImpl();
        void operator =(const ConfigurationImpl& src) = delete;
        virtual void Load();
        virtual void Save();
        virtual void OnCredentialsCollectionUpdate(CredentialsCollection&);
        virtual void OnCredentialsUpdate(Credentials&);
        virtual void OnTargetCollectionUpdate(TargetCollection&);
        virtual void OnTargetUpdate(Target&);

    private:

        void LoadUI(MSXML2::IXMLDOMNode* pNode);
        void LoadFont(MSXML2::IXMLDOMNode* pNode);
        void LoadButton(MSXML2::IXMLDOMNode* pNode);

        void LoadCredentialsList(MSXML2::IXMLDOMNode* pNode);
        void LoadCredentials(MSXML2::IXMLDOMNode* pNode);
        void LoadUsername(MSXML2::IXMLDOMNode* pNode);
        void LoadPassword(MSXML2::IXMLDOMNode* pNode);

        void LoadTargetList(MSXML2::IXMLDOMNode* pNode);
        void LoadTarget(MSXML2::IXMLDOMNode* pNode);
        void LoadFindWindow(MSXML2::IXMLDOMNode* pNode);
        void LoadTypeUsername(MSXML2::IXMLDOMNode* pNode);
        void LoadTypePassword(MSXML2::IXMLDOMNode* pNode);
        void LoadTypeDeleteSequence(MSXML2::IXMLDOMNode* pNode);
        void LoadType(MSXML2::IXMLDOMNode* pNode);

        void BuildUI(XmlDocument& document, MSXML2::IXMLDOMElement* pParent);
        void BuildCredentialsList(XmlDocument& document, MSXML2::IXMLDOMElement* pParent);
        void BuildCredentials(XmlDocument& document, MSXML2::IXMLDOMElement* pParent, size_t index);
        void BuildTargetList(XmlDocument& document, MSXML2::IXMLDOMElement* pParent);
        void BuildTarget(XmlDocument& document, MSXML2::IXMLDOMElement* pParent, size_t index);

        PCWSTR m_pszFileName;
        bool m_bSave;
        bool m_bLoading;
    };

    class ConfigurationElementLoadAction
        : public XmlElementLoadAction
    {
    public:

        ConfigurationElementLoadAction(ConfigurationImpl* pinst, void (ConfigurationImpl::* pfn)(MSXML2::IXMLDOMNode*));
        ConfigurationElementLoadAction(const ConfigurationElementLoadAction&) = delete;
        virtual ~ConfigurationElementLoadAction() = default;
        void operator =(const ConfigurationElementLoadAction&) = delete;
        virtual void Invoke(MSXML2::IXMLDOMNode* pNode);

    private:

        ConfigurationImpl* m_pinst;
        void (ConfigurationImpl::*m_pfn)(MSXML2::IXMLDOMNode*);
    };

    inline ConfigurationElementLoadAction::ConfigurationElementLoadAction(ConfigurationImpl* pinst, void (ConfigurationImpl::* pfn)(MSXML2::IXMLDOMNode*))
        : XmlElementLoadAction()
        , m_pinst(pinst)
        , m_pfn(pfn)
    {
    }

    inline void ConfigurationElementLoadAction::Invoke(MSXML2::IXMLDOMNode* pNode)
    {
        (m_pinst->*m_pfn)(pNode);
    }
}
