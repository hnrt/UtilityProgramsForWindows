#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


String String::Substring(int startIndex) const
{
	int length = static_cast<int>(Len);
	if (startIndex < 0)
	{
		startIndex += length;
		if (startIndex < 0)
		{
			startIndex = 0;
		}
	}
	else if (startIndex > length)
	{
		startIndex = length;
	}
	return String(m_psz + startIndex, length - startIndex);
}


String String::Substring(int startIndex, int endIndex) const
{
	int length = static_cast<int>(Len);
	if (startIndex < 0)
	{
		startIndex += length;
		if (startIndex < 0)
		{
			startIndex = 0;
		}
	}
	else if (startIndex > length)
	{
		startIndex = length;
	}
	if (endIndex < 0)
	{
		endIndex += length;
	}
	if (endIndex < startIndex)
	{
		endIndex = startIndex;
	}
	else if (endIndex > length)
	{
		endIndex = length;
	}
	return String(m_psz + startIndex, endIndex - startIndex);
}
