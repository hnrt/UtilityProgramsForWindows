#pragma once

#include <oleauto.h>

namespace hnrt
{
	struct Variant : VARIANT
	{
		Variant();
		Variant(VARTYPE, ...);
		Variant(const Variant&);
		~Variant();

		Variant& Assign(const Variant&);
		Variant& Assign(VARTYPE, ...);
		void VaAssign(VARTYPE, va_list);

		Variant& operator =(const Variant&);
	};

	inline Variant& Variant::operator =(const Variant& src)
	{
		return Assign(src);
	}
}