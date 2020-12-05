#pragma once


#include "hnrt/StringBuffer.h"


namespace hnrt
{
    class NumberText
        : public StringBuffer
    {
    public:

        NumberText(int);
        NumberText(unsigned int);
        NumberText(short);
        NumberText(unsigned short);
        NumberText(long);
        NumberText(unsigned long);
        NumberText(long long);
        NumberText(unsigned long long);
        NumberText(const NumberText&) = delete;
    };
}
