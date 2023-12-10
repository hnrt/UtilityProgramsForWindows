#pragma once

#include <Windows.h>
#include "hnrt/StringOptions.h"

namespace hnrt
{
	class StringUtils
	{
	public:
		static PWSTR Uppercase(PWSTR, INT_PTR = -1);
		static PWSTR Lowercase(PWSTR, INT_PTR = -1);
		static PWSTR Lettercase(StringOptions, PWSTR, INT_PTR = -1);
		static PSTR Uppercase(PSTR, INT_PTR = -1);
		static PSTR Lowercase(PSTR, INT_PTR = -1);
		static PSTR Lettercase(StringOptions, PSTR, INT_PTR = -1);
		static void TrimScan(StringOptions, PCWSTR, int&, int&);
		static void TrimScan(StringOptions, PCSTR, int&, int&);
		static PWSTR Trim(StringOptions, PWSTR);
		static PSTR Trim(StringOptions, PSTR);
		static PWSTR Trim(PWSTR);
		static PSTR Trim(PSTR);
		static PWSTR Concat(StringOptions, PCWSTR, ...);
		static PSTR Concat(StringOptions, PCSTR, ...);
		static size_t VaConcatSize(StringOptions option, PCWSTR psz, va_list argList);
		static size_t VaConcatSize(StringOptions option, PCSTR psz, va_list argList);
		static PWSTR VaConcat(StringOptions, PCWSTR, va_list, PWCHAR);
		static PSTR VaConcat(StringOptions, PCSTR, va_list, PCHAR);
		static size_t Length(const WCHAR* psz);
		static size_t Length(const CHAR* psz);
		static size_t Copy(WCHAR* pDst, const WCHAR* pSrc);
		static size_t Copy(CHAR* pDst, const CHAR* pSrc);
		static int IsWhitespace(WCHAR c);
		static int IsWhitespace(CHAR c);
	};

	inline PWSTR StringUtils::Uppercase(PWSTR psz, INT_PTR size)
	{
		return Lettercase(UPPERCASE, psz, size);
	}

	inline PWSTR StringUtils::Lowercase(PWSTR psz, INT_PTR size)
	{
		return Lettercase(LOWERCASE, psz, size);
	}

	inline PSTR StringUtils::Uppercase(PSTR psz, INT_PTR size)
	{
		return Lettercase(UPPERCASE, psz, size);
	}

	inline PSTR StringUtils::Lowercase(PSTR psz, INT_PTR size)
	{
		return Lettercase(LOWERCASE, psz, size);
	}

	inline PWSTR StringUtils::Trim(PWSTR psz)
	{
		return Trim(TRIM, psz);
	}

	inline PSTR StringUtils::Trim(PSTR psz)
	{
		return Trim(TRIM, psz);
	}

	inline size_t StringUtils::Length(const WCHAR* psz)
	{
		return wcslen(psz);
	}

	inline size_t StringUtils::Length(const CHAR* psz)
	{
		return strlen(psz);
	}

	inline size_t StringUtils::Copy(WCHAR* pDst, const WCHAR* pSrc)
	{
		size_t cch = wcslen(pSrc);
		wmemcpy_s(pDst, cch + 1, pSrc, cch + 1);
		return cch;
	}

	inline size_t StringUtils::Copy(CHAR* pDst, const CHAR* pSrc)
	{
		size_t cb = strlen(pSrc);
		memcpy_s(pDst, cb + 1, pSrc, cb + 1);
		return cb;
	}

	inline int StringUtils::IsWhitespace(WCHAR c)
	{
		return iswspace(c);
	}

	inline int StringUtils::IsWhitespace(CHAR c)
	{
		return isspace(c);
	}
}
