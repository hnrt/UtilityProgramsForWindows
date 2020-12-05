#pragma once


#include <Windows.h>


namespace hnrt
{
    class FileMapper
    {
    public:

        FileMapper(PCWSTR pszPath = NULL);
        FileMapper(const FileMapper&) = delete;
        virtual ~FileMapper();
        FileMapper& operator =(const FileMapper&) = delete;
        void Open();
        void Close();
        PCWSTR get_Path() const;
        void set_Path(PCWSTR pszPath);
        LPCVOID get_Ptr() const;
        UINT64 get_Len() const;

        __declspec(property(get = get_Path, put = set_Path)) PCWSTR Path;
        __declspec(property(get = get_Ptr)) LPCVOID Ptr;
        __declspec(property(get = get_Len)) UINT64 Len;

    protected:

        PCWSTR m_pszPath;
        HANDLE m_hFile;
        HANDLE m_hFileMapping;
        LPVOID m_ptr;
        UINT64 m_len;
    };

    inline PCWSTR FileMapper::get_Path() const
    {
        return m_pszPath;
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
