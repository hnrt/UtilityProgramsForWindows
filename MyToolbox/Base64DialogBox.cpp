#include "pch.h"
#include "Base64DialogBox.h"
#include "MyToolbox.h"
#include "hnrt/RegistryKey.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/ResourceString.h"
#include "hnrt/WindowLayoutSnapshot.h"
#include "hnrt/WhileInScope.h"
#include "hnrt/Base64.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/Exception.h"
#include "hnrt/ErrorMessage.h"
#include "resource.h"


using namespace hnrt;


Base64DialogBox::Base64DialogBox()
    : MyDialogBox(IDD_BS64, L"BASE64")
    , m_bEncodingError(FALSE)
    , m_bDecodingError(FALSE)
    , m_Decoded()
    , m_bEncoded(FALSE)
{
}


Base64DialogBox::~Base64DialogBox()
{
}


void Base64DialogBox::OnCreate()
{
    MyDialogBox::OnCreate();
    HFONT hFont = GetApp<MyToolbox>().GetFontForData();
    SetFont(IDC_BS64_ORG_EDIT, hFont);
    SetFont(IDC_BS64_ENC_EDIT, hFont);
    InitializeCodePageComboBox(IDC_BS64_ORG_CODEPAGE_COMBO);
    InitializeLineBreakComboBox(IDC_BS64_ORG_LINEBREAK_COMBO);
    ButtonCheck(IDC_BS64_ORG_HEX_RADIO);
    OnCommand(IDC_BS64_ORG_HEX_RADIO, 0);
    RegistryKey hKey;
    LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, m_szRegistryKeyName, 0, KEY_READ);
    if (rc == ERROR_SUCCESS)
    {
    }
    OnCommand(IDC_BS64_ORG_EDIT | (EN_CHANGE << 16), 0);
    OnCommand(IDC_BS64_ENC_EDIT | (EN_CHANGE << 16), 0);
    m_menuView
        .Add(ResourceString(IDS_MENU_BS64), IDM_VIEW_BS64);
}


void Base64DialogBox::OnDestroy()
{
    RegistryKey hKey;
    LSTATUS rc = hKey.Create(HKEY_CURRENT_USER, m_szRegistryKeyName, 0, KEY_WRITE);
    if (rc == ERROR_SUCCESS)
    {
    }
    MyDialogBox::OnDestroy();
}


void Base64DialogBox::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
    WindowLayoutSnapshot before, after;

    before.AddAllChildren(hDlg).Clone(after);

    after[IDC_BS64_ORG_STATIC].right += cxDelta;
    after[IDC_BS64_ORG_EDIT].right += cxDelta;

    MoveHorizontally(after[IDC_BS64_ORG_COPY_BUTTON], cxDelta);
    MoveHorizontally(after[IDC_BS64_ORG_ENCODE_BUTTON], cxDelta);
    MoveHorizontally(after[IDC_BS64_ORG_HEX_RADIO], cxDelta);
    MoveHorizontally(after[IDC_BS64_ORG_TEXT_RADIO], cxDelta);
    MoveHorizontally(after[IDC_BS64_ORG_CODEPAGE_COMBO], cxDelta);
    MoveHorizontally(after[IDC_BS64_ORG_LINEBREAK_COMBO], cxDelta);

    after[IDC_BS64_ENC_STATIC].right += cxDelta;
    after[IDC_BS64_ENC_EDIT].right += cxDelta;

    MoveHorizontally(after[IDC_BS64_ENC_COPY_BUTTON], cxDelta);
    MoveHorizontally(after[IDC_BS64_ENC_DECODE_BUTTON], cxDelta);

    ExtendVertically(after[IDC_BS64_ORG_EDIT], after[IDC_BS64_ENC_EDIT], cyDelta);
    LONG dy = after[IDC_BS64_ORG_EDIT].bottom - before[IDC_BS64_ORG_EDIT].bottom;
    MoveVertically(after[IDC_BS64_ENC_STATIC], dy);
    MoveVertically(after[IDC_BS64_ENC_COPY_BUTTON], dy);
    MoveVertically(after[IDC_BS64_ENC_DECODE_BUTTON], dy);

    after.Apply();
}


