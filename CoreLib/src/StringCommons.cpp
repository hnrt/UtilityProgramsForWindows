#include "pch.h"
#include "hnrt/StringCommons.h"
#include "hnrt/Exception.h"


using namespace hnrt;


SIZE_T StringCommons::Length(const WCHAR* psz, SSIZE_T cch)
{
	if (psz && cch)
	{
		return cch < 0 ? wcslen(psz) : wcsnlen(psz, cch);
	}
	else
	{
		return 0;
	}
}


SIZE_T StringCommons::Length(const CHAR* psz, SSIZE_T cch)
{
	if (psz && cch)
	{
		return cch < 0 ? strlen(psz) : strnlen(psz, cch);
	}
	else
	{
		return 0;
	}
}


SIZE_T StringCommons::Fill(WCHAR* psz, WCHAR c, SIZE_T cch)
{
	wmemset(psz, c, cch);
	return cch;
}


SIZE_T StringCommons::Fill(CHAR* psz, CHAR c, SIZE_T cch)
{
	memset(psz, c, cch);
	return cch;
}



SIZE_T StringCommons::Copy(WCHAR* psz1, const WCHAR* psz2, SSIZE_T cch)
{
	cch = Length(psz2, cch);
	wmemcpy_s(psz1, cch, psz2, cch);
	psz1[cch] = L'\0';
	return cch;
}


SIZE_T StringCommons::Copy(CHAR* psz1, const CHAR* psz2, SSIZE_T cch)
{
	cch = Length(psz2, cch);
	memcpy_s(psz1, cch, psz2, cch);
	psz1[cch] = '\0';
	return cch;
}


SIZE_T StringCommons::Move(WCHAR* p1, const WCHAR* p2, SIZE_T cch)
{
	wmemmove_s(p1, cch, p2, cch);
	return cch;
}


SIZE_T StringCommons::Move(CHAR* p1, const CHAR* p2, SIZE_T cch)
{
	memmove_s(p1, cch, p2, cch);
	return cch;
}


SIZE_T StringCommons::FormatLength(const WCHAR* pszFormat, ...)
{
	va_list argList;
	va_start(argList, pszFormat);
	int cch = _vcwprintf_s(pszFormat, argList);
	va_end(argList);
	if (cch < 0)
	{
		throw Exception(L"StringCommons::FormatLength failed.");
	}
	return static_cast<SIZE_T>(cch);
}


SIZE_T StringCommons::FormatLength(const CHAR* pszFormat, ...)
{
	va_list argList;
	va_start(argList, pszFormat);
	int cch = _vcprintf_s(pszFormat, argList);
	va_end(argList);
	if (cch < 0)
	{
		throw Exception(L"StringCommons::FormatLength failed.");
	}
	return static_cast<SIZE_T>(cch);
}


SIZE_T StringCommons::VaFormatLength(const WCHAR* pszFormat, va_list argList)
{
	va_list argList2;
	va_copy(argList2, argList);
	int cch = _vcwprintf_s(pszFormat, argList2);
	va_end(argList2);
	if (cch < 0)
	{
		throw Exception(L"StringCommons::VaFormatLength failed.");
	}
	return static_cast<SIZE_T>(cch);
}


SIZE_T StringCommons::VaFormatLength(const CHAR* pszFormat, va_list argList)
{
	va_list argList2;
	va_copy(argList2, argList);
	int cch = _vcprintf_s(pszFormat, argList2);
	va_end(argList2);
	if (cch < 0)
	{
		throw Exception(L"StringCommons::VaFormatLength failed.");
	}
	return static_cast<SIZE_T>(cch);
}


SIZE_T StringCommons::Format(WCHAR* psz, SIZE_T bufsz, const WCHAR* pszFormat, ...)
{
	va_list argList;
	va_start(argList, pszFormat);
	int cch = _vsnwprintf_s(psz, bufsz, _TRUNCATE, pszFormat, argList);
	va_end(argList);
	if (cch < 0)
	{
		throw Exception(L"StringCommons::Format failed.");
	}
	return static_cast<SIZE_T>(cch);
}


