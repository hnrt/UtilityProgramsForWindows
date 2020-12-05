#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include "hnrt/MsiDatabaseFactory.h"
#include "hnrt/MsiColumnDataTypeExtensions.h"
#include "hnrt/String.h"
#include "hnrt/Path.h"
#include "hnrt/NumberText.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/Exception.h"
#include "hnrt/MsiException.h"
#include "hnrt/ComException.h"
#include "hnrt/Win32Exception.h"
#include "MsiFileReader.h"


#pragma comment(lib, "Core")
#pragma comment(lib, "MsiFile")


using namespace hnrt;


int wmain(int argc, wchar_t* argv[])
{
    _wsetlocale(LC_ALL, L"");

    MsiFileReader app(argc, argv);

    try
    {
        app.Run();
        return EXIT_SUCCESS;
    }
    catch (int exitCode)
    {
        return exitCode;
    }
    catch (MsiException e)
    {
        fwprintf(stderr, L"ERROR: %s\n  Reason: %s (%u)\n", e.Message, ErrorMessage::Get(e.Error), e.Error);
    }
    catch (ComException e)
    {
        fwprintf(stderr, L"ERROR: %s\n  Reason: %08lx\n", e.Message, e.Result);
    }
    catch (Win32Exception e)
    {
        fwprintf(stderr, L"ERROR: %s\n  Reason: %s (%u)\n", e.Message, ErrorMessage::Get(e.Error), e.Error);
    }
    catch (Exception e)
    {
        fwprintf(stderr, L"ERROR: %s\n", e.Message);
    }
    catch (std::bad_alloc)
    {
        fwprintf(stderr, L"ERROR: Out of memory.\n");
    }
    catch (std::exception e)
    {
        fwprintf(stderr, L"ERROR: %hs\n", e.what());
    }
    catch (...)
    {
        fwprintf(stderr, L"ERROR: Unhandled exception caught.\n");
    }

    return EXIT_FAILURE;
}


MsiFileReader::MsiFileReader(int argc, wchar_t* argv[])
    : m_pszName(Path::GetFileNameWithoutExtension(argv[0]))
    , m_args(argc, argv)
    , m_optionsGeneral()
    , m_pMsiDatabase()
    , m_pComponentTable()
    , m_pDirectoryTable()
    , m_pFileTable()
    , m_pFeatureComponentsTable()
    , m_optionsCommand()
    , m_pszOutDir(nullptr)
    , m_bFlat(false)
    , m_verboseLevel(0)
    , m_bHeader(false)
    , m_pszItemSeparator(L",")
{
    m_optionsGeneral.insert(OptionMapEntry(L"-help", &MsiFileReader::OptionHelp));
    m_optionsGeneral.insert(OptionMapEntry(L"-h", &MsiFileReader::OptionHelp));
    m_optionsGeneral.insert(OptionMapEntry(L"-?", &MsiFileReader::OptionHelp));
    m_optionsCommand.insert(OptionMapEntry(L"-tables", &MsiFileReader::CommandPrintTableNames));
    m_optionsCommand.insert(OptionMapEntry(L"-table", &MsiFileReader::CommandPrintTable));
    m_optionsCommand.insert(OptionMapEntry(L"-describe", &MsiFileReader::CommandDescribeTable));
    m_optionsCommand.insert(OptionMapEntry(L"-desc", &MsiFileReader::CommandDescribeTable));
    m_optionsCommand.insert(OptionMapEntry(L"-print", &MsiFileReader::CommandPrintFilesByFilePattern));
    m_optionsCommand.insert(OptionMapEntry(L"-print-by-feature", &MsiFileReader::CommandPrintFilesByFeature));
    m_optionsCommand.insert(OptionMapEntry(L"-extract", &MsiFileReader::CommandExtractFiles));
    m_optionsCommand.insert(OptionMapEntry(L"-extract-by-feature", &MsiFileReader::CommandExtractFilesByFeature));
}


