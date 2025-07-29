#include "pch.h"
#include "hnrt/ZipArchive.h"


using namespace hnrt;


ZipArchive::~ZipArchive()
{
    m_pFolder->Release();
}
