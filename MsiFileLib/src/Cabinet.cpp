#include "pch.h"
#include "Cabinet.h"
#include <fcntl.h>
#include "hnrt/String.h"
#include "hnrt/Path.h"
#include "hnrt/CabinetException.h"
#include "hnrt/Heap.h"


#pragma comment(lib,"Cabinet")


using namespace hnrt;


static DWORD s_dwTlsIndex = TlsAlloc();


////////////////////////////////////////////////////////////////////////////
////
////  C L A S S   I M P L E M E N T E D
////
////////////////////////////////////////////////////////////////////////////


static const WCHAR s_szOnMemoryPseudoPath[] = { L"A:\\OnMemory.cab" };


Cabinet::Cabinet()
    : m_refs(1)
    , m_hfdi(nullptr)
    , m_pData(nullptr)
    , m_cbSize(0)
    , m_pszInputPath(nullptr)
    , m_pCallbacks(nullptr)
    , m_State(CabinetExtractionState::Idle)
    , m_pszOutputPath(nullptr)
{
    ZeroMemory(&m_erf, sizeof(m_erf));
    m_hfdi = FDICreate(Allocate, Free, Open, Read, Write, Close, Seek, cpu80386, &m_erf);
    if (!m_hfdi)
    {
        throw Exception(L"FDICreate failed unexpectedly.");
    }
}


Cabinet::~Cabinet()
{
    FDIDestroy(m_hfdi);
}


long Cabinet::AddRef()
{
    long refs = _InterlockedIncrement(&m_refs);
    return refs;
}


long Cabinet::Release()
{
    long refs = _InterlockedDecrement(&m_refs);
    if (!refs)
    {
        delete this;
    }
    return refs;
}


PCWSTR Cabinet::get_Path() const
{
    return m_pszInputPath;
}


void Cabinet::set_Path(PCWSTR pszPath)
{
    m_pData = nullptr;
    m_cbSize = 0;
    m_pszInputPath = String::Copy(pszPath);
}


void Cabinet::SetData(const void* pData, size_t cbSize)
{
    m_pData = pData;
    m_cbSize = cbSize;
    m_pszInputPath = s_szOnMemoryPseudoPath;
}


const BYTE* Cabinet::get_Ptr() const
{
    return reinterpret_cast<const BYTE*>(m_pData);
}


size_t Cabinet::get_Len() const
{
    return m_cbSize;
}


void Cabinet::Extract(ICabinetExtractCallbacks& callbacks)
{
    if (!TlsSetValue(s_dwTlsIndex, this))
    {
        throw Exception(L"CabinetReader: Thread local storage unavailable.");
    }
    m_State = CabinetExtractionState::Pending;
    m_pCallbacks = &callbacks;
    char szDirPath[MAX_PATH];
    char szFileName[MAX_PATH];
    strcpy_s(szDirPath, AcpString(Path::GetDirectoryName(m_pszInputPath, true)));
    strcpy_s(szFileName, AcpString(Path::GetFileName(m_pszInputPath)));
    BOOL bRet = FDICopy(m_hfdi, szFileName, szDirPath, 0, Notify, NULL, this);
    if (bRet)
    {
        if (m_State != CabinetExtractionState::Pending)
        {
            throw CabinetException(-1, -1);
        }
    }
    else if (m_erf.fError)
    {
        throw CabinetException(m_erf.erfOper, m_erf.erfType);
    }
    else
    {
        throw CabinetException(-1, -1);
    }
}


void* DIAMONDAPI Cabinet::Allocate(ULONG cb)
{
    return Malloc(cb);
}


void DIAMONDAPI Cabinet::Free(void* pv)
{
    free(pv);
}


INT_PTR DIAMONDAPI Cabinet::Open(LPSTR pszFile, int oflag, int pmode)
{
    Cabinet* pThis = reinterpret_cast<Cabinet*>(TlsGetValue(s_dwTlsIndex));
    if (!pThis)
    {
        return static_cast<INT_PTR>(-1);
    }

    ICabinetStream* pStream;

    if (!Path::Compare(pszFile, AcpString(s_szOnMemoryPseudoPath)))
    {
        pStream = new CabinetMemoryStream(pThis->m_pData, pThis->m_cbSize);
    }
    else
    {
        pStream = new CabinetFileStream();
    }

    if (!pStream->Open(pszFile, oflag, pmode))
    {
        return static_cast<INT_PTR>(-1);
    }

    return reinterpret_cast<INT_PTR>(pStream);
}


