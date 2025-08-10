#include "pch.h"
#include "hnrt/FolderItemsPtr.h"
#include "hnrt/FolderPtr.h"
#include "hnrt/ComException.h"
#include "hnrt/Interlocked.h"


using namespace hnrt;


FolderItemsPtr::FolderItemsPtr(FolderItems* ptr)
	: m_ptr(ptr)
{
}


FolderItemsPtr::FolderItemsPtr(const FolderItemsPtr& src)
	: m_ptr(src.m_ptr)
{
	m_ptr->AddRef();
}


FolderItemsPtr& FolderItemsPtr::operator =(const FolderItemsPtr& src)
{
	src.m_ptr->AddRef();
	Interlocked<FolderItems*>::ExchangePointer(&m_ptr, src.m_ptr)->Release();
	return *this;
}


FolderItemsPtr FolderItemsPtr::Get(const FolderPtr& pFolder)
{
	FolderItems* ptr = NULL;
	HRESULT hr = pFolder->Items(&ptr);
	if (hr == S_OK)
	{
		return FolderItemsPtr(ptr);
	}
	else
	{
		throw ComException(hr, L"Folder::Items failed.");
	}
}
