#include "Configuration.h"
#include <Windows.h>
#include "hnrt/Debug.h"
#include "hnrt/String.h"
#include "hnrt/Path.h"


using namespace hnrt;


#define DIR1 L"hnrt"
#define DIR2 L"MyGhostTypist"


static PCWSTR GetModuleDirectoryName()
{
    WCHAR psz[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, psz, MAX_PATH);
    return Path::GetDirectoryName(psz);
}


Configuration::Configuration()
    : RefObj()
    , m_pszAppDir(Path::Combine(Path::GetKnownFolder(FOLDERID_RoamingAppData), DIR1, DIR2))
    , m_pszInstallDir(GetModuleDirectoryName())
    , m_CredentialsList()
    , m_Window()
    , m_Button()
    , m_TypingInterval(10)
    , m_TypingDelay(1000)
    , m_TargetList()
{
    DBGFNC(L"Configuration::ctor");
}


Configuration::~Configuration()
{
    DBGFNC(L"Configuration::dtor");
}
