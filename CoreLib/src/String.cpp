#include "pch.h"
#include "hnrt/String.h"
#include "hnrt/RefString.h"
#include "hnrt/Interlocked.h"
#include "hnrt/Exception.h"
#include "hnrt/MultibyteString.h"


#define XLITERAL(x) L##x


using namespace hnrt;


typedef RefString<WCHAR> RefStr;


inline static PWSTR AddRef(PWSTR psz)
{
    if (psz)
    {
        RefStr::GetThis(psz)->AddRef();
    }
    return psz;
}


inline static void Release(PWSTR psz)
{
    if (psz)
    {
        RefStr::GetThis(psz)->Release();
    }
}


const String String::Empty = String();


String::String()
    : m_psz(nullptr)
{
}


String::String(const String& src)
    : m_psz(AddRef(src.m_psz))
{
}


String::String(PCWSTR psz, SSIZE_T cch)
    : m_psz(RefStr::Create(psz, cch))
{
}


String::String(PCWSTR pszFormat, va_list argList)
    : m_psz(RefStr::Create(pszFormat, argList))
{
}


String::String(StringOptions option, PCWSTR psz, ...)
    : m_psz(nullptr)
{
    va_list argList;
    va_start(argList, psz);
    switch (option)
    {
    case PRINTF:
        m_psz = RefStr::Create(psz, argList);
        break;
    case CONCAT:
    case CONCAT2:
    case CONCAT3:
    case CONCAT4:
    case CONCAT5:
    case CONCAT6:
    case CONCAT7:
    case CONCAT8:
    case CONCAT9:
        m_psz = RefStr::Create(option, psz, argList);
        break;
    case UPPERCASE:
    case LOWERCASE:
    case TRIM:
    case TRIM_HEAD:
    case TRIM_TAIL:
        m_psz = RefStr::Create(option, psz);
        break;
    default:
        throw Exception(L"String::ctor: Bad option.");
    }
    va_end(argList);
}


String::String(PCWSTR psz1, PCWSTR psz2)
    : m_psz(RefStr::Create(psz1, psz2))
{
}


String::String(SIZE_T cch)
    : m_psz(RefStr::Create(cch))
{
}


String::String(SIZE_T cch, WCHAR fill)
    : m_psz(RefStr::Create(cch, fill))
{
}


String::String(UINT cp, PCSTR psz, SSIZE_T cb)
    : m_psz(nullptr)
{
    cb = StrLen(psz, cb);
    if (cb)
    {
        int cch = MultiByteToWideChar(cp, MB_PRECOMPOSED, psz, static_cast<int>(cb), NULL, 0);
        if (cch <= 0)
        {
            throw Exception(L"String::ctor: MultiByteToWideChar failed.");
        }
        m_psz = RefStr::Create(cch);
        MultiByteToWideChar(cp, MB_PRECOMPOSED, psz, static_cast<int>(cb), m_psz, cch);
        m_psz[cch] = L'\0';
    }
    else
    {
        m_psz = RefStr::Create(L"");
    }
}


String::~String()
{
    Release(Interlocked<PWSTR>::ExchangePointer(&m_psz, nullptr));
}


String& String::ZeroFill()
{
    if (Len)
    {
        RefStr::GetThis(m_psz)->Fill(XLITERAL('\0'));
    }
    return *this;
}


String& String::Lettercase(StringOptions option)
{
    if (Len)
    {
        Release(Interlocked<PWSTR>::ExchangePointer(&m_psz, RefStr::Create(option, Ptr)));
    }
    return *this;
}


String& String::Trim(StringOptions option)
{
    if (Len)
    {
        Release(Interlocked<PWSTR>::ExchangePointer(&m_psz, RefStr::Create(option, Ptr)));
    }
    return *this;
}


String& String::Assign(const String& src)
{
    Release(Interlocked<PWSTR>::ExchangePointer(&m_psz, AddRef(src.m_psz)));
    return *this;
}


String& String::Assign(PCWSTR psz, SSIZE_T cch)
{
    Release(Interlocked<PWSTR>::ExchangePointer(&m_psz, RefStr::Create(psz, cch)));
    return *this;
}


String& String::Append(const String& src)
{
    if (src.Len)
    {
        Release(Interlocked<PWSTR>::ExchangePointer(&m_psz, RefStr::Create(Ptr, src.Ptr)));
    }
    return *this;
}


