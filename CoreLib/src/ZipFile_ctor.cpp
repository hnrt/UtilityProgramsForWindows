#include "pch.h"
#include "hnrt/ZipFile.h"
#include "hnrt/ShellDispatchPtr.h"


using namespace hnrt;


ZipFile::ZipFile(const String& pszFileName, const FolderPtr& pFolder)
	: m_pszFileName(pszFileName)
	, m_pFolder(pFolder)
{
}


ZipFile::ZipFile(const ZipFile& src)
	: m_pszFileName(src.m_pszFileName)
	, m_pFolder(src.m_pFolder)
{
}


ZipFile& ZipFile::operator =(const ZipFile& src)
{
	m_pszFileName = src.m_pszFileName;
	m_pFolder = src.m_pFolder;
	return *this;
}


ZipFile ZipFile::Open(const String& pszFileName)
{
	ShellDispatchPtr pShellDispatch = ShellDispatchPtr::Create();
	FolderPtr pFolder = FolderPtr::Get(pszFileName, pShellDispatch);
	return ZipFile(pszFileName, pFolder);
}
