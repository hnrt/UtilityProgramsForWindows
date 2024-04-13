#include "pch.h"
#include "hnrt/SFID.h"
#include "hnrt/SFIDException.h"
#include "hnrt/StringCommons.h"


using namespace hnrt;


SFID SFID::Parse(PCWSTR psz)
{
    psz += ParseBase62(psz, SFID_KEYPREFIX_LENGTH, SFID_KEYPREFIX_OFFSET, false);
    psz += ParseBase62(psz, SFID_INSTANCE_LENGTH, SFID_INSTANCE_OFFSET, false);
    psz += ParseBase62(psz, SFID_RESERVED_LENGTH, SFID_RESERVED_OFFSET, false);
    psz += ParseBase62(psz, SFID_UNIQUEID_LENGTH, SFID_UNIQUEID_OFFSET, false);
    if (psz[0])
    {
        psz += ParseBase62(psz, SFID_CHECKSUM_LENGTH, SFID_CHECKSUM_OFFSET, false);
        if (psz[0])
        {
            throw SFIDException(SFID_TOO_LONG, SFID_LENGTH, L"Too long identifier; %d>%d", static_cast<int>(StrLen(psz)) + SFID_LENGTH, SFID_LENGTH);
        }
        String szChecksumActual(&psz[-SFID_CHECKSUM_LENGTH]);
        String szChecksumExpected = ComputeChecksum(&psz[-SFID_LENGTH]);
        if (szChecksumActual != szChecksumExpected)
        {
            throw SFIDException(SFID_INCORRECT_CHECKSUM, SFID_LENGTH_EXCLUDING_CHECKSUM, L"Incorrect checksum. Expected=%s", szChecksumExpected);
        }
        return SFID(&psz[-SFID_LENGTH]);
    }
    else
    {
        String szChecksum = ComputeChecksum(&psz[-SFID_LENGTH_EXCLUDING_CHECKSUM]);
        return SFID(String(&psz[-SFID_LENGTH_EXCLUDING_CHECKSUM], szChecksum));
    }
}


SFID SFID::Parse(PCWSTR pszKeyPrefix, PCWSTR pszInstance, PCWSTR pszUniqueId)
{
    ParseBase62(pszKeyPrefix, SFID_KEYPREFIX_LENGTH, SFID_KEYPREFIX_OFFSET, true);
    ParseBase62(pszInstance, SFID_INSTANCE_LENGTH, SFID_INSTANCE_OFFSET, true);
    ParseBase62(pszUniqueId, SFID_UNIQUEID_LENGTH, SFID_UNIQUEID_OFFSET, true);
    return SFID(pszKeyPrefix, pszInstance, pszUniqueId);
}


static const WCHAR ChecksumTable[32] =
{
    L'A', L'B', L'C', L'D', L'E', L'F', L'G', L'H', L'I', L'J', L'K', L'L', L'M', L'N', L'O', L'P',
    L'Q', L'R', L'S', L'T', L'U', L'V', L'W', L'X', L'Y', L'Z', L'0', L'1', L'2', L'3', L'4', L'5'
};


String SFID::ComputeChecksum(const WCHAR s[])
{
    int a1 = 0;
    int a2 = 0;
    int a3 = 0;
    for (int index = 0; index < 5; index++)
    {
        WCHAR c1 = s[1 * 5 - 1 - index];
        WCHAR c2 = s[2 * 5 - 1 - index];
        WCHAR c3 = s[3 * 5 - 1 - index];
        int b1 = (static_cast<unsigned>(c1 - L'A') <= static_cast<unsigned>(L'Z' - L'A')) ? 1 : 0;
        int b2 = (static_cast<unsigned>(c2 - L'A') <= static_cast<unsigned>(L'Z' - L'A')) ? 1 : 0;
        int b3 = (static_cast<unsigned>(c3 - L'A') <= static_cast<unsigned>(L'Z' - L'A')) ? 1 : 0;
        a1 = (a1 << 1) | b1;
        a2 = (a2 << 1) | b2;
        a3 = (a3 << 1) | b3;
    }
    return String(PRINTF, L"%c%c%c", ChecksumTable[a1], ChecksumTable[a2], ChecksumTable[a3]);
}


SFID::SFID()
    : m_d()
{
    MemSet(m_d, L'0', SFID_LENGTH_EXCLUDING_CHECKSUM);
    String szChecksum = ComputeChecksum(m_d);
    MemCpy(&m_d[SFID_CHECKSUM_OFFSET], szChecksum, SFID_CHECKSUM_LENGTH + 1);
}


SFID::SFID(PCWSTR psz)
    : m_d()
{
    MemCpy(m_d, psz, SFID_LENGTH + 1);
}


