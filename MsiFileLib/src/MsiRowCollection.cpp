#include "pch.h"
#include "MsiRowCollection.h"
#include "hnrt/Exception.h"
#include "hnrt/Heap.h"


using namespace hnrt;


MsiRowCollection::~MsiRowCollection()
{
    for (long rowNo = 0; rowNo < m_count; rowNo++)
    {
        delete m_ppRows[rowNo];
    }
    free(m_ppRows);
}


MsiRowCollection::MsiRowCollection()
    : IMsiRowCollection()
    , m_count(0)
    , m_ppRows(nullptr)
    , m_capacity(0)
{
}


const IMsiRow& MsiRowCollection::operator[](long rowNo) const
{
    if (rowNo < 0 || m_count <= rowNo)
    {
        throw Exception(L"MsiRowCollection::operator[]: Index out of range.");
    }
    return *m_ppRows[rowNo];
}


IMsiRow& MsiRowCollection::operator[](long rowNo)
{
    if (rowNo < 0 || m_count <= rowNo)
    {
        throw Exception(L"MsiRowCollection::operator[]: Index out of range.");
    }
    return *m_ppRows[rowNo];
}


void MsiRowCollection::Add(IMsiRow* pRow)
{
    if (m_count + 1 > m_capacity)
    {
        long capacity = m_capacity + (!m_capacity ? 32 : m_capacity < 65536 ? m_capacity : 65536);
        m_ppRows = Allocate(m_ppRows, capacity);
        memset(&m_ppRows[m_count], 0, (static_cast<size_t>(capacity) - m_count) * sizeof(IMsiRow*));
        m_capacity = capacity;
    }
    m_ppRows[m_count++] = pRow;
}
