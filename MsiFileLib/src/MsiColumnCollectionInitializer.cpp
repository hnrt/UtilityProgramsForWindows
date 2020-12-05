#include "pch.h"
#include "MsiColumnCollectionInitializer.h"
#include "MsiUtils.h"
#include <set>
#include "hnrt/String.h"
#include "hnrt/MsiException.h"
#include "hnrt/Heap.h"


using namespace hnrt;


MsiColumnCollectionInitializer::MsiColumnCollectionInitializer()
    : m_map()
{
    m_map.insert(InitMapEntry(L"_Tables", Init_Tables));
    m_map.insert(InitMapEntry(L"_Columns", Init_Columns));
    m_map.insert(InitMapEntry(L"_Streams", Init_Streams));
    m_map.insert(InitMapEntry(L"_Storages", Init_Storages));
    m_map.insert(InitMapEntry(L"_TransformView", Init_TransformView));
}


void MsiColumnCollectionInitializer::Run(MSIHANDLE hDatabase, PCWSTR pszTableName, MsiColumn*& columns, long& count) const
{
    InitMap::const_iterator iter = m_map.find(pszTableName);
    if (iter != m_map.end())
    {
        (*iter->second)(hDatabase, pszTableName, columns, count);
    }
    else
    {
        InitGeneric(hDatabase, pszTableName, columns, count);
    }
}


