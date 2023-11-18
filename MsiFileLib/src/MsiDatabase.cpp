#include "pch.h"
#include "hnrt/String.h"
#include "hnrt/MsiException.h"
#include "hnrt/RefPtr.h"
#include "hnrt/Path.h"
#include "hnrt/CabinetFactory.h"
#include "hnrt/CabinetException.h"
#include "hnrt/Win32Exception.h"
#include "MsiDatabase.h"
#include "MsiTable.h"
#include "MsiFileRowCollection.h"


#pragma comment(lib,"Msi.lib")


using namespace hnrt;


MsiDatabase::MsiDatabase()
    : IMsiDatabase()
    , m_refs(1)
    , m_szPath()
    , m_phDatabase()
    , m_dirPaths()
{
}


MsiDatabase::~MsiDatabase()
{
    while (m_tables.size())
    {
        TableMap::iterator iter = m_tables.begin();
        iter->second->Release();
        m_tables.erase(iter);
    }
}


long MsiDatabase::AddRef()
{
    long refs = _InterlockedIncrement(&m_refs);
    return refs;
}


long MsiDatabase::Release()
{
    long refs = _InterlockedDecrement(&m_refs);
    if (!refs)
    {
        delete this;
    }
    return refs;
}


PCWSTR MsiDatabase::get_Path() const
{
    return m_szPath;
}


void MsiDatabase::set_Path(PCWSTR pszPath)
{
    m_szPath = pszPath;
}


void MsiDatabase::Load()
{
    UINT rc = MsiOpenDatabaseW(m_szPath, MSIDBOPEN_READONLY, &m_phDatabase);
    if (rc)
    {
        throw MsiException(rc, L"Failed to open.");
    }
}


IMsiTable* MsiDatabase::GetTable(PCWSTR pszName)
{
    IMsiTable* pTable;
    TableMap::const_iterator iter = m_tables.find(pszName);
    if (iter != m_tables.end())
    {
        pTable = iter->second;
    }
    else
    {
        pTable = new MsiTable(pszName, *this);
        m_tables.insert(TableMapEntry(pszName, pTable));
    }
    pTable->AddRef();
    return pTable;
}


IMsiFileRowCollection* MsiDatabase::CreateFileRowCollection()
{
    return new MsiFileRowCollection();
}


void MsiDatabase::AddByFileName(IMsiFileRowCollection* pCollection, PCWSTR pszPattern)
{
    RefPtr<IMsiTable> pFile = GetTable(L"File");
    for (long rowNo = 0; rowNo < pFile->Rows.Count; rowNo++)
    {
        const IMsiRow& row = pFile->Rows[rowNo];
        PCWSTR pszLongName = row[L"FileName"].ToFilename()->LongName;
        if (Path::WildcardMatch(pszPattern, pszLongName))
        {
            PCWSTR pszFile = row[L"File"].ToIdentifier()->Text;
            pCollection->Add(pszFile, rowNo);
        }
    }
}


void MsiDatabase::AddByFeature(IMsiFileRowCollection* pCollection, PCWSTR pszFeature)
{
    RefPtr<IMsiTable> pFile = GetTable(L"File");
    RefPtr<IMsiTable> pFeatureComponents = GetTable(L"FeatureComponents");
    long rowNo = pFeatureComponents->Find(L"Feature_", pszFeature);
    while (rowNo >= 0L)
    {
        const IMsiRow& rowFeatureComponents = pFeatureComponents->Rows[rowNo];
        PCWSTR pszComponent = rowFeatureComponents[L"Component_"].ToIdentifier()->Text;
        long rowNoFile = pFile->Find(L"Component_", pszComponent);
        while (rowNoFile >= 0L)
        {
            const IMsiRow& rowFile = pFile->Rows[rowNoFile];
            PCWSTR pszFile = rowFile[L"File"].ToIdentifier()->Text;
            pCollection->Add(pszFile, rowNoFile);
            rowNoFile = pFile->Find(L"Component_", pszComponent, rowNoFile + 1);
        }
        rowNo = pFeatureComponents->Find(L"Feature_", pszFeature, rowNo + 1);
    }
}


