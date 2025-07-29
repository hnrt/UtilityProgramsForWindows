#include "pch.h"
#include "hnrt/StringBuffer.h"


using namespace hnrt;


StringBuffer::~StringBuffer()
{
    free(m_ptr);
}