UINT DIAMONDAPI Cabinet::Read(INT_PTR hf, void* pv, UINT cb)
{
    ICabinetStream* pStream = reinterpret_cast<ICabinetStream*>(hf);
    return pStream ? pStream->Read(pv, cb) : (UINT)-1;
}


UINT DIAMONDAPI Cabinet::Write(INT_PTR hf, void* pv, UINT cb)
{
    ICabinetStream* pStream = reinterpret_cast<ICabinetStream*>(hf);
    return pStream ? pStream->Write(pv, cb) : (UINT)-1;
}


int DIAMONDAPI Cabinet::Close(INT_PTR hf)
{
    ICabinetStream* pStream = reinterpret_cast<ICabinetStream*>(hf);
    return pStream ? pStream->Close() : -1;
}


long DIAMONDAPI Cabinet::Seek(INT_PTR hf, long dist, int seektype)
{
    ICabinetStream* pStream = reinterpret_cast<ICabinetStream*>(hf);
    return pStream ? pStream->Seek(dist, seektype) : -1L;
}


#pragma warning(push)
#pragma warning(disable:26812)
INT_PTR DIAMONDAPI Cabinet::Notify(FDINOTIFICATIONTYPE fdint, PFDINOTIFICATION pfdin)
{
    Cabinet* pThis = reinterpret_cast<Cabinet*>(pfdin->pv);
    switch (fdint)
    {
    case fdintCABINET_INFO:
        return 0; // SUCCESS
    case fdintPARTIAL_FILE:
        return 0; // SUCCESS
    case fdintCOPY_FILE:
        return pThis->OnOpen(pfdin->psz1);
    case fdintCLOSE_FILE_INFO:
        return pThis->OnClose(pfdin->psz1, pfdin->hf);
    case fdintNEXT_CABINET:
        return 0; // SUCCESS
    case fdintENUMERATE:
        return 0; // SUCCESS
    default:
        return -1; // ABORT
    }
}
#pragma warning(pop)


INT_PTR Cabinet::OnOpen(PCSTR pszFileAcp)
{
    if (m_State == CabinetExtractionState::Pending)
    {
        String strFile(pszFileAcp);
        PCWSTR pszPath = m_pCallbacks->CabinetExtractGetPath(strFile);
        if (pszPath)
        {
            HANDLE h = CreateFileW(pszPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if (h != INVALID_HANDLE_VALUE)
            {
                ICabinetStream* pStream = new CabinetFileStream(h);
                m_State = CabinetExtractionState::Started;
                m_pszOutputPath = pszPath;
                m_pCallbacks->CabinetExtractOnStart(strFile, pszPath);
                return reinterpret_cast<INT_PTR>(pStream);
            }
            else
            {
                m_State = CabinetExtractionState::OpenFailure;
                bool bContinue = m_pCallbacks->CabinetExtractOnError(strFile, pszPath, GetLastError());
                return bContinue ? 0 : -1; // 0=SKIP / -1=ABORT
            }
        }
        else
        {
            return 0; // SKIP
        }
    }
    else
    {
        return -1; // ABORT
    }
}


INT_PTR Cabinet::OnClose(PCSTR pszFileAcp, INT_PTR hf)
{
    if (m_State == CabinetExtractionState::Started)
    {
        ICabinetStream* pStream = reinterpret_cast<ICabinetStream*>(hf);
        pStream->Close();
        m_State = CabinetExtractionState::Ended;
        bool bContinue = m_pCallbacks->CabinetExtractOnEnd(String(pszFileAcp), m_pszOutputPath);
        m_State = CabinetExtractionState::Pending;
        return bContinue ? TRUE : -1; // TRUE=SUCCESS / -1=ABORT
    }
    else
    {
        return -1; // ABORT
    }
}


////////////////////////////////////////////////////////////////////////////
////
////  F I L E   S T R E A M
////
////////////////////////////////////////////////////////////////////////////


CabinetFileStream::CabinetFileStream(HANDLE h)
    : m_h(h)
{
}


CabinetFileStream::~CabinetFileStream()
{
    if (m_h != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_h);
    }
}


