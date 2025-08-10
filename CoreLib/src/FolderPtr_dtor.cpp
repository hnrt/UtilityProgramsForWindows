#include "pch.h"
#include "hnrt/FolderPtr.h"
#include "hnrt/Interlocked.h"


using namespace hnrt;


FolderPtr::~FolderPtr()
{
	Interlocked<Folder*>::ExchangePointer(&m_ptr, NULL)->Release();
}
