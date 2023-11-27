#include "pch.h"
#include "hnrt/StringUtils.h"
#include "hnrt/Heap.h"
#include "hnrt/Exception.h"


using namespace hnrt;


PWSTR StringUtils::Lettercase(StringOptions option, PWSTR psz, INT_PTR size)
{
    if (size < 0)
    {
        size = wcslen(psz) + 1;
    }
    switch (option)
    {
    case UPPERCASE:
        _wcsupr_s(psz, size);
        break;
    case LOWERCASE:
        _wcslwr_s(psz, size);
        break;
    default:
        throw Exception(L"StringUtils::Lettercase(PWSTR): Bad option: %d", option);
    }
    return psz;
}


PSTR StringUtils::Lettercase(StringOptions option, PSTR psz, INT_PTR size)
{
    if (size < 0)
    {
        size = strlen(psz) + 1;
    }
    switch (option)
    {
    case UPPERCASE:
        _strupr_s(psz, size);
        break;
    case LOWERCASE:
        _strlwr_s(psz, size);
        break;
    default:
        throw Exception(L"StringUtils::Lettercase(PSTR): Bad option: %d", option);
    }
    return psz;
}


template<typename T>
void DoTrimScan(StringOptions option, const T* psz, int& start, int& end)
{
    const T* pCur = psz;
    T c = *pCur++;
    if (option == TRIM_TAIL)
    {
        start = 0;
    }
    else
    {
        while (StringUtils::IsWhitespace(c))
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
            if (StringUtils::IsWhitespace(c))
            {
                end = static_cast<int>(pCur - 1 - psz);
                c = *pCur++;
                while (StringUtils::IsWhitespace(c))
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


void StringUtils::TrimScan(StringOptions option, PCWSTR psz, int& start, int& end)
{
    DoTrimScan(option, psz, start, end);
}


void StringUtils::TrimScan(StringOptions option, PCSTR psz, int& start, int& end)
{
    DoTrimScan(option, psz, start, end);
}


PWSTR StringUtils::Trim(StringOptions option, PWSTR psz)
{
    int start = 0;
    int end = 0;
    TrimScan(option, psz, start, end);
    int len = end - start;
    wmemmove_s(psz, len, psz + start, len);
    psz[len] = L'\0';
    return psz;
}


PSTR StringUtils::Trim(StringOptions option, PSTR psz)
{
    int start = 0;
    int end = 0;
    TrimScan(option, psz, start, end);
    int len = end - start;
    memmove_s(psz, len, psz + start, len);
    psz[len] = '\0';
    return psz;
}


PWSTR StringUtils::Concat(StringOptions option, PCWSTR psz, ...)
{
    va_list argList;
    va_start(argList, psz);
    PWSTR pszReturn = VaConcat(option, psz, argList, Allocate<WCHAR>(VaConcatSize(option, psz, argList)));
    va_end(argList);
    return pszReturn;
}


PSTR StringUtils::Concat(StringOptions option, PCSTR psz, ...)
{
    va_list argList;
    va_start(argList, psz);
    PSTR pszReturn = VaConcat(option, psz, argList, Allocate<CHAR>(VaConcatSize(option, psz, argList)));
    va_end(argList);
    return pszReturn;
}


template<typename T>
size_t DoVaConcatSize(StringOptions option, const T* psz, va_list argList)
{
    size_t required = StringUtils::Length(psz);
    va_list argList2;
    va_copy(argList2, argList);
    switch (option)
    {
    case CONCAT9:
        required += StringUtils::Length(va_arg(argList2, const T*));
        //FALLTHROUGH
    case CONCAT8:
        required += StringUtils::Length(va_arg(argList2, const T*));
        //FALLTHROUGH
    case CONCAT7:
        required += StringUtils::Length(va_arg(argList2, const T*));
        //FALLTHROUGH
    case CONCAT6:
        required += StringUtils::Length(va_arg(argList2, const T*));
        //FALLTHROUGH
    case CONCAT5:
        required += StringUtils::Length(va_arg(argList2, const T*));
        //FALLTHROUGH
    case CONCAT4:
        required += StringUtils::Length(va_arg(argList2, const T*));
        //FALLTHROUGH
    case CONCAT3:
        required += StringUtils::Length(va_arg(argList2, const T*));
        //FALLTHROUGH
    case CONCAT2:
        required += StringUtils::Length(va_arg(argList2, const T*));
        break;
    default:
        while (true)
        {
            const T* psz2 = va_arg(argList2, const T*);
            if (!psz2)
            {
                break;
            }
            required += StringUtils::Length(psz2);
        }
        break;
    }
    va_end(argList2);
    return required + 1;
}


size_t StringUtils::VaConcatSize(StringOptions option, PCWSTR psz, va_list argList)
{
    return DoVaConcatSize(option, psz, argList);
}


size_t StringUtils::VaConcatSize(StringOptions option, PCSTR psz, va_list argList)
{
    return DoVaConcatSize(option, psz, argList);
}


template<typename T>
T* DoVaConcat(StringOptions option, const T* psz, va_list argList, T* pBuf)
{
    T* pCur = pBuf + StringUtils::Copy(pBuf, psz);
    switch (option)
    {
    case CONCAT9:
        pCur += StringUtils::Copy(pCur, va_arg(argList, const T*));
        //FALLTHROUGH
    case CONCAT8:
        pCur += StringUtils::Copy(pCur, va_arg(argList, const T*));
        //FALLTHROUGH
    case CONCAT7:
        pCur += StringUtils::Copy(pCur, va_arg(argList, const T*));
        //FALLTHROUGH
    case CONCAT6:
        pCur += StringUtils::Copy(pCur, va_arg(argList, const T*));
        //FALLTHROUGH
    case CONCAT5:
        pCur += StringUtils::Copy(pCur, va_arg(argList, const T*));
        //FALLTHROUGH
    case CONCAT4:
        pCur += StringUtils::Copy(pCur, va_arg(argList, const T*));
        //FALLTHROUGH
    case CONCAT3:
        pCur += StringUtils::Copy(pCur, va_arg(argList, const T*));
        //FALLTHROUGH
    case CONCAT2:
        pCur += StringUtils::Copy(pCur, va_arg(argList, const T*));
        break;
    default:
        while (true)
        {
            const T* psz2 = va_arg(argList, const T*);
            if (!psz2)
            {
                break;
            }
            pCur += StringUtils::Copy(pCur, psz2);
        }
        break;
    }
    return pBuf;
}


PWSTR StringUtils::VaConcat(StringOptions option, PCWSTR psz, va_list argList, PWCHAR pBuf)
{
    return DoVaConcat(option, psz, argList, pBuf);
}


PSTR StringUtils::VaConcat(StringOptions option, PCSTR psz, va_list argList, PCHAR pBuf)
{
    return DoVaConcat(option, psz, argList, pBuf);
}
