#include "pch.h"
#include "hnrt/Time.h"


using namespace hnrt;


FileTime& FileTime::FromLocalTime()
{
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	return From(st);
}
