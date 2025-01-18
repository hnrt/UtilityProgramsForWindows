#pragma once

#include <Windows.h>
#include <stdarg.h>
#include "hnrt/StringOptions.h"
#include "hnrt/Unicode.h"
#include "hnrt/CodePage.h"

namespace hnrt
{
	/// <summary>
	/// The function returns the length of string, not including the terminating null character, in WCHARs.
	/// If a null pointer is specified or the buffer size of zero is specified, the function returns zero.
	/// </summary>
	/// <param name="str">Null-terminated string.</param>
	/// <param name="numberOfElements">Size of the string buffer. A negative value indicates that the size is unspecified.</param>
	/// <returns>Length of the string, not including the terminating null character, in WCHARs.
	/// The return value cannot be greater than the size of the string buffer.</returns>
	SIZE_T StrLen(const WCHAR* str, SSIZE_T numberOfElements = -1);

	/// <summary>
	/// The function returns the length of string, not including the terminating null character, in CHARs.
	/// If a null pointer is specified or the buffer size of zero is specified, the function returns zero.
	/// </summary>
	/// <param name="str">Null-terminated string.</param>
	/// <param name="numberOfElements">Size of the string buffer. A negative value indicates that the size is unspecified.</param>
	/// <returns>Length of the string, not including the terminating null character, in CHARs.
	/// The return value cannot be greater than the size of the string buffer.</returns>
	SIZE_T StrLen(const CHAR* str, SSIZE_T numberOfElements = -1);

	/// <summary>
	/// The function copies characters between string buffers.
	/// The destination string buffer will be always null-terminated.
	/// </summary>
	/// <param name="dest">Destination string buffer to copy to.</param>
	/// <param name="src">Source string buffer to copy from.</param>
	/// <param name="count">Number of characters to copy at most.
	/// No characters beyond the terminating null character are to be copied from the source string buffer.
	/// A negative value specifies to copy all the characters from the source string buffer.</param>
	/// <returns>Number of WCHARs copied, not including the terminating null character.</returns>
	SIZE_T StrCpy(WCHAR* dest, const WCHAR* src, SSIZE_T count = -1);

	/// <summary>
	/// The function copies characters between string buffers.
	/// The destination string buffer will be always null-terminated.
	/// </summary>
	/// <param name="dest">Destination string buffer to copy to.</param>
	/// <param name="src">Source string buffer to copy from.</param>
	/// <param name="count">Number of characters to copy at most.
	/// No characters beyond the terminating null character are to be copied from the source string buffer.
	/// A negative value specifies to copy all the characters from the source string buffer.</param>
	/// <returns>Number of CHARs copied, not including the terminating null character.</returns>
	SIZE_T StrCpy(CHAR* dest, const CHAR* src, SSIZE_T count = -1);

	/// <summary>
	/// The function copies characters between string buffers.
	/// </summary>
	/// <param name="dest">Destination string buffer to copy to.</param>
	/// <param name="src">Source string buffer to copy from.</param>
	/// <param name="count">Number of WCHARs to copy.</param>
	/// <returns>Number of WCHARs copied.</returns>
	SIZE_T MemCpy(WCHAR* dest, const WCHAR* src, SIZE_T count);

	/// <summary>
	/// The function copies characters between string buffers.
	/// </summary>
	/// <param name="dest">Destination string buffer to copy to.</param>
	/// <param name="src">Source string buffer to copy from.</param>
	/// <param name="count">Number of CHARs to copy.</param>
	/// <returns>Number of CHARs copied.</returns>
	SIZE_T MemCpy(CHAR* dest, const CHAR* src, SIZE_T count);

	/// <summary>
	/// The function fills a string buffer with a specified character.
	/// </summary>
	/// <param name="dest">Pointer to the string buffer.</param>
	/// <param name="c">Character to fill a string buffer with.</param>
	/// <param name="count">Number of characters.</param>
	/// <returns>Number of WCHARs filled the string buffer.</returns>
	SIZE_T MemSet(WCHAR* dest, WCHAR c, SIZE_T count);

	/// <summary>
	/// The function fills a string buffer with a specified character.
	/// </summary>
	/// <param name="dest">Pointer to the string buffer.</param>
	/// <param name="c">Character to fill a string buffer with.</param>
	/// <param name="count">Number of characters.</param>
	/// <returns>Number of CHARs filled the string buffer.</returns>
	SIZE_T MemSet(CHAR* dest, CHAR c, SIZE_T count);

