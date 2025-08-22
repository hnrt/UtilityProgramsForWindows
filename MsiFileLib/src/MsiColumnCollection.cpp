#include "pch.h"
#include "hnrt/String.h"
#include "hnrt/MsiException.h"
#include "MsiColumnCollection.h"
#include "MsiColumnCollectionInitializer.h"
#include "MsiUtils.h"


using namespace hnrt;


MsiColumnCollection::MsiColumnCollection()
    : m_count(0)
    , m_columns(nullptr)
    , m_szPrimaryKey()
    , m_keyList()
    , m_keyCount(0)
    , m_next(0)
{
}


MsiColumnCollection::MsiColumnCollection(MSIHANDLE hDatabase, PCWSTR pszTableName)
    : m_count(0)
    , m_columns(nullptr)
    , m_szPrimaryKey()
    , m_keyList()
    , m_keyCount(0)
    , m_next(0)
{
    static MsiColumnCollectionInitializer initializer;
    initializer.Run(hDatabase, pszTableName, m_columns, m_count);
    for (long colNo = 0; colNo < m_count && m_keyCount < _countof(m_keyList); colNo++)
    {
        if (m_columns[colNo].Key)
        {
            if (m_szPrimaryKey)
            {
                m_szPrimaryKey += String::Format(L";%s", m_columns[colNo].Name);
            }
            else
            {
                m_szPrimaryKey = m_columns[colNo].Name;
            }
            m_keyList[m_keyCount++] = colNo;
        }
    }
    for (long keyNo = m_keyCount; keyNo < _countof(m_keyList); keyNo++)
    {
        m_keyList[keyNo] = -1L;
    }
}


MsiColumnCollection::~MsiColumnCollection()
{
    free(m_columns);
}


long MsiColumnCollection::get_Count() const
{
    return m_count;
}


PCWSTR MsiColumnCollection::get_PrimaryKey() const
{ 
    return m_szPrimaryKey;
}


long MsiColumnCollection::GetKeyIndex(long keyNo) const
{
    if (keyNo < 0 || m_keyCount <= keyNo)
    {
        throw Exception(L"MsiColumnCollection::GetKeyIndex: Index out of range.");
    }
    return m_keyList[keyNo];
}


long MsiColumnCollection::get_KeyCount() const
{
    return m_keyCount;
}


const IMsiColumn& MsiColumnCollection::operator[](long colNo) const
{
    if (colNo < 0 || m_count <= colNo)
    {
        throw Exception(L"MsiColumnCollection::operator[]: Index out of range.");
    }
    return m_columns[colNo];
}


IMsiColumn& MsiColumnCollection::operator[](long colNo)
{
    if (colNo < 0 || m_count <= colNo)
    {
        throw Exception(L"MsiColumnCollection::operator[]: Index out of range.");
    }
    return m_columns[colNo];
}


const IMsiColumn& MsiColumnCollection::operator[](PCWSTR colName) const
{
    long colNo = IndexOf(colName);
    if (colNo < 0)
    {
        throw Exception(L"MsiColumnCollection::operator[]: Name not found.");
    }
    return m_columns[colNo];
}


IMsiColumn& MsiColumnCollection::operator[](PCWSTR colName)
{
    long colNo = IndexOf(colName);
    if (colNo < 0)
    {
        throw Exception(L"MsiColumnCollection::operator[]: Name not found.");
    }
    return m_columns[colNo];
}


long MsiColumnCollection::IndexOf(PCWSTR colName) const
{
    for (long count = m_count; count; count--)
    {
        long colNo = m_next;
        const_cast<MsiColumnCollection*>(this)->m_next = (m_next + 1) % m_count;
        if (!String::Compare(m_columns[colNo].Name, colName))
        {
            return colNo;
        }
    }
    return -1;
}
