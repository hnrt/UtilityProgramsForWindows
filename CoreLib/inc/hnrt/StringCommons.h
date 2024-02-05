#pragma once

#include <Windows.h>
#include "hnrt/StringOptions.h"

namespace hnrt
{
	/// <summary>
	/// The function returns the length of string, not including the terminating null character, in WCHARs.
	/// If a null pointer is specified or the buffer size of zero is specified, the function returns zero.
	/// </summary>
	/// <param name="str">Null-terminated string.</param>
	/// <param name="numberOfElements">The size of the string buffer. A negative value means that the size is unspecified.</param>
	/// <returns>The length of string, not including the terminating null character, in WCHARs.</returns>
	SIZE_T StrLen(const WCHAR* str, SSIZE_T numberOfElements = -1);

	/// <summary>
	/// The function returns the length of string, not including the terminating null character, in CHARs.
	/// If a null pointer is specified or the buffer size of zero is specified, the function returns zero.
	/// </summary>
	/// <param name="str">Null-terminated string.</param>
	/// <param name="numberOfElements">The size of the string buffer. A negative value means that the size is unspecified.</param>
	/// <returns>The length of string, not including the terminating null character, in CHARs.</returns>
	SIZE_T StrLen(const CHAR* str, SSIZE_T numberOfElements = -1);

	/// <summary>
	/// The function copies characters between string buffers.
	/// The destination string buffer will be null-terminated.
	/// </summary>
	/// <param name="dest">Destination string buffer to copy to.</param>
	/// <param name="src">Source string buffer to copy from.</param>
	/// <param name="count">Number of characters to copy at most.
	/// No characters beyond the terminating null character are copied from the source string buffer.
	/// A negative value specifies to copy all the characters from the source string buffer.</param>
	/// <returns>Number of characters copied, not including the terminating null character.</returns>
	SIZE_T StrCopy(WCHAR* dest, const WCHAR* src, SSIZE_T count = -1);

	/// <summary>
	/// The function copies characters between string buffers.
	/// The destination string buffer will be null-terminated.
	/// </summary>
	/// <param name="dest">Destination string buffer to copy to.</param>
	/// <param name="src">Source string buffer to copy from.</param>
	/// <param name="count">Number of characters to copy at most.
	/// No characters beyond the terminating null character are copied from the source string buffer.
	/// A negative value specifies to copy all the characters from the source string buffer.</param>
	/// <returns>Number of characters copied, not including the terminating null character.</returns>
	SIZE_T StrCopy(CHAR* dest, const CHAR* src, SSIZE_T count = -1);

	/// <summary>
	/// The function fills a string buffer with a specified character.
	/// </summary>
	/// <param name="dest">Pointer to the string buffer.</param>
	/// <param name="c">Character to fill a string buffer with.</param>
	/// <param name="count">Number of characters.</param>
	/// <returns>Number of characters filled the string buffer with.</returns>
	SIZE_T StrFill(WCHAR* dest, WCHAR c, SIZE_T count);

	/// <summary>
	/// The function fills a string buffer with a specified character.
	/// </summary>
	/// <param name="dest">Pointer to the string buffer.</param>
	/// <param name="c">Character to fill a string buffer with.</param>
	/// <param name="count">Number of characters.</param>
	/// <returns>Number of characters filled the string buffer with.</returns>
	SIZE_T StrFill(CHAR* dest, CHAR c, SIZE_T count);

	/// <summary>
	/// The function moves a block of successive characters to another memory location.
	/// Any characters, including a terminating null character, will be moved.
	/// </summary>
	/// <param name="dest">Destination address.</param>
	/// <param name="src">Source address.</param>
	/// <param name="count">Number of WCHARs to move.</param>
	/// <returns>Number of WCHARs moved.</returns>
	SIZE_T StrMove(WCHAR* dest, const WCHAR* src, SIZE_T count);

	/// <summary>
	/// The function moves a block of successive characters to another memory location.
	/// Any characters, including a terminating null character, will be moved.
	/// </summary>
	/// <param name="dest">Destination address.</param>
	/// <param name="src">Source address.</param>
	/// <param name="count">Number of CHARs to move.</param>
	/// <returns>Number of CHARs moved.</returns>
	SIZE_T StrMove(CHAR* dest, const CHAR* src, SIZE_T count);

	/// <summary>
	/// The function converts the letter case of characters in a string.
	/// It finishes the conversion when it processes the specified number of WCHARs
	/// or it encounters the terminating null character.
	/// The resulting string will be null-terminated.
	/// </summary>
	/// <param name="option">Letter case to convert characters to, either UPPERCASE or LOWERCASE.</param>
	/// <param name="str">String in which characters are to be converted to the specified letter case.</param>
	/// <param name="count">Number of WCHARs to process at most. A negative value specifies to process all characters in the string.</param>
	/// <returns>Number of WCHARs processed, not including the terminating null character.</returns>
	SIZE_T StrCase(StringOptions option, WCHAR* str, SSIZE_T count = -1);

	/// <summary>
	/// The function converts the letter case of characters in a string.
	/// It finishes the conversion when it processes the specified number of CHARs
	/// or it encounters the terminating null character.
	/// The resulting string will be null-terminated.
	/// </summary>
	/// <param name="option">Letter case to convert characters to, either UPPERCASE or LOWERCASE.</param>
	/// <param name="str">String in which characters are to be converted to the specified letter case.</param>
	/// <param name="count">Number of CHARs to process at most. A negative value specifies to process all characters in the string.</param>
	/// <returns>Number of CHARs processed, not including the terminating null character.</returns>
	SIZE_T StrCase(StringOptions option, CHAR* str, SSIZE_T count = -1);

