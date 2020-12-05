#pragma once


#define KEYBOARDMOUSE_SERVER32_FAILURE          (0UL<<0)
#define KEYBOARDMOUSE_SERVER32_START_PENDING    (1UL<<0)
#define KEYBOARDMOUSE_SERVER32_RUNNING          (2UL<<0)
#define KEYBOARDMOUSE_SERVER32_MASK             ((1UL<<0)|(2UL<<0))
#define KEYBOARDMOUSE_SERVER64_FAILURE          (0UL<<2)
#define KEYBOARDMOUSE_SERVER64_START_PENDING    (1UL<<2)
#define KEYBOARDMOUSE_SERVER64_RUNNING          (2UL<<2)
#define KEYBOARDMOUSE_SERVER64_MASK             ((1UL<<2)|(2UL<<2))


#define KEYBOARD_FLAG_BLOCK     (1UL<<0)
#define KEYBOARD_FLAG_LANGUAGE  (1UL<<1)
#define KEYBOARD_FLAG_OPENCLOSE (1UL<<2)
#define KEYBOARD_FLAG_INPUTMODE (1UL<<3)


#define MOUSE_FLAG_BLOCK        (1UL<<16)


#define AGENT_FLAG_ENABLED      (1UL<<31)


#define KEYBOARD_STATE_PACK(T,langid,openclose,inputmode) \
    (((static_cast<T>(langid) & 0xFFFFUL) << 16) \
    |((static_cast<T>(openclose) & 1UL) << 15) \
    |((static_cast<T>(inputmode) & 0x7FFFUL) << 0) \
    )


#define KEYBOARD_STATE_UNPACK_LANGUAGE(x)   ((static_cast<LPARAM>(x)>>16)&0xFFFFUL) 
#define KEYBOARD_STATE_UNPACK_OPENCLOSE(x)  ((static_cast<LPARAM>(x)>>15)&1UL)
#define KEYBOARD_STATE_UNPACK_INPUTMODE(x)  ((static_cast<LPARAM>(x)>>0)&0x7FFFUL)
