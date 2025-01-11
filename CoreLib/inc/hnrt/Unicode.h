#pragma once

#include <Windows.h>

#define BYTE_ORDER_MARK       static_cast<WCHAR>(0xFEFF)
#define REPLACEMENT_CHARACTER static_cast<WCHAR>(0xFFFD)

//
// GENERAL PUNCTUATION
//
#define LINE_SEPARATOR        static_cast<WCHAR>(0x2028)
#define PARAGRAPH_SEPARATOR   static_cast<WCHAR>(0x2029)

#define IS_BASIC_MULTILINGUAL_PLANE(c) (static_cast<unsigned int>(c)<0x10000)
#define IS_BMP(c) IS_BASIC_MULTILINGUAL_PLANE(c)
#define HIGH_SURROGATE(c) static_cast<WCHAR>(HIGH_SURROGATE_START+((static_cast<unsigned int>(c)-0x10000)/0x400))
#define  LOW_SURROGATE(c) static_cast<WCHAR>(LOW_SURROGATE_START+((static_cast<unsigned int>(c)-0x10000)%0x400))
