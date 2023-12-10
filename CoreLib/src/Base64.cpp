#include "pch.h"
#include "hnrt/Base64.h"
#include "hnrt/Exception.h"
#include "hnrt/Debug.h"


using namespace hnrt;


//////////////////////////////////////////////////////////////////////
//
//   B A S E 6 4 E N C O D E R   C L A S S
//
//////////////////////////////////////////////////////////////////////


static const WCHAR s_EncodingTable[65] = { L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" };


Base64Encoder::Base64Encoder()
    : StringBuffer(64)
    , m_Remaining(0)
{
    m_Data[0] = 0;
    m_Data[1] = 0;
}


Base64Encoder& Base64Encoder::Append(const void* ptr, size_t len)
{
    size_t delta = ((m_Remaining + len) * 8 + 5) / 6;
    if (m_len + delta + 1 > m_cap)
    {
        Resize(m_len + delta + 1);
    }
    const unsigned char* pCur = reinterpret_cast<const unsigned char*>(ptr);
    const unsigned char* pEnd = pCur + len;
    if (m_Remaining == 1)
    {
        if (pCur + 2 <= pEnd)
        {
            m_ptr[m_len++] = s_EncodingTable[m_Data[0] >> 2];
            m_ptr[m_len++] = s_EncodingTable[((m_Data[0] & 0x03) << 4) | (pCur[0] >> 4)];
            m_ptr[m_len++] = s_EncodingTable[((pCur[0] & 0x0F) << 2) | (pCur[1] >> 6)];
            m_ptr[m_len++] = s_EncodingTable[pCur[1] & 0x3F];
            m_Remaining = 0;
            pCur += 2;
        }
        else if (pCur + 1 <= pEnd)
        {
            m_Data[1] = pCur[0];
            m_Remaining = 2;
            return *this;
        }
        else
        {
            return *this;
        }
    }
    else if (m_Remaining == 2)
    {
        if (pCur + 1 <= pEnd)
        {
            m_ptr[m_len++] = s_EncodingTable[m_Data[0] >> 2];
            m_ptr[m_len++] = s_EncodingTable[((m_Data[0] & 0x03) << 4) | (m_Data[1] >> 4)];
            m_ptr[m_len++] = s_EncodingTable[((m_Data[1] & 0x0F) << 2) | (pCur[0] >> 6)];
            m_ptr[m_len++] = s_EncodingTable[pCur[0] & 0x3F];
            m_Remaining = 0;
            pCur += 1;
        }
        else
        {
            return *this;
        }
    }
    while (pCur + 3 <= pEnd)
    {
        m_ptr[m_len++] = s_EncodingTable[pCur[0] >> 2];
        m_ptr[m_len++] = s_EncodingTable[((pCur[0] & 0x03) << 4) | (pCur[1] >> 4)];
        m_ptr[m_len++] = s_EncodingTable[((pCur[1] & 0x0F) << 2) | (pCur[2] >> 6)];
        m_ptr[m_len++] = s_EncodingTable[pCur[2] & 0x3F];
        pCur += 3;
    }
    m_ptr[m_len] = L'\0';
    m_Remaining = static_cast<unsigned char>(pEnd - pCur);
    if (m_Remaining == 1)
    {
        m_Data[0] = pCur[0];
    }
    else if (m_Remaining == 2)
    {
        m_Data[0] = pCur[0];
        m_Data[1] = pCur[1];
    }
    return *this;
}


Base64Encoder& Base64Encoder::End()
{
    if (m_Remaining == 1)
    {
        if (m_len + 5 > m_cap)
        {
            Resize(m_len + 5);
        }
        m_ptr[m_len++] = s_EncodingTable[m_Data[0] >> 2];
        m_ptr[m_len++] = s_EncodingTable[(m_Data[0] & 0x03) << 4];
        m_ptr[m_len++] = L'=';
        m_ptr[m_len++] = L'=';
        m_ptr[m_len] = L'\0';
        m_Remaining = 0;
    }
    else if (m_Remaining == 2)
    {
        if (m_len + 5 > m_cap)
        {
            Resize(m_len + 5);
        }
        m_ptr[m_len++] = s_EncodingTable[m_Data[0] >> 2];
        m_ptr[m_len++] = s_EncodingTable[((m_Data[0] & 0x03) << 4) | (m_Data[1] >> 4)];
        m_ptr[m_len++] = s_EncodingTable[(m_Data[1] & 0x0F) << 2];
        m_ptr[m_len++] = L'=';
        m_ptr[m_len] = L'\0';
        m_Remaining = 0;
    }
    return *this;
}


//////////////////////////////////////////////////////////////////////
//
//   B A S E 6 4 D E C O D E R   C L A S S
//
//////////////////////////////////////////////////////////////////////


static const signed char s_DecodingTable[128] =
{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
};


Base64Decoder::Base64Decoder()
    : ByteString()
{
}


#define BAD_CHARACTER throw Exception(L"Base64Decoder::Parse: Encountered a bad character.")


inline void Decode(PCWSTR pCur, unsigned char* pDst, unsigned char* pEnd)
{
    while (pDst <= pEnd)
    {
        int b0 = pCur[0] < 0x80 ? s_DecodingTable[pCur[0]] : -1;
        int b1 = pCur[1] < 0x80 ? s_DecodingTable[pCur[1]] : -1;
        int b2 = pCur[2] < 0x80 ? s_DecodingTable[pCur[2]] : -1;
        int b3 = pCur[3] < 0x80 ? s_DecodingTable[pCur[3]] : -1;
        if (b0 < 0 || b1 < 0 || b2 < 0 || b3 < 0)
        {
            BAD_CHARACTER;
        }
        pCur += 4;
        pDst[0] = static_cast<unsigned char>((b0 << 2) | (b1 >> 4));
        pDst[1] = static_cast<unsigned char>(((b1 & 0x0F) << 4) | (b2 >> 2));
        pDst[2] = static_cast<unsigned char>(((b2 & 0x03) << 6) | b3);
        pDst += 3;
    }
}


bool Base64Decoder::Parse(PCWSTR psz)
{
    DBGFNC(L"Base64Decoder::Parse");

    size_t len = wcslen(psz);
    DBGPUT(L"StrLen=%zu", len);

    if (!len)
    {
        Resize(0);
        return true;
    }

    if ((len % 4) > 0)
    {
        return false;
    }

    try
    {
        if (psz[len - 1] != L'=')
        {
            Resize((len / 4) * 3);
            unsigned char* pDst = *this;
            unsigned char* pEnd = pDst + Len - 3;
            Decode(psz, pDst, pEnd);
        }
        else if (psz[len - 2] != L'=')
        {
            Resize((len / 4) * 3 - 1);
            unsigned char* pDst = *this;
            unsigned char* pEnd = pDst + Len - 3;
            Decode(psz, pDst, pEnd);
            PCWSTR pCur = psz + len - 4;
            int b0 = pCur[0] < 0x80 ? s_DecodingTable[pCur[0]] : -1;
            int b1 = pCur[1] < 0x80 ? s_DecodingTable[pCur[1]] : -1;
            int b2 = pCur[2] < 0x80 ? s_DecodingTable[pCur[2]] : -1;
            if (b0 < 0 || b1 < 0 || b2 < 0)
            {
                BAD_CHARACTER;
            }
            pEnd[1] = static_cast<unsigned char>((b0 << 2) | (b1 >> 4));
            pEnd[2] = static_cast<unsigned char>(((b1 & 0x0F) << 4) | (b2 >> 2));
        }
        else
        {
            Resize((len / 4) * 3 - 2);
            unsigned char* pDst = *this;
            unsigned char* pEnd = pDst + Len - 3;
            Decode(psz, pDst, pEnd);
            PCWSTR pCur = psz + len - 4;
            int b0 = pCur[0] < 0x80 ? s_DecodingTable[pCur[0]] : -1;
            int b1 = pCur[1] < 0x80 ? s_DecodingTable[pCur[1]] : -1;
            if (b0 < 0 || b1 < 0)
            {
                BAD_CHARACTER;
            }
            pEnd[2] = static_cast<unsigned char>((b0 << 2) | (b1 >> 4));
        }
        DBGPUT(L"DataLen=%zu", Len);
        return true;
    }
    catch (Exception e)
    {
        return false;
    }
}
