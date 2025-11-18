#include "pch.h"
#include "hnrt/DialogBox.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/WindowHandle.h"
#include "hnrt/WindowDesign.h"
#include "hnrt/Clipboard.h"
#include "hnrt/Debug.h"
#include <map>


using namespace hnrt;


typedef std::map<String, int> StringIntMap;
typedef std::pair<String, int> StringIntPair;


DialogBox::DialogBox(UINT idTemplate)
    : WindowSize()
    , WindowLayout()
    , m_hwnd(nullptr)
    , m_idTemplate(idTemplate)
    , m_hFont()
{
}


DialogBox::~DialogBox()
{
    if (m_hwnd)
    {
        DestroyWindow(m_hwnd);
    }
}


void DialogBox::Open(HINSTANCE hInstance)
{
    if (!CreateDialogParamW(hInstance, MAKEINTRESOURCE(m_idTemplate), NULL, MessageCallback, reinterpret_cast<LPARAM>(this)))
    {
        throw Win32Exception(GetLastError(), L"CreateDialog failed.");
    }
}


void DialogBox::Open(HWND hwndParent)
{
    RectangleMetrics rect;
    rect.FromClient(hwndParent);
    DialogBox::Open(hwndParent, 0, 0, rect.cx, rect.cy);
}


void DialogBox::Open(HWND hwndParent, int cx, int cy)
{
    DialogBox::Open(hwndParent, 0, 0, cx, cy);
}


void DialogBox::Open(HWND hwndParent, int x, int y, int cx, int cy)
{
    if (!CreateDialogParamW(GetInstanceHandle(hwndParent), MAKEINTRESOURCE(m_idTemplate), hwndParent, MessageCallback, reinterpret_cast<LPARAM>(this)))
    {
        throw Win32Exception(GetLastError(), L"CreateDialog failed.");
    }
    SetWindowGeometry(m_hwnd, x, y, cx, cy);
}


void DialogBox::Close()
{
    DestroyWindow(m_hwnd);
}


static BOOL CALLBACK ApplyFontToChild(HWND hwnd, LPARAM lParam)
{
    SendMessageW(hwnd, WM_SETFONT, static_cast<WPARAM>(lParam), TRUE);
    return TRUE;
}


void DialogBox::SetFont(HFONT hFont)
{
    m_hFont = hFont;
    EnumChildWindows(m_hwnd, ApplyFontToChild, reinterpret_cast<LPARAM>(hFont));
}


