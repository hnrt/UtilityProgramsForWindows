#include "pch.h"
#include "hnrt/Time.h"


using namespace hnrt;


String hnrt::SystemTimeToString(const SYSTEMTIME& st)
{
	return String::Format(L"%04d-%02d-%02dT%02d:%02d:%02d.%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
}