void MsiFileReader::Run()
{
    if (!m_args.Exists())
    {
        OptionHelp();
    }

    ParseGeneralOptions();

    if (m_args.Exists())
    {
        m_pMsiDatabase = MsiDatabaseFactory::OpenRead(*m_args);
        m_args++;
    }
    else
    {
        throw Exception(L"No MSI file was given.");
    }

    DispatchCommand();
}


void MsiFileReader::ParseGeneralOptions()
{
    while (m_args.Exists())
    {
        PCWSTR a = *m_args;
        OptionMap::iterator iter = m_optionsGeneral.find(a);
        if (iter != m_optionsGeneral.end())
        {
            m_args++;
            (this->*iter->second)();
        }
        else
        {
            break;
        }
    }
}


#define BAD_SYNTAX throw Exception(L"Bad command line syntax. Please try -help for usage. Thank you.")


void MsiFileReader::DispatchCommand()
{
    if (m_args.Exists())
    {
        PCWSTR a = *m_args;
        OptionMap::iterator iter = m_optionsCommand.find(a);
        if (iter != m_optionsCommand.end())
        {
            m_args++;
            (this->*iter->second)();
            return;
        }
    }
    BAD_SYNTAX;
}


void MsiFileReader::OptionHelp()
{
    wprintf(L"Usage:\n");
    wprintf(L"  %s MSIFILE -tables\n", m_pszName);
    wprintf(L"  %s MSIFILE -table TABLENAME [-header] [-comma|-tab]\n", m_pszName);
    wprintf(L"  %s MSIFILE -describe TABLENAME [-header] [-comma|-tab]\n", m_pszName);
    wprintf(L"  %s MSIFILE -print FILEPATTERN [-header] [-comma|-tab]\n", m_pszName);
    wprintf(L"  %s MSIFILE -print-by-feature FEATURE [-header] [-comma|-tab]\n", m_pszName);
    wprintf(L"  %s MSIFILE -extract FILEPATTERN [-out DIR] [-flat] [-verbose]\n", m_pszName);
    wprintf(L"  %s MSIFILE -extract-by-feature FEATURE [-out DIR] [-flat] [-verbose]\n", m_pszName);
    wprintf(L"  %s -help\n", m_pszName);
    throw EXIT_SUCCESS;
}


void MsiFileReader::CommandPrintTableNames()
{
    if (m_args.Exists())
    {
        BAD_SYNTAX;
    }
    RefPtr<IMsiTable> pTable = m_pMsiDatabase->GetTable(L"_Tables");
    for (long rowNo = 0; rowNo < pTable->Rows.Count; rowNo++)
    {
        const IMsiRow& row = pTable->Rows[rowNo];
        const IMsiData& data = row[0L];
        wprintf(L"%s\n", data.ToText()->Text);
    }
}


void MsiFileReader::CommandPrintTable()
{
    PCWSTR pszTableName;
    ParsePrintOptions(&pszTableName, L"Table name was not given.");
    RefPtr<IMsiTable> pTable = m_pMsiDatabase->GetTable(pszTableName);
    if (!pTable->Columns.Count)
    {
        throw Exception(L"No such table.");
    }
    if (m_bHeader)
    {
        for (long colNo = 0; colNo < pTable->Columns.Count; colNo++)
        {
            const IMsiColumn& col = pTable->Columns[colNo];
            wprintf(L"%s%s", colNo ? m_pszItemSeparator : L"", col.Name);
        }
        wprintf(L"\n");
    }
    for (long rowNo = 0; rowNo < pTable->Rows.Count; rowNo++)
    {
        const IMsiRow& row = pTable->Rows[rowNo];
        for (long colNo = 0; colNo < pTable->Columns.Count; colNo++)
        {
            const IMsiColumn& col = pTable->Columns[colNo];
            const IMsiData& data = row[colNo];
            if (data.IsNull)
            {
                wprintf(L"%s", colNo ? m_pszItemSeparator : L"");
            }
            else if (col.Type == MsiColumnDataType::Binary)
            {
                wprintf(L"%s(%zu bytes)", colNo ? m_pszItemSeparator : L"", data.ToBinary()->Len);
            }
            else
            {
                wprintf(L"%s%s", colNo ? m_pszItemSeparator : L"", data.ToText()->Text);
            }
        }
        wprintf(L"\n");
    }
}


