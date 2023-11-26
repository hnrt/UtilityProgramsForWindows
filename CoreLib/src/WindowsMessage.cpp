#include "pch.h"
#include "hnrt/WindowsMessage.h"


using namespace hnrt;


PCWSTR WindowsMessage::get_Name() const
{
    if (!m_sz)
    {
        switch (m_value)
        {
#define C(x) case x: m_sz = String(STATIC_TEXT, L#x); break
            C(WM_NULL);
            C(WM_CREATE);
            C(WM_DESTROY);
            C(WM_MOVE);
            C(WM_SIZE);
            C(WM_ACTIVATE);
            C(WM_SETFOCUS);
            C(WM_KILLFOCUS);
            C(WM_ENABLE);
            C(WM_SETREDRAW);
            C(WM_SETTEXT);
            C(WM_GETTEXT);
            C(WM_GETTEXTLENGTH);
            C(WM_PAINT);
            C(WM_CLOSE);
            C(WM_QUERYENDSESSION);
            C(WM_QUERYOPEN);
            C(WM_ENDSESSION);
            C(WM_QUIT);
            C(WM_ERASEBKGND);
            C(WM_SYSCOLORCHANGE);
            C(WM_SHOWWINDOW);
            C(WM_SETTINGCHANGE);
            C(WM_DEVMODECHANGE);
            C(WM_ACTIVATEAPP);
            C(WM_FONTCHANGE);
            C(WM_TIMECHANGE);
            C(WM_CANCELMODE);
            C(WM_SETCURSOR);
            C(WM_MOUSEACTIVATE);
            C(WM_CHILDACTIVATE);
            C(WM_QUEUESYNC);
            C(WM_GETMINMAXINFO);
            C(WM_PAINTICON);
            C(WM_ICONERASEBKGND);
            C(WM_NEXTDLGCTL);
            C(WM_SPOOLERSTATUS);
            C(WM_DRAWITEM);
            C(WM_MEASUREITEM);
            C(WM_DELETEITEM);
            C(WM_VKEYTOITEM);
            C(WM_CHARTOITEM);
            C(WM_SETFONT);
            C(WM_GETFONT);
            C(WM_SETHOTKEY);
            C(WM_GETHOTKEY);
            C(WM_QUERYDRAGICON);
            C(WM_COMPAREITEM);
            C(WM_GETOBJECT);
            C(WM_COMPACTING);
            C(WM_COMMNOTIFY);
            C(WM_WINDOWPOSCHANGING);
            C(WM_WINDOWPOSCHANGED);
            C(WM_POWER);
            C(WM_COPYDATA);
            C(WM_CANCELJOURNAL);
            C(WM_NOTIFY);
            C(WM_INPUTLANGCHANGEREQUEST);
            C(WM_INPUTLANGCHANGE);
            C(WM_TCARD);
            C(WM_HELP);
            C(WM_USERCHANGED);
            C(WM_NOTIFYFORMAT);
            C(WM_CONTEXTMENU);
            C(WM_STYLECHANGING);
            C(WM_STYLECHANGED);
            C(WM_DISPLAYCHANGE);
            C(WM_GETICON);
            C(WM_SETICON);
            C(WM_NCCREATE);
            C(WM_NCDESTROY);
            C(WM_NCCALCSIZE);
            C(WM_NCHITTEST);
            C(WM_NCPAINT);
            C(WM_NCACTIVATE);
            C(WM_GETDLGCODE);
            C(WM_SYNCPAINT);
            C(WM_NCMOUSEMOVE);
            C(WM_NCLBUTTONDOWN);
            C(WM_NCLBUTTONUP);
            C(WM_NCLBUTTONDBLCLK);
            C(WM_NCRBUTTONDOWN);
            C(WM_NCRBUTTONUP);
            C(WM_NCRBUTTONDBLCLK);
            C(WM_NCMBUTTONDOWN);
            C(WM_NCMBUTTONUP);
            C(WM_NCMBUTTONDBLCLK);
            C(WM_NCXBUTTONDOWN);
            C(WM_NCXBUTTONUP);
            C(WM_NCXBUTTONDBLCLK);
            C(WM_INPUT_DEVICE_CHANGE);
            C(WM_INPUT);
            C(WM_KEYDOWN);
            C(WM_KEYUP);
            C(WM_CHAR);
            C(WM_DEADCHAR);
            C(WM_SYSKEYDOWN);
            C(WM_SYSKEYUP);
            C(WM_SYSCHAR);
            C(WM_SYSDEADCHAR);
            C(WM_UNICHAR);
            C(WM_IME_STARTCOMPOSITION);
            C(WM_IME_ENDCOMPOSITION);
            C(WM_IME_COMPOSITION);
            C(WM_INITDIALOG);
            C(WM_COMMAND);
            C(WM_SYSCOMMAND);
            C(WM_TIMER);
            C(WM_HSCROLL);
            C(WM_VSCROLL);
            C(WM_INITMENU);
            C(WM_INITMENUPOPUP);
            C(WM_GESTURE);
            C(WM_GESTURENOTIFY);
            C(WM_MENUSELECT);
            C(WM_MENUCHAR);
            C(WM_ENTERIDLE);
            C(WM_MENURBUTTONUP);
            C(WM_MENUDRAG);
            C(WM_MENUGETOBJECT);
            C(WM_UNINITMENUPOPUP);
            C(WM_MENUCOMMAND);
            C(WM_CHANGEUISTATE);
            C(WM_UPDATEUISTATE);
            C(WM_QUERYUISTATE);
            C(WM_CTLCOLORMSGBOX);
            C(WM_CTLCOLOREDIT);
            C(WM_CTLCOLORLISTBOX);
            C(WM_CTLCOLORBTN);
            C(WM_CTLCOLORDLG);
            C(WM_CTLCOLORSCROLLBAR);
            C(WM_CTLCOLORSTATIC);
            C(MN_GETHMENU);
            C(WM_MOUSEMOVE);
            C(WM_LBUTTONDOWN);
            C(WM_LBUTTONUP);
            C(WM_LBUTTONDBLCLK);
            C(WM_RBUTTONDOWN);
            C(WM_RBUTTONUP);
            C(WM_RBUTTONDBLCLK);
            C(WM_MBUTTONDOWN);
            C(WM_MBUTTONUP);
            C(WM_MBUTTONDBLCLK);
            C(WM_MOUSEWHEEL);
            C(WM_XBUTTONDOWN);
            C(WM_XBUTTONUP);
            C(WM_XBUTTONDBLCLK);
            C(WM_MOUSEHWHEEL);
            C(WM_PARENTNOTIFY);
            C(WM_ENTERMENULOOP);
            C(WM_EXITMENULOOP);
            C(WM_NEXTMENU);
            C(WM_SIZING);
            C(WM_CAPTURECHANGED);
            C(WM_MOVING);
            C(WM_POWERBROADCAST);
            C(WM_DEVICECHANGE);
            C(WM_MDICREATE);
            C(WM_MDIDESTROY);
            C(WM_MDIACTIVATE);
            C(WM_MDIRESTORE);
            C(WM_MDINEXT);
            C(WM_MDIMAXIMIZE);
            C(WM_MDITILE);
            C(WM_MDICASCADE);
            C(WM_MDIICONARRANGE);
            C(WM_MDIGETACTIVE);
            C(WM_MDISETMENU);
            C(WM_ENTERSIZEMOVE);
            C(WM_EXITSIZEMOVE);
            C(WM_DROPFILES);
            C(WM_MDIREFRESHMENU);
            C(WM_POINTERDEVICECHANGE);
            C(WM_POINTERDEVICEINRANGE);
            C(WM_POINTERDEVICEOUTOFRANGE);
            C(WM_TOUCH);
            C(WM_NCPOINTERUPDATE);
            C(WM_NCPOINTERDOWN);
            C(WM_NCPOINTERUP);
            C(WM_POINTERUPDATE);
            C(WM_POINTERDOWN);
            C(WM_POINTERUP);
            C(WM_POINTERENTER);
            C(WM_POINTERLEAVE);
            C(WM_POINTERACTIVATE);
            C(WM_POINTERCAPTURECHANGED);
            C(WM_TOUCHHITTESTING);
            C(WM_POINTERWHEEL);
            C(WM_POINTERHWHEEL);
            C(DM_POINTERHITTEST);
            C(WM_POINTERROUTEDTO);
            C(WM_POINTERROUTEDAWAY);
            C(WM_POINTERROUTEDRELEASED);
            C(WM_IME_SETCONTEXT);
            C(WM_IME_NOTIFY);
            C(WM_IME_CONTROL);
            C(WM_IME_COMPOSITIONFULL);
            C(WM_IME_SELECT);
            C(WM_IME_CHAR);
            C(WM_IME_REQUEST);
            C(WM_IME_KEYDOWN);
            C(WM_IME_KEYUP);
            C(WM_MOUSEHOVER);
            C(WM_MOUSELEAVE);
            C(WM_NCMOUSEHOVER);
            C(WM_NCMOUSELEAVE);
            C(WM_WTSSESSION_CHANGE);
            C(WM_DPICHANGED);
            C(WM_DPICHANGED_BEFOREPARENT);
            C(WM_DPICHANGED_AFTERPARENT);
            C(WM_GETDPISCALEDSIZE);
            C(WM_CUT);
            C(WM_COPY);
            C(WM_PASTE);
            C(WM_CLEAR);
            C(WM_UNDO);
            C(WM_RENDERFORMAT);
            C(WM_RENDERALLFORMATS);
            C(WM_DESTROYCLIPBOARD);
            C(WM_DRAWCLIPBOARD);
            C(WM_PAINTCLIPBOARD);
            C(WM_VSCROLLCLIPBOARD);
            C(WM_SIZECLIPBOARD);
            C(WM_ASKCBFORMATNAME);
            C(WM_CHANGECBCHAIN);
            C(WM_HSCROLLCLIPBOARD);
            C(WM_QUERYNEWPALETTE);
            C(WM_PALETTEISCHANGING);
            C(WM_PALETTECHANGED);
            C(WM_HOTKEY);
            C(WM_PRINT);
            C(WM_PRINTCLIENT);
            C(WM_APPCOMMAND);
            C(WM_THEMECHANGED);
            C(WM_CLIPBOARDUPDATE);
            C(WM_DWMCOMPOSITIONCHANGED);
            C(WM_DWMNCRENDERINGCHANGED);
            C(WM_DWMCOLORIZATIONCOLORCHANGED);
            C(WM_DWMWINDOWMAXIMIZEDCHANGE);
            C(WM_DWMSENDICONICTHUMBNAIL);
            C(WM_DWMSENDICONICLIVEPREVIEWBITMAP);
            C(WM_GETTITLEBARINFOEX);
        default:
            if (m_value < WM_USER)
            {
                m_sz = String(PRINTF, L"WM_%04X", m_value);
            }
            else if (m_value < WM_APP)
            {
                m_sz = String(PRINTF, L"WM_USER_%04X", m_value - WM_USER);
            }
            else if (m_value < 0xC000)
            {
                m_sz = String(PRINTF, L"WM_APP_%04X", m_value - WM_APP);
            }
            else if (m_value < 0x10000)
            {
                WCHAR sz[80];
                if (GetClipboardFormatNameW(m_value, sz, _countof(sz)))
                {
                    m_sz = sz;
                }
                else
                {
                    m_sz = String(PRINTF, L"?%04X?", m_value);
                }
            }
            else
            {
                m_sz = String(PRINTF, L"?%08X?", m_value);
            }
            break;
        }
    }
    return m_sz;
}