SFID::SFID(PCWSTR pszKeyPrefix, PCWSTR pszInstance, PCWSTR pszUniqueId)
    : m_d()
{
    MemCpy(&m_d[SFID_KEYPREFIX_OFFSET], pszKeyPrefix, SFID_KEYPREFIX_LENGTH);
    MemCpy(&m_d[SFID_INSTANCE_OFFSET], pszInstance, SFID_INSTANCE_LENGTH);
    MemCpy(&m_d[SFID_RESERVED_OFFSET], SFID_RESERVED_VALUE, SFID_RESERVED_LENGTH);
    MemCpy(&m_d[SFID_UNIQUEID_OFFSET], pszUniqueId, SFID_UNIQUEID_LENGTH);
    String szChecksum = ComputeChecksum(m_d);
    MemCpy(&m_d[SFID_CHECKSUM_OFFSET], szChecksum, SFID_CHECKSUM_LENGTH + 1);
}


SFID& SFID::operator =(const SFID& other)
{
    MemCpy(m_d, other.m_d, SFID_LENGTH + 1);
    return *this;
}


SFID& SFID::operator =(PCWSTR psz)
{
    return operator=(Parse(psz));
}


SFID& SFID::operator +=(int delta)
{
    LONGLONG value = ComputeSerialNumber(UniqueId);
    value += delta;
    if (value < 0)
    {
        do {
            value += UniqueIdMaxValue + 1;
        } while (value < 0);
    }
    else if (value > UniqueIdMaxValue)
    {
        value %= UniqueIdMaxValue + 1;
    }
    String szUniqueId = Base62Encode(value, SFID_UNIQUEID_LENGTH);
    MemCpy(&m_d[SFID_UNIQUEID_OFFSET], szUniqueId, SFID_UNIQUEID_LENGTH);
    String szChecksum = ComputeChecksum(m_d);
    MemCpy(&m_d[SFID_CHECKSUM_OFFSET], szChecksum, SFID_CHECKSUM_LENGTH + 1);
    return *this;
}


SFID& SFID::operator -=(int delta)
{
    return operator+=(-delta);
}


LONGLONG SFID::get_UniqueIdValue() const
{
    return ComputeSerialNumber(UniqueId);
}


void SFID::put_UniqueIdValue(LONGLONG value)
{
    if (value < 0)
    {
        do {
            value += UniqueIdMaxValue + 1;
        } while (value < 0);
    }
    else if (value > UniqueIdMaxValue)
    {
        value %= UniqueIdMaxValue + 1;
    }
    String szUniqueId = Base62Encode(value, SFID_UNIQUEID_LENGTH);
    MemCpy(&m_d[SFID_UNIQUEID_OFFSET], szUniqueId, SFID_UNIQUEID_LENGTH);
    String szChecksum = ComputeChecksum(m_d);
    MemCpy(&m_d[SFID_CHECKSUM_OFFSET], szChecksum, SFID_CHECKSUM_LENGTH + 1);
}


LONGLONG SFID::get_UniqueIdMaxValue() const
{
    static LONGLONG MaxValue = 0LL;
    if (!MaxValue)
    {
        MaxValue = ComputeSerialNumber(L"zzzzzzzzz");
    }
    return MaxValue;
}


static const int Base62DecodeTable[128] =
{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1,
    -1, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
    51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1,
};


