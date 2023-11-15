#pragma once


#include <Windows.h>
#include <list>


namespace hnrt
{
    enum class HostsNodeType
    {
        Root,
        End,
        Space,
        Comment,
        LineBreak,
        Address,
        Hostname,
        Error
    };

    class HostsNode
    {
    public:

        HostsNode(HostsNodeType type, const char* psz = nullptr);
        HostsNode(const HostsNode&) = delete;
        ~HostsNode();
        HostsNode& operator = (const HostsNode&) = delete;
        void SetText(PCWSTR psz);
        HostsNodeType get_Type() const;
        const char* get_Text() const;
        HostsNode* get_Next() const;
        void set_Next(HostsNode* pNode);
        HostsNode* get_Host() const;
        void set_Host(HostsNode* pNode);

        __declspec(property(get = get_Type)) HostsNodeType Type;
        __declspec(property(get = get_Text)) const char* Text;
        __declspec(property(get = get_Next, put = set_Next)) HostsNode* Next;
        __declspec(property(get = get_Host, put = set_Host)) HostsNode* Host;

    private:

        HostsNodeType m_type;
        char* m_psz;
        HostsNode* m_pNext;
        HostsNode* m_pHost;
    };

    class Hosts
    {
    public:

        Hosts(const void* ptr, size_t len);
        Hosts(const Hosts&) = delete;
        ~Hosts();
        Hosts& operator = (const Hosts&) = delete;
        std::list<HostsNode*>& get_List();
        HostsNode* get_Root() const;

        __declspec(property(get = get_List)) std::list<HostsNode*>& List;
        __declspec(property(get = get_Root)) HostsNode* Root;

    private:

        HostsNode* Read();
        int ReadChar();

        std::list<HostsNode*> m_list;
        HostsNode m_root;
        HostsNode* m_pLast;
        const char* m_pCur;
        const char* m_pEnd;
        int m_c;
        int m_t;
    };
}