void MsiFileReader::CommandDescribeTable()
{
    PCWSTR pszTableName;
    ParsePrintOptions(&pszTableName, L"Table name was not given.");
    RefPtr<IMsiTable> pTable = m_pMsiDatabase->GetTable(pszTableName);
    if (!pTable->Columns.Count)
    {
        throw Exception(L"No such table.");
    }
    static const WCHAR szFormat[] = { L"%s%s%s%s%s%s%s%s%s%s%s\n" };
    if (m_bHeader)
    {
        wprintf(szFormat,
            L"Column",
            m_pszItemSeparator, L"Key",
            m_pszItemSeparator, L"Nullable",
            m_pszItemSeparator, L"Type",
            m_pszItemSeparator, L"KeyTable",
            m_pszItemSeparator, L"KeyColumn");
    }
    for (long colNo = 0; colNo < pTable->Columns.Count; colNo++)
    {
        const IMsiColumn& col = pTable->Columns[colNo];
        wprintf(szFormat,
            col.Name,
            m_pszItemSeparator, col.Key ? L"Y" : L"N",
            m_pszItemSeparator, col.Nullable ? L"Y" : L"N",
            m_pszItemSeparator, MsiColumnDataTypeExtensions::ToString(col.Type),
            m_pszItemSeparator, col.KeyTable ? col.KeyTable : L"",
            m_pszItemSeparator, col.KeyColumn ? col.KeyColumn : L"");
    }
}


void MsiFileReader::CommandPrintFilesByFilePattern()
{
    PCWSTR pszFilePattern;
    ParsePrintOptions(&pszFilePattern, L"File pattern was not given.");
    m_pComponentTable = m_pMsiDatabase->GetTable(L"Component");
    m_pComponentTable->CreateIndex();
    m_pFeatureComponentsTable = m_pMsiDatabase->GetTable(L"FeatureComponents");
    m_pFileTable = m_pMsiDatabase->GetTable(L"File");
    PrintFeaturePathHeader();
    for (long fileRowNo = 0; fileRowNo < m_pFileTable->Rows.Count; fileRowNo++)
    {
        const IMsiRow& fileRow = m_pFileTable->Rows[fileRowNo];
        PCWSTR pszFileName = fileRow[L"FileName"].ToFilename()->LongName;
        if (Path::WildcardMatch(pszFilePattern, pszFileName))
        {
            PCWSTR pszComponent = fileRow[L"Component_"].ToIdentifier()->Text;
            PrintFeatureByFileName(pszFileName, pszComponent);
        }
    }
}


void MsiFileReader::CommandPrintFilesByFeature()
{
    PCWSTR pszFeature;
    ParsePrintOptions(&pszFeature, L"Feature was not given.");
    m_pComponentTable = m_pMsiDatabase->GetTable(L"Component");
    m_pComponentTable->CreateIndex();
    m_pFeatureComponentsTable = m_pMsiDatabase->GetTable(L"FeatureComponents");
    m_pFileTable = m_pMsiDatabase->GetTable(L"File");
    PrintFeaturePathHeader();
    if (wcspbrk(pszFeature, L"*?"))
    {
        for (long fcRowNo = 0; fcRowNo < m_pFeatureComponentsTable->Rows.Count; fcRowNo++)
        {
            const IMsiRow& fcRow = m_pFeatureComponentsTable->Rows[fcRowNo];
            PCWSTR pszNextFeature = fcRow[L"Feature_"].ToIdentifier()->Text;
            if (Path::WildcardMatch(pszFeature, pszNextFeature))
            {
                PCWSTR pszComponent = fcRow[L"Component_"].ToIdentifier()->Text;
                PrintPathByFeature(pszNextFeature, pszComponent);
            }
        }
    }
    else
    {
        long fcRowNo = m_pFeatureComponentsTable->Find(L"Feature_", pszFeature);
        while (fcRowNo >= 0L)
        {
            const IMsiRow& fcRow = m_pFeatureComponentsTable->Rows[fcRowNo];
            PCWSTR pszComponent = fcRow[L"Component_"].ToIdentifier()->Text;
            PrintPathByFeature(pszFeature, pszComponent);
            fcRowNo = m_pFeatureComponentsTable->Find(L"Feature_", pszFeature, fcRowNo + 1);
        }
    }
}


