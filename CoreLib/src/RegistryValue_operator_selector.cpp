#include "pch.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/Exception.h"


using namespace hnrt;


PCWSTR RegistryValue::operator [](SIZE_T index) const
{
	if (m_dwType == REG_MULTI_SZ)
	{
		if (index < m_dwSize)
		{
			PWCHAR pCur = m_value.psz;
			for (SIZE_T iCur = 0; iCur < index; iCur++)
			{
				pCur += wcslen(pCur) + 1;
			}
			return pCur;
		}
		return nullptr;
	}
	else
	{
		throw Exception(L"RegistryValue::operator []: Bad type.");
	}
}
