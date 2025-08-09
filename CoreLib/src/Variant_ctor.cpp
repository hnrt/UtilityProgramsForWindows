#include "pch.h"
#include "hnrt/Variant.h"
#include "hnrt/Exception.h"


using namespace hnrt;


Variant::Variant()
	: VARIANT()
{
	VariantInit(this);
}


Variant::Variant(VARTYPE vt_, ...)
	: VARIANT()
{
	this->vt = VT_EMPTY;
	va_list argList;
	va_start(argList, vt_);
	VaAssign(vt_, argList);
	va_end(argList);
}


Variant::Variant(const Variant& src)
	: VARIANT()
{
	Assign(src);
}
