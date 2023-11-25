#include "pch.h"
#include "ClipFile.h"
#include "hnrt/FileWriter.h"
#include "hnrt/FileMapper.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/Exception.h"
#include "hnrt/Interlocked.h"
#include "hnrt/Unicode.h"


using namespace hnrt;


static const WCHAR Separator = LINE_SEPARATOR;


ClipFile::ClipFile(const String& szPath)
	: m_szPath(szPath)
	, m_pMapper(nullptr)
	, m_offsets()
{
}


ClipFile::~ClipFile()
{
	Close();
}


void ClipFile::Open() const
{
	Close();
	m_pMapper = new FileMapper(m_szPath);
	const WCHAR* pStart = reinterpret_cast<const WCHAR*>(m_pMapper->Ptr);
	const WCHAR* pEnd = pStart + m_pMapper->Len / sizeof(WCHAR);
	const WCHAR* pSep1 = wmemchr(pStart, Separator, pEnd - pStart);
	if (!pSep1)
	{
		throw Exception(L"ClipFile(%s) looks broken!", m_szPath.Ptr);
	}
	const WCHAR* pHash = pSep1 + 1;
	const WCHAR* pSep2 = wmemchr(pHash, Separator, pEnd - pHash);
	if (!pSep2)
	{
		throw Exception(L"ClipFile(%s) looks broken!", m_szPath.Ptr);
	}
	const WCHAR* pContent = pSep2 + 1;
	m_offsets[0] = pHash - pStart;
	m_offsets[1] = pContent - pStart;
}


void ClipFile::Close() const
{
	if (m_pMapper)
	{
		delete Interlocked<FileMapper*>::ExchangePointer(&m_pMapper, nullptr);
		memset(m_offsets, 0, sizeof(m_offsets));
	}
}


void ClipFile::Save(const String& szHash, const String& szContent) const
{
	FileWriter file(m_szPath, CREATE_NEW);
	file.Write(&Separator, sizeof(WCHAR));
	file.Write(szHash, szHash.Len * sizeof(WCHAR));
	file.Write(&Separator, sizeof(WCHAR));
	file.Write(szContent, szContent.Len * sizeof(WCHAR));
	file.Close();
}


void ClipFile::Save(const String& szHeader, const String& szHash, const String& szContent) const
{
	FileWriter file(m_szPath, CREATE_ALWAYS);
	if (wmemchr(szHeader, Separator, szHeader.Len))
	{
		StringBuffer header(szHeader);
		header.Replace(Separator, L' ');
		file.Write(header.Ptr, header.Len * sizeof(WCHAR));
	}
	else
	{
		file.Write(szHeader, szHeader.Len * sizeof(WCHAR));
	}
	file.Write(&Separator, sizeof(WCHAR));
	file.Write(szHash, szHash.Len * sizeof(WCHAR));
	file.Write(&Separator, sizeof(WCHAR));
	file.Write(szContent, szContent.Len * sizeof(WCHAR));
	file.Close();
}


String ClipFile::get_Header() const
{
	if (!m_pMapper)
	{
		Open();
	}
#pragma warning(disable:6011)
	return String(reinterpret_cast<const WCHAR*>(m_pMapper->Ptr), m_offsets[0] - 1);
#pragma warning(default:6011)
}


String ClipFile::get_Hash() const
{
	if (!m_pMapper)
	{
		Open();
	}
#pragma warning(disable:6011)
	return String(reinterpret_cast<const WCHAR*>(m_pMapper->Ptr) + m_offsets[0], m_offsets[1] - m_offsets[0] - 1);
#pragma warning(default:6011)
}


String ClipFile::get_Content() const
{
	if (!m_pMapper)
	{
		Open();
	}
#pragma warning(disable:6011)
	return String(reinterpret_cast<const WCHAR*>(m_pMapper->Ptr) + m_offsets[1], m_pMapper->Len - m_offsets[1]);
#pragma warning(default:6011)
}
