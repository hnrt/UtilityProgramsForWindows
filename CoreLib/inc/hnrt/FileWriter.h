#pragma once


#include "hnrt/String.h"
#include "hnrt/WindowsHandle.h"


namespace hnrt
{
    class FileWriter
    {
    public:

        FileWriter(const String& szPath = String::Empty, DWORD dwCreationDisposition = CREATE_ALWAYS);
        FileWriter(const FileWriter&) = delete;
        virtual ~FileWriter() = default;
        FileWriter& operator =(const FileWriter&) = delete;
        void Open(DWORD dwCreationDisposition = CREATE_ALWAYS);
        void Write(LPCVOID, SIZE_T);
        void Close();
        const String& get_Path() const;
        void set_Path(const String& szPath);
        UINT64 get_ByteCount() const;

        __declspec(property(get = get_Path, put = set_Path)) const String& Path;
        __declspec(property(get = get_ByteCount)) UINT64 ByteCount;

    protected:

        String m_szPath;
        WindowsHandle m_h;
        UINT64 m_count;
    };

    inline void FileWriter::Close()
    {
        m_h = INVALID_HANDLE_VALUE;
        m_count = 0;
    }

    inline const String& FileWriter::get_Path() const
    {
        return m_szPath;
    }

    inline void FileWriter::set_Path(const String& szPath)
    {
        m_szPath = szPath;
    }

    inline UINT64 FileWriter::get_ByteCount() const
    {
        return m_count;
    }
}
