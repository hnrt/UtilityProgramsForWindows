#include "KeystrokeManager.h"
#include <Windows.h>
#include <atlbase.h>
#include "hnrt/ComException.h"
#include "hnrt/String.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/VirtualKey.h"
#include "hnrt/Debug.h"
#pragma warning(push)
#pragma warning(disable:4192)
#import <msxml6.dll> named_guids raw_interfaces_only
#pragma warning(pop)
#include <map>


using namespace hnrt;


static std::map<PCWSTR, WCHAR, StringLessThan> s_CharNameMap;


void KeystrokeManager::LoadLayout(PCWSTR pszFileName)
{
    if (!s_CharNameMap.size())
    {
#define INSERT(x,y) s_CharNameMap.insert(std::pair<PCWSTR, WCHAR>(x,y))
        INSERT(L"SPACE", 0x0020);
        INSERT(L"EXCLAMATION_MARK", 0x0021);
        INSERT(L"QUOTATION_MARK", 0x0022);
        INSERT(L"NUMBER_SIGN", 0x0023);
        INSERT(L"DOLLAR_SIGN", 0x0024);
        INSERT(L"PERCENT_SIGN", 0x0025);
        INSERT(L"AMPERSAND", 0x0026);
        INSERT(L"APOSTROPHE", 0x0027);
        INSERT(L"LEFT_PARENTHESIS", 0x0028);
        INSERT(L"RIGHT_PARENTHESIS", 0x0029);
        INSERT(L"ASTERISK", 0x002A);
        INSERT(L"PLUS_SIGN", 0x002B);
        INSERT(L"COMMA", 0x002C);
        INSERT(L"HYPHEN-MINUS", 0x002D);
        INSERT(L"FULL_STOP", 0x002E);
        INSERT(L"SOLIDUS", 0x002F);
        INSERT(L"DIGIT_ZERO", 0x0030);
        INSERT(L"DIGIT_ONE", 0x0031);
        INSERT(L"DIGIT_TWO", 0x0032);
        INSERT(L"DIGIT_THREE", 0x0033);
        INSERT(L"DIGIT_FOUR", 0x0034);
        INSERT(L"DIGIT_FIVE", 0x0035);
        INSERT(L"DIGIT_SIX", 0x0036);
        INSERT(L"DIGIT_SEVEN", 0x0037);
        INSERT(L"DIGIT_EIGHT", 0x0038);
        INSERT(L"DIGIT_NINE", 0x0039);
        INSERT(L"COLON", 0x003A);
        INSERT(L"SEMICOLON", 0x003B);
        INSERT(L"LESS-THAN_SIGN", 0x003C);
        INSERT(L"EQUALS_SIGN", 0x003D);
        INSERT(L"GREATER-THAN_SIGN", 0x003E);
        INSERT(L"QUESTION_MARK", 0x003F);
        INSERT(L"COMMERCIAL_AT", 0x0040);
        INSERT(L"LATIN_CAPITAL_LETTER_A", 0x0041);
        INSERT(L"LATIN_CAPITAL_LETTER_B", 0x0042);
        INSERT(L"LATIN_CAPITAL_LETTER_C", 0x0043);
        INSERT(L"LATIN_CAPITAL_LETTER_D", 0x0044);
        INSERT(L"LATIN_CAPITAL_LETTER_E", 0x0045);
        INSERT(L"LATIN_CAPITAL_LETTER_F", 0x0046);
        INSERT(L"LATIN_CAPITAL_LETTER_G", 0x0047);
        INSERT(L"LATIN_CAPITAL_LETTER_H", 0x0048);
        INSERT(L"LATIN_CAPITAL_LETTER_I", 0x0049);
        INSERT(L"LATIN_CAPITAL_LETTER_J", 0x004A);
        INSERT(L"LATIN_CAPITAL_LETTER_K", 0x004B);
        INSERT(L"LATIN_CAPITAL_LETTER_L", 0x004C);
        INSERT(L"LATIN_CAPITAL_LETTER_M", 0x004D);
        INSERT(L"LATIN_CAPITAL_LETTER_N", 0x004E);
        INSERT(L"LATIN_CAPITAL_LETTER_O", 0x004F);
        INSERT(L"LATIN_CAPITAL_LETTER_P", 0x0050);
        INSERT(L"LATIN_CAPITAL_LETTER_Q", 0x0051);
        INSERT(L"LATIN_CAPITAL_LETTER_R", 0x0052);
        INSERT(L"LATIN_CAPITAL_LETTER_S", 0x0053);
        INSERT(L"LATIN_CAPITAL_LETTER_T", 0x0054);
        INSERT(L"LATIN_CAPITAL_LETTER_U", 0x0055);
        INSERT(L"LATIN_CAPITAL_LETTER_V", 0x0056);
        INSERT(L"LATIN_CAPITAL_LETTER_W", 0x0057);
        INSERT(L"LATIN_CAPITAL_LETTER_X", 0x0058);
        INSERT(L"LATIN_CAPITAL_LETTER_Y", 0x0059);
        INSERT(L"LATIN_CAPITAL_LETTER_Z", 0x005A);
        INSERT(L"LEFT_SQUARE_BRACKET", 0x005B);
        INSERT(L"REVERSE_SOLIDUS", 0x005C);
        INSERT(L"RIGHT_SQUARE_BRACKET", 0x005D);
        INSERT(L"CIRCUMFLEX_ACCENT", 0x005E);
        INSERT(L"LOW_LINE", 0x005F);
        INSERT(L"GRAVE_ACCENT", 0x0060);
        INSERT(L"LATIN_SMALL_LETTER_A", 0x0061);
        INSERT(L"LATIN_SMALL_LETTER_B", 0x0062);
        INSERT(L"LATIN_SMALL_LETTER_C", 0x0063);
        INSERT(L"LATIN_SMALL_LETTER_D", 0x0064);
        INSERT(L"LATIN_SMALL_LETTER_E", 0x0065);
        INSERT(L"LATIN_SMALL_LETTER_F", 0x0066);
        INSERT(L"LATIN_SMALL_LETTER_G", 0x0067);
        INSERT(L"LATIN_SMALL_LETTER_H", 0x0068);
        INSERT(L"LATIN_SMALL_LETTER_I", 0x0069);
        INSERT(L"LATIN_SMALL_LETTER_J", 0x006A);
        INSERT(L"LATIN_SMALL_LETTER_K", 0x006B);
        INSERT(L"LATIN_SMALL_LETTER_L", 0x006C);
        INSERT(L"LATIN_SMALL_LETTER_M", 0x006D);
        INSERT(L"LATIN_SMALL_LETTER_N", 0x006E);
        INSERT(L"LATIN_SMALL_LETTER_O", 0x006F);
        INSERT(L"LATIN_SMALL_LETTER_P", 0x0070);
        INSERT(L"LATIN_SMALL_LETTER_Q", 0x0071);
        INSERT(L"LATIN_SMALL_LETTER_R", 0x0072);
        INSERT(L"LATIN_SMALL_LETTER_S", 0x0073);
        INSERT(L"LATIN_SMALL_LETTER_T", 0x0074);
        INSERT(L"LATIN_SMALL_LETTER_U", 0x0075);
        INSERT(L"LATIN_SMALL_LETTER_V", 0x0076);
        INSERT(L"LATIN_SMALL_LETTER_W", 0x0077);
        INSERT(L"LATIN_SMALL_LETTER_X", 0x0078);
        INSERT(L"LATIN_SMALL_LETTER_Y", 0x0079);
        INSERT(L"LATIN_SMALL_LETTER_Z", 0x007A);
        INSERT(L"LEFT_CURLY_BRACKET", 0x007B);
        INSERT(L"VERTICAL_LINE", 0x007C);
        INSERT(L"RIGHT_CURLY_BRACKET", 0x007D);
        INSERT(L"TILDE", 0x007E);
#undef  INSERT
    }

    IXMLDOMDocumentPtr pDomDocument;
    HRESULT hr = pDomDocument.CreateInstance(CLSID_DOMDocument);
    if (FAILED(hr))
    {
        throw ComException(hr, L"Failed to get xml document parser.");
    }

    VARIANT_BOOL result = 0;
    hr = pDomDocument->load(CComVariant(pszFileName), &result);
    if (FAILED(hr) || !result)
    {
        throw ComException(hr, L"%s: Failed to load.", pszFileName);
    }

    IXMLDOMElementPtr pElement;
    hr = pDomDocument->get_documentElement(&pElement);
    if (FAILED(hr))
    {
        throw ComException(hr, L"%s: Failed to get layout element.", pszFileName);
    }

    IXMLDOMNodeListPtr pChildren;
    hr = pElement->get_childNodes(&pChildren);
    if (FAILED(hr))
    {
        throw ComException(hr, L"%s: Failed to get layout children.", pszFileName);
    }

    while (true)
    {
        IXMLDOMNodePtr pChild;
        hr = pChildren->nextNode(&pChild);
        if (FAILED(hr))
        {
            throw ComException(hr, L"%s: Failed to get layout child.", pszFileName);
        }
        else if (hr == S_FALSE)
        {
            break;
        }

        DOMNodeType type = NODE_INVALID;
        hr = pChild->get_nodeType(&type);
        if (FAILED(hr))
        {
            throw ComException(hr, L"%s: Failed to get layout child type.", pszFileName);
        }
        else if (type != NODE_ELEMENT)
        {
            continue;
        }

        CComBSTR strName;
        hr = pChild->get_nodeName(&strName);
        if (FAILED(hr))
        {
            throw ComException(hr, L"%s: Failed to get layout child name.", pszFileName);
        }

        std::map<PCWSTR, WCHAR, StringLessThan>::const_iterator iter = s_CharNameMap.find(strName);
        if (iter == s_CharNameMap.end())
        {
            throw Exception(L"%s: Invalid element: %s", pszFileName, strName);
        }

        CComBSTR strValue;
        hr = pChild->get_text(&strValue);
        if (FAILED(hr))
        {
            throw ComException(hr, L"%s: Failed to get %s inner text.", pszFileName, strName);
        }

        unsigned short flags = 0;
        PCWSTR pStart = strValue;
        PCWSTR pEnd = pStart;
        while (true)
        {
            WCHAR c = *pEnd++;
            if (c == L'\0')
            {
                PCWSTR psz = String::Copy(pStart, (pEnd - 1) - pStart);
                VirtualKey vk(psz);
                m_map[iter->second] = vk.Value | flags;
                break;
            }
            else if (c == L'+')
            {
                PCWSTR psz = String::Copy(pStart, (pEnd - 1) - pStart);
                VirtualKey vk(psz);
                if (vk.Value == VK_SHIFT)
                {
                    flags |= KM_FLAG_SHIFT;
                }
                else
                {
                    throw Exception(L"%s: Invalid prefix: %s", pszFileName, psz);
                }
                pStart = pEnd;
            }
        }

        DBGPUT(L"map[%s=%04X]=%04X (%s%s)", strName, iter->second, m_map[iter->second], (m_map[iter->second] & KM_FLAG_SHIFT) ? L"VK_SHIFT+" : L"", VirtualKey((WORD)(m_map[iter->second] & KM_MASK_VIRTUALKEY)).Name);
    }
}
