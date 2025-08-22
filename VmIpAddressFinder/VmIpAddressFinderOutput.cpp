#include "VmIpAddressFinder.h"
#include <io.h>
#include <fcntl.h>
#include "hnrt/SpinLock.h"
#include "hnrt/Path.h"
#include "hnrt/Exception.h"
#include "hnrt/Debug.h"


using namespace hnrt;


static SpinLock::Type s_lock = SPIN_LOCK_INITIALIZER;
static bool s_bServiceMode = false;
static FILE* s_pOutStream = stdout;
static FILE* s_pErrStream = stderr;


void VmIpAddressFinder::OpenStream()
{
    DBGFNC(L"VmIpAddressFinder::OpenStream");
    String szPath = Path::Combine(Path::GetKnownFolder(FOLDERID_PublicDocuments), String::Format(L"%s.log", Instance().m_szName.Ptr));
    DBGPUT(L"Path=%s", szPath.Ptr);
    int fd = -1;
    errno_t err = _wsopen_s(&fd, szPath, _O_WRONLY | _O_CREAT | _O_APPEND | _O_U8TEXT, _SH_DENYWR, _S_IREAD | _S_IWRITE);
    if (err)
    {
        DBGPUT(L"Failed to open: %u", err);
        throw Exception(L"Failed to open \"%s\": %u", szPath.Ptr, err);
    }
    FILE* fp = _wfdopen(fd, L"a,ccs=UTF-8");
    if (!fp)
    {
        DBGPUT(L"Failed to open with file descriptor: %u", err);
        _close(fd);
        throw Exception(L"Failed to open \"%s\".", szPath.Ptr);
    }
    s_pOutStream = fp;
    s_pErrStream = fp;
    DBGPUT(L"Successfully done.");
}


void __cdecl VmIpAddressFinder::CloseStream()
{
    DBGFNC(L"VmIpAddressFinder::CloseStream");
    fclose(s_pOutStream);
}


void VmIpAddressFinder::EnableServiceMode()
{
    DBGFNC(L"VmIpAddressFinder::EnableServiceMode");
    SpinLock lock(s_lock);
    if (!s_bServiceMode)
    {
        s_bServiceMode = true;
        OpenStream();
        atexit(CloseStream);
    }
}


void VmIpAddressFinder::Put(PCWSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    VaPut(s_pOutStream, pszFormat, argList);
    va_end(argList);
}


void VmIpAddressFinder::PutError(PCWSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    VaPut(s_pErrStream, pszFormat, argList);
    va_end(argList);
}


void VmIpAddressFinder::VaPut(FILE* fp, PCWSTR pszFormat, va_list argList)
{
    SpinLock lock(s_lock);
    if (s_bServiceMode)
    {
        SYSTEMTIME t;
        GetLocalTime(&t);
        fwprintf(fp, L"%u-%02u-%02u %02u:%02u:%02u.%03u ", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
    }
    vfwprintf(fp, pszFormat, argList);
    fwprintf(fp, L"\n");
    fflush(fp);
}