	/// <summary>
	/// The function converts lowercase characters to uppercase in a string.
	/// It finishes the conversion when it processes the specified number of WCHARs
	/// or it encounters the terminating null character.
	/// The resulting string will be null-terminated.
	/// </summary>
	/// <param name="str">String in which lowercase characters are to be converted to uppercase.</param>
	/// <param name="count">Number of WCHARs to process at most. A negative value specifies to process all characters in the string.</param>
	/// <returns>Number of WCHARs processed, not including the terminating null character.</returns>
	SIZE_T StrUpr(WCHAR* str, SSIZE_T count = -1);

	/// <summary>
	/// The function converts lowercase characters to uppercase in a string.
	/// It finishes the conversion when it processes the specified number of CHARs
	/// or it encounters the terminating null character.
	/// The resulting string will be null-terminated.
	/// </summary>
	/// <param name="str">String in which lowercase characters are to be converted to uppercase.</param>
	/// <param name="count">Number of CHARs to process at most. A negative value specifies to process all characters in the string.</param>
	/// <returns>Number of CHARs processed, not including the terminating null character.</returns>
	SIZE_T StrUpr(CHAR* str, SSIZE_T count = -1);

	/// <summary>
	/// The function converts uppercase characters to lowercase in a string.
	/// It finishes the conversion when it processes the specified number of WCHARs
	/// or it encounters the terminating null character.
	/// The resulting string will be null-terminated.
	/// </summary>
	/// <param name="str">String in which uppercase characters are to be converted to lowercase.</param>
	/// <param name="count">Number of WCHARs to process at most. A negative value specifies to process all characters in the string.</param>
	/// <returns>Number of WCHARs processed, not including the terminating null character.</returns>
	SIZE_T StrLwr(WCHAR* str, SSIZE_T count = -1);

	/// <summary>
	/// The function converts uppercase characters to lowercase in a string.
	/// It finishes the conversion when it processes the specified number of CHARs
	/// or it encounters the terminating null character.
	/// The resulting string will be null-terminated.
	/// </summary>
	/// <param name="str">String in which uppercase characters are to be converted to lowercase.</param>
	/// <param name="count">Number of CHARs to process at most. A negative value specifies to process all characters in the string.</param>
	/// <returns>Number of CHARs processed, not including the terminating null character.</returns>
	SIZE_T StrLwr(CHAR* str, SSIZE_T count = -1);

	/// <summary>
	/// The function finds the range of the resulting substring in case of trimming a string in the specified manner.
	/// Trimming is done for the following characters, HT(9), LF(10), VT(11), FF(12) and CR(13).
	/// </summary>
	/// <param name="str">String to be examined.</param>
	/// <param name="start">Receives the start index of the resulting substring.</param>
	/// <param name="end">Receives the end index of the resulting substring.</param>
	/// <param name="option">Trimming manner; TRIM, TRIM_HEAD or TRIM_TAIL.</param>
	void StrTrimScan(const WCHAR* str, int& start, int& end, StringOptions option = TRIM);

	/// <summary>
	/// The function finds the range of the resulting substring in case of trimming a string in the specified manner.
	/// Trimming is done for the following characters, HT(9), LF(10), VT(11), FF(12) and CR(13).
	/// </summary>
	/// <param name="str">String to be examined.</param>
	/// <param name="start">Receives the start index of the resulting substring.</param>
	/// <param name="end">Receives the end index of the resulting substring.</param>
	/// <param name="option">Trimming manner; TRIM, TRIM_HEAD or TRIM_TAIL.</param>
	void StrTrimScan(const CHAR* str, int& start, int& end, StringOptions option = TRIM);

	/// <summary>
	/// The function trims a string in the specified manner.
	/// </summary>
	/// <param name="str">String to be trimmed.</param>
	/// <param name="option">Trimming manner; TRIM, TRIM_HEAD or TRIM_TAIL.</param>
	/// <returns>String trimmed.</returns>
	WCHAR* StrTrim(WCHAR* str, StringOptions option = TRIM);

	/// <summary>
	/// The function trims a string in the specified manner.
	/// </summary>
	/// <param name="str">String to be trimmed.</param>
	/// <param name="option">Trimming manner; TRIM, TRIM_HEAD or TRIM_TAIL.</param>
	/// <returns>String trimmed.</returns>
	CHAR* StrTrim(CHAR* str, StringOptions option = TRIM);

	class StringCommons
	{
	public:

		static SIZE_T FormatLength(const WCHAR*, ...);
		static SIZE_T FormatLength(const CHAR*, ...);
		static SIZE_T VaFormatLength(const WCHAR*, va_list);
		static SIZE_T VaFormatLength(const CHAR*, va_list);
		static SIZE_T Format(WCHAR* psz, SIZE_T, const WCHAR*, ...);
		static SIZE_T Format(CHAR* psz, SIZE_T, const CHAR*, ...);
		static SIZE_T VaFormat(WCHAR* psz, SIZE_T, const WCHAR*, va_list);
		static SIZE_T VaFormat(CHAR* psz, SIZE_T, const CHAR*, va_list);
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
