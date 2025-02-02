#include "hnrt/RefString.h"
#include "hnrt/Interlocked.h"
#include "hnrt/Exception.h"


#if defined(STRINGUCS)
#include "hnrt/String.h"
#define XCHAR WCHAR
#define PXCHAR PWCHAR
#define PXSTR PWSTR
#define PCXSTR PCWSTR
#define XSTRING String
#define XLITERAL(x) L##x
#elif defined(STRINGACP)
#include "hnrt/StringAcp.h"
#define XCHAR CHAR
#define PXCHAR PCHAR
#define PXSTR PSTR
#define PCXSTR PCSTR
#define XSTRING StringAcp
#define XLITERAL(x) x
#define XCODEPAGE CP_ACP
#elif defined(STRINGUTF8)
#include "hnrt/StringUTF8.h"
#define XCHAR CHAR
#define PXCHAR PCHAR
#define PXSTR PSTR
#define PCXSTR PCSTR
#define XSTRING StringUTF8
#define XLITERAL(x) x
#define XCODEPAGE CP_UTF8
#else
#error Define STRINGUCS or STRINGACP or STRINGUTF8
#endif


using namespace hnrt;


typedef RefString<XCHAR> RefStr;


inline PXSTR AddRef(PXSTR psz)
{
    if (psz)
    {
        RefStr::GetThis(psz)->AddRef();
    }
    return psz;
}


inline void Release(PXSTR psz)
{
    if (psz)
    {
        RefStr::GetThis(psz)->Release();
    }
}


const XSTRING XSTRING::Empty = XSTRING();


XSTRING::XSTRING()
    : m_psz(nullptr)
{
}


XSTRING::XSTRING(const XSTRING& other)
    : m_psz(AddRef(other.m_psz))
{
}


XSTRING::XSTRING(PCXSTR psz, SSIZE_T cch)
    : m_psz(RefStr::Create(psz, cch))
{
}


XSTRING::XSTRING(PCXSTR pszFormat, va_list argList)
    : m_psz(RefStr::Create(pszFormat, argList))
{
}


XSTRING::XSTRING(StringOptions option, PCXSTR psz, ...)
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


XSTRING::XSTRING(PCXSTR psz1, PCXSTR psz2)
    : m_psz(RefStr::Create(psz1, psz2))
{
}


XSTRING::XSTRING(SIZE_T cch)
    : m_psz(RefStr::Create(cch))
{
}


XSTRING::XSTRING(SIZE_T cch, XCHAR fill)
    : m_psz(RefStr::Create(cch, fill))
{
}


#if defined(STRINGUCS)
XSTRING::XSTRING(UINT cp, PCSTR psz, SSIZE_T cb)
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
#endif //STRINGUCS


#if defined(STRINGACP) || defined(STRINGUTF8)
XSTRING::XSTRING(PCWSTR psz, SSIZE_T cch)
    : m_psz(nullptr)
{
    cch = StrLen(psz, cch);
    if (cch)
    {
        int cb = WideCharToMultiByte(XCODEPAGE, WC_ERR_INVALID_CHARS, psz, static_cast<int>(cch), NULL, 0, NULL, NULL);
        if (cb <= 0)
        {
            throw Exception(L"String::ctor: WideCharToMultiByte failed.");
        }
        m_psz = RefStr::Create(cb);
        WideCharToMultiByte(XCODEPAGE, WC_ERR_INVALID_CHARS, psz, static_cast<int>(cch), m_psz, cb, NULL, NULL);
        m_psz[cb] = '\0';
    }
    else
    {
        m_psz = RefStr::Create("");
    }
}
#endif //STRINGACP


XSTRING::~XSTRING()
{
    Release(Interlocked<PXSTR>::ExchangePointer(&m_psz, nullptr));
}


XSTRING& XSTRING::ZeroFill()
{
    if (Len)
    {
        RefStr::GetThis(m_psz)->Fill(XLITERAL('\0'));
    }
    return *this;
}


XSTRING& XSTRING::Lettercase(StringOptions option)
{
    if (Len)
    {
        Release(Interlocked<PXSTR>::ExchangePointer(&m_psz, RefStr::Create(option, Ptr)));
    }
    return *this;
}


XSTRING& XSTRING::Trim(StringOptions option)
{
    if (Len)
    {
        Release(Interlocked<PXSTR>::ExchangePointer(&m_psz, RefStr::Create(option, Ptr)));
    }
    return *this;
}


XSTRING& XSTRING::Assign(const XSTRING& other)
{
    Release(Interlocked<PXSTR>::ExchangePointer(&m_psz, AddRef(other.m_psz)));
    return *this;
}


XSTRING& XSTRING::Assign(PCXSTR psz, SSIZE_T cch)
{
    Release(Interlocked<PXSTR>::ExchangePointer(&m_psz, RefStr::Create(psz, cch)));
    return *this;
}


XSTRING& XSTRING::Append(const XSTRING& other)
{
    if (other.Len)
    {
        Release(Interlocked<PXSTR>::ExchangePointer(&m_psz, RefStr::Create(Ptr, other.Ptr)));
    }
    return *this;
}