void MsiDatabase::ExtractFiles(IMsiFileRowCollection* pCollection, PCWSTR pszOutputDirectory, bool bFlat, IMsiDatabaseExtractFilesCallback* pCallback)
{
    struct LocalContext
        : public ICabinetExtractCallbacks
    {
        MsiDatabase* m_pThis;
        IMsiFileRowCollection* m_pCollection;
        PCWSTR m_pszOutputDirectory;
        IMsiDatabaseExtractFilesCallback* m_pCallback;
        const IMsiRow* m_pCurrentRow;
        unsigned long m_errorCode;
        PCWSTR m_pszErrorPath;
        RefPtr<IMsiTable> m_pFileTable;
        RefPtr<IMsiTable> m_pComponentTable;
        RefPtr<IMsiTable> m_pDirectoryTable;
        RefPtr<ICabinet> m_pCabinet;

        LocalContext(MsiDatabase* pThis, IMsiFileRowCollection* pCollection, PCWSTR pszOutputDirectory, bool bFlat, IMsiDatabaseExtractFilesCallback* pCallback)
            : m_pThis(pThis)
            , m_pCollection(pCollection)
            , m_pszOutputDirectory(pszOutputDirectory)
            , m_pCallback(pCallback)
            , m_pCurrentRow(nullptr)
            , m_errorCode(ERROR_SUCCESS)
            , m_pszErrorPath(nullptr)
            , m_pFileTable(pThis->GetTable(L"File"))
            , m_pComponentTable()
            , m_pDirectoryTable()
            , m_pCabinet(CabinetFactory::Create())
        {
            if (!bFlat)
            {
                m_pComponentTable = m_pThis->GetTable(L"Component");
                m_pComponentTable->CreateIndex();
                m_pDirectoryTable = m_pThis->GetTable(L"Directory");
                m_pDirectoryTable->CreateIndex();
            }
        }

        void Extract()
        {
            RefPtr<IMsiTable> pMedia = m_pThis->GetTable(L"Media");
            for (long rowNo = 0; rowNo < pMedia->Rows.Count; rowNo++)
            {
                const IMsiRow& row = pMedia->Rows[rowNo];
                const IMsiData& data = row[L"Cabinet"];
                if (!data.IsNull)
                {
                    PCWSTR pszCab = data.ToCabinet()->Text;
                    if (pszCab[0] == L'#')
                    {
                        ExtractFromStreams(&pszCab[1]);
                    }
                    else
                    {
                        ExtractFromFile(Path::Combine(Path::GetDirectoryName(m_pThis->m_szPath), pszCab));
                    }
                }
            }
        }

        void ExtractFromStreams(PCWSTR pszName)
        {
            RefPtr<IMsiTable> pStreams = m_pThis->GetTable(L"_Streams");
            long rowNo = pStreams->Find(L"Name", pszName);
            if (rowNo >= 0L)
            {
                const IMsiRow& row = pStreams->Rows[rowNo];
                const IMsiBinary* pBin = row[L"Data"].ToBinary();
                try
                {
                    m_pCabinet->SetData(pBin->Ptr, pBin->Len);
                    m_pCabinet->Extract(*this);
                }
                catch (CabinetException e)
                {
                    if (e.Type == CabinetError::AbortedByUser)
                    {
                        throw Win32Exception(m_errorCode, L"Failed to extract \"%s\".", m_pszErrorPath);
                    }
                    else
                    {
                        throw;
                    }
                }
            }
        }

        void ExtractFromFile(PCWSTR pszPath)
        {
            try
            {
                m_pCabinet->Path = pszPath;
                m_pCabinet->Extract(*this);
            }
            catch (CabinetException e)
            {
                if (e.Type == CabinetError::AbortedByUser)
                {
                    throw Win32Exception(m_errorCode, L"Failed to extract \"%s\".", m_pszErrorPath);
                }
                else
                {
                    throw;
                }
            }
        }

        virtual String CabinetExtractGetPath(PCWSTR pszFile)
        {
            long rowNo = m_pCollection->Find(pszFile);
            if (rowNo >= 0L)
            {
                m_pCurrentRow = &m_pFileTable->Rows[rowNo];
                return Path::Combine(GetOutputDirectory(), (*m_pCurrentRow)[L"FileName"].ToFilename()->LongName);
            }
            return String::Empty;
        }

        virtual void CabinetExtractOnStart(PCWSTR pszFile, PCWSTR pszPath)
        {
        }

        virtual bool CabinetExtractOnEnd(PCWSTR pszFile, PCWSTR pszPath)
        {
            bool bRet = true;
            if (m_pCallback)
            {
                bRet = m_pCallback->OnMsiDatabaseExtract(pszFile, pszPath, ERROR_SUCCESS);
            }
            m_pCollection->Remove(pszFile);
            m_pCurrentRow = nullptr;
            return bRet;
        }

        virtual bool CabinetExtractOnError(PCWSTR pszFile, PCWSTR pszPath, unsigned long error)
        {
            bool bRet = false;
            m_errorCode = error;
            m_pszErrorPath = pszPath;
            if (m_pCallback)
            {
                bRet = m_pCallback->OnMsiDatabaseExtract(pszFile, pszPath, error);
            }
            m_pCollection->Remove(pszFile);
            m_pCurrentRow = nullptr;
            return bRet;
        }

        PCWSTR GetOutputDirectory()
        {
            PCWSTR pszPath;
            if (m_pComponentTable.Ptr)
            {
                long rowNo = m_pComponentTable->Find(L"Component", (*m_pCurrentRow)[L"Component_"].ToIdentifier()->Text);
                if (rowNo >= 0L)
                {
                    const IMsiRow& row = m_pComponentTable->Rows[rowNo];
                    pszPath = m_pThis->GetDirectoryPath(row[L"Directory_"].ToIdentifier()->Text);
                    pszPath = Path::Combine(m_pszOutputDirectory, pszPath);
                }
                else
                {
                    pszPath = m_pszOutputDirectory;
                }
            }
            else
            {
                pszPath = m_pszOutputDirectory;
            }
            Path::ValidateDirectory(pszPath);
            return pszPath;
        }

    } ctx(this, pCollection, pszOutputDirectory, bFlat, pCallback);

    ctx.Extract();
}


