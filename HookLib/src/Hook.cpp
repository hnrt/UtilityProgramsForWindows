#include "pch.h"
#include "HookInternal.h"
#include "hnrt/SpinLock.h"
#include "hnrt/Interlocked.h"


using namespace hnrt;


static SpinLock::Type theLock = SPIN_LOCK_INITIALIZER;
static HookInternal* theInstance = nullptr;


extern "C" static void __cdecl UninstallHook(void)
{
	SpinLock spin(theLock);
	if (theInstance)
	{
		delete Interlocked<HookInternal*>::ExchangePointer(&theInstance, nullptr);
	}
}


Hook& Hook::Instance()
{
	SpinLock spin(theLock);
	if (!theInstance)
	{
		theInstance = new HookInternal();
		atexit(UninstallHook);
	}
	return *theInstance;
}