SIZE_T StringCommons::Format(CHAR* psz, SIZE_T bufsz, const CHAR* pszFormat, ...)
{
	va_list argList;
	va_start(argList, pszFormat);
	int cch = _vsnprintf_s(psz, bufsz, _TRUNCATE, pszFormat, argList);
	va_end(argList);
	if (cch < 0)
	{
		throw Exception(L"StringCommons::Format failed.");
	}
	return static_cast<SIZE_T>(cch);
}


SIZE_T StringCommons::VaFormat(WCHAR* psz, SIZE_T bufsz, const WCHAR* pszFormat, va_list argList)
{
	int cch = _vsnwprintf_s(psz, bufsz, _TRUNCATE, pszFormat, argList);
	if (cch < 0)
	{
		throw Exception(L"StringCommons::VaFormat failed.");
	}
	return static_cast<SIZE_T>(cch);
}


SIZE_T StringCommons::VaFormat(CHAR* psz, SIZE_T bufsz, const CHAR* pszFormat, va_list argList)
{
	int cch = _vsnprintf_s(psz, bufsz, _TRUNCATE, pszFormat, argList);
	if (cch < 0)
	{
		throw Exception(L"StringCommons::VaFormat failed.");
	}
	return static_cast<SIZE_T>(cch);
}


SIZE_T StringCommons::Uppercase(WCHAR* psz, SSIZE_T cch)
{
	cch = Length(psz, cch);
	_wcsupr_s(psz, cch);
	return cch;
}


SIZE_T StringCommons::Uppercase(CHAR* psz, SSIZE_T cch)
{
	cch = Length(psz, cch);
	_strupr_s(psz, cch);
	return cch;
}


SIZE_T StringCommons::Lowercase(WCHAR* psz, SSIZE_T cch)
{
	cch = Length(psz, cch);
	_wcslwr_s(psz, cch);
	return cch;
}


SIZE_T StringCommons::Lowercase(CHAR* psz, SSIZE_T cch)
{
	cch = Length(psz, cch);
	_strlwr_s(psz, cch);
	return cch;
}


SIZE_T StringCommons::Lettercase(StringOptions option, WCHAR* psz, SSIZE_T cch)
{
	cch = Length(psz, cch);
	switch (option)
	{
	case UPPERCASE:
		_wcsupr_s(psz, cch + 1);
		break;
	case LOWERCASE:
		_wcslwr_s(psz, cch + 1);
		break;
	default:
		break;
	}
	return cch;
}


SIZE_T StringCommons::Lettercase(StringOptions option, CHAR* psz, SSIZE_T cch)
{
	cch = Length(psz, cch);
	switch (option)
	{
	case UPPERCASE:
		_strupr_s(psz, cch + 1);
		break;
	case LOWERCASE:
		_strlwr_s(psz, cch + 1);
		break;
	default:
		break;
	}
	return cch;
}


template<typename T>
void DoTrimScan(const T* psz, int& start, int& end, StringOptions option)
{
	const T* pCur = psz;
	T c = *pCur++;
	if (option == TRIM_TAIL)
	{
		start = 0;
	}
	else
	{
		while (StringCommons::IsWhitespace(c))
		{
			c = *pCur++;
		}
		start = static_cast<int>(pCur - 1 - psz);
		if (c)
		{
			c = *pCur++;
		}
	}
	if (option == TRIM_HEAD)
	{
		while (c)
		{
			c = *pCur++;
		}
		end = static_cast<int>(pCur - 1 - psz);
	}
	else
	{
		while (true)
		{
			if (StringCommons::IsWhitespace(c))
			{
				end = static_cast<int>(pCur - 1 - psz);
				c = *pCur++;
				while (StringCommons::IsWhitespace(c))
				{
					c = *pCur++;
				}
				if (c)
				{
					c = *pCur++;
				}
				else
				{
					break;
				}
			}
			else if (c)
			{
				c = *pCur++;
			}
			else
			{
				end = static_cast<int>(pCur - 1 - psz);
				break;
			}
		}
	}
}


void StringCommons::TrimScan(const WCHAR* psz, int& start, int& end, StringOptions option)
{
	DoTrimScan(psz, start, end, option);
}


