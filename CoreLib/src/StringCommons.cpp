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


template<typename T>
void DoTrimScan(const T* str, int& start, int& end, StringOptions option)
{
	const T* pCur = str;
	T c = *pCur++;
	if (option == TRIM_TAIL)
	{
		start = 0;
	}
	else
	{
		while (IsWhitespace(c))
		{
			c = *pCur++;
		}
		start = static_cast<int>(pCur - 1 - str);
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
		end = static_cast<int>(pCur - 1 - str);
	}
	else
	{
		while (true)
		{
			if (IsWhitespace(c))
			{
				end = static_cast<int>(pCur - 1 - str);
				c = *pCur++;
				while (IsWhitespace(c))
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
				end = static_cast<int>(pCur - 1 - str);
				break;
			}
		}
	}
}


void hnrt::StrTrimScan(const WCHAR* str, int& start, int& end, StringOptions option)
{
	DoTrimScan(str, start, end, option);
}


void hnrt::StrTrimScan(const CHAR* str, int& start, int& end, StringOptions option)
{
	DoTrimScan(str, start, end, option);
}


WCHAR* hnrt::StrTrim(WCHAR* str, StringOptions option)
{
	int start = 0;
	int end = 0;
	StrTrimScan(str, start, end, option);
	int len = end - start;
	wmemmove_s(str, len, str + start, len);
	str[len] = L'\0';
	return str;
}


CHAR* hnrt::StrTrim(CHAR* str, StringOptions option)
{
	int start = 0;
	int end = 0;
	StrTrimScan(str, start, end, option);
	int len = end - start;
	memmove_s(str, len, str + start, len);
	str[len] = '\0';
	return str;
}


SIZE_T hnrt::FormatLength(const WCHAR* format, ...)
{
	va_list argList;
	va_start(argList, format);
	int cch = _vscwprintf(format, argList);
	va_end(argList);
	if (cch < 0)
	{
		throw Exception(L"_vscwprintf failed.");
	}
	return static_cast<SIZE_T>(cch);
}


SIZE_T hnrt::FormatLength(const CHAR* format, ...)
{
	va_list argList;
	va_start(argList, format);
	int cch = _vscprintf(format, argList);
	va_end(argList);
	if (cch < 0)
	{
		throw Exception(L"_vscprintf failed.");
	}
	return static_cast<SIZE_T>(cch);
}


SIZE_T hnrt::VaFormatLength(const WCHAR* format, va_list argList)
{
	va_list argList2;
	va_copy(argList2, argList);
	int cch = _vscwprintf(format, argList2);
	va_end(argList2);
	if (cch < 0)
	{
		throw Exception(L"_vscwprintf failed.");
	}
	return static_cast<SIZE_T>(cch);
}


SIZE_T hnrt::VaFormatLength(const CHAR* format, va_list argList)
{
	va_list argList2;
	va_copy(argList2, argList);
	int cch = _vscprintf(format, argList2);
	va_end(argList2);
	if (cch < 0)
	{
		throw Exception(L"_vscprintf failed.");
	}
	return static_cast<SIZE_T>(cch);
}


SIZE_T hnrt::Format(WCHAR* buf, SIZE_T bufsz, const WCHAR* format, ...)
{
	va_list argList;
	va_start(argList, format);
	int cch = _vsnwprintf_s(buf, bufsz, _TRUNCATE, format, argList);
	va_end(argList);
	if (cch < 0)
	{
		throw Exception(L"_vsnwprintf_s failed.");
	}
	return static_cast<SIZE_T>(cch);
}


SIZE_T hnrt::Format(CHAR* psz, SIZE_T bufsz, const CHAR* pszFormat, ...)
{
	va_list argList;
	va_start(argList, pszFormat);
	int cch = _vsnprintf_s(psz, bufsz, _TRUNCATE, pszFormat, argList);
	va_end(argList);
	if (cch < 0)
	{
		throw Exception(L"_vsnprintf_s failed.");
	}
	return static_cast<SIZE_T>(cch);
}


