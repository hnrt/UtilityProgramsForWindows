#pragma once

#include "hnrt/StringOptions.h"

namespace hnrt
{
	class StringCommons
	{
	public:

		static SIZE_T Length(const WCHAR*, SSIZE_T = -1);
		static SIZE_T Length(const CHAR*, SSIZE_T = -1);
		static SIZE_T Fill(WCHAR*, WCHAR, SIZE_T);
		static SIZE_T Fill(CHAR*, CHAR, SIZE_T);
		static SIZE_T Copy(WCHAR*, const WCHAR*, SSIZE_T = -1);
		static SIZE_T Copy(CHAR*, const CHAR*, SSIZE_T = -1);
		static SIZE_T Move(WCHAR*, const WCHAR*, SIZE_T);
		static SIZE_T Move(CHAR*, const CHAR*, SIZE_T);
		static SIZE_T FormatLength(const WCHAR*, ...);
		static SIZE_T FormatLength(const CHAR*, ...);
		static SIZE_T VaFormatLength(const WCHAR*, va_list);
		static SIZE_T VaFormatLength(const CHAR*, va_list);
		static SIZE_T Format(WCHAR* psz, SIZE_T, const WCHAR*, ...);
		static SIZE_T Format(CHAR* psz, SIZE_T, const CHAR*, ...);
		static SIZE_T VaFormat(WCHAR* psz, SIZE_T, const WCHAR*, va_list);
		static SIZE_T VaFormat(CHAR* psz, SIZE_T, const CHAR*, va_list);
		static SIZE_T Uppercase(WCHAR*, SSIZE_T = -1);
		static SIZE_T Uppercase(CHAR*, SSIZE_T = -1);
		static SIZE_T Lowercase(WCHAR*, SSIZE_T = -1);
		static SIZE_T Lowercase(CHAR*, SSIZE_T = -1);
		static SIZE_T Lettercase(StringOptions, WCHAR*, SSIZE_T = -1);
		static SIZE_T Lettercase(StringOptions, CHAR*, SSIZE_T = -1);
		static void TrimScan(const WCHAR*, int&, int&, StringOptions = TRIM);
		static void TrimScan(const CHAR*, int&, int&, StringOptions = TRIM);
		static WCHAR* Trim(WCHAR*, StringOptions = TRIM);
		static CHAR* Trim(CHAR*, StringOptions = TRIM);
		static SIZE_T VaConcatLength(StringOptions, const WCHAR*, va_list);
		static SIZE_T VaConcatLength(StringOptions, const CHAR*, va_list);
		static SIZE_T VaConcat(StringOptions, const WCHAR*, va_list, WCHAR*);
		static SIZE_T VaConcat(StringOptions, const CHAR*, va_list, CHAR*);
		static int IndexOf(const WCHAR*, WCHAR, SIZE_T);
		static int IndexOf(const CHAR*, CHAR, SIZE_T);
		static int IndexOf(WCHAR*, WCHAR, SIZE_T);
		static int IndexOf(CHAR*, CHAR, SIZE_T);
		static BOOL IsWhitespace(WCHAR c);
		static BOOL IsWhitespace(CHAR c);
		static BOOL IsDigit(WCHAR c);
		static BOOL IsDigit(CHAR c);
		static int Compare(PCWSTR psz1, SSIZE_T cch1, PCWSTR psz2, SSIZE_T cch2);
		static int Compare(PCSTR psz1, SSIZE_T cch1, PCSTR psz2, SSIZE_T cch2);
		static int CaseCompare(PCWSTR psz1, SSIZE_T cch1, PCWSTR psz2, SSIZE_T cch2);
		static int CaseCompare(PCSTR psz1, SSIZE_T cch1, PCSTR psz2, SSIZE_T cch2);
	};
}