	/// <summary>
	/// The function moves a block of successive characters to another memory location.
	/// Any characters, including a terminating null character, will be moved.
	/// The source block and the destination block can overlap each other.
	/// </summary>
	/// <param name="dest">Destination address.</param>
	/// <param name="src">Source address.</param>
	/// <param name="count">Number of WCHARs to move.</param>
	/// <returns>Number of WCHARs moved.</returns>
	SIZE_T MemMove(WCHAR* dest, const WCHAR* src, SIZE_T count);

	/// <summary>
	/// The function moves a block of successive characters to another memory location.
	/// Any characters, including a terminating null character, will be moved.
	/// The source block and the destination block can overlap each other.
	/// </summary>
	/// <param name="dest">Destination address.</param>
	/// <param name="src">Source address.</param>
	/// <param name="count">Number of CHARs to move.</param>
	/// <returns>Number of CHARs moved.</returns>
	SIZE_T MemMove(CHAR* dest, const CHAR* src, SIZE_T count);

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

	/// <summary>
	/// The function counts the number of WCHARs in the formatted string.
	/// </summary>
	/// <param name="format">Format-control string.</param>
	/// <returns>Number of WCHARs in the formatted string, not including the terminating null character.</returns>
	SIZE_T StrFmtLen(const WCHAR* format, ...);

	/// <summary>
	/// The function counts the number of CHARs in the formatted string.
	/// </summary>
	/// <param name="format">Format-control string. Zero or more extra arguments are to be required based on this string.</param>
	/// <returns>Number of CHARs in the formatted string, not including the terminating null character.</returns>
	SIZE_T StrFmtLen(const CHAR* format, ...);

	/// <summary>
	/// The function counts the number of WCHARs in the formatted string.
	/// </summary>
	/// <param name="format">Format-control string.</param>
	/// <param name="argList">Pointer to the list of arguments.</param>
	/// <returns>Number of WCHARs in the formatted string, not including the terminating null character.</returns>
	SIZE_T VaStrFmtLen(const WCHAR* format, va_list argList);

	/// <summary>
	/// The function counts the number of CHARs in the formatted string.
	/// </summary>
	/// <param name="format">Format-control string.</param>
	/// <param name="argList">Pointer to the list of arguments.</param>
	/// <returns>Number of CHARs in the formatted string, not including the terminating null character.</returns>
	SIZE_T VaStrFmtLen(const CHAR* format, va_list argList);

	/// <summary>
	/// The function writes a formatted string to a buffer.
	/// </summary>
	/// <param name="buf">Pointer to the buffer.</param>
	/// <param name="bufsz">Size of the buffer in WCHARs.</param>
	/// <param name="format">Format-control string. Zero or more extra arguments are to be required based on this string.</param>
	/// <returns>Number of WCHARs in the formatted string, not including the terminating null character.</returns>
	SIZE_T StrFmt(WCHAR* buf, SIZE_T bufsz, const WCHAR* format, ...);

	/// <summary>
	/// The function writes a formatted string to a buffer.
	/// </summary>
	/// <param name="buf">Pointer to the buffer.</param>
	/// <param name="bufsz">Size of the buffer in CHARs.</param>
	/// <param name="format">Format-control string. Zero or more extra arguments are to be required based on this string.</param>
	/// <returns>Number of CHARs in the formatted string, not including the terminating null character.</returns>
	SIZE_T StrFmt(CHAR* buf, SIZE_T bufsz, const CHAR* format, ...);

	/// <summary>
	/// The function writes a formatted string to a buffer.
	/// </summary>
	/// <param name="buf">Pointer to the buffer.</param>
	/// <param name="bufsz">Size of the buffer in WCHARs.</param>
	/// <param name="format">Format-control string.</param>
	/// <param name="argList">Pointer to the list of arguments.</param>
	/// <returns>Number of WCHARs in the formatted string, not including the terminating null character.</returns>
	SIZE_T VaStrFmt(WCHAR* buf, SIZE_T bufsz, const WCHAR* format, va_list argList);

	/// <summary>
	/// The function writes a formatted string to a buffer.
	/// </summary>
	/// <param name="buf">Pointer to the buffer.</param>
	/// <param name="bufsz">Size of the buffer in CHARs.</param>
	/// <param name="format">Format-control string.</param>
	/// <param name="argList">Pointer to the list of arguments.</param>
	/// <returns>Number of CHARs in the formatted string, not including the terminating null character.</returns>
	SIZE_T VaStrFmt(CHAR* buf, SIZE_T bufsz, const CHAR* format, va_list argList);

