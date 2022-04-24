#include "pch.h"
#include <locale.h>
#include <stdexcept>
#include "hnrt/CoreLibrary.h"
#include "hnrt/Exception.h"
#include "hnrt/ResourceString.h"
#include "Checksum.h"


#pragma comment(lib, "Core")
#pragma comment(lib, "WindowApp")


using namespace hnrt;


static CoreLibrary theCore;


int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    Checksum app;
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
        MessageBoxW(NULL, e.Message, ResourceString(IDS_CAPTION), MB_OK | MB_ICONERROR);
    }
    catch (std::bad_alloc)
    {
        MessageBoxW(NULL, ResourceString(IDS_OUT_OF_MEMORY), ResourceString(IDS_CAPTION), MB_OK | MB_ICONERROR);
    }
    catch (...)
    {
        MessageBoxW(NULL, ResourceString(IDS_UNKNOWN_FAILURE), ResourceString(IDS_CAPTION), MB_OK | MB_ICONERROR);
    }
    return app.ExitCode;
}
