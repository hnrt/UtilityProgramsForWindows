#include "pch.h"
#include "hnrt/ZipArchive.h"


using namespace hnrt;


ZipArchive::ZipArchive(const String& szFileName, Folder* pFolder)
    : m_szFileName(szFileName)
    , m_pFolder(pFolder)
{
}


ZipArchive::ZipArchive(const ZipArchive& src)
    : m_szFileName(src.m_szFileName)
    , m_pFolder(src.m_pFolder)
{
    m_pFolder->AddRef();
}
