#include "hnrt/RefString.h"
#include "hnrt/Interlocked.h"
#include "hnrt/Exception.h"


#if defined(STRINGUCS)
#include "hnrt/StringCaseInsensitive.h"
#include "hnrt/String.h"
#define XCHAR WCHAR
#define PXCHAR PWCHAR
#define PXSTR PWSTR
#define PCXSTR PCWSTR
#define XSTRING StringCaseInsensitive
#define XSTRING2 String
#define XLITERAL(x) L##x
#elif defined(STRINGACP)
#include "hnrt/StringCaseInsensitiveAcp.h"
#include "hnrt/StringAcp.h"
#define XCHAR CHAR
#define PXCHAR PCHAR
#define PXSTR PSTR
#define PCXSTR PCSTR
#define XSTRING StringCaseInsensitiveAcp
#define XSTRING2 StringAcp
#define XLITERAL(x) x
#else
#error Define STRINGUCS or STRINGACP
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


XSTRING::XSTRING(const XSTRING2& other)
    : m_psz(AddRef(other.m_psz))
{
}


XSTRING::~XSTRING()
{
    Release(Interlocked<PXSTR>::ExchangePointer(&m_psz, nullptr));
}


XSTRING& XSTRING::operator =(const XSTRING& other)
{
    Release(Interlocked<PXSTR>::ExchangePointer(&m_psz, AddRef(other.m_psz)));
    return *this;
}


XSTRING& XSTRING::operator =(const XSTRING2& other)
{
    Release(Interlocked<PXSTR>::ExchangePointer(&m_psz, AddRef(other.m_psz)));
    return *this;
}


XSTRING& XSTRING::operator +=(const XSTRING& other)
{
    if (other.Len)
    {
        Release(Interlocked<PXSTR>::ExchangePointer(&m_psz, RefStr::Create(Ptr, other.Ptr)));
    }
    return *this;
}


XSTRING& XSTRING::operator +=(const XSTRING2& other)
{
    if (other.Len)
    {
        Release(Interlocked<PXSTR>::ExchangePointer(&m_psz, RefStr::Create(Ptr, other.Ptr)));
    }
    return *this;
}


bool XSTRING::operator ==(const XSTRING& other) const
{
    return StringCommons::CaseCompare(Ptr, -1, other.Ptr, -1) == 0;
}


bool XSTRING::operator !=(const XSTRING& other) const
{
    return StringCommons::CaseCompare(Ptr, -1, other.Ptr, -1) != 0;
}


bool XSTRING::operator <(const XSTRING& other) const
{
    return StringCommons::CaseCompare(Ptr, -1, other.Ptr, -1) < 0;
}


bool XSTRING::operator <=(const XSTRING& other) const
{
    return StringCommons::CaseCompare(Ptr, -1, other.Ptr, -1) <= 0;
}


bool XSTRING::operator >(const XSTRING& other) const
{
    return StringCommons::CaseCompare(Ptr, -1, other.Ptr, -1) > 0;
}


bool XSTRING::operator >=(const XSTRING& other) const
{
    return StringCommons::CaseCompare(Ptr, -1, other.Ptr, -1) >= 0;
}


XSTRING XSTRING::operator +(const XSTRING& other) const
{
    return XSTRING(XSTRING2(Ptr, other.Ptr));
}


XSTRING XSTRING::operator +(const XSTRING2& other) const
{
    return XSTRING(XSTRING2(Ptr, other.Ptr));
}


SIZE_T XSTRING::get_len() const
{
    return m_psz ? RefStr::GetThis(m_psz)->Len : 0;
}


int XSTRING::Compare(PCXSTR psz1, SSIZE_T cch1, PCXSTR psz2, SSIZE_T cch2)
{
    return StringCommons::CaseCompare(psz1, cch1, psz2, cch2);
}
