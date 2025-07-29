#include "pch.h"
#include "hnrt/VirtualKey.h"
#include "hnrt/StringCaseLessThan.h"
#include <map>


using namespace hnrt;


class VirtualKeyHelper
    : public std::map<PCWSTR, unsigned char, StringCaseLessThan>
{
public:

    VirtualKeyHelper();
    VirtualKeyHelper(const VirtualKeyHelper&) = delete;
    ~VirtualKeyHelper() = default;
    void operator =(const VirtualKeyHelper&) = delete;
    unsigned char Find(PCWSTR psz);
};


static VirtualKeyHelper s_Helper;


VirtualKey::VirtualKey(PCWSTR psz)
    : m_value(s_Helper.Find(psz))
{
}


PCWSTR VirtualKey::get_Name() const
{
    switch (m_value)
    {
#define C(x) case x: return L#x
#define R(x) case x: return L"(reserved)"
#define U(x) case x: return L"(unassigned)"
#define D(x,y) case x: return y
        C(VK_LBUTTON);
        C(VK_RBUTTON);
        C(VK_CANCEL);
        C(VK_MBUTTON);    /* NOT contiguous with L & RBUTTON */
        C(VK_XBUTTON1);    /* NOT contiguous with L & RBUTTON */
        C(VK_XBUTTON2);    /* NOT contiguous with L & RBUTTON */
        R(0x07);
        C(VK_BACK);
        C(VK_TAB);
        R(0x0A);
        R(0x0B);
        C(VK_CLEAR);
        C(VK_RETURN);
        U(0x0E);
        U(0x0F);
        C(VK_SHIFT);
        C(VK_CONTROL);
        C(VK_MENU);
        C(VK_PAUSE);
        C(VK_CAPITAL);
        D(VK_KANA, L"VK_KANA/HANGUL");
        U(0x16);
        C(VK_JUNJA);
        C(VK_FINAL);
        D(VK_KANJI, L"VK_KANJI/HANJA");
        U(0x1A);
        C(VK_ESCAPE);
        C(VK_CONVERT);
        C(VK_NONCONVERT);
        C(VK_ACCEPT);
        C(VK_MODECHANGE);
        C(VK_SPACE);
        C(VK_PRIOR);
        C(VK_NEXT);
        C(VK_END);
        C(VK_HOME);
        C(VK_LEFT);
        C(VK_UP);
        C(VK_RIGHT);
        C(VK_DOWN);
        C(VK_SELECT);
        C(VK_PRINT);
        C(VK_EXECUTE);
        C(VK_SNAPSHOT);
        C(VK_INSERT);
        C(VK_DELETE);
        C(VK_HELP);
        D(0x30, L"VK_0");
        D(0x31, L"VK_1");
        D(0x32, L"VK_2");
        D(0x33, L"VK_3");
        D(0x34, L"VK_4");
        D(0x35, L"VK_5");
        D(0x36, L"VK_6");
        D(0x37, L"VK_7");
        D(0x38, L"VK_8");
        D(0x39, L"VK_9");
        U(0x3A);
        U(0x3B);
        U(0x3C);
        U(0x3D);
        U(0x3E);
        U(0x3F);
        U(0x40);
        D(0x41, L"VK_A");
        D(0x42, L"VK_B");
        D(0x43, L"VK_C");
        D(0x44, L"VK_D");
        D(0x45, L"VK_E");
        D(0x46, L"VK_F");
        D(0x47, L"VK_G");
        D(0x48, L"VK_H");
        D(0x49, L"VK_I");
        D(0x4A, L"VK_J");
        D(0x4B, L"VK_K");
        D(0x4C, L"VK_L");
        D(0x4D, L"VK_M");
        D(0x4E, L"VK_N");
        D(0x4F, L"VK_O");
        D(0x50, L"VK_P");
        D(0x51, L"VK_Q");
        D(0x52, L"VK_R");
        D(0x53, L"VK_S");
        D(0x54, L"VK_T");
        D(0x55, L"VK_U");
        D(0x56, L"VK_V");
        D(0x57, L"VK_W");
        D(0x58, L"VK_X");
        D(0x59, L"VK_Y");
        D(0x5A, L"VK_Z");
        C(VK_LWIN);
        C(VK_RWIN);
        C(VK_APPS);
        R(0x5E);
        C(VK_SLEEP);
        C(VK_NUMPAD0);
        C(VK_NUMPAD1);
        C(VK_NUMPAD2);
        C(VK_NUMPAD3);
        C(VK_NUMPAD4);
        C(VK_NUMPAD5);
        C(VK_NUMPAD6);
        C(VK_NUMPAD7);
        C(VK_NUMPAD8);
        C(VK_NUMPAD9);
        C(VK_MULTIPLY);
        C(VK_ADD);
        C(VK_SEPARATOR);
        C(VK_SUBTRACT);
        C(VK_DECIMAL);
        C(VK_DIVIDE);
        C(VK_F1);
        C(VK_F2);
        C(VK_F3);
        C(VK_F4);
        C(VK_F5);
        C(VK_F6);
        C(VK_F7);
        C(VK_F8);
        C(VK_F9);
        C(VK_F10);
        C(VK_F11);
        C(VK_F12);
        C(VK_F13);
        C(VK_F14);
        C(VK_F15);
        C(VK_F16);
        C(VK_F17);
        C(VK_F18);
        C(VK_F19);
        C(VK_F20);
        C(VK_F21);
        C(VK_F22);
        C(VK_F23);
        C(VK_F24);
        C(VK_NAVIGATION_VIEW); // reserved
        C(VK_NAVIGATION_MENU); // reserved
        C(VK_NAVIGATION_UP); // reserved
        C(VK_NAVIGATION_DOWN); // reserved
        C(VK_NAVIGATION_LEFT); // reserved
        C(VK_NAVIGATION_RIGHT); // reserved
        C(VK_NAVIGATION_ACCEPT); // reserved
        C(VK_NAVIGATION_CANCEL); // reserved
        C(VK_NUMLOCK);
        C(VK_SCROLL);
        D(0x92, L"VK_OEM_NEC_EQUAL/OEM_FJ_JISHO");   // '=' key on numpad
        C(VK_OEM_FJ_MASSHOU);   // 'Unregister word' key
        C(VK_OEM_FJ_TOUROKU);   // 'Register word' key
        C(VK_OEM_FJ_LOYA);   // 'Left OYAYUBI' key
        C(VK_OEM_FJ_ROYA);   // 'Right OYAYUBI' key
        U(0x97);
        U(0x98);
        U(0x99);
        U(0x9A);
        U(0x9B);
        U(0x9C);
        U(0x9D);
        U(0x9E);
        U(0x9F);
        C(VK_LSHIFT);
        C(VK_RSHIFT);
        C(VK_LCONTROL);
        C(VK_RCONTROL);
        C(VK_LMENU);
        C(VK_RMENU);
        C(VK_BROWSER_BACK);
        C(VK_BROWSER_FORWARD);
        C(VK_BROWSER_REFRESH);
        C(VK_BROWSER_STOP);
        C(VK_BROWSER_SEARCH);
        C(VK_BROWSER_FAVORITES);
        C(VK_BROWSER_HOME);
        C(VK_VOLUME_MUTE);
        C(VK_VOLUME_DOWN);
        C(VK_VOLUME_UP);
        C(VK_MEDIA_NEXT_TRACK);
        C(VK_MEDIA_PREV_TRACK);
        C(VK_MEDIA_STOP);
        C(VK_MEDIA_PLAY_PAUSE);
        C(VK_LAUNCH_MAIL);
        C(VK_LAUNCH_MEDIA_SELECT);
        C(VK_LAUNCH_APP1);
        C(VK_LAUNCH_APP2);
        R(0xB8);
        R(0xB9);
        C(VK_OEM_1);   // ';:' for US
        C(VK_OEM_PLUS);   // '+' any country
        C(VK_OEM_COMMA);   // ',' any country
        C(VK_OEM_MINUS);   // '-' any country
        C(VK_OEM_PERIOD);   // '.' any country
        C(VK_OEM_2);   // '/?' for US
        C(VK_OEM_3);   // '`~' for US
        R(0xC1);
        R(0xC2);
        C(VK_GAMEPAD_A); // reserved
        C(VK_GAMEPAD_B); // reserved
        C(VK_GAMEPAD_X); // reserved
        C(VK_GAMEPAD_Y); // reserved
        C(VK_GAMEPAD_RIGHT_SHOULDER); // reserved
        C(VK_GAMEPAD_LEFT_SHOULDER); // reserved
        C(VK_GAMEPAD_LEFT_TRIGGER); // reserved
        C(VK_GAMEPAD_RIGHT_TRIGGER); // reserved
        C(VK_GAMEPAD_DPAD_UP); // reserved
        C(VK_GAMEPAD_DPAD_DOWN); // reserved
        C(VK_GAMEPAD_DPAD_LEFT); // reserved
        C(VK_GAMEPAD_DPAD_RIGHT); // reserved
        C(VK_GAMEPAD_MENU); // reserved
        C(VK_GAMEPAD_VIEW); // reserved
        C(VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON); // reserved
        C(VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON); // reserved
        C(VK_GAMEPAD_LEFT_THUMBSTICK_UP); // reserved
        C(VK_GAMEPAD_LEFT_THUMBSTICK_DOWN); // reserved
        C(VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT); // reserved
        C(VK_GAMEPAD_LEFT_THUMBSTICK_LEFT); // reserved
        C(VK_GAMEPAD_RIGHT_THUMBSTICK_UP); // reserved
        C(VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN); // reserved
        C(VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT); // reserved
        C(VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT); // reserved
        C(VK_OEM_4);  //  '[{' for US
        C(VK_OEM_5);  //  '\|' for US
        C(VK_OEM_6);  //  ']}' for US
        C(VK_OEM_7);  //  ''"' for US
        C(VK_OEM_8);
        R(0xE0);
        C(VK_OEM_AX);  //  'AX' key on Japanese AX kbd
        C(VK_OEM_102);  //  "<>" or "\|" on RT 102-key kbd.
        C(VK_ICO_HELP);  //  Help key on ICO
        C(VK_ICO_00);  //  00 key on ICO
        C(VK_PROCESSKEY);
        C(VK_ICO_CLEAR);
        C(VK_PACKET);
        U(0xE8);
        C(VK_OEM_RESET);
        C(VK_OEM_JUMP);
        C(VK_OEM_PA1);
        C(VK_OEM_PA2);
        C(VK_OEM_PA3);
        C(VK_OEM_WSCTRL);
        C(VK_OEM_CUSEL);
        C(VK_OEM_ATTN);
        C(VK_OEM_FINISH);
        C(VK_OEM_COPY);
        C(VK_OEM_AUTO);
        C(VK_OEM_ENLW);
        C(VK_OEM_BACKTAB);
        C(VK_ATTN);
        C(VK_CRSEL);
        C(VK_EXSEL);
        C(VK_EREOF);
        C(VK_PLAY);
        C(VK_ZOOM);
        C(VK_NONAME);
        C(VK_PA1);
        C(VK_OEM_CLEAR);
        default: return L"(unassigned)";
    }
}