XSTRING& XSTRING::Append(PCXSTR psz, SSIZE_T cch)
{
    if (psz && cch)
    {
        Release(Interlocked<PXSTR>::ExchangePointer(&m_psz, RefStr::Create(Ptr, psz, cch)));
    }
    return *this;
}


XSTRING& XSTRING::Format(PCXSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    Release(Interlocked<PXSTR>::ExchangePointer(&m_psz, RefStr::Create(pszFormat, argList)));
    va_end(argList);
    return *this;
}


XSTRING& XSTRING::VaFormat(PCXSTR pszFormat, va_list argList)
{
    Release(Interlocked<PXSTR>::ExchangePointer(&m_psz, RefStr::Create(pszFormat, argList)));
    return *this;
}


XSTRING& XSTRING::AppendFormat(PCXSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    Release(Interlocked<PXSTR>::ExchangePointer(&m_psz, RefStr::Create(Ptr, pszFormat, argList)));
    va_end(argList);
    return *this;
}


XSTRING& XSTRING::VaAppendFormat(PCXSTR pszFormat, va_list argList)
{
    Release(Interlocked<PXSTR>::ExchangePointer(&m_psz, RefStr::Create(Ptr, pszFormat, argList)));
    return *this;
}


XSTRING& XSTRING::TruncateHead(SIZE_T cch)
{
    if (m_psz)
    {
        RefStr* pThis = RefStr::GetThis(m_psz);
        if (pThis->RefCnt > 1)
        {
            Release(Interlocked<PXSTR>::ExchangePointer(&m_psz, cch < pThis->Len ? RefStr::Create(&m_psz[cch]) : nullptr));
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


XSTRING& XSTRING::TruncateTail(SIZE_T cch)
{
    if (m_psz)
    {
        RefStr* pThis = RefStr::GetThis(m_psz);
        if (pThis->RefCnt > 1)
        {
            Release(Interlocked<PXSTR>::ExchangePointer(&m_psz, cch < pThis->Len ? RefStr::Create(m_psz, pThis->Len - cch) : nullptr));
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


int XSTRING::IndexOf(int c, SIZE_T fromIndex) const
{
    SIZE_T length = Len;
    if (m_psz && fromIndex < length)
    {
        int index = hnrt::IndexOf(m_psz + fromIndex, c, length - fromIndex);
        return index >= 0 ? (static_cast<int>(fromIndex) + index) : -1;
    }
    return -1;
}


int XSTRING::IndexOf(const XSTRING& s, SIZE_T fromIndex) const
{
    SIZE_T length = Len;
    if (m_psz && fromIndex < length)
    {
        int index = hnrt::IndexOf(m_psz + fromIndex, s, length - fromIndex);
        return index >= 0 ? (static_cast<int>(fromIndex) + index) : -1;
    }
    return -1;
}


bool XSTRING::StartsWith(PCXSTR psz, SSIZE_T cch) const
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


bool XSTRING::EndsWith(PCXSTR psz, SSIZE_T cch) const
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


XSTRING XSTRING::Replace(PCXSTR psz1, PCXSTR psz2, int count) const
{
    if (m_psz)
    {
        SSIZE_T len = Len;
        SSIZE_T len1 = StrLen(psz1);
        PXCHAR pEnd = m_psz + len;
        PXCHAR pCur = m_psz;
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
                XSTRING sz(len + delta);
                PXCHAR pDst = sz.m_psz;
                pCur = m_psz;
                while (cnt1--)
                {
                    SSIZE_T index = hnrt::IndexOf(pCur, psz1, pEnd - pCur);
                    pDst += MemCpy(pDst, pCur, index);
                    pDst += MemCpy(pDst, psz2, len2);
                    pCur += index + len1;
                }
                pDst += MemCpy(pDst, pCur, pEnd - pCur);
                pDst[0] = static_cast<XCHAR>(0);
                return sz;
            }
        }
    }
    return *this;
}


XSTRING XSTRING::Wrap(UINT width, PCXSTR pszNewLine) const
{
    if (!width)
    {
        return XSTRING(*this);
    }
    SIZE_T len = Len;
    SIZE_T lenNewLine = StrLen(pszNewLine);
    SIZE_T delta = (len / static_cast<SIZE_T>(width)) * lenNewLine;
    XSTRING sz(len + delta);
    PXCHAR pSrc = &m_psz[0];
    PXCHAR pEnd = &m_psz[len];
    PXCHAR pDst = sz.m_psz;
    while (pSrc + width <= pEnd)
    {
        pDst += MemCpy(pDst, pSrc, width);
        pDst += MemCpy(pDst, pszNewLine, lenNewLine);
        pSrc += width;
    }
    pDst += MemCpy(pDst, pSrc, pEnd - pSrc);
    pDst[0] = static_cast<XCHAR>(0);
    return sz;
}


XSTRING XSTRING::ChangeLineBreak(LineBreak lbSpec) const
{
    SIZE_T len = Len;
    if (len > 0 && (lbSpec == LineBreak::CRLF || lbSpec == LineBreak::LF))
    {
        SIZE_T cLF = 0;
        SIZE_T cCRLF = 0;
        PXCHAR pCur = &m_psz[0];
        PXCHAR pStop = &m_psz[len - 1];
        PXCHAR pEnd = &m_psz[len];
        while (pCur < pStop)
        {
            XCHAR c = *pCur++;
            if (c == static_cast<XCHAR>(10))
            {
                cLF++;
            }
            else if (c == static_cast<XCHAR>(13) && *pCur == static_cast<XCHAR>(10))
            {
                cCRLF++;
                pCur++;
            }
        }
        if (pCur < pEnd && *pCur == static_cast<XCHAR>(10))
        {
            cLF++;
        }
        if (lbSpec == LineBreak::CRLF)
        {
            if (cLF > 0)
            {
                XSTRING sz(len + cLF);
                PXCHAR pDst = sz.m_psz;
                pCur = &m_psz[0];
                while (pCur < pStop)
                {
                    XCHAR c = *pCur++;
                    if (c == static_cast<XCHAR>(10))
                    {
                        *pDst++ = static_cast<XCHAR>(13);
                        *pDst++ = static_cast<XCHAR>(10);
                    }
                    else if (c == static_cast<XCHAR>(13) && *pCur == static_cast<XCHAR>(10))
                    {
                        *pDst++ = static_cast<XCHAR>(13);
                        *pDst++ = static_cast<XCHAR>(10);
                        pCur++;
                    }
                    else
                    {
                        *pDst++ = c;
                    }
                }
                if (pCur < pEnd)
                {
                    XCHAR c = *pCur++;
                    if (c == static_cast<XCHAR>(10))
                    {
                        *pDst++ = static_cast<XCHAR>(13);
                        *pDst++ = static_cast<XCHAR>(10);
                    }
                    else
                    {
                        *pDst++ = c;
                    }
                }
                *pDst = static_cast<XCHAR>(0);
                return sz;
            }
        }
        else if (cCRLF > 0)
        {
            XSTRING sz(len - cCRLF);
            PXCHAR pDst = sz.m_psz;
            pCur = &m_psz[0];
            while (pCur < pStop)
            {
                XCHAR c = *pCur++;
                if (c == static_cast<XCHAR>(13) && *pCur == static_cast<XCHAR>(10))
                {
                    *pDst++ = static_cast<XCHAR>(10);
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
            *pDst = static_cast<XCHAR>(0);
            return sz;
        }
    }
    return XSTRING(*this);
}


long XSTRING::ToLong(long defaultValue, BOOL* pbSuccessful, int nRadix)
{
    if (Len)
    {
        PXCHAR pStop = nullptr;
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


unsigned long XSTRING::ToUnsignedLong(unsigned long defaultValue, BOOL* pbSuccessful, int nRadix)
{
    if (Len)
    {
        PXCHAR pStop = nullptr;
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


long long XSTRING::ToLongLong(long long defaultValue, BOOL* pbSuccessful, int nRadix)
{
    if (Len)
    {
        PXCHAR pStop = nullptr;
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


unsigned long long XSTRING::ToUnsignedLongLong(unsigned long long defaultValue, BOOL* pbSuccessful, int nRadix)
{
    if (Len)
    {
        PXCHAR pStop = nullptr;
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


double XSTRING::ToDouble(double defaultValue, BOOL* pbSuccessful)
{
    if (Len)
    {
        PXCHAR pStop = nullptr;
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


bool XSTRING::operator ==(const XSTRING& other) const
{
    return StrCmp(Ptr, -1, other.Ptr, -1) == 0;
}


bool XSTRING::operator !=(const XSTRING& other) const
{
    return StrCmp(Ptr, -1, other.Ptr, -1) != 0;
}


bool XSTRING::operator <(const XSTRING& other) const
{
    return StrCmp(Ptr, -1, other.Ptr, -1) < 0;
}


bool XSTRING::operator <=(const XSTRING& other) const
{
    return StrCmp(Ptr, -1, other.Ptr, -1) <= 0;
}


bool XSTRING::operator >(const XSTRING& other) const
{
    return StrCmp(Ptr, -1, other.Ptr, -1) > 0;
}


bool XSTRING::operator >=(const XSTRING& other) const
{
    return StrCmp(Ptr, -1, other.Ptr, -1) >= 0;
}


XSTRING XSTRING::operator +(const XSTRING& other) const
{
    return XSTRING(Ptr, other.Ptr);
}


SIZE_T XSTRING::get_len() const
{
    return m_psz ? RefStr::GetThis(m_psz)->Len : 0;
}


void XSTRING::set_len(SIZE_T len)
{
    if (m_psz)
    {
        RefStr::GetThis(m_psz)->SetLength(len);
    }
}


int XSTRING::Compare(PCXSTR psz1, SSIZE_T cch1, PCXSTR psz2, SSIZE_T cch2)
{
    return StrCmp(psz1, cch1, psz2, cch2);
}