void StringCommons::TrimScan(const CHAR* psz, int& start, int& end, StringOptions option)
{
	DoTrimScan(psz, start, end, option);
}


WCHAR* StringCommons::Trim(WCHAR* psz, StringOptions option)
{
	int start = 0;
	int end = 0;
	TrimScan(psz, start, end, option);
	int len = end - start;
	wmemmove_s(psz, len, psz + start, len);
	psz[len] = L'\0';
	return psz;
}


CHAR* StringCommons::Trim(CHAR* psz, StringOptions option)
{
	int start = 0;
	int end = 0;
	TrimScan(psz, start, end, option);
	int len = end - start;
	memmove_s(psz, len, psz + start, len);
	psz[len] = '\0';
	return psz;
}


template<typename T>
SIZE_T DoVaConcatLength(StringOptions option, const T* psz, va_list argList)
{
	SIZE_T required = StringCommons::Length(psz);
	va_list argList2;
	va_copy(argList2, argList);
	switch (option)
	{
	case CONCAT9:
		required += StringCommons::Length(va_arg(argList2, const T*));
		//FALLTHROUGH
	case CONCAT8:
		required += StringCommons::Length(va_arg(argList2, const T*));
		//FALLTHROUGH
	case CONCAT7:
		required += StringCommons::Length(va_arg(argList2, const T*));
		//FALLTHROUGH
	case CONCAT6:
		required += StringCommons::Length(va_arg(argList2, const T*));
		//FALLTHROUGH
	case CONCAT5:
		required += StringCommons::Length(va_arg(argList2, const T*));
		//FALLTHROUGH
	case CONCAT4:
		required += StringCommons::Length(va_arg(argList2, const T*));
		//FALLTHROUGH
	case CONCAT3:
		required += StringCommons::Length(va_arg(argList2, const T*));
		//FALLTHROUGH
	case CONCAT2:
		required += StringCommons::Length(va_arg(argList2, const T*));
		break;
	default:
		while (true)
		{
			const T* psz2 = va_arg(argList2, const T*);
			if (!psz2)
			{
				break;
			}
			required += StringCommons::Length(psz2);
		}
		break;
	}
	va_end(argList2);
	return required;
}


SIZE_T StringCommons::VaConcatLength(StringOptions option, const WCHAR* psz, va_list argList)
{
	return DoVaConcatLength(option, psz, argList);
}


SIZE_T StringCommons::VaConcatLength(StringOptions option, const CHAR* psz, va_list argList)
{
	return DoVaConcatLength(option, psz, argList);
}


template<typename T>
SIZE_T DoVaConcat(StringOptions option, const T* psz, va_list argList, T* pBuf)
{
	T* pCur = pBuf + StringCommons::Copy(pBuf, psz);
	switch (option)
	{
	case CONCAT9:
		pCur += StringCommons::Copy(pCur, va_arg(argList, const T*));
		//FALLTHROUGH
	case CONCAT8:
		pCur += StringCommons::Copy(pCur, va_arg(argList, const T*));
		//FALLTHROUGH
	case CONCAT7:
		pCur += StringCommons::Copy(pCur, va_arg(argList, const T*));
		//FALLTHROUGH
	case CONCAT6:
		pCur += StringCommons::Copy(pCur, va_arg(argList, const T*));
		//FALLTHROUGH
	case CONCAT5:
		pCur += StringCommons::Copy(pCur, va_arg(argList, const T*));
		//FALLTHROUGH
	case CONCAT4:
		pCur += StringCommons::Copy(pCur, va_arg(argList, const T*));
		//FALLTHROUGH
	case CONCAT3:
		pCur += StringCommons::Copy(pCur, va_arg(argList, const T*));
		//FALLTHROUGH
	case CONCAT2:
		pCur += StringCommons::Copy(pCur, va_arg(argList, const T*));
		break;
	default:
		while (true)
		{
			const T* psz2 = va_arg(argList, const T*);
			if (!psz2)
			{
				break;
			}
			pCur += StringCommons::Copy(pCur, psz2);
		}
		break;
	}
	return pCur - pBuf;
}