bool DIAMONDAPI CabinetFileStream::Open(LPSTR pszFile, int oflag, int pmode)
{
    HANDLE hFile = NULL;
    DWORD dwDesiredAccess = 0;
    DWORD dwCreationDisposition = 0;

    UNREFERENCED_PARAMETER(pmode);

    if ((oflag & _O_RDWR))
    {
        dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
    }
    else if ((oflag & _O_WRONLY))
    {
        dwDesiredAccess = GENERIC_WRITE;
    }
    else
    {
        dwDesiredAccess = GENERIC_READ;
    }

    if ((oflag & _O_CREAT))
    {
        dwCreationDisposition = CREATE_ALWAYS;
    }
    else
    {
        dwCreationDisposition = OPEN_EXISTING;
    }

    m_h = CreateFileA(pszFile, dwDesiredAccess, FILE_SHARE_READ, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);

    return m_h != INVALID_HANDLE_VALUE;
}


UINT DIAMONDAPI CabinetFileStream::Read(void* pv, UINT cb)
{
    DWORD dwBytesRead = 0;
    if (!ReadFile(m_h, pv, cb, &dwBytesRead, NULL))
    {
        dwBytesRead = (DWORD)-1L;
    }
    return dwBytesRead;
}


UINT DIAMONDAPI CabinetFileStream::Write(void* pv, UINT cb)
{
    DWORD dwBytesWritten = 0;
    if (!WriteFile(m_h, pv, cb, &dwBytesWritten, NULL))
    {
        dwBytesWritten = (DWORD)-1;
    }
    return dwBytesWritten;
}


int DIAMONDAPI CabinetFileStream::Close()
{
    int rc = CloseHandle(m_h) ? 0 : -1;
    m_h = INVALID_HANDLE_VALUE;
    delete this;
    return rc;
}


long DIAMONDAPI CabinetFileStream::Seek(long dist, int seektype)
{
    return SetFilePointer(m_h, dist, NULL, seektype);
}


////////////////////////////////////////////////////////////////////////////
////
////  M E M O R Y   S T R E A M
////
////////////////////////////////////////////////////////////////////////////


CabinetMemoryStream::CabinetMemoryStream(const void* pData, size_t cbSize)
    : m_pStart(reinterpret_cast<const BYTE*>(pData))
    , m_pEnd(reinterpret_cast<const BYTE*>(pData) + cbSize)
    , m_pCurrent(reinterpret_cast<const BYTE*>(pData))
{
}


CabinetMemoryStream::~CabinetMemoryStream()
{
}


bool DIAMONDAPI CabinetMemoryStream::Open(LPSTR pszFile, int oflag, int pmode)
{
    return true;
}


UINT DIAMONDAPI CabinetMemoryStream::Read(void* pv, UINT cb)
{
    UINT remaining = static_cast<UINT>(m_pEnd - m_pCurrent);
    if (cb > remaining)
    {
        cb = remaining;
    }
    memcpy(pv, m_pCurrent, cb);
    m_pCurrent += cb;
    return cb;
}


UINT DIAMONDAPI CabinetMemoryStream::Write(void* pv, UINT cb)
{
    throw Exception(L"CabinetMemoryStream::Write: Unexpected.");
}


int DIAMONDAPI CabinetMemoryStream::Close()
{
    delete this;
    return 0;
}


long DIAMONDAPI CabinetMemoryStream::Seek(long dist, int seektype)
{
    switch (seektype)
    {
    case 0: // SEEK_SET
        m_pCurrent = m_pStart + dist;
        break;
    case 1: // SEEK_CUR
        m_pCurrent += dist;
        break;
    case 2: // SEEK_END
        m_pCurrent = m_pEnd + dist;
        break;
    default:
        throw Exception(L"CabinetMemoryStream::Write: Invalid seektype.");
    }
    if (m_pCurrent < m_pStart)
    {
        m_pCurrent = m_pStart;
    }
    else if (m_pCurrent > m_pEnd)
    {
        m_pCurrent = m_pEnd;
    }
    return static_cast<long>(m_pCurrent - m_pStart);
}
