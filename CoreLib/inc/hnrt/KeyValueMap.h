#pragma once


#include <map>
#include <list>
#include "hnrt/String.h"


namespace hnrt
{
    class KeyValueMap
        : protected std::map<PCWSTR, PCWSTR, StringLessThan>
    {
    public:

        typedef std::map<PCWSTR, PCWSTR, StringLessThan> Base;
        typedef std::pair<PCWSTR, PCWSTR> Entry;
        typedef Base::const_iterator ConstIterator;
        typedef Base::iterator Iterator;

        KeyValueMap();
        KeyValueMap(const KeyValueMap&);
        virtual ~KeyValueMap() = default;
        KeyValueMap& operator =(const KeyValueMap&);
        size_t Size() const;
        KeyValueMap& Clear();
        KeyValueMap& Add(PCWSTR pszKey, PCWSTR pszValue);
        KeyValueMap& Remove(PCWSTR pszKey);
        PCWSTR GetValue(PCWSTR pszKey) const;
        ConstIterator Begin() const;
        ConstIterator End() const;
        Iterator Begin();
        Iterator End();
    };

    class KeyValueMapList
        : protected std::list<KeyValueMap*>
    {
    public:

        typedef std::list<KeyValueMap*> Base;
        typedef Base::const_iterator ConstIterator;
        typedef Base::iterator Iterator;

        KeyValueMapList();
        KeyValueMapList(const KeyValueMapList&);
        virtual ~KeyValueMapList();
        KeyValueMapList& operator =(const KeyValueMapList&);
        size_t Size() const;
        KeyValueMapList& Clear();
        KeyValueMap* Add();
        KeyValueMapList& Remove(KeyValueMap* pMap);
        KeyValueMap* GetMap(PCWSTR pszKey, PCWSTR pszValue) const;
        ConstIterator Begin() const;
        ConstIterator End() const;
        Iterator Begin();
        Iterator End();
    };
}