void Base64DialogBox::OnTabSelectionChanging()
{
    MyDialogBox::OnTabSelectionChanging();
    if (m_LastModified)
    {
    }
    m_menuView
        .Enable(IDM_VIEW_BS64, MF_ENABLED);
}


void Base64DialogBox::OnTabSelectionChanged()
{
    MyDialogBox::OnTabSelectionChanged();
    m_menuEdit
        .Add(ResourceString(IDS_MENU_COPYALL), IDM_EDIT_COPYALL)
        .AddSeparator();
    AddEditControlMenus(m_CurrentEdit);
    m_menuEdit
        .AddSeparator()
        .Add(ResourceString(IDS_MENU_CLEAR), IDM_EDIT_CLEAR);
    m_menuView
        .Enable(IDM_VIEW_BS64, MF_DISABLED);
}


INT_PTR Base64DialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
    if (m_cProcessing)
    {
        return TRUE;
    }
    UNREFERENCED_PARAMETER(lParam);
    UINT idChild = LOWORD(wParam);
    UINT idNotif = HIWORD(wParam);
    switch (idChild)
    {
    case IDC_BS64_ORG_COPY_BUTTON:
        if (!Clipboard::Write(hwnd, GetText(IDC_BS64_ORG_EDIT)))
        {
            MessageBoxW(hwnd, ResourceString(IDS_MSG_CLIPBOARD_COPY_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
        }
        break;
    case IDC_BS64_ORG_ENCODE_BUTTON:
        Encode();
        break;
    case IDC_BS64_ENC_COPY_BUTTON:
        if (!Clipboard::Write(hwnd, GetText(IDC_BS64_ENC_EDIT)))
        {
            MessageBoxW(hwnd, ResourceString(IDS_MSG_CLIPBOARD_COPY_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
        }
        break;
    case IDC_BS64_ENC_DECODE_BUTTON:
        Decode();
        break;
    case IDC_BS64_ORG_EDIT:
        if (idNotif == EN_CHANGE)
        {
            OnEditChanged(idChild);
            if (m_Decoded.Len)
            {
                m_Decoded.Resize(0);
                InvalidateRect(GetDlgItem(hwnd, IDC_BS64_ORG_EDIT), NULL, TRUE);
                if (ButtonIsChecked(IDC_BS64_ORG_HEX_RADIO))
                {
                    DisableWindow(IDC_BS64_ORG_CODEPAGE_COMBO);
                    DisableWindow(IDC_BS64_ORG_LINEBREAK_COMBO);
                }
            }
            if (m_bEncodingError)
            {
                m_bEncodingError = FALSE;
                SetText(IDC_BS64_ORG_STATIC, ResourceString(IDS_ORIGINAL));
                InvalidateRect(GetDlgItem(hwnd, IDC_BS64_ORG_STATIC), NULL, TRUE);
                InvalidateRect(GetDlgItem(hwnd, IDC_BS64_ORG_EDIT), NULL, TRUE);
            }
            UpdateControlsState();
        }
        else if (idNotif == EN_SETFOCUS)
        {
            OnEditSetFocus(idChild);
        }
        else if (idNotif == EN_KILLFOCUS)
        {
            OnEditKillFocus(idChild);
        }
        break;
    case IDC_BS64_ENC_EDIT:
        if (idNotif == EN_CHANGE)
        {
            m_bEncoded = FALSE;
            OnEditChanged(idChild);
            if (m_Decoded.Len)
            {
                m_Decoded.Resize(0);
                InvalidateRect(GetDlgItem(hwnd, IDC_BS64_ORG_EDIT), NULL, TRUE);
                if (ButtonIsChecked(IDC_BS64_ORG_HEX_RADIO))
                {
                    DisableWindow(IDC_BS64_ORG_CODEPAGE_COMBO);
                    DisableWindow(IDC_BS64_ORG_LINEBREAK_COMBO);
                }
            }
            if (m_bDecodingError)
            {
                m_bDecodingError = FALSE;
                SetText(IDC_BS64_ENC_STATIC, ResourceString(IDS_ENCODED));
                InvalidateRect(GetDlgItem(hwnd, IDC_BS64_ENC_STATIC), NULL, TRUE);
                InvalidateRect(GetDlgItem(hwnd, IDC_BS64_ENC_EDIT), NULL, TRUE);
            }
            UpdateControlsState();
        }
        else if (idNotif == EN_SETFOCUS)
        {
            OnEditSetFocus(idChild);
        }
        else if (idNotif == EN_KILLFOCUS)
        {
            OnEditKillFocus(idChild);
        }
        break;
    case IDC_BS64_ORG_HEX_RADIO:
        if (m_Decoded.Len)
        {
            SetDecoded();
        }
        else
        {
            DisableWindow(IDC_BS64_ORG_CODEPAGE_COMBO);
            DisableWindow(IDC_BS64_ORG_LINEBREAK_COMBO);
        }
        break;
    case IDC_BS64_ORG_TEXT_RADIO:
        if (m_Decoded.Len)
        {
            SetDecoded();
        }
        else
        {
            EnableWindow(IDC_BS64_ORG_CODEPAGE_COMBO);
            EnableWindow(IDC_BS64_ORG_LINEBREAK_COMBO);
        }
        break;
    case IDC_BS64_ORG_CODEPAGE_COMBO:
        if (idNotif == CBN_SELCHANGE)
        {
            if (m_Decoded.Len && ButtonIsChecked(IDC_BS64_ORG_TEXT_RADIO))
            {
                SetDecoded();
            }
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}


INT_PTR Base64DialogBox::OnControlColorStatic(WPARAM wParam, LPARAM lParam)
{
    HDC hdc = reinterpret_cast<HDC>(wParam);
    int id = GetDlgCtrlID(reinterpret_cast<HWND>(lParam));
    switch (id)
    {
    case IDC_BS64_ORG_STATIC:
        SetTextColor(hdc,
            m_bEncodingError ? RGB_ERROR :
            GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_3DFACE));
    case IDC_BS64_ENC_STATIC:
        SetTextColor(hdc,
            m_bDecodingError ? RGB_ERROR :
            GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_3DFACE));
    default:
        break;
    }
    return 0;
}


INT_PTR Base64DialogBox::OnControlColorEdit(WPARAM wParam, LPARAM lParam)
{
    HDC hdc = reinterpret_cast<HDC>(wParam);
    int id = GetDlgCtrlID(reinterpret_cast<HWND>(lParam));
    int len = GetTextLength(id);
    switch (id)
    {
    case IDC_BS64_ORG_EDIT:
        SetTextColor(hdc,
            m_bEncodingError ? RGB_ERROR :
            m_Decoded.Len ? RGB_SUCCESSFUL :
            GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_WINDOW));
    case IDC_BS64_ENC_EDIT:
        SetTextColor(hdc,
            m_bDecodingError ? RGB_ERROR :
            m_bEncoded ? RGB_SUCCESSFUL :
            GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_WINDOW));
    default:
        break;
    }
    return 0;
}


void Base64DialogBox::OnClear()
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    SetText(IDC_BS64_ORG_EDIT);
    SetText(IDC_BS64_ENC_EDIT);
    UpdateControlsState();
    m_LastModified.By = 0;
}


