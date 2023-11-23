#include "ComboBox.h"
#include "hnrt/Buffer.h"


using namespace hnrt;


ComboBox::ComboBox()
    : m_hwnd(nullptr)
{
}


void ComboBox::Init(HWND hwnd)
{
    m_hwnd = hwnd;
}


void ComboBox::Clear()
{
    SendMessageW(m_hwnd, CB_RESETCONTENT, 0, 0);
}


void ComboBox::Add(PCWSTR psz)
{
    SendMessageW(m_hwnd, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(psz ? psz : L""));
}


bool ComboBox::Select(PCWSTR psz)
{
    LRESULT ret = SendMessageW(m_hwnd, CB_SELECTSTRING, 0, reinterpret_cast<LPARAM>(psz ? psz : L""));
    return ret != CB_ERR;
}


void ComboBox::Set(PCWSTR psz)
{
    SetWindowTextW(m_hwnd, psz ? psz : L"");
}


String ComboBox::get_Selected() const
{
    LRESULT index = SendMessageW(m_hwnd, CB_GETCURSEL, 0, 0);
    if (index == CB_ERR)
    {
        LRESULT length = GetWindowTextLengthW(m_hwnd);
        if (length <= 0)
        {
            return L"";
        }
        Buffer<WCHAR> buf(length + 1);
        GetWindowTextW(m_hwnd, &buf, static_cast<int>(buf.Len));
        return String(buf);
    }
    else
    {
        LRESULT length = SendMessageW(m_hwnd, CB_GETLBTEXTLEN, index, 0);
        if (length == CB_ERR)
        {
            return L"";
        }
        Buffer<WCHAR> buf(length + 1);
        length = SendMessageW(m_hwnd, CB_GETLBTEXT, index, reinterpret_cast<LPARAM>(&buf));
        if (length == CB_ERR)
        {
            return L"";
        }
        return String(buf);
    }
}