SIZE_T hnrt::VaFormat(WCHAR* psz, SIZE_T bufsz, const WCHAR* pszFormat, va_list argList)
{
	int cch = _vsnwprintf_s(psz, bufsz, _TRUNCATE, pszFormat, argList);
	if (cch < 0)
	{
		throw Exception(L"_vsnwprintf_s failed.");
	}
	return static_cast<SIZE_T>(cch);
}


SIZE_T hnrt::VaFormat(CHAR* psz, SIZE_T bufsz, const CHAR* pszFormat, va_list argList)
{
	int cch = _vsnprintf_s(psz, bufsz, _TRUNCATE, pszFormat, argList);
	if (cch < 0)
	{
		throw Exception(L"_vsnprintf_s failed.");
	}
	return static_cast<SIZE_T>(cch);
}


template<typename T>
SIZE_T DoVaConcatLength(StringOptions option, const T* str, va_list argptr)
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
		throw Exception(L"hnrt::VaConcatLength: Bad option: %d", option);
	}
	va_end(argptr2);
	return required;
}


SIZE_T hnrt::VaConcatLength(StringOptions option, const WCHAR* str, va_list argptr)
{
	return DoVaConcatLength(option, str, argptr);
}


SIZE_T hnrt::VaConcatLength(StringOptions option, const CHAR* str, va_list argptr)
{
	return DoVaConcatLength(option, str, argptr);
}


template<typename T>
SIZE_T DoVaConcat(StringOptions option, const T* str, va_list argptr, T* buf)
{
	T* pCur = buf + StrCopy(buf, str);
	switch (option)
	{
	case CONCAT9:
		pCur += StrCopy(pCur, va_arg(argptr, const T*));
		//FALLTHROUGH
	case CONCAT8:
		pCur += StrCopy(pCur, va_arg(argptr, const T*));
		//FALLTHROUGH
	case CONCAT7:
		pCur += StrCopy(pCur, va_arg(argptr, const T*));
		//FALLTHROUGH
	case CONCAT6:
		pCur += StrCopy(pCur, va_arg(argptr, const T*));
		//FALLTHROUGH
	case CONCAT5:
		pCur += StrCopy(pCur, va_arg(argptr, const T*));
		//FALLTHROUGH
	case CONCAT4:
		pCur += StrCopy(pCur, va_arg(argptr, const T*));
		//FALLTHROUGH
	case CONCAT3:
		pCur += StrCopy(pCur, va_arg(argptr, const T*));
		//FALLTHROUGH
	case CONCAT2:
		pCur += StrCopy(pCur, va_arg(argptr, const T*));
		break;
	case CONCAT:
		while (true)
		{
			const T* psz = va_arg(argptr, const T*);
			if (!psz)
			{
				break;
			}
			pCur += StrCopy(pCur, psz);
		}
		break;
	default:
		throw Exception(L"hnrt::VaConcat: Bad option: %d", option);
	}
	return pCur - buf;
}


SIZE_T hnrt::VaConcat(StringOptions option, const WCHAR* str, va_list argptr, WCHAR* buf)
{
	return DoVaConcat(option, str, argptr, buf);
}


SIZE_T hnrt::VaConcat(StringOptions option, const CHAR* str, va_list argptr, CHAR* buf)
{
	return DoVaConcat(option, str, argptr, buf);
}


int hnrt::StrIndexOf(const WCHAR* str, WCHAR c, SSIZE_T size)
{
	if (c)
	{
		SIZE_T cch = StrLen(str, size);
		const WCHAR* q = wmemchr(str, c, cch);
		return q ? static_cast<int>(q - str) : -1;
	}
	else if (size >= 0)
	{
		const WCHAR* q = wmemchr(str, L'\0', size);
		return q ? static_cast<int>(q - str) : -1;
	}
	else
	{
		return static_cast<int>(StrLen(str));
	}
}


int hnrt::StrIndexOf(const CHAR* str, CHAR c, SSIZE_T size)
{
	if (c)
	{
		SIZE_T cch = StrLen(str, size);
		const CHAR* q = reinterpret_cast<const CHAR*>(memchr(str, c, cch));
		return q ? static_cast<int>(q - str) : -1;
	}
	else if (size >= 0)
	{
		const CHAR* q = reinterpret_cast<const CHAR*>(memchr(str, '\0', size));
		return q ? static_cast<int>(q - str) : -1;
	}
	else
	{
		return static_cast<int>(StrLen(str));
	}
}


