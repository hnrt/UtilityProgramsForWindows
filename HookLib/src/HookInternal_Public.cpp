#include "pch.h"
#include "HookInternal.h"
#include "hnrt/Time.h"


using namespace hnrt;


void HookInternal::SetTimeDisplacement(LONGLONG value)
{
    _InterlockedExchange64(&m_TimeDisplacement, value);
}


void HookInternal::SetDate(int year, int month, int day)
{
    SYSTEMTIME st1 = { 0 };
    GetLocalTime(&st1);
    SYSTEMTIME st2 = st1;
    st2.wYear = static_cast<WORD>(year);
    st2.wMonth = static_cast<WORD>(month);
    st2.wDay = static_cast<WORD>(day);
    FileTime ft1(st1);
    FileTime ft2(st2);
    SetTimeDisplacement(ft2.Intervals - ft1.Intervals);
}
