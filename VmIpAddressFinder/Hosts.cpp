#include <string.h>
#include <cctype>
#include <list>
#include <Windows.h>
#include "Hosts.h"
#include "hnrt/MultibyteString.h"


using namespace hnrt;


Hosts::Hosts(const void* ptr, size_t len)
    : m_list()
    , m_root(HostsNodeType::Root)
    , m_pLast(&m_root)
    , m_pCur(reinterpret_cast<const char*>(ptr))
    , m_pEnd(m_pCur + len)
    , m_c(-1)
    , m_t(0)
{
    ReadChar();
    HostsNode* pNode = Read();
    while (pNode->Type != HostsNodeType::End)
    {
        while (pNode->Type == HostsNodeType::Space)
        {
            pNode = Read();
        }
        if (pNode->Type == HostsNodeType::Address)
        {
            HostsNode* pAddr = pNode;
            HostsNode* pHost = pNode;
            pNode = Read();
            while (pNode->Type == HostsNodeType::Space)
            {
                pNode = Read();
            }
            if (pNode->Type == HostsNodeType::Hostname)
            {
                pHost->Host = pNode;
                pHost = pNode;
                pNode = Read();
                while (pNode->Type == HostsNodeType::Space)
                {
                    pNode = Read();
                }
                while (pNode->Type == HostsNodeType::Hostname)
                {
                    pHost->Host = pNode;
                    pHost = pNode;
                    pNode = Read();
                    while (pNode->Type == HostsNodeType::Space)
                    {
                        pNode = Read();
                    }
                }
                m_list.push_back(pAddr);
            }
        }
        while (pNode->Type == HostsNodeType::Comment)
        {
            pNode = Read();
        }
        while (pNode->Type != HostsNodeType::LineBreak)
        {
            if (pNode->Type == HostsNodeType::End)
            {
                return;
            }
            pNode = Read();
        }
        pNode = Read();
    }
}


Hosts::~Hosts()
{
    HostsNode* pCur = m_root.Next;
    while (pCur)
    {
        HostsNode* pNext = pCur->Next;
        delete pCur;
        pCur = pNext;
    }
}


std::list<HostsNode*>& Hosts::get_List()
{
    return m_list;
}


HostsNode* Hosts::get_Root() const
{
    return m_root.Next;
}


HostsNode* Hosts::Read()
{
    HostsNode* pNode;
    char text[512] = { 0 };
    char* p = text;
    char* q = text + sizeof(text) - 1;
    if (m_c == -1)
    {
        pNode = new HostsNode(HostsNodeType::End);
        m_pLast->Next = pNode;
        return pNode;
    }
    else if (m_c == '\r')
    {
        *p++ = m_c;
        ReadChar();
        if (m_c == '\n')
        {
            *p++ = m_c;
            ReadChar();
        }
        *p = '\0';
        m_t = 0;
        pNode = new HostsNode(HostsNodeType::LineBreak, text);
    }
    else if (m_c == '\n')
    {
        *p++ = m_c;
        ReadChar();
        *p = '\0';
        m_t = 0;
        pNode = new HostsNode(HostsNodeType::LineBreak, text);
    }
    else if (m_pLast->Type == HostsNodeType::Error || m_pLast->Type == HostsNodeType::Comment)
    {
        *p++ = m_c;
        ReadChar();
        while (p < q && m_c != -1 && m_c != '\r' && m_c != '\n')
        {
            *p++ = m_c;
            ReadChar();
        }
        *p = '\0';
        pNode = new HostsNode(m_pLast->Type, text);
    }
    else if (m_c == '#')
    {
        *p++ = m_c;
        ReadChar();
        while (p < q && m_c != -1 && m_c != '\r' && m_c != '\n')
        {
            *p++ = m_c;
            ReadChar();
        }
        *p = '\0';
        pNode = new HostsNode(HostsNodeType::Comment, text);
    }
    else if (m_c == ' ' || m_c == '\t')
    {
        *p++ = m_c;
        ReadChar();
        while (p < q && m_c == ' ' || m_c == '\t')
        {
            *p++ = m_c;
            ReadChar();
        }
        *p = '\0';
        pNode = new HostsNode(HostsNodeType::Space, text);
    }
    else if (m_t == 0 && (isdigit(m_c) || strchr(":ABCDEFabcdef", m_c)))
    {
        *p++ = m_c;
        ReadChar();
        while (p < q && isdigit(m_c) || m_c == '.' || strchr(":ABCDEFabcdef", m_c))
        {
            *p++ = m_c;
            ReadChar();
        }
        *p = '\0';
        m_t++;
        pNode = new HostsNode(HostsNodeType::Address, text);
    }
    else if (isalpha(m_c))
    {
        *p++ = m_c;
        ReadChar();
        while (p < q && isalnum(m_c) || m_c == '-' || m_c == '.')
        {
            *p++ = m_c;
            ReadChar();
        }
        *p = '\0';
        m_t++;
        pNode = new HostsNode(HostsNodeType::Hostname, text);
    }
    else
    {
        *p++ = m_c;
        ReadChar();
        while (p < q && m_c != -1 && m_c != '\r' && m_c != '\n')
        {
            *p++ = m_c;
            ReadChar();
        }
        *p = '\0';
        pNode = new HostsNode(HostsNodeType::Error, text);
    }
    m_pLast->Next = pNode;
    m_pLast = pNode;
    return pNode;
}


int Hosts::ReadChar()
{
    if (m_pCur < m_pEnd)
    {
        m_c = *m_pCur++ & 0xFF;
    }
    else
    {
        m_c = -1;
    }
    return m_c;
}


HostsNode::HostsNode(HostsNodeType type, const char* psz)
    : m_type(type)
    , m_psz(psz)
    , m_pNext(nullptr)
    , m_pHost(nullptr)
{
}


void HostsNode::SetText(PCWSTR psz)
{
    m_psz = MultibyteString(CP_ACP, psz);
}


HostsNodeType HostsNode::get_Type() const
{
    return m_type;
}


const char* HostsNode::get_Text() const
{
    return m_psz;
}


HostsNode* HostsNode::get_Next() const
{
    return m_pNext;
}


void HostsNode::set_Next(HostsNode* pNode)
{
    m_pNext = pNode;
}


HostsNode* HostsNode::get_Host() const
{
    return m_pHost;
}


void HostsNode::set_Host(HostsNode* pNode)
{
    m_pHost = pNode;
}
