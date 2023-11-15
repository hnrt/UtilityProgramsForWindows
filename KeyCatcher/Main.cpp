#include <Windows.h>
#include <stdlib.h>
#include <stdexcept>
#include "KeyCatcher.h"
#include "hnrt/ResourceString.h"
#include "hnrt/Args.h"
#include "hnrt/String.h"
#include "hnrt/Exception.h"
#include "resource.h"


#pragma comment(lib, "Core")


using namespace hnrt;


int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    try
    {
        ResourceString::m_hInstance = hInstance;
        int argc = 0;
        PWSTR* argv = CommandLineToArgvW(lpCmdLine, &argc);
        KeyCatcher app(hInstance);
        Args a(argc, argv, 0);
        while (a)
        {
            if (!wcscmp(*a, L"-h") || !wcscmp(*a, L"-?"))
            {

                MessageBoxW(NULL, String::Format2(ResourceString(IDS_FMT_USAGE), app.Name), ResourceString(IDS_CAPTION), MB_OK | MB_ICONINFORMATION);
                return EXIT_SUCCESS;
            }
            else if (!app.FileName)
            {
                app.FileName = *a;
                a++;
            }
            else
            {
                throw Exception(ResourceString(IDS_BADCMDLINE));
            }
        }
        app.Initialize(nCmdShow);
        return app.Run();
    }
    catch (std::bad_alloc)
    {
        MessageBoxW(NULL, ResourceString(IDS_OUTOFMEMORY), ResourceString(IDS_CAPTION), MB_OK | MB_ICONERROR);
    }
    catch (Exception e)
    {
        MessageBoxW(NULL, e.Message, ResourceString(IDS_CAPTION), MB_OK | MB_ICONERROR);
    }
    catch (...)
    {
        MessageBoxW(NULL, ResourceString(IDS_UNHANDLEDEXCEPTION), ResourceString(IDS_CAPTION), MB_OK | MB_ICONERROR);
    }
    return EXIT_FAILURE;
}