String& String::Append(PCWSTR psz, SSIZE_T cch)
{
    if (psz && cch)
    {
        Release(Interlocked<PWSTR>::ExchangePointer(&m_psz, RefStr::Create(Ptr, psz, cch)));
    }
    return *this;
}


String& String::Format(PCWSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    Release(Interlocked<PWSTR>::ExchangePointer(&m_psz, RefStr::Create(pszFormat, argList)));
    va_end(argList);
    return *this;
}


String& String::VaFormat(PCWSTR pszFormat, va_list argList)
{
    Release(Interlocked<PWSTR>::ExchangePointer(&m_psz, RefStr::Create(pszFormat, argList)));
    return *this;
}


String& String::AppendFormat(PCWSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    Release(Interlocked<PWSTR>::ExchangePointer(&m_psz, RefStr::Create(Ptr, pszFormat, argList)));
    va_end(argList);
    return *this;
}


String& String::VaAppendFormat(PCWSTR pszFormat, va_list argList)
{
    Release(Interlocked<PWSTR>::ExchangePointer(&m_psz, RefStr::Create(Ptr, pszFormat, argList)));
    return *this;
}


String& String::TruncateHead(SIZE_T cch)
{
    if (m_psz)
    {
        RefStr* pThis = RefStr::GetThis(m_psz);
        if (pThis->RefCnt > 1)
        {
            Release(Interlocked<PWSTR>::ExchangePointer(&m_psz, cch < pThis->Len ? RefStr::Create(&m_psz[cch]) : nullptr));
        }
        else if (cch < pThis->Len)
        {
            SIZE_T newLen = pThis->Len - cch;
            MemMove(&m_psz[0], &m_psz[cch], newLen);
            MemSet(&m_psz[newLen], XLITERAL('\0'), cch);
            pThis->SetLength(newLen);
        }
        else
        {
            pThis->Fill(XLITERAL('\0'));
        }
    }
    return *this;
}


String& String::TruncateTail(SIZE_T cch)
{
    if (m_psz)
    {
        RefStr* pThis = RefStr::GetThis(m_psz);
        if (pThis->RefCnt > 1)
        {
            Release(Interlocked<PWSTR>::ExchangePointer(&m_psz, cch < pThis->Len ? RefStr::Create(m_psz, pThis->Len - cch) : nullptr));
        }
        else if (cch < pThis->Len)
        {
            SIZE_T newLen = pThis->Len - cch;
            MemSet(&m_psz[newLen], XLITERAL('\0'), cch);
            pThis->SetLength(newLen);
        }
        else
        {
            pThis->Fill(XLITERAL('\0'));
        }
    }
    return *this;
}


int String::IndexOf(int c, SIZE_T fromIndex) const
{
    SIZE_T length = Len;
    if (m_psz && fromIndex < length)
    {
        int index = hnrt::IndexOf(m_psz + fromIndex, c, length - fromIndex);
        return index >= 0 ? (static_cast<int>(fromIndex) + index) : -1;
    }
    return -1;
}


int String::IndexOf(const String& s, SIZE_T fromIndex) const
{
    SIZE_T length = Len;
    if (m_psz && fromIndex < length)
    {
        int index = hnrt::IndexOf(m_psz + fromIndex, s, length - fromIndex);
        return index >= 0 ? (static_cast<int>(fromIndex) + index) : -1;
    }
    return -1;
}


bool String::StartsWith(PCWSTR psz, SSIZE_T cch) const
{
    SIZE_T cchActual = StrLen(psz, cch);
    if (cchActual == 0)
    {
        return true;
    }
    else if (cchActual <= Len)
    {
        return Compare(m_psz, cchActual, psz, cchActual) == 0;
    }
    else
    {
        return false;
    }
}


bool String::EndsWith(PCWSTR psz, SSIZE_T cch) const
{
    SIZE_T cchActual = StrLen(psz, cch);
    if (cchActual == 0)
    {
        return true;
    }
    else if (cchActual <= Len)
    {
        return Compare(m_psz + Len - cchActual, cchActual, psz, cchActual) == 0;
    }
    else
    {
        return false;
    }
}