int hnrt::StrIndexOf(WCHAR* str, WCHAR c, SSIZE_T size)
{
	if (c)
	{
		SIZE_T cch = StrLen(str, size);
		WCHAR* q = wmemchr(str, c, cch);
		return q ? static_cast<int>(q - str) : -1;
	}
	else if (size >= 0)
	{
		WCHAR* q = wmemchr(str, L'\0', size);
		return q ? static_cast<int>(q - str) : -1;
	}
	else
	{
		return static_cast<int>(StrLen(str));
	}
}


int hnrt::StrIndexOf(CHAR* str, CHAR c, SSIZE_T size)
{
	if (c)
	{
		SIZE_T cch = StrLen(str, size);
		CHAR* q = reinterpret_cast<CHAR*>(memchr(str, c, cch));
		return q ? static_cast<int>(q - str) : -1;
	}
	else if (size >= 0)
	{
		CHAR* q = reinterpret_cast<CHAR*>(memchr(str, '\0', size));
		return q ? static_cast<int>(q - str) : -1;
	}
	else
	{
		return static_cast<int>(StrLen(str));
	}
}


BOOL hnrt::IsWhitespace(WCHAR c)
{
	return iswspace(c) ? TRUE : FALSE;
}


BOOL hnrt::IsWhitespace(CHAR c)
{
	return isspace(c) ? TRUE : FALSE;
}


BOOL hnrt::IsDigit(WCHAR c)
{
	return iswdigit(c) ? TRUE : FALSE;
}


BOOL hnrt::IsDigit(CHAR c)
{
	return isdigit(c) ? TRUE : FALSE;
}


int hnrt::StrCmp(PCWSTR str1, SSIZE_T count1, PCWSTR str2, SSIZE_T count2)
{
	switch (CompareStringW(LOCALE_INVARIANT, 0, str1, static_cast<int>(count1), str2, static_cast<int>(count2)))
	{
	case CSTR_EQUAL:
		return 0;
	case CSTR_LESS_THAN:
		return -1;
	case CSTR_GREATER_THAN:
		return 1;
	default:
		throw Exception(L"CompareStringW(LOCALE_INVARIANT) failed.");
	}
}


int hnrt::StrCmp(PCSTR str1, SSIZE_T count1, PCSTR str2, SSIZE_T count2)
{
	switch (CompareStringA(LOCALE_INVARIANT, 0, str1, static_cast<int>(count1), str2, static_cast<int>(count2)))
	{
	case CSTR_EQUAL:
		return 0;
	case CSTR_LESS_THAN:
		return -1;
	case CSTR_GREATER_THAN:
		return 1;
	default:
		throw Exception(L"CompareStringW(LOCALE_INVARIANT) failed.");
	}
}


int hnrt::StrCaseCmp(PCWSTR str1, SSIZE_T count1, PCWSTR str2, SSIZE_T count2)
{
	switch (CompareStringW(LOCALE_INVARIANT, LINGUISTIC_IGNORECASE, str1, static_cast<int>(count1), str2, static_cast<int>(count2)))
	{
	case CSTR_EQUAL:
		return 0;
	case CSTR_LESS_THAN:
		return -1;
	case CSTR_GREATER_THAN:
		return 1;
	default:
		throw Exception(L"CompareStringW(LOCALE_INVARIANT,LINGUISTIC_IGNORECASE) failed.");
	}
}


int hnrt::StrCaseCmp(PCSTR str1, SSIZE_T count1, PCSTR str2, SSIZE_T count2)
{
	switch (CompareStringA(LOCALE_INVARIANT, LINGUISTIC_IGNORECASE, str1, static_cast<int>(count1), str2, static_cast<int>(count2)))
	{
	case CSTR_EQUAL:
		return 0;
	case CSTR_LESS_THAN:
		return -1;
	case CSTR_GREATER_THAN:
		return 1;
	default:
		throw Exception(L"CompareStringA(LOCALE_INVARIANT,LINGUISTIC_IGNORECASE) failed.");
	}
}
