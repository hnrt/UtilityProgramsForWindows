#include "pch.h"
#include "hnrt/KeyValueMap.h"


using namespace hnrt;


////////////////////////////////////////////////////////////////////////////
////
////  K E Y V A L U E M A P
////
////////////////////////////////////////////////////////////////////////////


KeyValueMap::KeyValueMap()
    : Base()
{
}


KeyValueMap::KeyValueMap(const KeyValueMap& src)
    : Base(src)
{
}


KeyValueMap& KeyValueMap::operator =(const KeyValueMap& src)
{
    Base::operator =(src);
    return *this;
}


size_t KeyValueMap::Size() const
{
    return Base::size();
}


KeyValueMap& KeyValueMap::Clear()
{
    Base::clear();
    return *this;
}


KeyValueMap& KeyValueMap::Add(PCWSTR pszKey, PCWSTR pszValue)
{
    Base::insert(Entry(String(pszKey), String(pszValue)));
    return *this;
}


KeyValueMap& KeyValueMap::Remove(PCWSTR pszKey)
{
    Base::erase(String(pszKey));
    return *this;
}


PCWSTR KeyValueMap::GetValue(PCWSTR pszKey) const
{
    ConstIterator iter = Base::find(String(pszKey));
    return iter != Base::end() ? iter->second : nullptr;
}


KeyValueMap::ConstIterator KeyValueMap::Begin() const
{
    return Base::begin();
}


KeyValueMap::ConstIterator KeyValueMap::End() const
{
    return Base::end();
}


KeyValueMap::Iterator KeyValueMap::Begin()
{
    return Base::begin();
}


KeyValueMap::Iterator KeyValueMap::End()
{
    return Base::end();
}


////////////////////////////////////////////////////////////////////////////
////
////  K E Y V A L U E M A P L I S T
////
////////////////////////////////////////////////////////////////////////////


KeyValueMapList::KeyValueMapList()
    : Base()
{
}


KeyValueMapList::KeyValueMapList(const KeyValueMapList& src)
    : Base()
{
    operator =(src);
}


KeyValueMapList::~KeyValueMapList()
{
    Clear();
}


KeyValueMapList& KeyValueMapList::operator =(const KeyValueMapList& src)
{
    for (ConstIterator iter = src.begin(); iter != src.end(); iter++)
    {
        Base::push_back(new KeyValueMap(**iter));
    }
    return *this;
}


size_t KeyValueMapList::Size() const
{
    return Base::size();
}


KeyValueMapList& KeyValueMapList::Clear()
{
    while (Base::size())
    {
        Iterator iter = Base::begin();
        delete *iter;
        Base::erase(iter);
    }
    return *this;
}


KeyValueMap* KeyValueMapList::Add()
{
    KeyValueMap* pMap = new KeyValueMap();
    Base::push_back(pMap);
    return pMap;
}


KeyValueMapList& KeyValueMapList::Remove(KeyValueMap* pMap)
{
    for (Iterator iter = Base::begin(); iter != Base::end(); iter++)
    {
        if (*iter == pMap)
        {
            Base::erase(iter);
            break;
        }
    }
    delete pMap;
    return *this;
}


KeyValueMap* KeyValueMapList::GetMap(PCWSTR pszKey, PCWSTR pszValue) const
{
    for (ConstIterator iter = Base::begin(); iter != Base::end(); iter++)
    {
        KeyValueMap* pMap = *iter;
        PCWSTR pszValue2 = pMap->GetValue(pszKey);
        if (pszValue2 && !String::Compare(pszValue2, pszValue))
        {
            return pMap;
        }
    }
    return nullptr;
}


KeyValueMapList::ConstIterator KeyValueMapList::Begin() const
{
    return Base::begin();
}


KeyValueMapList::ConstIterator KeyValueMapList::End() const
{
    return Base::end();
}


KeyValueMapList::Iterator KeyValueMapList::Begin()
{
    return Base::begin();
}


KeyValueMapList::Iterator KeyValueMapList::End()
{
    return Base::end();
}
