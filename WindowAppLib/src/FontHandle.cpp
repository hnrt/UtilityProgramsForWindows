#include "pch.h"
#include "hnrt/Interlocked.h"
#include "hnrt/FontHandle.h"


using namespace hnrt;


FontHandle::FontHandle(HFONT h)
	: m_h(h)
{
}


FontHandle::~FontHandle()
{
	HFONT h = Interlocked<HFONT>::ExchangePointer(&m_h, nullptr);
	if (h)
	{
		DeleteObject(h);
	}
}


FontHandle& FontHandle::operator =(HFONT h)
{
	h = Interlocked<HFONT>::ExchangePointer(&m_h, h);
	if (h)
	{
		DeleteObject(h);
	}
	return *this;
}
