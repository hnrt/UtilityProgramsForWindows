#include "pch.h"
#include "hnrt/ShellDispatchPtr.h"
#include "hnrt/Interlocked.h"


using namespace hnrt;


ShellDispatchPtr::~ShellDispatchPtr()
{
	Interlocked<IShellDispatch6*>::ExchangePointer(&m_ptr, NULL)->Release();
}
