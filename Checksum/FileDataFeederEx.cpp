#include "pch.h"
#include "FileDataFeederEx.h"
#include "Checksum.h"


using namespace hnrt;


FileDataFeederEx::FileDataFeederEx(PCWSTR pszFileName, HWND hDlg)
    : FileDataFeeder(pszFileName)
    , m_hDlg(hDlg)
    , m_LastTick(0ULL)
{
}


bool FileDataFeederEx::HasNext()
{
    bool bRet = FileDataFeeder::HasNext();
    ULONGLONG tick = GetTickCount64();
    if (m_LastTick + 100 <= tick || !bRet)
    {
        m_LastTick = tick;
        Checksum& app = *static_cast<Checksum*>(DialogApp::GetInstance(m_hDlg));
        app.SetResultHeader(m_hDlg, TotalLength);
        while (1)
        {
            int rc = app.TryProcessMessage();
            if (rc < 0)
            {
                return false;
            }
            else if (!rc)
            {
                break;
            }
        }
    }
    return bRet;
}
