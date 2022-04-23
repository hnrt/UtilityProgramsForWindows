#include "pch.h"
#include <locale.h>
#include "Checksum.h"
#include "hnrt/Exception.h"
#include "hnrt/ResourceString.h"


using namespace hnrt;


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    Checksum app;
    try
    {
        _wsetlocale(LC_ALL, L"");
        app.Open(hInstance, lpCmdLine, nCmdShow);
        app.Run();
        app.Close(hInstance);
    }
    catch (Exception e)
    {
        MessageBoxW(NULL, e.Message, ResourceString(IDS_CAPTION), MB_OK | MB_ICONERROR);
    }
    catch (...)
    {
        MessageBoxW(NULL, ResourceString(IDS_UNKNOWN_FAILURE), ResourceString(IDS_CAPTION), MB_OK | MB_ICONERROR);
    }
    return app.ExitCode;
}
