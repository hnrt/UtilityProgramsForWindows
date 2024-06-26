#include "pch.h"
#include <locale.h>
#include <stdexcept>
#include "hnrt/CoreLibrary.h"
#include "hnrt/Exception.h"
#include "hnrt/ResourceString.h"
#include "MyToolbox.h"
#include "resource.h"


#pragma comment(lib, "Ole32")
#pragma comment(lib, "Comctl32")
#pragma comment(lib, "Core")
#pragma comment(lib, "Cron")
#pragma comment(lib, "WindowApp")
#pragma comment(lib, "Industry")


using namespace hnrt;


static CoreLibrary theCore;


int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    MyToolbox app;
    try
    {
        _wsetlocale(LC_ALL, L"");
        ResourceString::m_hInstance = hInstance;
        app.Open(hInstance, lpCmdLine, nCmdShow);
        app.Run();
        app.Close();
    }
    catch (Exception e)
    {
        MessageBoxW(NULL, e.Message, ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
    }
    catch (std::bad_alloc)
    {
        MessageBoxW(NULL, ResourceString(IDS_OUT_OF_MEMORY), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
    }
    catch (...)
    {
        MessageBoxW(NULL, ResourceString(IDS_UNKNOWN_FAILURE), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
    }
    return app.ExitCode;
}