void MsiFileReader::ParsePrintOptions(PCWSTR* ppszTarget, PCWSTR pszErrorMessage)
{
    if (m_args.Exists())
    {
        *ppszTarget = *m_args;
        m_args++;
    }
    else
    {
        throw Exception(pszErrorMessage);
    }
    while (m_args.Exists())
    {
        PCWSTR a = *m_args;
        if (!String::Compare(a, L"-header"))
        {
            m_args++;
            m_bHeader = true;
        }
        else if (!String::Compare(a, L"-comma"))
        {
            m_args++;
            m_pszItemSeparator = L",";
        }
        else if (!String::Compare(a, L"-tab"))
        {
            m_args++;
            m_pszItemSeparator = L"\t";
        }
        else if (!String::Compare(a, L"-verbose") || !String::Compare(a, L"-v"))
        {
            m_args++;
            m_verboseLevel++;
        }
        else
        {
            BAD_SYNTAX;
        }
    }
}


void MsiFileReader::PrintFeatureByFileName(PCWSTR pszFileName, PCWSTR pszComponent)
{
    long fcRowNo = m_pFeatureComponentsTable->Find(L"Component_", pszComponent);
    if (fcRowNo >= 0L)
    {
        const IMsiRow& fcRow = m_pFeatureComponentsTable->Rows[fcRowNo];
        PCWSTR pszFeature = fcRow[L"Feature_"].ToIdentifier()->Text;
        PrintFeaturePath(pszFeature, pszFileName, pszComponent);
    }
    else
    {
        fwprintf(stderr, L"ERROR: %s not found in FeatureComponents table.\n", pszComponent);
    }
}


void MsiFileReader::PrintPathByFeature(PCWSTR pszFeature, PCWSTR pszComponent)
{
    long fileRowNo = m_pFileTable->Find(L"Component_", pszComponent);
    while (fileRowNo >= 0L)
    {
        const IMsiRow& fileRow = m_pFileTable->Rows[fileRowNo];
        PCWSTR pszFileName = fileRow[L"FileName"].ToFilename()->LongName;
        PrintFeaturePath(pszFeature, pszFileName, pszComponent);
        fileRowNo = m_pFileTable->Find(L"Component_", pszComponent, fileRowNo + 1);
    }
}


void MsiFileReader::PrintFeaturePathHeader()
{
    if (m_bHeader)
    {
        if (m_verboseLevel)
        {
            wprintf(L"%s%s%s%s%s\n", L"Feature", m_pszItemSeparator, L"FilePath", m_pszItemSeparator, L"Component");
        }
        else
        {
            wprintf(L"%s%s%s\n", L"Feature", m_pszItemSeparator, L"FilePath");
        }
    }
}


void MsiFileReader::PrintFeaturePath(PCWSTR pszFeature, PCWSTR pszFileName, PCWSTR pszComponent)
{
    long componentRowNo = m_pComponentTable->Find(L"Component", pszComponent);
    if (componentRowNo >= 0L)
    {
        const IMsiRow& componentRow = m_pComponentTable->Rows[componentRowNo];
        PCWSTR pszDirId = componentRow[L"Directory_"].ToIdentifier()->Text;
        PCWSTR pszDirPath = m_pMsiDatabase->GetDirectoryPath(pszDirId);
        PCWSTR pszPath = Path::Combine(pszDirPath, pszFileName);
        wprintf(m_verboseLevel ? L"%s%s%s%s%s\n" : L"%s%s%s\n", pszFeature, m_pszItemSeparator, pszPath, m_pszItemSeparator, pszComponent);
    }
    else
    {
        fwprintf(stderr, L"ERROR: %s not found in Component table.\n", pszComponent);
        wprintf(m_verboseLevel ? L"%s%s%s%s%s\n" : L"%s%s%s\n", pszFeature, m_pszItemSeparator, pszFileName, m_pszItemSeparator, pszComponent);
    }
}


