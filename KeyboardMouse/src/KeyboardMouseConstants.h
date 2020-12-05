#pragma once


#include <Windows.h>
#include "hnrt/KeyboardMouse.h"


#define WM_APP_START_AGENT      (WM_APP+1)
#define WM_APP_STOP_AGENT       (WM_APP+2)
#define WM_APP_GET_LANGUAGE     (WM_APP+3)
#define WM_APP_SET_LANGUAGE     (WM_APP+4)
#define WM_APP_GET_OPENCLOSE    (WM_APP+5)
#define WM_APP_SET_OPENCLOSE    (WM_APP+6)
#define WM_APP_GET_INPUTMODE    (WM_APP+7)
#define WM_APP_SET_INPUTMODE    (WM_APP+8)


#define AGENT_INITIALIZE        1
#define AGENT_UNINITIALIZE      2
#define AGENT_GET_LANGUAGE      3
#define AGENT_SET_LANGUAGE      4
#define AGENT_GET_OPENCLOSE     5
#define AGENT_SET_OPENCLOSE     6
#define AGENT_GET_INPUTMODE     7
#define AGENT_SET_INPUTMODE     8


#define AGENT_COMMAND_TIMEOUT   1000


#define START_TIMEOUT               3000
#define SHUTDOWN_TIMEOUT            3000
#define SHUTDOWN_WAIT_INTERVAL      100
#define MIN_MILLISECONDS_TO_CHANGE  2000
