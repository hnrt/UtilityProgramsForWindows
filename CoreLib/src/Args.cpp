#include "pch.h"
#include "hnrt/Args.h"


using namespace hnrt;


Args::Args(int argc, PWSTR argv[], int startIndex)
    : m_argv(argv)
    , m_argc(argc)
    , m_index(startIndex)
{
}
