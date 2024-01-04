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
    cb = StringCommons::Length(psz, cb);
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
    cch = StringCommons::Length(psz, cch);
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
            StringCommons::Move(&m_psz[0], &m_psz[cch], newLen);
            StringCommons::Fill(&m_psz[newLen], XLITERAL('\0'), cch);
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
            StringCommons::Fill(&m_psz[newLen], XLITERAL('\0'), cch);
            pThis->SetLength(newLen);
        }
        else
        {
            pThis->Fill(XLITERAL('\0'));
        }
    }
    return *this;
}


int XSTRING::IndexOf(XCHAR c, SIZE_T fromIndex) const
{
    SIZE_T length = Len;
    if (m_psz && fromIndex < length)
    {
        int index = StringCommons::IndexOf(m_psz + fromIndex, c, length - fromIndex);
        return index >= 0 ? (static_cast<int>(fromIndex) + index) : -1;
    }
    return -1;
}


int XSTRING::IndexOf(const XSTRING& s, SIZE_T fromIndex) const
{
    SIZE_T length = Len;
    if (m_psz && fromIndex < length)
    {
        PXSTR psz = s.m_psz;
        SSIZE_T cch = s.Len;
        if (cch)
        {
            cch--;
            XCHAR c = *psz++;
            int index = StringCommons::IndexOf(m_psz + fromIndex, c, length - fromIndex);
            PXSTR pCur = index >= 0 ? (m_psz + fromIndex + static_cast<SIZE_T>(index)) : nullptr;
            if (cch)
            {
                PCXSTR pEnd = m_psz + length;
                while (pCur)
                {
                    pCur++;
                    if (pEnd < pCur + cch)
                    {
                        break;
                    }
                    if (!Compare(psz, cch, pCur, cch))
                    {
                        return static_cast<int>((pCur - 1) - m_psz);
                    }
                    int index = StringCommons::IndexOf(pCur, c, pEnd - pCur);
                    pCur = index >= 0 ? (pCur + index) : nullptr;
                }
            }
            else if (pCur)
            {
                return static_cast<int>(pCur - m_psz);
            }
        }
    }
    return -1;
}


bool XSTRING::StartsWith(PCXSTR psz, SSIZE_T cch) const
{
    SIZE_T cchActual = StringCommons::Length(psz, cch);
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
    SIZE_T cchActual = StringCommons::Length(psz, cch);
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


XSTRING XSTRING::Wrap(UINT width, PCXSTR pszNewLine) const
{
    if (!width)
    {
        return XSTRING(*this);
    }
    SIZE_T len = Len;
    SIZE_T lenNewLine = StringCommons::Length(pszNewLine);
    SIZE_T delta = (len / static_cast<SIZE_T>(width)) * lenNewLine;
    XSTRING sz(len + delta);
    PXCHAR pSrc = &m_psz[0];
    PXCHAR pEnd = &m_psz[len];
    PXCHAR pDst = const_cast<PXCHAR>(sz.Ptr);
    while (pSrc + width <= pEnd)
    {
        pDst += StringCommons::Copy(pDst, pSrc, width);
        pDst += StringCommons::Copy(pDst, pszNewLine, lenNewLine);
        pSrc += width;
    }
    pDst += StringCommons::Copy(pDst, pSrc, pEnd - pSrc);
    pDst[0] = (XCHAR)0;
    return sz;
}


bool XSTRING::operator ==(const XSTRING& other) const
{
    return StringCommons::Compare(Ptr, -1, other.Ptr, -1) == 0;
}


bool XSTRING::operator !=(const XSTRING& other) const
{
    return StringCommons::Compare(Ptr, -1, other.Ptr, -1) != 0;
}


bool XSTRING::operator <(const XSTRING& other) const
{
    return StringCommons::Compare(Ptr, -1, other.Ptr, -1) < 0;
}


bool XSTRING::operator <=(const XSTRING& other) const
{
    return StringCommons::Compare(Ptr, -1, other.Ptr, -1) <= 0;
}


bool XSTRING::operator >(const XSTRING& other) const
{
    return StringCommons::Compare(Ptr, -1, other.Ptr, -1) > 0;
}


bool XSTRING::operator >=(const XSTRING& other) const
{
    return StringCommons::Compare(Ptr, -1, other.Ptr, -1) >= 0;
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
    return StringCommons::Compare(psz1, cch1, psz2, cch2);
}


XSTRING XSTRING::ToHex(const void* ptr, SIZE_T len, StringOptions option)
{
    static const XCHAR HexLU[2][16] = {
        { XLITERAL('0'), XLITERAL('1'), XLITERAL('2'), XLITERAL('3'),
          XLITERAL('4'), XLITERAL('5'), XLITERAL('6'), XLITERAL('7'),
          XLITERAL('8'), XLITERAL('9'), XLITERAL('a'), XLITERAL('b'),
          XLITERAL('c'), XLITERAL('d'), XLITERAL('e'), XLITERAL('f') },
        { XLITERAL('0'), XLITERAL('1'), XLITERAL('2'), XLITERAL('3'),
          XLITERAL('4'), XLITERAL('5'), XLITERAL('6'), XLITERAL('7'),
          XLITERAL('8'), XLITERAL('9'), XLITERAL('A'), XLITERAL('B'),
          XLITERAL('C'), XLITERAL('D'), XLITERAL('E'), XLITERAL('F') }
    };
    const XCHAR* hex = HexLU[option == UPPERCASE ? 1 : 0];
    XSTRING sz(len * 2, XLITERAL('*'));
    PXCHAR pW = sz.m_psz;
    const unsigned char* pCur = reinterpret_cast<const unsigned char*>(ptr);
    const unsigned char* pEnd = pCur + len;
    while (pCur < pEnd)
    {
        unsigned char c = *pCur++;
        pW[0] = hex[(c >> 4) & 0xF];
        pW[1] = hex[(c >> 0) & 0xF];
        pW += 2;
    }
    return sz;
}
