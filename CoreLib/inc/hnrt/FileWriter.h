#pragma once


#include "hnrt/String.h"
#include "hnrt/WindowsHandle.h"


namespace hnrt
{
    class FileWriter
    {
    public:

        FileWriter(PCWSTR pszPath = nullptr, DWORD dwCreationDisposition = CREATE_ALWAYS);
        FileWriter(const FileWriter&) = delete;
        virtual ~FileWriter() = default;
        FileWriter& operator =(const FileWriter&) = delete;
        void Open(DWORD dwCreationDisposition = CREATE_ALWAYS);
        void Write(LPCVOID, SIZE_T);
        void Close();
        PCWSTR get_Path() const;
        void set_Path(PCWSTR pszPath);
        UINT64 get_ByteCount() const;

        __declspec(property(get = get_Path, put = set_Path)) PCWSTR Path;
        __declspec(property(get = get_ByteCount)) UINT64 ByteCount;

    protected:

        String m_Path;
        WindowsHandle m_h;
        UINT64 m_count;
    };

    inline void FileWriter::Close()
    {
        m_h = INVALID_HANDLE_VALUE;
        m_count = 0;
    }

    inline PCWSTR FileWriter::get_Path() const
    {
        return m_Path;
    }

    inline void FileWriter::set_Path(PCWSTR pszPath)
    {
        m_Path = pszPath;
    }

    inline UINT64 FileWriter::get_ByteCount() const
    {
        return m_count;
    }
}
