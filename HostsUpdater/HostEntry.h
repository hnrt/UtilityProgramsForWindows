#pragma once

#include "TextLocatorCollection.h"

namespace hnrt
{
	struct HostEntry
	{
		TextLocator Address;
		TextLocatorCollection Names;

		void* operator new(size_t, void*);
	};

	inline void* HostEntry::operator new(size_t unused, void* ptr)
	{
		return ptr;
	}
}
