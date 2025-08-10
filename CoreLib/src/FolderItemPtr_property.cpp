#include "pch.h"
#include "hnrt/FolderItemPtr.h"
#include "hnrt/ComException.h"


using namespace hnrt;


String FolderItemPtr::get_Name() const
{
	BSTR bstr = NULL;
	HRESULT hr = m_ptr->get_Name(&bstr);
	if (hr == S_OK)
	{
		String str(bstr);
		SysFreeString(bstr);
		return str;
	}
	else
	{
		throw ComException(hr, L"FolderItem::get_Name failed.");
	}
}


String FolderItemPtr::get_Type() const
{
	BSTR bstr = NULL;
	HRESULT hr = m_ptr->get_Type(&bstr);
	if (hr == S_OK)
	{
		String str(bstr);
		SysFreeString(bstr);
		return str;
	}
	else
	{
		throw ComException(hr, L"FolderItem::get_Type failed.");
	}
}


LONG FolderItemPtr::get_Size() const
{
	LONG size = 0L;
	HRESULT hr = m_ptr->get_Size(&size);
	if (hr == S_OK)
	{
		return size;
	}
	else
	{
		throw ComException(hr, L"FolderItem::get_Size failed.");
	}
}


BOOL FolderItemPtr::get_IsFolder() const
{
	VARIANT_BOOL b = 0;
	HRESULT hr = m_ptr->get_IsFolder(&b);
	if (hr == S_OK)
	{
		return b != 0 ? TRUE : FALSE;
	}
	else
	{
		throw ComException(hr, L"FolderItem::get_IsFolder failed.");
	}
}