int SFID::ParseBase62(PCWSTR psz, int length, int offset, bool bCheckEnd)
{
    if (offset == SFID_RESERVED_OFFSET)
    {
        if (StrCmp(psz, SFID_RESERVED_LENGTH, SFID_RESERVED_VALUE, SFID_RESERVED_LENGTH))
        {
            throw SFIDException(SFID_MALFORMED_RESERVED, SFID_RESERVED_OFFSET, L"Malformed identifier at offset %d.", SFID_RESERVED_OFFSET);
        }
        return length;
    }
    for (int index = 0; index < length; index++)
    {
        WCHAR c = psz[index];
        if (!c)
        {
            switch (offset)
            {
            case SFID_KEYPREFIX_OFFSET:
                throw SFIDException(SFID_KEYPREFIX_TOO_SHORT, index, L"Too short key prefix; %d<%d", index, SFID_KEYPREFIX_LENGTH);
            case SFID_INSTANCE_OFFSET:
                throw SFIDException(SFID_INSTANCE_TOO_SHORT, index, L"Too short instance; %d<%d", index, SFID_INSTANCE_LENGTH);
            case SFID_UNIQUEID_OFFSET:
                throw SFIDException(SFID_UNIQUEID_TOO_SHORT, index, L"Too short unique identifier; %d<%d", index, SFID_UNIQUEID_LENGTH);
            case SFID_CHECKSUM_OFFSET:
                throw SFIDException(SFID_CHECKSUM_TOO_SHORT, index, L"Too short checksum; %d<%d", index, SFID_CHECKSUM_LENGTH);
            default:
                throw Exception(L"SFID::ParseBase62: Bad offset: %d", offset);
            }
        }
        int d = c < 128 ? Base62DecodeTable[c] : -1;
        if (d < 0)
        {
            switch (offset)
            {
            case SFID_KEYPREFIX_OFFSET:
                throw SFIDException(SFID_MALFORMED_KEYPREFIX, index, L"Malformed key prefix at offset %d.", index);
            case SFID_INSTANCE_OFFSET:
                throw SFIDException(SFID_MALFORMED_INSTANCE, index, L"Malformed instance at offset %d.", index);
            case SFID_UNIQUEID_OFFSET:
                throw SFIDException(SFID_MALFORMED_UNIQUEID, index, L"Malformed unique identifier at offset %d.", index);
            case SFID_CHECKSUM_OFFSET:
                throw SFIDException(SFID_MALFORMED_CHECKSUM, index, L"Malformed checksum at offset %d.", index);
            default:
                throw Exception(L"SFID::ParseBase62: Bad offset: %d", offset);
            }
        }
    }
    if (bCheckEnd)
    {
        if (psz[length])
        {
            switch (offset)
            {
            case SFID_KEYPREFIX_OFFSET:
                throw SFIDException(SFID_KEYPREFIX_TOO_LONG, length, L"Too long key prefix; %d>%d", static_cast<int>(StrLen(psz)), SFID_KEYPREFIX_LENGTH);
            case SFID_INSTANCE_OFFSET:
                throw SFIDException(SFID_INSTANCE_TOO_LONG, length, L"Too long instance; %d>%d", static_cast<int>(StrLen(psz)), SFID_INSTANCE_LENGTH);
            case SFID_UNIQUEID_OFFSET:
                throw SFIDException(SFID_UNIQUEID_TOO_LONG, length, L"Too long unique identifier; %d>%d", static_cast<int>(StrLen(psz)), SFID_UNIQUEID_LENGTH);
            case SFID_CHECKSUM_OFFSET:
                throw SFIDException(SFID_CHECKSUM_TOO_LONG, length, L"Too long checksum; %d>%d", static_cast<int>(StrLen(psz)), SFID_CHECKSUM_LENGTH);
            default:
                throw Exception(L"SFID::ParseBase62: Bad offset: %d", offset);
            }
        }
    }
    return length;
}


LONGLONG SFID::ComputeSerialNumber(String sz)
{
    ULONGLONG value = 0;
    int length = static_cast<int>(sz.Len);
    for (int index = 0; index < length; index++)
    {
        WCHAR c = sz[index];
        int d = c < 128 ? Base62DecodeTable[c] : -1;
        if (d < 0)
        {
            return -1LL;
        }
        value = value * 62 + d;
    }
    return value;
}


static const WCHAR Base62EncodeTable[62] =
{
    L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L'A', L'B', L'C', L'D', L'E', L'F',
    L'G', L'H', L'I', L'J', L'K', L'L', L'M', L'N', L'O', L'P', L'Q', L'R', L'S', L'T', L'U', L'V',
    L'W', L'X', L'Y', L'Z', L'a', L'b', L'c', L'd', L'e', L'f', L'g', L'h', L'i', L'j', L'k', L'l',
    L'm', L'n', L'o', L'p', L'q', L'r', L's', L't', L'u', L'v', L'w', L'x', L'y', L'z'
};


String SFID::Base62Encode(ULONGLONG value, int length)
{
    switch (length)
    {
    case SFID_KEYPREFIX_LENGTH:
    case SFID_INSTANCE_LENGTH:
    case SFID_UNIQUEID_LENGTH:
        break;
    default:
        throw Exception(L"SFID::Base62Encode: Bad length.");
    }
    WCHAR buf[10] = { L'0', L'0', L'0', L'0', L'0', L'0', L'0', L'0', L'0', L'\0' };
    int index = length;
    buf[index] = L'\0';
    do {
        buf[--index] = Base62EncodeTable[value % 62];
        value /= 62;
    } while (index > 0 && value > 0ULL);
    return String(buf);
}


BOOL SFID::IsValid(WCHAR c)
{
    return (0 <= c && c < _countof(Base62DecodeTable) ? Base62DecodeTable[c] : -1) >= 0 ? TRUE : FALSE;
}
