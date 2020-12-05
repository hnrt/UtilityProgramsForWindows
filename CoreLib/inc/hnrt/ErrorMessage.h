#pragma once


namespace hnrt
{
    struct ErrorMessage
    {
        static PCWSTR Get(unsigned long code);
    };
}
