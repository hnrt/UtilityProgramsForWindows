#include "pch.h"
#include <stdexcept>
#include "hnrt/CoreLibrary.h"
#include "hnrt/Debug.h"


using namespace hnrt;


CoreLibrary::CoreLibrary()
    : m_pnh0(nullptr)
    , m_NewMode0(-1)
{
    DBGFNC(L"CoreLibrary::ctor");
    m_pnh0 = _set_new_handler(OutOfMemory);
    m_NewMode0 = _set_new_mode(1);
}


CoreLibrary::~CoreLibrary()
{
    DBGFNC(L"CoreLibrary::dtor");
    _set_new_handler(m_pnh0);
    _set_new_mode(m_NewMode0);
}


int __cdecl CoreLibrary::OutOfMemory(size_t size)
{
    throw std::bad_alloc();
}