void MsiFileReader::CommandExtractFiles()
{
    PCWSTR pszFilePattern;
    ParseExtractFilesOptions(&pszFilePattern, L"File pattern was not given.");
    ValidateOutDir();
    RefPtr<IMsiFileRowCollection> pCollection = m_pMsiDatabase->CreateFileRowCollection();
    m_pMsiDatabase->AddByFileName(pCollection.Ptr, pszFilePattern);
    m_pMsiDatabase->ExtractFiles(pCollection.Ptr, m_pszOutDir, m_bFlat, this);
}


void MsiFileReader::CommandExtractFilesByFeature()
{
    PCWSTR pszFeature;
    ParseExtractFilesOptions(&pszFeature, L"Feature was not given.");
    ValidateOutDir();
    RefPtr<IMsiFileRowCollection> pCollection = m_pMsiDatabase->CreateFileRowCollection();
    m_pMsiDatabase->AddByFeature(pCollection.Ptr, pszFeature);
    m_pMsiDatabase->ExtractFiles(pCollection.Ptr, m_pszOutDir, m_bFlat, this);
}


void MsiFileReader::ParseExtractFilesOptions(PCWSTR* ppszTarget, PCWSTR pszErrorMessage)
{
    if (m_args.Exists())
    {
        *ppszTarget = *m_args;
        m_args++;
    }
    else
    {
        throw Exception(pszErrorMessage);
    }
    while (m_args.Exists())
    {
        PCWSTR a = *m_args;
        if (!String::Compare(a, L"-out") || !String::Compare(a, L"-o"))
        {
            m_args++;
            if (m_args.Exists())
            {
                if (m_pszOutDir)
                {
                    throw Exception(L"Duplicate -out definition.");
                }
                m_pszOutDir = *m_args;
                m_args++;
            }
            else
            {
                BAD_SYNTAX;
            }
        }
        else if (!String::Compare(a, L"-flat"))
        {
            m_args++;
            m_bFlat = true;
        }
        else if (!String::Compare(a, L"-verbose") || !String::Compare(a, L"-v"))
        {
            m_args++;
            m_verboseLevel++;
        }
        else
        {
            BAD_SYNTAX;
        }
    }
}


void MsiFileReader::ValidateOutDir()
{
    if (m_pszOutDir)
    {
        if (!Path::IsDirectory(m_pszOutDir))
        {
            throw Win32Exception(GetLastError(), L"Failed to check if %s exists.", m_pszOutDir);
        }
    }
    else
    {
        m_pszOutDir = Path::Combine(Path::GetKnownFolder(FOLDERID_RoamingAppData), L"hnrt");
        if (!Path::IsDirectory(m_pszOutDir) && !CreateDirectoryW(m_pszOutDir, NULL))
        {
            throw Win32Exception(GetLastError(), L"Failed to create %s.", m_pszOutDir);
        }
        m_pszOutDir = Path::Combine(m_pszOutDir, m_pszName);
        if (!Path::IsDirectory(m_pszOutDir) && !CreateDirectoryW(m_pszOutDir, NULL))
        {
            throw Win32Exception(GetLastError(), L"Failed to create %s.", m_pszOutDir);
        }
    }
}


bool MsiFileReader::OnMsiDatabaseExtract(PCWSTR pszFile, PCWSTR pszPath, unsigned long status)
{
    wprintf(L"%s\n", pszPath);
    if (status != ERROR_SUCCESS)
    {
        wprintf(L"  ERROR: %s (%lu)\n", ErrorMessage::Get(status), status);
    }
    else if (m_verboseLevel)
    {
        long long cbSize = Path::GetSize(pszPath);
        if (cbSize > 0)
        {
            wprintf(L"  %s bytes written.\n", &NumberText(cbSize));
        }
        else if (cbSize == 0)
        {
            wprintf(L"  0 byte written.\n");
        }
        else
        {
            wprintf(L"  File size unavailable.\n");
        }
    }
    return true;
}
