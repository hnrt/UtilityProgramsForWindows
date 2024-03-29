#include <Windows.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include "hnrt/CoreLibrary.h"
#include "hnrt/Debug.h"
#include "hnrt/ResourceString.h"
#include "hnrt/ComException.h"
#include "MainWindow.h"
#include "resource.h"


#pragma comment(lib, "Core")
#pragma comment(lib, "WindowApp")
#pragma comment(lib, "KeyboardMouseBridge")


using namespace hnrt;


static CoreLibrary theCore;


int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd
)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    DBGFNC(L"WinMain");
    MainWindow app;
    try
    {
        _wsetlocale(LC_ALL, L"");
        ResourceString::m_hInstance = hInstance;
        app.Open(hInstance, lpCmdLine, nShowCmd);
        app.Run();
        app.Close();
    }
    catch (Exception e)
    {
        MessageBoxW(NULL, e.Message, ResourceString(IDS_CAPTION), MB_OK | MB_ICONERROR);
    }
    catch (std::bad_alloc)
    {
        MessageBoxW(NULL, ResourceString(IDS_OUT_OF_MEMORY), ResourceString(IDS_CAPTION), MB_OK | MB_ICONERROR);
    }
    catch (...)
    {
        MessageBoxW(NULL, ResourceString(IDS_UNHANDLED_EXCEPTION), ResourceString(IDS_CAPTION), MB_OK | MB_ICONERROR);
    }
    return app.ExitCode;
}
