#include "ActionCollection.h"
#include "hnrt/Exception.h"


using namespace hnrt;


ActionCollection::ActionCollection()
    : m_vector()
{
}


ActionCollection::~ActionCollection()
{
}


const RefPtr<Action>& ActionCollection::operator [](ULONG index) const
{
    if (index < Count)
    {
        return m_vector[index];
    }
    else
    {
        throw Exception(L"ActionCollection::operator[]: Index out of range: %lu", index);
    }
}


RefPtr<Action>& ActionCollection::operator [](ULONG index)
{
    if (index < Count)
    {
        return m_vector[index];
    }
    else
    {
        throw Exception(L"ActionCollection::operator[]: Index out of range: %lu", index);
    }
}