String String::Replace(PCWSTR psz1, PCWSTR psz2, int count) const
{
    if (m_psz)
    {
        SSIZE_T len = Len;
        SSIZE_T len1 = StrLen(psz1);
        PWCHAR pEnd = m_psz + len;
        PWCHAR pCur = m_psz;
        int cnt1 = 0;
        while (pCur < pEnd)
        {
            SSIZE_T index = hnrt::IndexOf(pCur, psz1, pEnd - pCur);
            if (index < 0 || ++cnt1 >= count)
            {
                break;
            }
            pCur += index + len1;
        }
        if (cnt1)
        {
            SSIZE_T len2 = StrLen(psz2);
            SSIZE_T delta = (len2 - len1) * cnt1;
            if (len + delta)
            {
                String sz(len + delta);
                PWCHAR pDst = sz.m_psz;
                pCur = m_psz;
                while (cnt1--)
                {
                    SSIZE_T index = hnrt::IndexOf(pCur, psz1, pEnd - pCur);
                    pDst += MemCpy(pDst, pCur, index);
                    pDst += MemCpy(pDst, psz2, len2);
                    pCur += index + len1;
                }
                pDst += MemCpy(pDst, pCur, pEnd - pCur);
                pDst[0] = static_cast<WCHAR>(0);
                return sz;
            }
        }
    }
    return *this;
}


String String::Wrap(UINT width, PCWSTR pszNewLine) const
{
    if (!width)
    {
        return String(*this);
    }
    SIZE_T len = Len;
    SIZE_T lenNewLine = StrLen(pszNewLine);
    SIZE_T delta = (len / static_cast<SIZE_T>(width)) * lenNewLine;
    String sz(len + delta);
    PWCHAR pSrc = &m_psz[0];
    PWCHAR pEnd = &m_psz[len];
    PWCHAR pDst = sz.m_psz;
    while (pSrc + width <= pEnd)
    {
        pDst += MemCpy(pDst, pSrc, width);
        pDst += MemCpy(pDst, pszNewLine, lenNewLine);
        pSrc += width;
    }
    pDst += MemCpy(pDst, pSrc, pEnd - pSrc);
    pDst[0] = static_cast<WCHAR>(0);
    return sz;
}


String String::ChangeLineBreak(LineBreak lbSpec) const
{
    SIZE_T len = Len;
    if (len > 0 && (lbSpec == LineBreak::CRLF || lbSpec == LineBreak::LF))
    {
        SIZE_T cLF = 0;
        SIZE_T cCRLF = 0;
        PWCHAR pCur = &m_psz[0];
        PWCHAR pStop = &m_psz[len - 1];
        PWCHAR pEnd = &m_psz[len];
        while (pCur < pStop)
        {
            WCHAR c = *pCur++;
            if (c == static_cast<WCHAR>(10))
            {
                cLF++;
            }
            else if (c == static_cast<WCHAR>(13) && *pCur == static_cast<WCHAR>(10))
            {
                cCRLF++;
                pCur++;
            }
        }
        if (pCur < pEnd && *pCur == static_cast<WCHAR>(10))
        {
            cLF++;
        }
        if (lbSpec == LineBreak::CRLF)
        {
            if (cLF > 0)
            {
                String sz(len + cLF);
                PWCHAR pDst = sz.m_psz;
                pCur = &m_psz[0];
                while (pCur < pStop)
                {
                    WCHAR c = *pCur++;
                    if (c == static_cast<WCHAR>(10))
                    {
                        *pDst++ = static_cast<WCHAR>(13);
                        *pDst++ = static_cast<WCHAR>(10);
                    }
                    else if (c == static_cast<WCHAR>(13) && *pCur == static_cast<WCHAR>(10))
                    {
                        *pDst++ = static_cast<WCHAR>(13);
                        *pDst++ = static_cast<WCHAR>(10);
                        pCur++;
                    }
                    else
                    {
                        *pDst++ = c;
                    }
                }
                if (pCur < pEnd)
                {
                    WCHAR c = *pCur++;
                    if (c == static_cast<WCHAR>(10))
                    {
                        *pDst++ = static_cast<WCHAR>(13);
                        *pDst++ = static_cast<WCHAR>(10);
                    }
                    else
                    {
                        *pDst++ = c;
                    }
                }
                *pDst = static_cast<WCHAR>(0);
                return sz;
            }
        }
        else if (cCRLF > 0)
        {
            String sz(len - cCRLF);
            PWCHAR pDst = sz.m_psz;
            pCur = &m_psz[0];
            while (pCur < pStop)
            {
                WCHAR c = *pCur++;
                if (c == static_cast<WCHAR>(13) && *pCur == static_cast<WCHAR>(10))
                {
                    *pDst++ = static_cast<WCHAR>(10);
                    pCur++;
                }
                else
                {
                    *pDst++ = c;
                }
            }
            if (pCur < pEnd)
            {
                *pDst++ = *pCur++;
            }
            *pDst = static_cast<WCHAR>(0);
            return sz;
        }
    }
    return String(*this);
}


