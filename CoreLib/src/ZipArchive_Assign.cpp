#include "pch.h"
#include "hnrt/ZipArchive.h"


using namespace hnrt;


ZipArchive& ZipArchive::Assign(const ZipArchive& src)
{
    m_szFileName = src.m_szFileName;
    if (m_pFolder)
    {
        m_pFolder->Release();
    }
    m_pFolder = src.m_pFolder;
    m_pFolder->AddRef();
    return *this;
}