VirtualKeyHelper::VirtualKeyHelper()
    : std::map<PCWSTR, unsigned char, StringCaseLessThan>()
{
#define INSERT(x) insert(std::pair<PCWSTR, unsigned char>(L#x,x))
#define INSERT2(x,y) insert(std::pair<PCWSTR, unsigned char>(x,y))
    INSERT(VK_LBUTTON);
    INSERT(VK_RBUTTON);
    INSERT(VK_CANCEL);
    INSERT(VK_MBUTTON);    /* NOT contiguous with L & RBUTTON */
    INSERT(VK_XBUTTON1);    /* NOT contiguous with L & RBUTTON */
    INSERT(VK_XBUTTON2);    /* NOT contiguous with L & RBUTTON */
    INSERT(VK_BACK);
    INSERT(VK_TAB);
    INSERT(VK_CLEAR);
    INSERT(VK_RETURN);
    INSERT(VK_SHIFT);
    INSERT(VK_CONTROL);
    INSERT(VK_MENU);
    INSERT(VK_PAUSE);
    INSERT(VK_CAPITAL);
    INSERT(VK_KANA);
    INSERT(VK_HANGUL);
    INSERT(VK_JUNJA);
    INSERT(VK_FINAL);
    INSERT(VK_KANJI);
    INSERT(VK_HANJA);
    INSERT(VK_ESCAPE);
    INSERT(VK_CONVERT);
    INSERT(VK_NONCONVERT);
    INSERT(VK_ACCEPT);
    INSERT(VK_MODECHANGE);
    INSERT(VK_SPACE);
    INSERT(VK_PRIOR);
    INSERT(VK_NEXT);
    INSERT(VK_END);
    INSERT(VK_HOME);
    INSERT(VK_LEFT);
    INSERT(VK_UP);
    INSERT(VK_RIGHT);
    INSERT(VK_DOWN);
    INSERT(VK_SELECT);
    INSERT(VK_PRINT);
    INSERT(VK_EXECUTE);
    INSERT(VK_SNAPSHOT);
    INSERT(VK_INSERT);
    INSERT(VK_DELETE);
    INSERT(VK_HELP);
    INSERT2(L"VK_0", 0x30);
    INSERT2(L"VK_1", 0x31);
    INSERT2(L"VK_2", 0x32);
    INSERT2(L"VK_3", 0x33);
    INSERT2(L"VK_4", 0x34);
    INSERT2(L"VK_5", 0x35);
    INSERT2(L"VK_6", 0x36);
    INSERT2(L"VK_7", 0x37);
    INSERT2(L"VK_8", 0x38);
    INSERT2(L"VK_9", 0x39);
    INSERT2(L"VK_A", 0x41);
    INSERT2(L"VK_B", 0x42);
    INSERT2(L"VK_C", 0x43);
    INSERT2(L"VK_D", 0x44);
    INSERT2(L"VK_E", 0x45);
    INSERT2(L"VK_F", 0x46);
    INSERT2(L"VK_G", 0x47);
    INSERT2(L"VK_H", 0x48);
    INSERT2(L"VK_I", 0x49);
    INSERT2(L"VK_J", 0x4A);
    INSERT2(L"VK_K", 0x4B);
    INSERT2(L"VK_L", 0x4C);
    INSERT2(L"VK_M", 0x4D);
    INSERT2(L"VK_N", 0x4E);
    INSERT2(L"VK_O", 0x4F);
    INSERT2(L"VK_P", 0x50);
    INSERT2(L"VK_Q", 0x51);
    INSERT2(L"VK_R", 0x52);
    INSERT2(L"VK_S", 0x53);
    INSERT2(L"VK_T", 0x54);
    INSERT2(L"VK_U", 0x55);
    INSERT2(L"VK_V", 0x56);
    INSERT2(L"VK_W", 0x57);
    INSERT2(L"VK_X", 0x58);
    INSERT2(L"VK_Y", 0x59);
    INSERT2(L"VK_Z", 0x5A);
    INSERT(VK_LWIN);
    INSERT(VK_RWIN);
    INSERT(VK_APPS);
    INSERT(VK_SLEEP);
    INSERT(VK_NUMPAD0);
    INSERT(VK_NUMPAD1);
    INSERT(VK_NUMPAD2);
    INSERT(VK_NUMPAD3);
    INSERT(VK_NUMPAD4);
    INSERT(VK_NUMPAD5);
    INSERT(VK_NUMPAD6);
    INSERT(VK_NUMPAD7);
    INSERT(VK_NUMPAD8);
    INSERT(VK_NUMPAD9);
    INSERT(VK_MULTIPLY);
    INSERT(VK_ADD);
    INSERT(VK_SEPARATOR);
    INSERT(VK_SUBTRACT);
    INSERT(VK_DECIMAL);
    INSERT(VK_DIVIDE);
    INSERT(VK_F1);
    INSERT(VK_F2);
    INSERT(VK_F3);
    INSERT(VK_F4);
    INSERT(VK_F5);
    INSERT(VK_F6);
    INSERT(VK_F7);
    INSERT(VK_F8);
    INSERT(VK_F9);
    INSERT(VK_F10);
    INSERT(VK_F11);
    INSERT(VK_F12);
    INSERT(VK_F13);
    INSERT(VK_F14);
    INSERT(VK_F15);
    INSERT(VK_F16);
    INSERT(VK_F17);
    INSERT(VK_F18);
    INSERT(VK_F19);
    INSERT(VK_F20);
    INSERT(VK_F21);
    INSERT(VK_F22);
    INSERT(VK_F23);
    INSERT(VK_F24);
    INSERT(VK_NAVIGATION_VIEW); // reserved
    INSERT(VK_NAVIGATION_MENU); // reserved
    INSERT(VK_NAVIGATION_UP); // reserved
    INSERT(VK_NAVIGATION_DOWN); // reserved
    INSERT(VK_NAVIGATION_LEFT); // reserved
    INSERT(VK_NAVIGATION_RIGHT); // reserved
    INSERT(VK_NAVIGATION_ACCEPT); // reserved
    INSERT(VK_NAVIGATION_CANCEL); // reserved
    INSERT(VK_NUMLOCK);
    INSERT(VK_SCROLL);
    INSERT2(L"VK_OEM_NEC_EQUAL", 0x92);   // '=' key on numpad
    INSERT2(L"VK_OEM_FJ_JISHO", 0x92);   // '=' key on numpad
    INSERT(VK_OEM_FJ_MASSHOU);   // 'Unregister word' key
    INSERT(VK_OEM_FJ_TOUROKU);   // 'Register word' key
    INSERT(VK_OEM_FJ_LOYA);   // 'Left OYAYUBI' key
    INSERT(VK_OEM_FJ_ROYA);   // 'Right OYAYUBI' key
    INSERT(VK_LSHIFT);
    INSERT(VK_RSHIFT);
    INSERT(VK_LCONTROL);
    INSERT(VK_RCONTROL);
    INSERT(VK_LMENU);
    INSERT(VK_RMENU);
    INSERT(VK_BROWSER_BACK);
    INSERT(VK_BROWSER_FORWARD);
    INSERT(VK_BROWSER_REFRESH);
    INSERT(VK_BROWSER_STOP);
    INSERT(VK_BROWSER_SEARCH);
    INSERT(VK_BROWSER_FAVORITES);
    INSERT(VK_BROWSER_HOME);
    INSERT(VK_VOLUME_MUTE);
    INSERT(VK_VOLUME_DOWN);
    INSERT(VK_VOLUME_UP);
    INSERT(VK_MEDIA_NEXT_TRACK);
    INSERT(VK_MEDIA_PREV_TRACK);
    INSERT(VK_MEDIA_STOP);
    INSERT(VK_MEDIA_PLAY_PAUSE);
    INSERT(VK_LAUNCH_MAIL);
    INSERT(VK_LAUNCH_MEDIA_SELECT);
    INSERT(VK_LAUNCH_APP1);
    INSERT(VK_LAUNCH_APP2);
    INSERT(VK_OEM_1);   // ';:' for US
    INSERT(VK_OEM_PLUS);   // '+' any country
    INSERT(VK_OEM_COMMA);   // ',' any country
    INSERT(VK_OEM_MINUS);   // '-' any country
    INSERT(VK_OEM_PERIOD);   // '.' any country
    INSERT(VK_OEM_2);   // '/?' for US
    INSERT(VK_OEM_3);   // '`~' for US
    INSERT(VK_GAMEPAD_A); // reserved
    INSERT(VK_GAMEPAD_B); // reserved
    INSERT(VK_GAMEPAD_X); // reserved
    INSERT(VK_GAMEPAD_Y); // reserved
    INSERT(VK_GAMEPAD_RIGHT_SHOULDER); // reserved
    INSERT(VK_GAMEPAD_LEFT_SHOULDER); // reserved
    INSERT(VK_GAMEPAD_LEFT_TRIGGER); // reserved
    INSERT(VK_GAMEPAD_RIGHT_TRIGGER); // reserved
    INSERT(VK_GAMEPAD_DPAD_UP); // reserved
    INSERT(VK_GAMEPAD_DPAD_DOWN); // reserved
    INSERT(VK_GAMEPAD_DPAD_LEFT); // reserved
    INSERT(VK_GAMEPAD_DPAD_RIGHT); // reserved
    INSERT(VK_GAMEPAD_MENU); // reserved
    INSERT(VK_GAMEPAD_VIEW); // reserved
    INSERT(VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON); // reserved
    INSERT(VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON); // reserved
    INSERT(VK_GAMEPAD_LEFT_THUMBSTICK_UP); // reserved
    INSERT(VK_GAMEPAD_LEFT_THUMBSTICK_DOWN); // reserved
    INSERT(VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT); // reserved
    INSERT(VK_GAMEPAD_LEFT_THUMBSTICK_LEFT); // reserved
    INSERT(VK_GAMEPAD_RIGHT_THUMBSTICK_UP); // reserved
    INSERT(VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN); // reserved
    INSERT(VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT); // reserved
    INSERT(VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT); // reserved
    INSERT(VK_OEM_4);  //  '[{' for US
    INSERT(VK_OEM_5);  //  '\|' for US
    INSERT(VK_OEM_6);  //  ']}' for US
    INSERT(VK_OEM_7);  //  ''"' for US
    INSERT(VK_OEM_8);
    INSERT(VK_OEM_AX);  //  'AX' key on Japanese AX kbd
    INSERT(VK_OEM_102);  //  "<>" or "\|" on RT 102-key kbd.
    INSERT(VK_ICO_HELP);  //  Help key on ICO
    INSERT(VK_ICO_00);  //  00 key on ICO
    INSERT(VK_PROCESSKEY);
    INSERT(VK_ICO_CLEAR);
    INSERT(VK_PACKET);
    INSERT(VK_OEM_RESET);
    INSERT(VK_OEM_JUMP);
    INSERT(VK_OEM_PA1);
    INSERT(VK_OEM_PA2);
    INSERT(VK_OEM_PA3);
    INSERT(VK_OEM_WSCTRL);
    INSERT(VK_OEM_CUSEL);
    INSERT(VK_OEM_ATTN);
    INSERT(VK_OEM_FINISH);
    INSERT(VK_OEM_COPY);
    INSERT(VK_OEM_AUTO);
    INSERT(VK_OEM_ENLW);
    INSERT(VK_OEM_BACKTAB);
    INSERT(VK_ATTN);
    INSERT(VK_CRSEL);
    INSERT(VK_EXSEL);
    INSERT(VK_EREOF);
    INSERT(VK_PLAY);
    INSERT(VK_ZOOM);
    INSERT(VK_NONAME);
    INSERT(VK_PA1);
    INSERT(VK_OEM_CLEAR);
}


unsigned char VirtualKeyHelper::Find(PCWSTR psz)
{
    std::map<PCWSTR, unsigned char, StringCaseLessThan>::const_iterator iter = find(psz);
    return iter != end() ? iter->second : 0;
}
