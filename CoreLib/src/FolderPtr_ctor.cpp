#include "pch.h"
#include "hnrt/FolderPtr.h"
#include "hnrt/ComException.h"
#include "hnrt/ShellDispatchPtr.h"
#include "hnrt/FolderItemPtr.h"
#include "hnrt/Variant.h"
#include "hnrt/Interlocked.h"


using namespace hnrt;


FolderPtr::FolderPtr(Folder* ptr)
	: m_ptr(ptr)
{
}


FolderPtr::FolderPtr(const FolderPtr& src)
	: m_ptr(src.m_ptr)
{
	m_ptr->AddRef();
}


FolderPtr& FolderPtr::operator =(const FolderPtr& src)
{
    src.m_ptr->AddRef();
    Interlocked<Folder*>::ExchangePointer(&m_ptr, src.m_ptr)->Release();
    return *this;
}


FolderPtr FolderPtr::Get(PCWSTR pszPath, const ShellDispatchPtr& pShellDispatch)
{
    Folder* ptr = NULL;
    Variant varPath(VT_BSTR, pszPath);
    HRESULT hr = pShellDispatch->NameSpace(varPath, &ptr);
    if (hr == S_OK)
    {
        return FolderPtr(ptr);
    }
    else
    {
        throw ComException(hr, L"Folder unavailable.");
    }
}


FolderPtr FolderPtr::Get(const FolderItemPtr& pFolderItem)
{
    IDispatch* pDispatch = NULL;
    HRESULT hr = pFolderItem->get_GetFolder(&pDispatch);
    if (hr == S_OK)
    {
        return FolderPtr(static_cast<Folder*>(pDispatch));
    }
    else
    {
        throw ComException(hr, L"FolderItem::get_GetFolder failed.");
    }
}