	/// <summary>
	/// The function counts the number of WCHARs in the concatenated string.
	/// </summary>
	/// <param name="option">Concatenation mode.
	/// CONCAT indicates to concatenate as many arguments as while the argument is not NULL.
	/// CONCAT2 to CONCAT9 indicate to concatenate 2 to 9 arguments.</param>
	/// <param name="str">The first string to concatenate.</param>
	/// <param name="argptr">Pointer to the list of arguments including the second string and more to concatenate.</param>
	/// <returns>Number of WCHARs in the resulting string, not including the terminating null character.</returns>
	SIZE_T VaStrCatLen(StringOptions option, const WCHAR* str, va_list argptr);

	/// <summary>
	/// The function counts the number of CHARs in the concatenated string.
	/// </summary>
	/// <param name="option">Concatenation mode.
	/// CONCAT indicates to concatenate as many arguments as while the argument is not NULL.
	/// CONCAT2 to CONCAT9 indicate to concatenate 2 to 9 arguments.</param>
	/// <param name="str">The first string to concatenate.</param>
	/// <param name="argptr">Pointer to the list of arguments including the second string and more to concatenate.</param>
	/// <returns>Number of CHARs in the resulting string, not including the terminating null character.</returns>
	SIZE_T VaStrCatLen(StringOptions option, const CHAR* str, va_list argptr);

	/// <summary>
	/// The function concatenates arguments of string and writes a single string to a buffer.
	/// </summary>
	/// <param name="option">Concatenation mode.
	/// CONCAT indicates to concatenate as many arguments as while the argument is not NULL.
	/// CONCAT2 to CONCAT9 indicate to concatenate 2 to 9 arguments.</param>
	/// <param name="str">The first string to concatenate.</param>
	/// <param name="argptr">Pointer to the list of arguments including the second string and more to concatenate.</param>
	/// <param name="buf">Buffer of the string to create.
	/// The size of this buffer should be determined by VaStrCatLen call using the same input.</param>
	/// <returns>Number of WCHARs in the resulting string, not including the terminating null character.</returns>
	SIZE_T VaStrCat(StringOptions option, const WCHAR* str, va_list argptr, WCHAR* buf);

	/// <summary>
	/// The function concatenates arguments of string and writes a single string to a buffer.
	/// </summary>
	/// <param name="option">Concatenation mode.
	/// CONCAT indicates to concatenate as many arguments as while the argument is not NULL.
	/// CONCAT2 to CONCAT9 indicate to concatenate 2 to 9 arguments.</param>
	/// <param name="str">The first string to concatenate.</param>
	/// <param name="argptr">Pointer to the list of arguments including the second string and more to concatenate.</param>
	/// <param name="buf">Buffer of the string to create.
	/// The size of this buffer should be determined by VaStrCatLen call using the same input.</param>
	/// <returns>Number of CHARs in the resulting string, not including the terminating null character.</returns>
	SIZE_T VaStrCat(StringOptions option, const CHAR* str, va_list argptr, CHAR* buf);

	/// <summary>
	/// The function searches a null-terminated string for a character.
	/// </summary>
	/// <param name="str">String to search.</param>
	/// <param name="c">UNICODE codepoint to search for.</param>
	/// <param name="size">Size of the string buffer in WCHARs.
	/// A negative value indicates that the size is unspecified.</param>
	/// <returns>Zero-based index of the character found in the string.
	/// -1 if the character is not found.</returns>
	int IndexOf(const WCHAR* str, int c, SSIZE_T size = -1);

	/// <summary>
	/// The function searches a null-terminated string for a character.
	/// </summary>
	/// <param name="str">String to search.</param>
	/// <param name="c">Character to search for.</param>
	/// <param name="size">Size of the string buffer in CHARs.
	/// A negative value indicates that the size is unspecified.</param>
	/// <returns>Zero-based index of the character found in the string.
	/// -1 if the character is not found.</returns>
	int IndexOf(const CHAR* str, int c, SSIZE_T size = -1);

	/// <summary>
	/// The function searches a null-terminated string for a sequence of characters.
	/// </summary>
	/// <param name="str">String to search.</param>
	/// <param name="seq">Sequence of characters to search for.</param>
	/// <param name="size">Size of the string buffer in WCHARs.
	/// A negative value indicates that the size is unspecified.</param>
	/// <returns>Zero-based index of the sequence of characters found in the string.
	/// -1 if the character is not found.</returns>
	int IndexOf(const WCHAR* str, const WCHAR* seq, SSIZE_T size = -1);