String MsiDatabase::GetDirectoryPath(PCWSTR pszDirectory)
{
    std::map<String, String>::const_iterator iter = m_dirPaths.find(pszDirectory);
    if (iter != m_dirPaths.end())
    {
        return iter->second;
    }
    RefPtr<IMsiTable> pDirectoryTable = GetTable(L"Directory");
    pDirectoryTable->CreateIndex();
    String next(pszDirectory);
    String path(L".");
    while (true)
    {
        long rowNo = pDirectoryTable->Find(L"Directory", next);
        if (rowNo < 0L)
        {
            break;
        }
        const IMsiRow& row = pDirectoryTable->Rows[rowNo];
        const IMsiFilename& defaultDir = *row[L"DefaultDir"].ToFilename();
        if (Path::Compare(defaultDir.LongName, L"."))
        {
            if (Path::Compare(path, L"."))
            {
                path = Path::Combine(defaultDir.LongName, path);
            }
            else
            {
                path = defaultDir.LongName;
            }
        }
        const IMsiIdentifier& parentDir = *row[L"Directory_Parent"].ToIdentifier();
        if (parentDir.IsNull)
        {
            break;
        }
        next = parentDir.Text;
        if (!String::Compare(next, L"TARGETDIR"))
        {
            if (Path::Compare(path, L"."))
            {
                path = Path::Combine(L"TARGETDIR", path);
            }
            else
            {
                path = L"TARGETDIR";
            }
            break;
        }
    }
    m_dirPaths.insert(ValueMapEntry(pszDirectory, path));
    return path;
}