void DialogBox::SetFont(int id, HFONT hFont) const
{
    SendMessage(id, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
}


HWND DialogBox::GetChild(int id) const
{
    return GetDlgItem(m_hwnd, id);
}


DWORD DialogBox::GetChildStyle(int id) const
{
    return GetWindowLongW(GetChild(id), GWL_STYLE);
}


BOOL DialogBox::EnableWindow(int id, BOOL bEnabled) const
{
    return ::EnableWindow(GetChild(id), bEnabled);
}


BOOL DialogBox::DisableWindow(int id) const
{
    return ::EnableWindow(GetChild(id), FALSE);
}


void DialogBox::SetFocus(int id) const
{
    ::SetFocus(GetDlgItem(hwnd, id));
}


LRESULT DialogBox::SendMessage(int id, UINT msg, WPARAM wParam, LPARAM lParam) const
{
    return SendDlgItemMessageW(m_hwnd, id, msg, wParam, lParam);
}


UINT DialogBox::GetTextLength(int id) const
{
    return static_cast<UINT>(SendMessage(id, WM_GETTEXTLENGTH));
}


BOOL DialogBox::InvalidateRect(int id, const RECT* pRect, BOOL bErase) const
{
    return ::InvalidateRect(GetChild(id), pRect, bErase);
}


String DialogBox::GetText(int id) const
{
    LRESULT cch = SendMessage(id, WM_GETTEXTLENGTH);
    String text(cch);
    SendMessage(id, WM_GETTEXT, cch + 1, reinterpret_cast<LPARAM>(text.Buf));
    return text;
}


void DialogBox::SetText(int id, PCWSTR psz) const
{
    SendMessage(id, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(psz ? psz : L""));
}


// The window system doesn't notify MULTILINE Edit Control of EN_CHANGE.
// Use this function to do so intentionally.
void DialogBox::SetTextAndNotify(int id, PCWSTR psz)
{
    SendMessage(id, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(psz ? psz : L""));
    OnCommand(MAKELONG(id, EN_CHANGE), reinterpret_cast<LPARAM>(GetChild(id)));
}


void DialogBox::CopyAllText(int id) const
{
    if (!Clipboard::Write(hwnd, hwnd, id))
    {
        MessageBoxW(hwnd, L"Unable to write text.", L"CLIPBOARD", MB_ICONERROR | MB_OK);
    }
}


//////////////////////////////////////////////////////////////////////
// 
//  E D I T   C O N T R O L
// 
//////////////////////////////////////////////////////////////////////


void DialogBox::EditGetSelection(int id, int& start, int& end) const
{
    SendMessage(id, EM_GETSEL, reinterpret_cast<WPARAM>(&start), reinterpret_cast<LPARAM>(&end));
}


int DialogBox::EditGetSelectionLength(int id) const
{
    int start = 0;
    int end = 0;
    EditGetSelection(id, start, end);
    return end - start;
}


void DialogBox::EditSetSelection(int id, int start, int end) const
{
    if (end < start)
    {
        end = start;
    }
    SendMessage(id, EM_SETSEL, start, end);
}


void DialogBox::EditCut(int id) const
{
    //
    // |<-------------------- cch ---------------------->|
    // |<--- off2 --->|<--- SELECTION --->|<--- cch3 --->|
    // |<----------- off3 --------------->|
    //
    // |<--- off2 --->|<--- cch3 --->|
    //
    int cch = GetTextLength(id);
    int off2 = 0;
    int off3 = 0;
    EditGetSelection(id, off2, off3);
    if (off2 > off3 || off3 > cch)
    {
        Debug::Put(L"DialogBox::EditCut: Unexpected state: cch=%d off2=%d off3=%d", cch, off2, off3);
        return;
    }
    String szText = GetText(id);
    int cch2 = off3 - off2;
    if (!Clipboard::Write(hwnd, &szText[off2], cch2))
    {
        MessageBoxW(hwnd, L"Unable to write text.", L"CLIPBOARD", MB_ICONERROR | MB_OK);
        return;
    }
    int cch3 = cch - off3;
    String szText2;
    szText2.Append(&szText[0], off2);
    szText2.Append(&szText[off3], cch3);
    SetText(id, szText2);
    EditSetSelection(id, off2, off2);
    SetFocus(id);
}


void DialogBox::EditCopy(int id) const
{
    //
    // |<-------------------- cch ---------------------->|
    // |<--- off2 --->|<--- SELECTION --->|<--- cch3 --->|
    // |<----------- off3 --------------->|
    //
    int cch = GetTextLength(id);
    int off2 = 0;
    int off3 = 0;
    EditGetSelection(id, off2, off3);
    if (off2 > off3 || off3 > cch)
    {
        Debug::Put(L"DialogBox::EditCopy: Unexpected state: cch=%d off2=%d off3=%d", cch, off2, off3);
        return;
    }
    String szText = GetText(id);
    int cch2 = off3 - off2;
    if (!Clipboard::Write(hwnd, &szText[off2], cch2))
    {
        MessageBoxW(hwnd, L"Unable to write text.", L"CLIPBOARD", MB_ICONERROR | MB_OK);
        return;
    }
    SetFocus(id);
}


void DialogBox::EditPaste(int id) const
{
    //
    // |<-------------------- cch ---------------------->|
    // |<--- off2 --->|<--- SELECTION --->|<--- cch3 --->|
    // |<----------- off3 --------------->|
    // |<-------------------------------------- size ------------------>|
    // |<-------------------- cch ---------------------->|<--- cch2 --->|
    // |<----------- off4 ---------->|
    // |<--- off2 --->|<--- cch2 --->|<--- cch3 --->|
    //
    String szText1 = GetText(id);
    int cch = static_cast<int>(szText1.Len);
    int off2 = 0;
    int off3 = 0;
    EditGetSelection(id, off2, off3);
    if (off2 > off3 || off3 > cch)
    {
        Debug::Put(L"DialogBox::EditPaste: Unexpected state: cch=%d off2=%d off3=%d", cch, off2, off3);
        return;
    }
    String szText2 = Clipboard::Read(hwnd);
    if (!szText2.Len)
    {
        MessageBoxW(hwnd, L"Unable to read text.", L"CLIPBOARD", MB_ICONERROR | MB_OK);
        return;
    }
    String szText3;
    szText3.Append(&szText1[0], off2);
    szText3.Append(szText2);
    szText3.Append(&szText1[off3]);
    SetText(id, szText3);
    int off4 = off2 + static_cast<int>(szText2.Len);
    EditSetSelection(id, off4, off4);
    SetFocus(id);
}


void DialogBox::EditDelete(int id) const
{
    //
    // |<-------------------- cch ---------------------->|
    // |<--- off2 --->|<--- SELECTION --->|<--- cch3 --->|
    // |<----------- off3 --------------->|
    //
    // |<--- off2 --->|<--- cch3 --->|
    //
    String szText1 = GetText(id);
    int cch = static_cast<int>(szText1.Len);
    int off2 = 0;
    int off3 = 0;
    EditGetSelection(id, off2, off3);
    if (off2 > off3 || off3 > cch)
    {
        Debug::Put(L"DialogBox::DeleteText: Unexpected state: cch=%d off2=%d off3=%d", cch, off2, off3);
        return;
    }
    if (off2 == off3 && off3 == cch)
    {
        return;
    }
    String szText2;
    szText2.Append(&szText1[0], off2);
    szText2.Append(&szText1[off3]);
    SetText(id, szText2);
    EditSetSelection(id, off2, off2);
    SetFocus(id);
}


void DialogBox::EditSelectAll(int id) const
{
    int cch = GetTextLength(id);
    EditSetSelection(id, 0, cch);
    SetFocus(id);
}


BOOL DialogBox::EditGetReadOnly(int id) const
{
    return (GetChildStyle(id) & ES_READONLY) == ES_READONLY;
}


void DialogBox::EditSetReadOnly(int id, BOOL bReadOnly) const
{
    SendMessage(id, EM_SETREADONLY, bReadOnly);
}


void DialogBox::EditClear(int id) const
{
    SetText(id);
    SetFocus(id);
}


BOOL DialogBox::EditIsMultiLine(int id) const
{
    return (GetChildStyle(id) & ES_MULTILINE) == ES_MULTILINE;
}


//////////////////////////////////////////////////////////////////////
// 
//  B U T T O N   C O N T R O L
// 
//////////////////////////////////////////////////////////////////////


void DialogBox::ButtonCheck(int id, BOOL bCheck) const
{
    SendMessage(id, BM_SETCHECK, bCheck ? BST_CHECKED : BST_UNCHECKED);
}


void DialogBox::ButtonUncheck(int id) const
{
    SendMessage(id, BM_SETCHECK, BST_UNCHECKED);
}


BOOL DialogBox::ButtonIsChecked(int id) const
{
    switch (SendMessage(id, BM_GETCHECK))
    {
    case BST_CHECKED:
        return TRUE;
    case BST_UNCHECKED:
        return FALSE;
    case BST_INDETERMINATE:
        return FALSE;
    default:
        return FALSE;
    }
}


//////////////////////////////////////////////////////////////////////
// 
//  C O M B O   B O X   C O N T R O L
// 
//////////////////////////////////////////////////////////////////////


int DialogBox::ComboBoxGetCount(int id) const
{
    return static_cast<int>(SendMessage(id, CB_GETCOUNT));
}


void DialogBox::ComboBoxAdd(int id, const String& szText, int value) const
{
    StringIntMap* pMap = GetWindowUserData<StringIntMap>(GetChild(id));
    if (!pMap)
    {
        pMap = SetWindowUserData<StringIntMap>(GetChild(id), new StringIntMap());
    }
    pMap->insert(StringIntPair(szText, value));
    SendMessage(id, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(szText.Ptr));
}


void DialogBox::ComboBoxRemove(int id, int value) const
{
    StringIntMap* pMap = GetWindowUserData<StringIntMap>(GetChild(id));
    if (!pMap)
    {
        Debug::Put(L"DialogBox::ComboBoxRemoveValue(%d): Has no map.", id);
        return;
    }
    for (StringIntMap::const_iterator iter = pMap->cbegin(); iter != pMap->cend(); iter++)
    {
        if (iter->second == value)
        {
            LRESULT index = SendMessage(id, CB_FINDSTRING, -1, reinterpret_cast<LPARAM>(iter->first.Ptr));
            if (index != CB_ERR)
            {
                SendMessage(id, CB_DELETESTRING, index);
            }
            return;
        }
    }
    Debug::Put(L"DialogBox::ComboBoxRemoveValue(%d): Map has no entry for %d.", id, value);
}


int DialogBox::ComboBoxGetSelection(int id, int defaultValue) const
{
    StringIntMap* pMap = GetWindowUserData<StringIntMap>(GetChild(id));
    if (!pMap)
    {
        Debug::Put(L"DialogBox::ComboBoxGetSelection(%d): Has no map.", id);
        return defaultValue;
    }
    int selected = static_cast<int>(SendMessage(id, CB_GETCURSEL));
    if (selected == CB_ERR)
    {
        Debug::Put(L"DialogBox::ComboBoxGetSelection(%d): CB_GETCURSEL failed.", id);
        return defaultValue;
    }
    String szText = ComboBoxGetText(id, selected);
    StringIntMap::const_iterator iter = pMap->find(szText);
    if (iter == pMap->cend())
    {
        Debug::Put(L"DialogBox::ComboBoxGetSelection(%d): Map has no entry for %s.", id, szText.Ptr);
        return defaultValue;
    }
    return iter->second;
}


void DialogBox::ComboBoxSetSelection(int id, int value) const
{
    StringIntMap* pMap = GetWindowUserData<StringIntMap>(GetChild(id));
    if (!pMap)
    {
        Debug::Put(L"DialogBox::ComboBoxSetSelection(%d): Has no map.", id);
        return;
    }
    for (StringIntMap::const_iterator iter = pMap->cbegin(); iter != pMap->cend(); iter++)
    {
        if (iter->second == value)
        {
            ComboBoxSetSelection(id, iter->first);
            return;
        }
    }
    Debug::Put(L"DialogBox::ComboBoxSetSelection(%d): Map has no entry for %d.", id, value);
}


void DialogBox::ComboBoxSetSelection(int id, const String& szText) const
{
    SendMessage(id, CB_SELECTSTRING, 0, reinterpret_cast<LPARAM>(szText.Ptr));
}


void DialogBox::ComboBoxClearSelection(int id) const
{
    SendMessage(id, CB_SETCURSEL, -1);
}


String DialogBox::ComboBoxGetText(int id, int index, const String& szDefault) const
{
    if (index < 0)
    {
        int count = ComboBoxGetCount(id);
        if (count <= 0)
        {
            return String(szDefault);
        }
        index += count;
    }
    LRESULT cch = SendMessage(id, CB_GETLBTEXTLEN, index);
    if (cch == CB_ERR)
    {
        return String(szDefault);
    }
    String szText(cch);
    LRESULT length = SendMessage(id, CB_GETLBTEXT, index, reinterpret_cast<LPARAM>(szText.Buf));
    if (length == CB_ERR)
    {
        return String(szDefault);
    }
    return szText;
}


//////////////////////////////////////////////////////////////////////
// 
//  L I S T   B O X   C O N T R O L
// 
//////////////////////////////////////////////////////////////////////


int DialogBox::ListBoxGetCount(int id) const
{
    return static_cast<int>(SendMessage(id, LB_GETCOUNT));
}


int DialogBox::ListBoxGetSelection(int id, int defaultValue) const
{
    LRESULT index = SendMessage(id, LB_GETCURSEL);
    if (index == LB_ERR)
    {
        return defaultValue;
    }
    return static_cast<int>(index);
}


bool DialogBox::ListBoxSetSelection(int id, int index) const
{
    LRESULT result = SendMessage(id, LB_SETCURSEL, index);
    return result != LB_ERR;
}


bool DialogBox::ListBoxIsSelected(int id, int index) const
{
    return SendMessage(id, LB_GETSEL, index, 0) > 0;
}


// The return value is the zero-based index of the string in the list box.
// If an error occurs, the return value is less than zero.
int DialogBox::ListBoxAddText(int id, const String& szText) const
{
    return static_cast<int>(SendMessage(id, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(szText.Ptr)));
}


String DialogBox::ListBoxGetText(int id, int index, const String& szDefault) const
{
    LRESULT cch = SendMessage(id, LB_GETTEXTLEN, index);
    if (cch == LB_ERR)
    {
        return String(szDefault);
    }
    String szText(cch);
    if (SendMessage(id, LB_GETTEXT, index, reinterpret_cast<LPARAM>(szText.Buf)) == LB_ERR)
    {
        return String(szDefault);
    }
    return szText;
}


// The return value is the index of the position at which the string was inserted.
// If an error occurs, the return value is less than zero.
int DialogBox::ListBoxInsertText(int id, int index, const String& szText) const
{
    return static_cast<int>(SendMessage(id, LB_INSERTSTRING, index, reinterpret_cast<LPARAM>(szText.Ptr)));
}


// If an error occurs, the return value is less than zero.
int DialogBox::ListBoxSetText(int id, int index, const String& szText) const
{
    bool bSelected = ListBoxIsSelected(id, index);
    if (ListBoxDelete(id, index) < 0)
    {
        return -1;
    }
    index = ListBoxInsertText(id, index, szText);
    if (index >= 0)
    {
        if (bSelected)
        {
            ListBoxSetSelection(id, index);
        }
    }
    return index;
}


// The return value is the zero-based index of the string in the list box.
// If an error occurs, the return value is less than zero.
int DialogBox::ListBoxFind(int id, const String& szText, int fromIndex) const
{
    return static_cast<int>(SendMessage(id, LB_FINDSTRING, fromIndex, reinterpret_cast<LPARAM>(szText.Ptr)));
}


// The return value is a count of the strings remaining in the list.
// If an error occurs, the return value is less than zero.
int DialogBox::ListBoxDelete(int id, int index) const
{
    if (index < 0)
    {
        int count = ListBoxGetCount(id);
        if (count <= 0)
        {
            return -1;
        }
        index += count;
    }
    return static_cast<int>(SendMessage(id, LB_DELETESTRING, index, 0));
}


//////////////////////////////////////////////////////////////////////
// 
//  P R O T E C T E D  /  P R I V A T E
// 
//////////////////////////////////////////////////////////////////////


INT_PTR CALLBACK DialogBox::MessageCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        SetWindowUserData<DialogBox>(hwnd, lParam)->OnCreate(hwnd);
        break;
    }
    case WM_DESTROY:
        ExchangeWindowUserData<DialogBox>(hwnd, nullptr)->OnDestroy(hwnd);
        break;
    case WM_CLOSE:
        GetWindowUserData<DialogBox>(hwnd)->OnClose();
        break;
    case WM_SIZE:
        GetWindowUserData<DialogBox>(hwnd)->OnSize(wParam, lParam);
        break;
    case WM_COMMAND:
        return GetWindowUserData<DialogBox>(hwnd)->OnCommand(wParam, lParam);
    case WM_TIMER:
        return GetWindowUserData<DialogBox>(hwnd)->OnTimer(wParam, lParam);
    case WM_NOTIFY:
        return GetWindowUserData<DialogBox>(hwnd)->OnNotify(wParam, lParam);
    case WM_CTLCOLORSTATIC:
        return GetWindowUserData<DialogBox>(hwnd)->OnControlColorStatic(wParam, lParam);
    case WM_CTLCOLOREDIT:
        return GetWindowUserData<DialogBox>(hwnd)->OnControlColorEdit(wParam, lParam);
    default:
        return FALSE;
    }
    return TRUE;
}


void DialogBox::OnCreate(HWND hwnd)
{
    m_hwnd = hwnd;
    InitializeSize(hwnd);
    OnCreate();
}


void DialogBox::OnDestroy(HWND hwnd)
{
    UNREFERENCED_PARAMETER(hwnd);
    OnDestroy();
    m_hwnd = nullptr;
}


void DialogBox::OnCreate()
{
}


void DialogBox::OnDestroy()
{
}


void DialogBox::OnClose()
{
}


void DialogBox::OnSize(WPARAM wParam, LPARAM lParam)
{
    WindowSize::OnSize(m_hwnd, wParam, lParam, *this);
}


INT_PTR DialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    return FALSE;
}


INT_PTR DialogBox::OnNotify(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    return FALSE;
}


INT_PTR DialogBox::OnTimer(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    return 0;
}


INT_PTR DialogBox::OnControlColorStatic(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    return FALSE;
}


INT_PTR DialogBox::OnControlColorEdit(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    return FALSE;
}
