#pragma once


#include "hnrt/String.h"
#include "hnrt/Time.h"


namespace hnrt
{
    struct DirectoryEntry
    {
        DWORD dwFileAttributes;
        FileTime ftCreationTime;
        FileTime ftLastAccessTime;
        FileTime ftLastWriteTime;
        ULONGLONG nFileSize;
        String szFileName;

        DirectoryEntry(const WIN32_FIND_DATAW&);
        DirectoryEntry(const DirectoryEntry&);
        ~DirectoryEntry() = default;
        DirectoryEntry& operator =(const DirectoryEntry&);
    };

    inline DirectoryEntry::DirectoryEntry(const WIN32_FIND_DATAW& fd)
        : dwFileAttributes(fd.dwFileAttributes)
        , ftCreationTime(fd.ftCreationTime)
        , ftLastAccessTime(fd.ftLastAccessTime)
        , ftLastWriteTime(fd.ftLastWriteTime)
        , nFileSize((static_cast<ULONGLONG>(fd.nFileSizeHigh) << 32) + static_cast<ULONGLONG>(fd.nFileSizeLow))
        , szFileName(fd.cFileName)
    {
    }

    inline DirectoryEntry::DirectoryEntry(const DirectoryEntry& other)
        : dwFileAttributes(other.dwFileAttributes)
        , ftCreationTime(other.ftCreationTime)
        , ftLastAccessTime(other.ftLastAccessTime)
        , ftLastWriteTime(other.ftLastWriteTime)
        , nFileSize(other.nFileSize)
        , szFileName(other.szFileName)
    {
    }

    inline DirectoryEntry& DirectoryEntry::operator =(const DirectoryEntry& other)
    {
        dwFileAttributes = other.dwFileAttributes;
        ftCreationTime = other.ftCreationTime;
        ftLastAccessTime = other.ftLastAccessTime;
        ftLastWriteTime = other.ftLastWriteTime;
        nFileSize = other.nFileSize;
        szFileName = other.szFileName;
        return *this;
    }
}
