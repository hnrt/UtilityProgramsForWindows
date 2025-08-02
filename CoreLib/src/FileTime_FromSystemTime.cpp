#include "pch.h"
#include "hnrt/Time.h"


using namespace hnrt;


FileTime& FileTime::FromSystemTime()
{
	GetSystemTimeAsFileTime(this);
	return *this;
}