	/// <summary>
	/// The function searches a null-terminated string for a sequence of characters.
	/// </summary>
	/// <param name="str">String to search.</param>
	/// <param name="seq">Sequence of characters to search for.</param>
	/// <param name="size">Size of the string buffer in CHARs.
	/// A negative value indicates that the size is unspecified.</param>
	/// <returns>Zero-based index of the sequence of characters found in the string.
	/// -1 if the character is not found.</returns>
	int IndexOf(const CHAR* str, const CHAR* seq, SSIZE_T size = -1);

	/// <summary>
	/// The function searches a null-terminated string for a character in the reverse order.
	/// </summary>
	/// <param name="str">String to search.</param>
	/// <param name="c">UNICODE codepoint to search for.</param>
	/// <param name="size">Size of the string buffer in WCHARs.
	/// A negative value indicates that the size is unspecified.</param>
	/// <returns>Zero-based index of the sequence of characters found in the string.
	/// -1 if the character is not found.</returns>
	int LastIndexOf(const WCHAR* str, int c, SSIZE_T size = -1);

	/// <summary>
	/// The function searches a null-terminated string for a character in the reverse order.
	/// </summary>
	/// <param name="str">String to search.</param>
	/// <param name="c">Character to search for.</param>
	/// <param name="size">Size of the string buffer in CHARs.
	/// A negative value indicates that the size is unspecified.</param>
	/// <returns>Zero-based index of the sequence of characters found in the string.
	/// -1 if the character is not found.</returns>
	int LastIndexOf(const CHAR* str, int c, SSIZE_T size = -1);

	/// <summary>
	/// The function searches a null-terminated string for a sequence of characters in the reverse order.
	/// </summary>
	/// <param name="str">String to search.</param>
	/// <param name="seq">Sequence of characters to search for.</param>
	/// <param name="size">Size of the string buffer in WCHARs.
	/// A negative value indicates that the size is unspecified.</param>
	/// <returns>Zero-based index of the sequence of characters found in the string.
	/// -1 if the character is not found.</returns>
	int LastIndexOf(const WCHAR* str, const WCHAR* seq, SSIZE_T size = -1);

	/// <summary>
	/// The function searches a null-terminated string for a sequence of characters in the reverse order.
	/// </summary>
	/// <param name="str">String to search.</param>
	/// <param name="seq">Sequence of characters to search for.</param>
	/// <param name="size">Size of the string buffer in CHARs.
	/// A negative value indicates that the size is unspecified.</param>
	/// <returns>Zero-based index of the sequence of characters found in the string.
	/// -1 if the character is not found.</returns>
	int LastIndexOf(const CHAR* str, const CHAR* seq, SSIZE_T size = -1);

	/// <summary>
	/// This function checks if a character is of whitespace.
	/// Whitespace characters are HT(9), LF(10), VT(11), FF(12) and CR(13).
	/// </summary>
	/// <param name="c">Character to check.</param>
	/// <returns>TRUE if the character is of whitespace. FALSE otherwise.</returns>
	BOOL IsWhitespace(WCHAR c);

	/// <summary>
	/// This function checks if a character is of whitespace.
	/// Whitespace characters are HT(9), LF(10), VT(11), FF(12) and CR(13).
	/// </summary>
	/// <param name="c">Character to check.</param>
	/// <returns>TRUE if the character is of whitespace. FALSE otherwise.</returns>
	BOOL IsWhitespace(CHAR c);

	/// <summary>
	/// This function checks if a character is of digit.
	/// </summary>
	/// <param name="c">Character to check.</param>
	/// <returns>TRUE if the character is of digit. FALSE otherwise.</returns>
	BOOL IsDigit(WCHAR c);

	/// <summary>
	/// This function checks if a character is of digit.
	/// </summary>
	/// <param name="c">Character to check.</param>
	/// <returns>TRUE if the character is of digit. FALSE otherwise.</returns>
	BOOL IsDigit(CHAR c);

