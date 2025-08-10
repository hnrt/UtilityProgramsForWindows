#include "pch.h"
#include "hnrt/ShellDispatchPtr.h"
#include "hnrt/ComException.h"
#include "hnrt/Interlocked.h"


using namespace hnrt;


ShellDispatchPtr::ShellDispatchPtr(IShellDispatch6* ptr)
	: m_ptr(ptr)
{
}


ShellDispatchPtr::ShellDispatchPtr(const ShellDispatchPtr& src)
    : m_ptr(src.m_ptr)
{
    m_ptr->AddRef();
}


ShellDispatchPtr& ShellDispatchPtr::operator =(const ShellDispatchPtr& src)
{
    src.m_ptr->AddRef();
    Interlocked<IShellDispatch6*>::ExchangePointer(&m_ptr, src.m_ptr)->Release();
    return *this;
}


ShellDispatchPtr& ShellDispatchPtr::Create()
{
    IShellDispatch6* ptr = NULL;
    HRESULT hr = CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch6, reinterpret_cast<LPVOID*>(&ptr));
    if (hr == S_OK)
    {
        return *(new ShellDispatchPtr(ptr));
    }
    else
    {
        throw ComException(hr, L"IShellDispatchPtr unavailable.");
    }
}