static void ConvertToLF(StringBuffer& buf, String sz)
{
    PCWCH pCur = sz.Ptr;
    PCWCH pEnd = sz.Ptr + sz.Len;
    PCWCH pStop = pEnd - 1;
    PCWCH pStart = pCur;
    while (pCur < pStop)
    {
        if (pCur[0] == L'\r' && pCur[1] == L'\n')
        {
            buf.Append(pStart, pCur - pStart);
            buf.Append(L"\n", 1);
            pCur += 2;
            pStart = pCur;
        }
        else
        {
            pCur++;
        }
    }
    buf.Append(pStart, pEnd - pStart);
}


static void ConvertToCRLF(StringBuffer& buf, String sz)
{
    PCWCH pCur = sz.Ptr;
    PCWCH pEnd = sz.Ptr + sz.Len;
    PCWCH pStop = pEnd - 1;
    PCWCH pStart = pCur;
    while (pCur < pStop)
    {
        if (pCur[0] == L'\r' && pCur[1] == L'\n')
        {
            pCur += 2;
        }
        else if (pCur[0] == L'\n')
        {
            buf.Append(pStart, pCur - pStart);
            buf.Append(L"\r\n", 2);
            pCur++;
            pStart = pCur;
        }
        else
        {
            pCur++;
        }
    }
    if (pCur < pEnd && pCur[0] == L'\n')
    {
        buf.Append(pStart, pCur - pStart);
        buf.Append(L"\r\n", 2);
    }
    else
    {
        buf.Append(pStart, pEnd - pStart);
    }
}


