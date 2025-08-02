#include "pch.h"
#include "hnrt/Variant.h"


using namespace hnrt;


Variant::~Variant()
{
	VariantClear(this);
}