	/// <summary>
	/// This function compares two strings.
	/// </summary>
	/// <param name="str1">The first string to compare.</param>
	/// <param name="count1">Number of WCHARs in the first string, excluding the terminating null character.
	/// A negative value makes the function count the number of WCHARs until the terminating null character.</param>
	/// <param name="str2">The second string to compare.</param>
	/// <param name="count2">Number of WCHARs in the second string, excluding the terminating null character.
	/// A negative value makes the function count the number of WCHARs until the terminating null character.</param>
	/// <returns>0 on the exact match,
	/// 1 if the lexical order of the first string is greater than that of the second one,
	/// -1 if the lexical order of the first string is less than that of the second one.</returns>
	int StrCmp(PCWSTR str1, SSIZE_T count1, PCWSTR str2, SSIZE_T count2);

	/// <summary>
	/// This function compares two strings.
	/// </summary>
	/// <param name="str1">The first string to compare.</param>
	/// <param name="count1">Number of CHARs in the first string, excluding the terminating null character.
	/// A negative value makes the function count the number of CHARs until the terminating null character.</param>
	/// <param name="str2">The second string to compare.</param>
	/// <param name="count2">Number of CHARs in the second string, excluding the terminating null character.
	/// A negative value makes the function count the number of CHARs until the terminating null character.</param>
	/// <returns>0 on the exact match,
	/// 1 if the lexical order of the first string is greater than that of the second one,
	/// -1 if the lexical order of the first string is less than that of the second one.</returns>
	int StrCmp(PCSTR str1, SSIZE_T count1, PCSTR str2, SSIZE_T count2);

	/// <summary>
	/// This function compares two strings.
	/// Comparison is done in the lettercase insensitive manner.
	/// </summary>
	/// <param name="str1">The first string to compare.</param>
	/// <param name="count1">Number of WCHARs in the first string, excluding the terminating null character.
	/// A negative value makes the function count the number of WCHARs until the terminating null character.</param>
	/// <param name="str2">The second string to compare.</param>
	/// <param name="count2">Number of WCHARs in the second string, excluding the terminating null character.
	/// A negative value makes the function count the number of WCHARs until the terminating null character.</param>
	/// <returns>0 on the exact match,
	/// 1 if the lexical order of the first string is greater than that of the second one,
	/// -1 if the lexical order of the first string is less than that of the second one.</returns>
	int StrCaseCmp(PCWSTR str1, SSIZE_T count1, PCWSTR str2, SSIZE_T count2);

	/// <summary>
	/// This function compares two strings.
	/// Comparison is done in the lettercase insensitive manner.
	/// </summary>
	/// <param name="str1">The first string to compare.</param>
	/// <param name="count1">Number of CHARs in the first string, excluding the terminating null character.
	/// A negative value makes the function count the number of CHARs until the terminating null character.</param>
	/// <param name="str2">The second string to compare.</param>
	/// <param name="count2">Number of CHARs in the second string, excluding the terminating null character.
	/// A negative value makes the function count the number of CHARs until the terminating null character.</param>
	/// <returns>0 on the exact match,
	/// 1 if the lexical order of the first string is greater than that of the second one,
	/// -1 if the lexical order of the first string is less than that of the second one.</returns>
	int StrCaseCmp(PCSTR str1, SSIZE_T count1, PCSTR str2, SSIZE_T count2);

	/// <summary>
	/// This function converts a string to a long integer value.
	/// </summary>
	/// <param name="str">Null-terminated string to convert.</param>
	/// <param name="endptr">An output parameter, set to point to the character after the last interpreted character. Ignored, if NULL.</param>
	/// <param name="radix">Number base to use.</param>
	/// <returns>A long integer value. Zero if unsuccessful.</returns>
	long StrToLong(PCWSTR str, PWSTR* endptr, int radix);

	/// <summary>
	/// This function converts a string to a long integer value.
	/// </summary>
	/// <param name="str">Null-terminated string to convert.</param>
	/// <param name="endptr">An output parameter, set to point to the character after the last interpreted character. Ignored, if NULL.</param>
	/// <param name="radix">Number base to use.</param>
	/// <returns>A long integer value. Zero if unsuccessful.</returns>
	long StrToLong(PCSTR str, PSTR* endptr, int radix);

	/// <summary>
	/// This function converts a string to an unsigned long integer value.
	/// </summary>
	/// <param name="str">Null-terminated string to convert.</param>
	/// <param name="endptr">An output parameter, set to point to the character after the last interpreted character. Ignored, if NULL.</param>
	/// <param name="radix">Number base to use.</param>
	/// <returns>An unsigned long integer value. Zero if unsuccessful.</returns>
	unsigned long StrToUnsignedLong(PCWSTR str, PWSTR* endptr, int radix);