void Base64DialogBox::Encode()
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    m_bEncodingError = FALSE;
    ByteString bs;
    String szOriginal = GetText(IDC_BS64_ORG_EDIT);
    if (szOriginal.Len)
    {
        if (ButtonIsChecked(IDC_BS64_ORG_HEX_RADIO))
        {
            try
            {
                bs = ByteString::FromHex(szOriginal);
            }
            catch (Exception e)
            {
                SetText(IDC_BS64_ORG_STATIC, String(PRINTF, L"%s [%s]", ResourceString(IDS_ORIGINAL), L"Parse error"));
                m_bEncodingError = TRUE;
            }
        }
        else
        {
            StringBuffer buf(szOriginal.Len);
            if (ComboBoxGetSelection(IDC_BS64_ORG_LINEBREAK_COMBO) == 0x0a)
            {
                ConvertToLF(buf, szOriginal);
            }
            else
            {
                ConvertToCRLF(buf, szOriginal);
            }
            if (szOriginal.Len != buf.Len)
            {
                szOriginal = buf;
            }
            int CodePage = ComboBoxGetSelection(IDC_BS64_ORG_CODEPAGE_COMBO);
            if (CodePage == CP_UTF16)
            {
                bs = ByteString(szOriginal.Ptr, szOriginal.Len * sizeof(WCHAR));
            }
            else
            {
                DWORD dwFlags = CodePage == CP_EUCJP ? 0 : WC_ERR_INVALID_CHARS;
                int length = WideCharToMultiByte(CodePage, dwFlags, szOriginal.Ptr, static_cast<int>(szOriginal.Len), NULL, 0, NULL, NULL);
                if (length > 0)
                {
                    bs.Resize(length);
                    BOOL bConvError = FALSE;
                    WideCharToMultiByte(CodePage, dwFlags, szOriginal.Ptr, static_cast<int>(szOriginal.Len), reinterpret_cast<LPSTR>(bs.Ptr), length, NULL, &bConvError);
                    if (bConvError)
                    {
                        m_bEncodingError = TRUE;
                        SetText(IDC_BS64_ORG_STATIC, String(PRINTF, L"%s [%s]", ResourceString(IDS_ORIGINAL), L"Conversion error"));
                    }
                }
                else
                {
                    m_bEncodingError = TRUE;
                    SetText(IDC_BS64_ORG_STATIC, String(PRINTF, L"%s [%s]", ResourceString(IDS_ORIGINAL), ErrorMessage::Get(GetLastError())));
                }
            }
        }
    }
    if (m_bEncodingError)
    {
        m_bEncoded = FALSE;
        InvalidateRect(GetDlgItem(hwnd, IDC_BS64_ORG_STATIC), NULL, TRUE);
        InvalidateRect(GetDlgItem(hwnd, IDC_BS64_ORG_EDIT), NULL, TRUE);
    }
    else
    {
        Base64Encoder encoder;
        encoder.Append(bs.Ptr, bs.Len);
        encoder.End();
        m_bDecodingError = FALSE;
        m_Decoded.Resize(0);
        m_bEncoded = TRUE;
        SetText(IDC_BS64_ENC_EDIT, encoder);
        SetText(IDC_BS64_ENC_STATIC, ResourceString(IDS_ENCODED));
        SetText(IDC_BS64_ORG_STATIC, ResourceString(IDS_ORIGINAL));
    }
    UpdateControlsState();
    m_LastModified.By = 0;
}


void Base64DialogBox::Decode()
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    String szEncoded = GetText(IDC_BS64_ENC_EDIT);
    Base64Decoder decoder;
    if (decoder.Parse(szEncoded))
    {
        m_Decoded = decoder;
        m_bDecodingError = FALSE;
        SetDecoded();
    }
    else
    {
        m_Decoded.Resize(0);
        m_bDecodingError = TRUE;
        SetText(IDC_BS64_ENC_STATIC, String(PRINTF, L"%s [%s]", ResourceString(IDS_ENCODED), L"Parse error"));
    }
    UpdateControlsState();
    m_LastModified.By = 0;
}


