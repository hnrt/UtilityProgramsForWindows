#include "pch.h"
#include "MyFileDataFeeder.h"
#include "MyToolbox.h"
#include "hnrt/WindowHandle.h"


using namespace hnrt;


MyFileDataFeeder::MyFileDataFeeder(MyToolbox& app)
	: FileDataFeeder()
    , m_app(app)
    , m_LastTick(0ULL)
{
}


bool MyFileDataFeeder::HasNext()
{
    bool bRet = FileDataFeeder::HasNext();
    ULONGLONG tick = GetTickCount64();
    if (m_LastTick + 100 <= tick || !bRet)
    {
        m_LastTick = tick;
        if (!m_app.OnFeederNotify(TotalLength))
        {
            return false;
        }
    }
    return bRet;
}