long String::ToLong(long defaultValue, BOOL* pbSuccessful, int nRadix)
{
    if (Len)
    {
        PWCHAR pStop = nullptr;
        long value = StrToLong(&m_psz[0], &pStop, nRadix);
        if (!*pStop)
        {
            if (pbSuccessful)
            {
                *pbSuccessful = TRUE;
            }
            return value;
        }
    }
    if (pbSuccessful)
    {
        *pbSuccessful = FALSE;
    }
    return defaultValue;
}


unsigned long String::ToUnsignedLong(unsigned long defaultValue, BOOL* pbSuccessful, int nRadix)
{
    if (Len)
    {
        PWCHAR pStop = nullptr;
        unsigned long value = StrToUnsignedLong(&m_psz[0], &pStop, nRadix);
        if (!*pStop)
        {
            if (pbSuccessful)
            {
                *pbSuccessful = TRUE;
            }
            return value;
        }
    }
    if (pbSuccessful)
    {
        *pbSuccessful = FALSE;
    }
    return defaultValue;
}


long long String::ToLongLong(long long defaultValue, BOOL* pbSuccessful, int nRadix)
{
    if (Len)
    {
        PWCHAR pStop = nullptr;
        long long value = StrToLongLong(&m_psz[0], &pStop, nRadix);
        if (!*pStop)
        {
            if (pbSuccessful)
            {
                *pbSuccessful = TRUE;
            }
            return value;
        }
    }
    if (pbSuccessful)
    {
        *pbSuccessful = FALSE;
    }
    return defaultValue;
}


unsigned long long String::ToUnsignedLongLong(unsigned long long defaultValue, BOOL* pbSuccessful, int nRadix)
{
    if (Len)
    {
        PWCHAR pStop = nullptr;
        unsigned long long value = StrToUnsignedLongLong(&m_psz[0], &pStop, nRadix);
        if (!*pStop)
        {
            if (pbSuccessful)
            {
                *pbSuccessful = TRUE;
            }
            return value;
        }
    }
    if (pbSuccessful)
    {
        *pbSuccessful = FALSE;
    }
    return defaultValue;
}


double String::ToDouble(double defaultValue, BOOL* pbSuccessful)
{
    if (Len)
    {
        PWCHAR pStop = nullptr;
        double value = StrToDouble(&m_psz[0], &pStop);
        if (!*pStop)
        {
            if (pbSuccessful)
            {
                *pbSuccessful = TRUE;
            }
            return value;
        }
    }
    if (pbSuccessful)
    {
        *pbSuccessful = FALSE;
    }
    return defaultValue;
}


bool String::operator ==(const String& other) const
{
    return StrCmp(Ptr, -1, other.Ptr, -1) == 0;
}


bool String::operator !=(const String& other) const
{
    return StrCmp(Ptr, -1, other.Ptr, -1) != 0;
}


bool String::operator <(const String& other) const
{
    return StrCmp(Ptr, -1, other.Ptr, -1) < 0;
}


bool String::operator <=(const String& other) const
{
    return StrCmp(Ptr, -1, other.Ptr, -1) <= 0;
}


bool String::operator >(const String& other) const
{
    return StrCmp(Ptr, -1, other.Ptr, -1) > 0;
}


bool String::operator >=(const String& other) const
{
    return StrCmp(Ptr, -1, other.Ptr, -1) >= 0;
}


String String::operator +(const String& other) const
{
    return String(Ptr, other.Ptr);
}


SIZE_T String::get_len() const
{
    return m_psz ? RefStr::GetThis(m_psz)->Len : 0;
}


void String::set_len(SIZE_T len)
{
    if (m_psz)
    {
        RefStr::GetThis(m_psz)->SetLength(len);
    }
}


int String::Compare(PCWSTR psz1, SSIZE_T cch1, PCWSTR psz2, SSIZE_T cch2)
{
    return StrCmp(psz1, cch1, psz2, cch2);
}


MultibyteString String::ToAcp() const
{
	return MultibyteString(CP_ACP, Ptr);
}


MultibyteString String::ToUTF8() const
{
	return MultibyteString(CP_UTF8, Ptr);
}


MultibyteString String::ToMultibyteString(UINT cp) const
{
    return MultibyteString(cp, Ptr);
}