void Base64DialogBox::SetDecoded()
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    if (m_Decoded.Len)
    {
        String sz;
        m_bEncodingError = FALSE;
        if (ButtonIsChecked(IDC_BS64_ORG_HEX_RADIO))
        {
            sz = String::ToHex(m_Decoded.Ptr, m_Decoded.Len, UPPERCASE);
        }
        else
        {
            int CodePage = ComboBoxGetSelection(IDC_BS64_ORG_CODEPAGE_COMBO);
            if (CodePage == CP_UTF16)
            {
                if ((m_Decoded.Len % sizeof(WCHAR)) == 0)
                {
                    int length = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, reinterpret_cast<LPCWCH>(m_Decoded.Ptr), static_cast<int>(m_Decoded.Len / sizeof(WCHAR)), NULL, 0, NULL, NULL);
                    if (length > 0)
                    {
                        sz = String(reinterpret_cast<LPCWCH>(m_Decoded.Ptr), static_cast<int>(m_Decoded.Len / sizeof(WCHAR)));
                    }
                    else
                    {
                        m_bEncodingError = TRUE;
                        SetText(IDC_BS64_ORG_STATIC, String(PRINTF, L"%s [%s]", ResourceString(IDS_ORIGINAL), L"Conversion error"));
                    }
                }
                else
                {
                    m_bEncodingError = TRUE;
                    SetText(IDC_BS64_ORG_STATIC, String(PRINTF, L"%s [%s]", ResourceString(IDS_ORIGINAL), L"Invalid length"));
                }
            }
            else
            {
                int length = MultiByteToWideChar(CodePage, MB_PRECOMPOSED, reinterpret_cast<LPCCH>(m_Decoded.Ptr), static_cast<int>(m_Decoded.Len), NULL, 0);
                if (length > 0)
                {
                    StringBuffer buf(length);
                    MultiByteToWideChar(CodePage, MB_PRECOMPOSED, reinterpret_cast<LPCCH>(m_Decoded.Ptr), static_cast<int>(m_Decoded.Len), buf, length);
                    sz = String(buf, length);
                }
                else
                {
                    m_bEncodingError = TRUE;
                    SetText(IDC_BS64_ORG_STATIC, String(PRINTF, L"%s [%s]", ResourceString(IDS_ORIGINAL), ErrorMessage::Get(GetLastError())));
                }
            }
        }
        if (m_bEncodingError)
        {
            InvalidateRect(GetDlgItem(hwnd, IDC_BS64_ORG_STATIC), NULL, TRUE);
            InvalidateRect(GetDlgItem(hwnd, IDC_BS64_ORG_EDIT), NULL, TRUE);
        }
        else
        {
            SetText(IDC_BS64_ORG_EDIT, sz);
            SetText(IDC_BS64_ORG_STATIC, ResourceString(IDS_ORIGINAL));
        }
    }
}


void Base64DialogBox::UpdateControlsState()
{
    if (GetTextLength(IDC_BS64_ORG_EDIT) > 0)
    {
        EnableWindow(IDC_BS64_ORG_COPY_BUTTON);
        EnableWindow(IDC_BS64_ORG_ENCODE_BUTTON);
    }
    else
    {
        DisableWindow(IDC_BS64_ORG_COPY_BUTTON);
        DisableWindow(IDC_BS64_ORG_ENCODE_BUTTON);
    }
    if (m_Decoded.Len > 0 || ButtonIsChecked(IDC_BS64_ORG_TEXT_RADIO))
    {
        EnableWindow(IDC_BS64_ORG_CODEPAGE_COMBO);
        EnableWindow(IDC_BS64_ORG_LINEBREAK_COMBO);
    }
    else
    {
        DisableWindow(IDC_BS64_ORG_CODEPAGE_COMBO);
        DisableWindow(IDC_BS64_ORG_LINEBREAK_COMBO);
    }
    if (GetTextLength(IDC_BS64_ENC_EDIT) > 0)
    {
        EnableWindow(IDC_BS64_ENC_COPY_BUTTON);
        EnableWindow(IDC_BS64_ENC_DECODE_BUTTON);
    }
    else
    {
        DisableWindow(IDC_BS64_ENC_COPY_BUTTON);
        DisableWindow(IDC_BS64_ENC_DECODE_BUTTON);
    }
}
