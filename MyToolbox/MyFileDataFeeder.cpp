#include "pch.h"
#include "MyFileDataFeeder.h"
#include "MyToolbox.h"


using namespace hnrt;


MyFileDataFeeder::MyFileDataFeeder()
	: FileDataFeeder()
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
        if (!GetApp<MyToolbox>().OnFeederNotify(TotalLength))
        {
            return false;
        }
    }
    return bRet;
}
