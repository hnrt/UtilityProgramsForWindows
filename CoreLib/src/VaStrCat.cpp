#include "pch.h"
#include "hnrt/StringCommons.h"
#include "hnrt/Exception.h"


using namespace hnrt;


template<typename T>
SIZE_T DoVaStrCatLen(StringOptions option, const T* str, va_list argptr)
{
	SIZE_T required = StrLen(str);
	va_list argptr2;
	va_copy(argptr2, argptr);
	switch (option)
	{
	case CONCAT9:
		required += StrLen(va_arg(argptr2, const T*));
		//FALLTHROUGH
	case CONCAT8:
		required += StrLen(va_arg(argptr2, const T*));
		//FALLTHROUGH
	case CONCAT7:
		required += StrLen(va_arg(argptr2, const T*));
		//FALLTHROUGH
	case CONCAT6:
		required += StrLen(va_arg(argptr2, const T*));
		//FALLTHROUGH
	case CONCAT5:
		required += StrLen(va_arg(argptr2, const T*));
		//FALLTHROUGH
	case CONCAT4:
		required += StrLen(va_arg(argptr2, const T*));
		//FALLTHROUGH
	case CONCAT3:
		required += StrLen(va_arg(argptr2, const T*));
		//FALLTHROUGH
	case CONCAT2:
		required += StrLen(va_arg(argptr2, const T*));
		break;
	case CONCAT:
		while (true)
		{
			const T* psz = va_arg(argptr2, const T*);
			if (!psz)
			{
				break;
			}
			required += StrLen(psz);
		}
		break;
	default:
		throw Exception(L"hnrt::VaStrCatLen: Bad option: %d", option);
	}
	va_end(argptr2);
	return required;
}


SIZE_T hnrt::VaStrCatLen(StringOptions option, const WCHAR* str, va_list argptr)
{
	return DoVaStrCatLen(option, str, argptr);
}


SIZE_T hnrt::VaStrCatLen(StringOptions option, const CHAR* str, va_list argptr)
{
	return DoVaStrCatLen(option, str, argptr);
}


template<typename T>
SIZE_T DoVaStrCat(StringOptions option, const T* str, va_list argptr, T* buf)
{
	T* pCur = buf + StrCpy(buf, str);
	switch (option)
	{
	case CONCAT9:
		pCur += StrCpy(pCur, va_arg(argptr, const T*));
		//FALLTHROUGH
	case CONCAT8:
		pCur += StrCpy(pCur, va_arg(argptr, const T*));
		//FALLTHROUGH
	case CONCAT7:
		pCur += StrCpy(pCur, va_arg(argptr, const T*));
		//FALLTHROUGH
	case CONCAT6:
		pCur += StrCpy(pCur, va_arg(argptr, const T*));
		//FALLTHROUGH
	case CONCAT5:
		pCur += StrCpy(pCur, va_arg(argptr, const T*));
		//FALLTHROUGH
	case CONCAT4:
		pCur += StrCpy(pCur, va_arg(argptr, const T*));
		//FALLTHROUGH
	case CONCAT3:
		pCur += StrCpy(pCur, va_arg(argptr, const T*));
		//FALLTHROUGH
	case CONCAT2:
		pCur += StrCpy(pCur, va_arg(argptr, const T*));
		break;
	case CONCAT:
		while (true)
		{
			const T* psz = va_arg(argptr, const T*);
			if (!psz)
			{
				break;
			}
			pCur += StrCpy(pCur, psz);
		}
		break;
	default:
		throw Exception(L"hnrt::VaStrCat: Bad option: %d", option);
	}
	return pCur - buf;
}


SIZE_T hnrt::VaStrCat(StringOptions option, const WCHAR* str, va_list argptr, WCHAR* buf)
{
	return DoVaStrCat(option, str, argptr, buf);
}


SIZE_T hnrt::VaStrCat(StringOptions option, const CHAR* str, va_list argptr, CHAR* buf)
{
	return DoVaStrCat(option, str, argptr, buf);
}