	/// <summary>
	/// This function converts a string to an unsigned long integer value.
	/// </summary>
	/// <param name="str">Null-terminated string to convert.</param>
	/// <param name="endptr">An output parameter, set to point to the character after the last interpreted character. Ignored, if NULL.</param>
	/// <param name="radix">Number base to use.</param>
	/// <returns>An unsigned long integer value. Zero if unsuccessful.</returns>
	unsigned long StrToUnsignedLong(PCSTR str, PSTR* endptr, int radix);

	/// <summary>
	/// This function converts a string to a long long integer value.
	/// </summary>
	/// <param name="str">Null-terminated string to convert.</param>
	/// <param name="endptr">An output parameter, set to point to the character after the last interpreted character. Ignored, if NULL.</param>
	/// <param name="radix">Number base to use.</param>
	/// <returns>A long long integer value. Zero if unsuccessful.</returns>
	long long StrToLongLong(PCWSTR str, PWSTR* endptr, int radix);

	/// <summary>
	/// This function converts a string to a long long integer value.
	/// </summary>
	/// <param name="str">Null-terminated string to convert.</param>
	/// <param name="endptr">An output parameter, set to point to the character after the last interpreted character. Ignored, if NULL.</param>
	/// <param name="radix">Number base to use.</param>
	/// <returns>A long long integer value. Zero if unsuccessful.</returns>
	long long StrToLongLong(PCSTR str, PSTR* endptr, int radix);

	/// <summary>
	/// This function converts a string to an unsigned long long integer value.
	/// </summary>
	/// <param name="str">Null-terminated string to convert.</param>
	/// <param name="endptr">An output parameter, set to point to the character after the last interpreted character. Ignored, if NULL.</param>
	/// <param name="radix">Number base to use.</param>
	/// <returns>An unsigned long long integer value. Zero if unsuccessful.</returns>
	unsigned long long StrToUnsignedLongLong(PCWSTR str, PWSTR* endptr, int radix);

	/// <summary>
	/// This function converts a string to an unsigned long long integer value.
	/// </summary>
	/// <param name="str">Null-terminated string to convert.</param>
	/// <param name="endptr">An output parameter, set to point to the character after the last interpreted character. Ignored, if NULL.</param>
	/// <param name="radix">Number base to use.</param>
	/// <returns>An unsigned long long integer value. Zero if unsuccessful.</returns>
	unsigned long long StrToUnsignedLongLong(PCSTR str, PSTR* endptr, int radix);

	/// <summary>
	/// This function converts a string to a double-precision floating point number.
	/// </summary>
	/// <param name="str">Null-terminated string to convert.</param>
	/// <param name="endptr">An output parameter, set to point to the character after the last interpreted character. Ignored, if NULL.</param>
	/// <returns>A double-precision floating point number.
	/// +/-HUGE_VAL when the representation would cause an overflow.
	/// The sign of HUGE_VAL matches the sign of the value that can't be represented.
	/// Zero if no conversion can be performed or an underflow occurs.</returns>
	double StrToDouble(PCWSTR str, PWSTR* endptr);

	/// <summary>
	/// This function converts a string to a double-precision floating point number.
	/// </summary>
	/// <param name="str">Null-terminated string to convert.</param>
	/// <param name="endptr">An output parameter, set to point to the character after the last interpreted character. Ignored, if NULL.</param>
	/// <returns>A double-precision floating point number.
	/// +/-HUGE_VAL when the representation would cause an overflow.
	/// The sign of HUGE_VAL matches the sign of the value that can't be represented.
	/// Zero if no conversion can be performed or an underflow occurs.</returns>
	double StrToDouble(PCSTR str, PSTR* endptr);

	/// <summary>
	/// This function returns the number of the specified code point in the string.
	/// </summary>
	/// <param name="str">String to check</param>
	/// <param name="count">Number of characters to check</param>
	/// <param name="c">Code point to count</param>
	/// <returns></returns>
	SIZE_T CountCharacter(PCWCH str, SSIZE_T count, int codepoint);

	/// <summary>
	/// This function returns the number of the specified code point in the string.
	/// </summary>
	/// <param name="str">String to check</param>
	/// <param name="count">Number of characters to check</param>
	/// <param name="c">Code point to count</param>
	/// <returns></returns>
	SIZE_T CountCharacter(PCCH str, SSIZE_T count, int codepoint);

}