SIZE_T StringCommons::VaConcat(StringOptions option, const WCHAR* psz, va_list argList, WCHAR* pBuf)
{
	return DoVaConcat(option, psz, argList, pBuf);
}


SIZE_T StringCommons::VaConcat(StringOptions option, const CHAR* psz, va_list argList, CHAR* pBuf)
{
	return DoVaConcat(option, psz, argList, pBuf);
}


int StringCommons::IndexOf(const WCHAR* p, WCHAR c, SIZE_T n)
{
	const WCHAR* q = wmemchr(p, c, n);
	return q ? static_cast<int>(q - p) : -1;
}


int StringCommons::IndexOf(const CHAR* p, CHAR c, SIZE_T n)
{
	const CHAR* q = reinterpret_cast<const CHAR*>(memchr(p, c, n));
	return q ? static_cast<int>(q - p) : -1;
}


int StringCommons::IndexOf(WCHAR* p, WCHAR c, SIZE_T n)
{
	WCHAR* q = wmemchr(p, c, n);
	return q ? static_cast<int>(q - p) : -1;
}


int StringCommons::IndexOf(CHAR* p, CHAR c, SIZE_T n)
{
	CHAR* q = reinterpret_cast<CHAR*>(memchr(p, c, n));
	return q ? static_cast<int>(q - p) : -1;
}


BOOL StringCommons::IsWhitespace(WCHAR c)
{
	return iswspace(c) ? TRUE : FALSE;
}


BOOL StringCommons::IsWhitespace(CHAR c)
{
	return isspace(c) ? TRUE : FALSE;
}


BOOL StringCommons::IsDigit(WCHAR c)
{
	return iswdigit(c) ? TRUE : FALSE;
}


BOOL StringCommons::IsDigit(CHAR c)
{
	return isdigit(c) ? TRUE : FALSE;
}


int StringCommons::Compare(PCWSTR psz1, SSIZE_T cch1, PCWSTR psz2, SSIZE_T cch2)
{
	switch (CompareStringW(LOCALE_INVARIANT, 0, psz1, static_cast<int>(cch1), psz2, static_cast<int>(cch2)))
	{
	case CSTR_EQUAL:
		return 0;
	case CSTR_LESS_THAN:
		return -1;
	case CSTR_GREATER_THAN:
		return 1;
	default:
		throw Exception(L"StringCommons::Compare failed.");
	}
}


int StringCommons::Compare(PCSTR psz1, SSIZE_T cch1, PCSTR psz2, SSIZE_T cch2)
{
	switch (CompareStringA(LOCALE_INVARIANT, 0, psz1, static_cast<int>(cch1), psz2, static_cast<int>(cch2)))
	{
	case CSTR_EQUAL:
		return 0;
	case CSTR_LESS_THAN:
		return -1;
	case CSTR_GREATER_THAN:
		return 1;
	default:
		throw Exception(L"StringCommons::Compare failed.");
	}
}


int StringCommons::CaseCompare(PCWSTR psz1, SSIZE_T cch1, PCWSTR psz2, SSIZE_T cch2)
{
	switch (CompareStringW(LOCALE_INVARIANT, LINGUISTIC_IGNORECASE, psz1, static_cast<int>(cch1), psz2, static_cast<int>(cch2)))
	{
	case CSTR_EQUAL:
		return 0;
	case CSTR_LESS_THAN:
		return -1;
	case CSTR_GREATER_THAN:
		return 1;
	default:
		throw Exception(L"StringCommons::CaseCompare failed.");
	}
}


int StringCommons::CaseCompare(PCSTR psz1, SSIZE_T cch1, PCSTR psz2, SSIZE_T cch2)
{
	switch (CompareStringA(LOCALE_INVARIANT, LINGUISTIC_IGNORECASE, psz1, static_cast<int>(cch1), psz2, static_cast<int>(cch2)))
	{
	case CSTR_EQUAL:
		return 0;
	case CSTR_LESS_THAN:
		return -1;
	case CSTR_GREATER_THAN:
		return 1;
	default:
		throw Exception(L"StringCommons::CaseCompare failed.");
	}
}
