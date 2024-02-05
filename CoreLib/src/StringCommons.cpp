#include "pch.h"
#include "hnrt/StringCommons.h"
#include "hnrt/Exception.h"


using namespace hnrt;


SIZE_T hnrt::StrLen(const WCHAR* str, SSIZE_T numberOfElements)
{
	if (str && numberOfElements)
	{
		return numberOfElements < 0 ? wcslen(str) : wcsnlen(str, numberOfElements);
	}
	else
	{
		return 0;
	}
}


SIZE_T hnrt::StrLen(const CHAR* str, SSIZE_T numberOfElements)
{
	if (str && numberOfElements)
	{
		return numberOfElements < 0 ? strlen(str) : strnlen(str, numberOfElements);
	}
	else
	{
		return 0;
	}
}


SIZE_T hnrt::StrCopy(WCHAR* dest, const WCHAR* src, SSIZE_T count)
{
	SIZE_T cch = StrLen(src, count);
	wmemcpy_s(dest, cch, src, cch);
	dest[cch] = L'\0';
	return cch;
}


SIZE_T hnrt::StrCopy(CHAR* dest, const CHAR* src, SSIZE_T count)
{
	SIZE_T cch = StrLen(src, count);
	memcpy_s(dest, cch, src, cch);
	dest[cch] = '\0';
	return cch;
}


SIZE_T hnrt::StrFill(WCHAR* dest, WCHAR c, SIZE_T count)
{
	wmemset(dest, c, count);
	return count;
}


SIZE_T hnrt::StrFill(CHAR* dest, CHAR c, SIZE_T count)
{
	memset(dest, c, count);
	return count;
}


SIZE_T hnrt::StrMove(WCHAR* dest, const WCHAR* src, SIZE_T count)
{
	wmemmove_s(dest, count, src, count);
	return count;
}


SIZE_T hnrt::StrMove(CHAR* dest, const CHAR* src, SIZE_T count)
{
	memmove_s(dest, count, src, count);
	return count;
}


SIZE_T hnrt::StrCase(StringOptions option, WCHAR* str, SSIZE_T count)
{
	if (str)
	{
		SIZE_T cch;
		switch (option)
		{
		case UPPERCASE:
			cch = StrLen(str, count);
			str[cch] = L'\0';
			_wcsupr_s(str, cch + 1);
			break;
		case LOWERCASE:
			cch = StrLen(str, count);
			str[cch] = L'\0';
			_wcslwr_s(str, cch + 1);
			break;
		default:
			throw Exception(L"hnrt::StrCase: Bad option: %d", option);
		}
		return cch;
	}
	else
	{
		return 0;
	}
}


SIZE_T hnrt::StrCase(StringOptions option, CHAR* str, SSIZE_T count)
{
	if (str)
	{
		SIZE_T cch;
		switch (option)
		{
		case UPPERCASE:
			cch = StrLen(str, count);
			str[cch] = '\0';
			_strupr_s(str, cch + 1);
			break;
		case LOWERCASE:
			cch = StrLen(str, count);
			str[cch] = '\0';
			_strlwr_s(str, cch + 1);
			break;
		default:
			throw Exception(L"hnrt::StrCase: Bad option: %d", option);
		}
		return cch;
	}
	else
	{
		return 0;
	}
}


SIZE_T hnrt::StrUpr(WCHAR* str, SSIZE_T count)
{
	if (str)
	{
		SIZE_T cch = StrLen(str, count);
		str[cch] = L'\0';
		_wcsupr_s(str, cch + 1);
		return cch;
	}
	else
	{
		return 0;
	}
}


SIZE_T hnrt::StrUpr(CHAR* str, SSIZE_T count)
{
	if (str)
	{
		SIZE_T cch = StrLen(str, count);
		str[cch] = '\0';
		_strupr_s(str, cch + 1);
		return cch;
	}
	else
	{
		return 0;
	}
}


SIZE_T hnrt::StrLwr(WCHAR* str, SSIZE_T count)
{
	if (str)
	{
		SIZE_T cch = StrLen(str, count);
		str[cch] = L'\0';
		_wcslwr_s(str, cch + 1);
		return cch;
	}
	else
	{
		return 0;
	}
}


SIZE_T hnrt::StrLwr(CHAR* str, SSIZE_T count)
{
	if (str)
	{
		SIZE_T cch = StrLen(str, count);
		str[cch] = '\0';
		_strlwr_s(str, cch + 1);
		return cch;
	}
	else
	{
		return 0;
	}
}


SIZE_T StringCommons::FormatLength(const WCHAR* pszFormat, ...)
{
	va_list argList;
	va_start(argList, pszFormat);
	int cch = _vscwprintf(pszFormat, argList);
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
	int cch = _vscprintf(pszFormat, argList);
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
	int cch = _vscwprintf(pszFormat, argList2);
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
	int cch = _vscprintf(pszFormat, argList2);
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
	SIZE_T required = StrLen(psz);
	va_list argList2;
	va_copy(argList2, argList);
	switch (option)
	{
	case CONCAT9:
		required += StrLen(va_arg(argList2, const T*));
		//FALLTHROUGH
	case CONCAT8:
		required += StrLen(va_arg(argList2, const T*));
		//FALLTHROUGH
	case CONCAT7:
		required += StrLen(va_arg(argList2, const T*));
		//FALLTHROUGH
	case CONCAT6:
		required += StrLen(va_arg(argList2, const T*));
		//FALLTHROUGH
	case CONCAT5:
		required += StrLen(va_arg(argList2, const T*));
		//FALLTHROUGH
	case CONCAT4:
		required += StrLen(va_arg(argList2, const T*));
		//FALLTHROUGH
	case CONCAT3:
		required += StrLen(va_arg(argList2, const T*));
		//FALLTHROUGH
	case CONCAT2:
		required += StrLen(va_arg(argList2, const T*));
		break;
	default:
		while (true)
		{
			const T* psz2 = va_arg(argList2, const T*);
			if (!psz2)
			{
				break;
			}
			required += StrLen(psz2);
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
	T* pCur = pBuf + StrCopy(pBuf, psz);
	switch (option)
	{
	case CONCAT9:
		pCur += StrCopy(pCur, va_arg(argList, const T*));
		//FALLTHROUGH
	case CONCAT8:
		pCur += StrCopy(pCur, va_arg(argList, const T*));
		//FALLTHROUGH
	case CONCAT7:
		pCur += StrCopy(pCur, va_arg(argList, const T*));
		//FALLTHROUGH
	case CONCAT6:
		pCur += StrCopy(pCur, va_arg(argList, const T*));
		//FALLTHROUGH
	case CONCAT5:
		pCur += StrCopy(pCur, va_arg(argList, const T*));
		//FALLTHROUGH
	case CONCAT4:
		pCur += StrCopy(pCur, va_arg(argList, const T*));
		//FALLTHROUGH
	case CONCAT3:
		pCur += StrCopy(pCur, va_arg(argList, const T*));
		//FALLTHROUGH
	case CONCAT2:
		pCur += StrCopy(pCur, va_arg(argList, const T*));
		break;
	default:
		while (true)
		{
			const T* psz2 = va_arg(argList, const T*);
			if (!psz2)
			{
				break;
			}
			pCur += StrCopy(pCur, psz2);
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
