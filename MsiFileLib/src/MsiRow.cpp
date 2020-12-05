#include "pch.h"
#include "MsiRow.h"
#include "hnrt/IMsiTable.h"
#include "hnrt/Heap.h"
#include "hnrt/Exception.h"


using namespace hnrt;


MsiRow::MsiRow(IMsiTable* pTable)
    : m_pTable(pTable)
    , m_ppItems(ZeroAllocate<IMsiData*>(pTable->Columns.Count))
{
}


MsiRow::~MsiRow()
{
    for (long colNo = 0; colNo < m_pTable->Columns.Count; colNo++)
    {
        delete m_ppItems[colNo];
    }
    free(m_ppItems);
}


void MsiRow::SetData(long colNo, IMsiData* pData)
{
    if (colNo < 0 || m_pTable->Columns.Count <= colNo)
    {
        throw Exception(L"MsiRow::SetData: Index out of range.");
    }
    m_ppItems[colNo] = pData;
}


void MsiRow::SetData(PCWSTR colName, IMsiData* pData)
{
    long colNo = m_pTable->Columns.IndexOf(colName);
    if (colNo < 0)
    {
        throw Exception(L"MsiRow::SetData: Column name not found.");
    }
    m_ppItems[colNo] = pData;
}


const IMsiData& MsiRow::operator[](long colNo) const
{
    if (colNo < 0 || m_pTable->Columns.Count <= colNo)
    {
        throw Exception(L"MsiRow::operator[]: Index out of range.");
    }
    if (!m_ppItems[colNo])
    {
        throw Exception(L"MsiRow::operator[]: No data.");
    }
    return *m_ppItems[colNo];
}


const IMsiData& MsiRow::operator[](PCWSTR colName) const
{
    long colNo = m_pTable->Columns.IndexOf(colName);
    if (colNo < 0)
    {
        throw Exception(L"MsiRow::operator[]: Column name not found.");
    }
    if (!m_ppItems[colNo])
    {
        throw Exception(L"MsiRow::operator[]: No data.");
    }
    return *m_ppItems[colNo];
}


IMsiTable& MsiRow::get_Table()
{
    return *m_pTable;
}
