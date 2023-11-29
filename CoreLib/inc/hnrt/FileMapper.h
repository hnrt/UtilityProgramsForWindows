#pragma once


#include "hnrt/String.h"
#include "hnrt/WindowsHandle.h"


namespace hnrt
{
    class FileMapper
    {
    public:

        FileMapper(const String& szPath = String::Empty);
        FileMapper(const FileMapper&) = delete;
        virtual ~FileMapper();
        FileMapper& operator =(const FileMapper&) = delete;
        void Open();
        void Close();
        const String& get_Path() const;
        void set_Path(const String& szPath);
        LPCVOID get_Ptr() const;
        UINT64 get_Len() const;

        __declspec(property(get = get_Path, put = set_Path)) const String& Path;
        __declspec(property(get = get_Ptr)) LPCVOID Ptr;
        __declspec(property(get = get_Len)) UINT64 Len;

    protected:

        String m_szPath;
        WindowsHandle m_hFile;
        WindowsHandle m_hFileMapping;
        LPVOID m_ptr;
        UINT64 m_len;
    };

    inline const String& FileMapper::get_Path() const
    {
        return m_szPath;
    }

    inline void FileMapper::set_Path(const String& szPath)
    {
        m_szPath = szPath;
    }

    inline LPCVOID FileMapper::get_Ptr() const
    {
        return m_ptr;
    }

    inline UINT64 FileMapper::get_Len() const
    {
        return m_len;
    }
}
