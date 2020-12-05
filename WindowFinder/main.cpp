#include "pch.h"
#include <locale.h>
#include "hnrt/Exception.h"
#include "WindowFinder.h"


#pragma comment(lib, "Core")


using namespace hnrt;


int wmain(int argc, wchar_t* argv[])
{
    try
    {
        _wsetlocale(LC_ALL, L"");
        WindowFinder app;
        app.ParseCommandLine(argc, argv);
        app.Run();
        return EXIT_SUCCESS;
    }
    catch (std::bad_alloc)
    {
        fwprintf(stderr, L"ERROR: Out of memory.\n");
    }
    catch (Exception e)
    {
        fwprintf(stderr, L"ERROR: %s\n", e.Message);
    }
    catch (...)
    {
        fwprintf(stderr, L"ERROR: Unhandled exception caught.\n");
    }
    return EXIT_FAILURE;
}
