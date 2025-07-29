#include "pch.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/String.h"
#include "hnrt/Heap.h"


using namespace hnrt;


String StringBuffer::Detach()
{
    String str(m_ptr ? m_ptr : L"");
    Deallocate(m_ptr);
    m_cap = 0;
    m_len = 0;
    return str;
}
