#include "pch.h"
#include "hnrt/FolderItemPtr.h"
#include "hnrt/FolderItemsPtr.h"
#include "hnrt/ComException.h"
#include "hnrt/Interlocked.h"
#include "hnrt/Variant.h"


using namespace hnrt;


FolderItemPtr::FolderItemPtr(FolderItem* ptr)
	: m_ptr(ptr)
{
}


FolderItemPtr::FolderItemPtr(const FolderItemPtr& src)
	: m_ptr(src.m_ptr)
{
	m_ptr->AddRef();
}


FolderItemPtr& FolderItemPtr::operator =(const FolderItemPtr& src)
{
	src.m_ptr->AddRef();
	Interlocked<FolderItem*>::ExchangePointer(&m_ptr, src.m_ptr)->Release();
	return *this;
}


FolderItemPtr FolderItemPtr::Get(long index, const FolderItemsPtr& pFolderItems)
{
	FolderItem* ptr = NULL;
	Variant varIndex(VT_I4, index);
	HRESULT hr = pFolderItems->Item(varIndex, &ptr);
	if (hr == S_OK)
	{
		return FolderItemPtr(ptr);
	}
	else
	{
		throw ComException(hr, L"FolderItems::Item failed.");
	}
}
