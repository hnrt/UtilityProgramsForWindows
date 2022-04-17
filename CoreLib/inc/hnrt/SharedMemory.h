#pragma once


#include "hnrt/WindowsHandle.h"
#include "hnrt/Interlocked.h"


namespace hnrt
{
    template<typename T>
    class SharedMemory
    {
    public:

        SharedMemory();
        SharedMemory(const SharedMemory&) = delete;
        ~SharedMemory();
        void operator =(const SharedMemory&) = delete;
        const T* operator -> () const;
        T* operator -> ();
        DWORD Map(PCWSTR);
        DWORD Unmap();

    private:

        WindowsHandle m_h;
        T* m_p;
    };

    template<typename T>
    SharedMemory<T>::SharedMemory()
        : m_h()
        , m_p(nullptr)
    {
    }

    template<typename T>
    SharedMemory<T>::~SharedMemory()
    {
        if (m_p)
        {
            UnmapViewOfFile(m_p);
        }
    }

    template<typename T>
    const T* SharedMemory<T>::operator -> () const
    {
        return m_p;
    }

    template<typename T>
    T* SharedMemory<T>::operator -> ()
    {
        return m_p;
    }

    template<typename T>
    DWORD SharedMemory<T>::Map(PCWSTR pszName)
    {
        Unmap();
        DWORD dwRet;
        LARGE_INTEGER size = { 0, 0 }; // to stop a bit silly lint warning; the compile initializes only the first named member...
        size.QuadPart = sizeof(T);
        m_h = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, size.HighPart, size.LowPart, pszName);
        if (m_h)
        {
            m_p = reinterpret_cast<T*>(MapViewOfFile(m_h, FILE_MAP_ALL_ACCESS, 0, 0, 0));
            if (m_p)
            {
                dwRet = ERROR_SUCCESS;
            }
            else
            {
                dwRet = GetLastError();
                m_h.Close();
            }
        }
        else
        {
            dwRet = GetLastError();
        }
        return dwRet;
    }

    template<typename T>
    DWORD SharedMemory<T>::Unmap()
    {
        DWORD dwRet = ERROR_SUCCESS;
        if (m_p)
        {
            PVOID p = Interlocked<T*>::ExchangePointer(&m_p, nullptr);
            if (UnmapViewOfFile(p))
            {
                dwRet = ERROR_SUCCESS;
            }
            else
            {
                dwRet = GetLastError();
            }
            m_h.Close();
        }
        return dwRet;
    }
}