void MsiColumnCollectionInitializer::InitGeneric(MSIHANDLE hDatabase, PCWSTR pszTableName, MsiColumn*& columns, long& count)
{
    std::set<PCWSTR, StringLessThan> keys;
    PMSIHANDLE phRecord;
    UINT rc = MsiDatabaseGetPrimaryKeysW(hDatabase, pszTableName, &phRecord);
    if (!rc)
    {
        rc = MsiRecordGetFieldCount(phRecord);
        for (UINT i = 0; i < rc; i++)
        {
            PCWSTR pszKey = MsiUtils::GetString(phRecord, i + 1);
            keys.insert(pszKey);
        }
    }
    PMSIHANDLE phView;
    rc = MsiDatabaseOpenViewW(hDatabase, L"SELECT `Column`,`Nullable`,`MinValue`,`MaxValue`,`KeyTable`,`KeyColumn`,`Category`,`Set`,`Description` FROM `_Columns`,`_Validation` WHERE `_Columns`.`Table` = ? AND `_Columns`.`Table` = `_Validation`.`Table` AND `_Columns`.`Name` = `_Validation`.`Column` ORDER BY `Number`", &phView);
    if (rc)
    {
        throw MsiException(rc, L"Failed to open view.");
    }
    phRecord = MsiCreateRecord(1);
    rc = MsiRecordSetStringW(phRecord, 1, pszTableName);
    if (rc)
    {
        throw MsiException(rc, L"Failed to set string to record.");
    }
    rc = MsiViewExecute(phView, phRecord);
    if (rc)
    {
        throw MsiException(rc, L"Failed to execute.");
    }
    int capacity = 8;
    columns = Allocate<MsiColumn>(capacity);
    for (;;)
    {
        rc = MsiViewFetch(phView, &phRecord);
        if (!rc)
        {
            PCWSTR pszColumn = MsiUtils::GetString(phRecord, 1);
            bool bKey = keys.find(pszColumn) != keys.end() ? true : false;
            PCWSTR pszNullable = MsiUtils::GetString(phRecord, 2);
            PCWSTR pszMinValue = MsiRecordIsNull(phRecord, 3) ? nullptr : MsiUtils::GetString(phRecord, 3);
            PCWSTR pszMaxValue = MsiRecordIsNull(phRecord, 4) ? nullptr : MsiUtils::GetString(phRecord, 4);
            PCWSTR pszKeyTable = MsiRecordIsNull(phRecord, 5) ? nullptr : MsiUtils::GetString(phRecord, 5);
            PCWSTR pszKeyColumn = MsiRecordIsNull(phRecord, 6) ? nullptr : MsiUtils::GetString(phRecord, 6);
            PCWSTR pszCategory = MsiRecordIsNull(phRecord, 7) ? nullptr : MsiUtils::GetString(phRecord, 7);
            PCWSTR pszSet = MsiRecordIsNull(phRecord, 8) ? nullptr : MsiUtils::GetString(phRecord, 8);
            PCWSTR pszDescription = MsiRecordIsNull(phRecord, 9) ? nullptr : MsiUtils::GetString(phRecord, 9);
            if (count + 1 > capacity)
            {
                capacity *= 2;
                columns = Allocate(columns, capacity);
            }
            new(&columns[count++]) MsiColumn(pszColumn, bKey, pszNullable, pszMinValue, pszMaxValue, pszKeyTable, pszKeyColumn, pszCategory, pszSet, pszDescription);
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


void MsiColumnCollectionInitializer::Init_Tables(MSIHANDLE hDatabase, PCWSTR pszTableName, MsiColumn*& columns, long& count)
{
    columns = Allocate<MsiColumn>(1);
    new(&columns[count++]) MsiColumn(L"Name", true, L"N", nullptr, nullptr, nullptr, nullptr, L"Text", nullptr, L"Name of the table.");
}


void MsiColumnCollectionInitializer::Init_Columns(MSIHANDLE hDatabase, PCWSTR pszTableName, MsiColumn*& columns, long& count)
{
    columns = Allocate<MsiColumn>(3);
    new(&columns[count++]) MsiColumn(L"Table", true, L"N", nullptr, nullptr, nullptr, nullptr, L"Text", nullptr, L"The name of the table that contains the column.");
    new(&columns[count++]) MsiColumn(L"Number", true, L"N", nullptr, nullptr, nullptr, nullptr, L"Integer", nullptr, L"The order of the column within the table.");
    new(&columns[count++]) MsiColumn(L"Name", false, L"N", nullptr, nullptr, nullptr, nullptr, L"Text", nullptr, L"The name of the column.");
}


void MsiColumnCollectionInitializer::Init_Streams(MSIHANDLE hDatabase, PCWSTR pszTableName, MsiColumn*& columns, long& count)
{
    columns = Allocate<MsiColumn>(2);
    new(&columns[count++]) MsiColumn(L"Name", true, L"N", nullptr, nullptr, nullptr, nullptr, L"Text", nullptr, L"A unique key that identifies the stream.");
    new(&columns[count++]) MsiColumn(L"Data", false, L"Y", nullptr, nullptr, nullptr, nullptr, L"Binary", nullptr, L"The unformatted binary data.");
}


void MsiColumnCollectionInitializer::Init_Storages(MSIHANDLE hDatabase, PCWSTR pszTableName, MsiColumn*& columns, long& count)
{
    columns = Allocate<MsiColumn>(2);
    new(&columns[count++]) MsiColumn(L"Name", true, L"N", nullptr, nullptr, nullptr, nullptr, L"Text", nullptr, L"A unique key that identifies the storage.");
    new(&columns[count++]) MsiColumn(L"Data", false, L"Y", nullptr, nullptr, nullptr, nullptr, L"Binary", nullptr, L"The unformatted binary data.");
}


void MsiColumnCollectionInitializer::Init_TransformView(MSIHANDLE hDatabase, PCWSTR pszTableName, MsiColumn*& columns, long& count)
{
    columns = Allocate<MsiColumn>(5);
    new(&columns[count++]) MsiColumn(L"Table", true, L"N", nullptr, nullptr, nullptr, nullptr, L"Identifier", nullptr, L"Name of an altered database table.");
    new(&columns[count++]) MsiColumn(L"Column", true, L"N", nullptr, nullptr, nullptr, nullptr, L"Text", nullptr, L"Name of an altered table column or INSERT, DELETE, CREATE, or DROP.");
    new(&columns[count++]) MsiColumn(L"Row", true, L"Y", nullptr, nullptr, nullptr, nullptr, L"Text", nullptr, L"A list of the primary key values separated by tabs.");
    new(&columns[count++]) MsiColumn(L"Data", false, L"Y", nullptr, nullptr, nullptr, nullptr, L"Text", nullptr, L"Data, name of a data stream, or a column definition.");
    new(&columns[count++]) MsiColumn(L"Current", false, L"Y", nullptr, nullptr, nullptr, nullptr, L"Text", nullptr, L"Current value from reference database, or column a number.");
}
