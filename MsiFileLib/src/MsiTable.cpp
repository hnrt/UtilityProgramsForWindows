#include "pch.h"
#include "hnrt/MsiException.h"
#include "hnrt/String.h"
#include "hnrt/StringBuffer.h"
#include "MsiTable.h"
#include "MsiRow.h"
#include "MsiDataFactory.h"
#include "MsiUtils.h"


using namespace hnrt;


MsiTable::MsiTable(PCWSTR pszName, MsiDatabase& database)
    : IMsiTable()
    , m_refs(1)
    , m_szName(pszName)
    , m_database(database)
    , m_columns(database, pszName)
    , m_rows()
    , m_pIndex(nullptr)
{
    if (m_columns.Count)
    {
        Load();
    }
}


MsiTable::~MsiTable()
{
    delete m_pIndex;
}


long MsiTable::AddRef()
{
    long refs = _InterlockedIncrement(&m_refs);
    return refs;
}


long MsiTable::Release()
{
    long refs = _InterlockedDecrement(&m_refs);
    if (!refs)
    {
        delete this;
    }
    return refs;
}


void MsiTable::Load()
{
    StringBuffer query(300);
    query.Len = 0;
    query.AppendFormat(L"SELECT `%s`", m_columns[0L].Name);
    for (long colNo = 1; colNo < m_columns.Count; colNo++)
    {
        query.AppendFormat(L",`%s`", m_columns[colNo].Name);
    }
    query.AppendFormat(L" FROM `%s`", m_szName);
    PMSIHANDLE phView;
    UINT rc = MsiDatabaseOpenViewW(m_database, query, &phView);
    if (rc)
    {
        throw MsiException(rc, L"Failed to open view.");
    }
    rc = MsiViewExecute(phView, NULL);
    if (rc)
    {
        throw MsiException(rc, L"Failed to execute.");
    }
    for (;;)
    {
        PMSIHANDLE phRecord;
        rc = MsiViewFetch(phView, &phRecord);
        if (!rc)
        {
            IMsiRow* pRow = NewRow();
            for (long colNo = 0; colNo < m_columns.Count; colNo++)
            {
                IMsiData* pData = MsiDataFactory::CreateData(phRecord, colNo + 1, m_columns[colNo].Type);
                pRow->SetData(colNo, pData);
            }
            m_rows.Add(pRow);
        }
        else if (rc == ERROR_NO_MORE_ITEMS)
        {
            break;
        }
        else
        {
            throw MsiException(rc, L"Failed to fetch.");
        }
    }
}


IMsiRow* MsiTable::NewRow()
{
    return new MsiRow(this);
}


PCWSTR MsiTable::get_Name() const
{
    return m_szName;
}


const IMsiColumnCollection& MsiTable::get_Columns() const
{
    return m_columns;
}


const IMsiRowCollection& MsiTable::get_Rows() const
{
    return m_rows;
}


long MsiTable::Find(PCWSTR pszColumnName, PCWSTR pszValue, long startRowNo) const
{
    if (!String::Compare(pszColumnName, m_columns.PrimaryKey) && m_pIndex)
    {
        Index::const_iterator iter = m_pIndex->find(pszValue);
        if (iter != m_pIndex->end())
        {
            return iter->second;
        }
        else
        {
            return -1L;
        }
    }
    PCWSTR pSep = wcschr(pszColumnName, L';');
    if (pSep)
    {
        long colNoList[32];
        long keys = 0;
        PCWSTR pCur = pszColumnName;
        while (true)
        {
            long colNo = m_columns.IndexOf(String::Copy(pCur, pSep - pCur));
            if (colNo < 0)
            {
                throw MsiException(ERROR_INVALID_PARAMETER, L"Invalid column name.");
            }
            if (m_columns[colNo].Type == MsiColumnDataType::Binary)
            {
                throw MsiException(ERROR_INVALID_PARAMETER, L"Column is of binary.");
            }
            colNoList[keys++] = colNo;
            if (!*pSep)
            {
                break;
            }
            if (keys >= _countof(colNoList))
            {
                throw MsiException(ERROR_INVALID_PARAMETER, L"Too many columns.");
            }
            pCur = pSep + 1;
            pSep = wcschr(pCur, L';');
            if (!pSep)
            {
                pSep = wcschr(pCur, L'\0');
            }
        }
        StringBuffer buf(300);
        for (long rowNo = startRowNo > 0L ? startRowNo : 0L; rowNo < m_rows.Count; rowNo++)
        {
            const IMsiRow& row = m_rows[rowNo];
            buf.Len = 0;
            for (long keyNo = 0L; keyNo < keys; keyNo++)
            {
                const IMsiData& data = row[colNoList[keyNo]];
                buf.AppendFormat(keyNo ? L";%s" : L"%s", data.IsNull ? L"" : data.ToText()->Text);
            }
            if (!String::Compare(buf, pszValue))
            {
                return rowNo;
            }
        }
    }
    else
    {
        long colNo = m_columns.IndexOf(pszColumnName);
        if (colNo < 0)
        {
            throw MsiException(ERROR_INVALID_PARAMETER, L"Invalid column name.");
        }
        if (m_columns[colNo].Type == MsiColumnDataType::Binary)
        {
            throw MsiException(ERROR_INVALID_PARAMETER, L"Column is of binary.");
        }
        for (long rowNo = startRowNo > 0L ? startRowNo : 0L; rowNo < m_rows.Count; rowNo++)
        {
            const IMsiRow& row = m_rows[rowNo];
            const IMsiData& data = row[colNo];
            if (!String::Compare(data.IsNull ? L"" : data.ToText()->Text, pszValue))
            {
                return rowNo;
            }
        }
    }
    return -1L;
}


void MsiTable::CreateIndex()
{
    if (m_pIndex)
    {
        return;
    }
    if (m_columns.KeyCount == 0)
    {
        throw MsiException(ERROR_INVALID_PARAMETER, L"No primary key was found.");
    }
    m_pIndex = new Index();
    if (m_columns.KeyCount == 1)
    {
        long colNo = m_columns.GetKeyIndex(0L);
        for (long rowNo = 0L; rowNo < m_rows.Count; rowNo++)
        {
            m_pIndex->insert(IndexEntry(m_rows[rowNo][colNo].ToText()->Text, rowNo));
        }
    }
    else
    {
        StringBuffer buf(300);
        for (long rowNo = 0L; rowNo < m_rows.Count; rowNo++)
        {
            buf.Len = 0;
            buf.AppendFormat(L"%s", m_rows[rowNo][m_columns.GetKeyIndex(0L)].ToText()->Text);
            for (long keyNo = 1L; keyNo < m_columns.KeyCount; keyNo++)
            {
                buf.AppendFormat(L";%s", m_rows[rowNo][m_columns.GetKeyIndex(keyNo)].ToText()->Text);
            }
            m_pIndex->insert(IndexEntry(String::Copy(&buf), rowNo));
        }
    }
}
