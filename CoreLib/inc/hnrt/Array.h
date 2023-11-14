#pragma once

#include "hnrt/Heap.h"
#include <stdexcept>

namespace hnrt
{
    template<typename T>
    class Array
    {
    public:

        Array(size_t size = 0);
        Array(const Array<T>& src);
        ~Array();
        Array<T>& operator =(const Array<T>& src);
        const T& operator [](LONG index) const;
        T& operator [](LONG index);
        T& operator +=(const T& src);
        Array<T>& Resize(size_t size);

        DWORD get_Count() const;

        __declspec(property(get = get_Count)) DWORD Count;

    protected:

        T* m_pBase;
        DWORD m_dwSize;
        DWORD m_dwCount;
    };

    template<typename T>
    inline Array<T>::Array(size_t size)
        : m_pBase(nullptr)
        , m_dwSize(0)
        , m_dwCount(0)
    {
        if (size)
        {
            m_pBase = Allocate<T>(size);
            m_dwSize = static_cast<DWORD>(size);
        }
    }

    template<typename T>
    inline Array<T>::Array(const Array<T>& src)
        : m_pBase(nullptr)
        , m_dwSize(0)
        , m_dwCount(0)
    {
        if (src.m_dwSize)
        {
            m_pBase = Allocate<T>(src.m_dwSize);
            m_dwSize = src.m_dwSize;
            for (DWORD dwIndex = 0; dwIndex < src.m_dwCount; dwIndex++)
            {
                new (&m_pBase[dwIndex]) T(src.m_pBase[dwIndex]);
            }
            m_dwCount = src.m_dwCount;
        }
    }

    template<typename T>
    inline Array<T>::~Array()
    {
        for (DWORD dwIndex = 0; dwIndex < m_dwCount; dwIndex++)
        {
            m_pBase[dwIndex].~T();
        }
        free(m_pBase);
    }

    template<typename T>
    inline Array<T>& Array<T>::operator =(const Array<T>& src)
    {
        for (DWORD dwIndex = 0; dwIndex < m_dwCount; dwIndex++)
        {
            m_pBase[dwIndex].~T();
        }
        free(m_pBase);
        if (src.m_dwSize)
        {
            m_pBase = Allocate<T>(src.m_dwSize);
            m_dwSize = src.m_dwSize;
            for (DWORD dwIndex = 0; dwIndex < src.m_dwCount; dwIndex++)
            {
                new (&m_pBase[dwIndex]) T(src.m_pBase[dwIndex]);
            }
            m_dwCount = src.m_dwCount;
        }
        else
        {
            m_pBase = nullptr;
            m_dwSize = 0;
            m_dwCount = 0;
        }
        return *this;
    }

    template<typename T>
    inline const T& Array<T>::operator [](LONG index) const
    {
        if (0 <= index && index < static_cast<LONG>(m_dwCount))
        {
            return m_pBase[index];
        }
        else if (0 <= static_cast<LONG>(m_dwCount) + index && index < 0)
        {
            return m_pBase[m_dwCount + index];
        }
        else
        {
            throw std::runtime_error("Array<T>::operator[]: Index out of range.");
        }
    }

    template<typename T>
    inline T& Array<T>::operator [](LONG index)
    {
        if (0 <= index && index < static_cast<LONG>(m_dwCount))
        {
            return m_pBase[index];
        }
        else if (0 <= static_cast<LONG>(m_dwCount) + index && index < 0)
        {
            return m_pBase[m_dwCount + index];
        }
        else
        {
            throw std::runtime_error("Array<T>::operator[]: Index out of range.");
        }
    }

    template<typename T>
    inline T& Array<T>::operator +=(const T& src)
    {
        if (m_dwCount + 1 > m_dwSize)
        {
            DWORD dwSize = m_dwSize < 16 ? 32 : m_dwSize < 65536 ? (m_dwSize * 2) : (m_dwSize + 65536);
            Resize(dwSize);
        }
        new (&m_pBase[m_dwCount++]) T(src);
        return m_pBase[m_dwCount - 1];
    }

    template<typename T>
    inline Array<T>& Array<T>::Resize(size_t size)
    {
        DWORD dwSize = static_cast<DWORD>(size);
        if (m_dwSize < dwSize)
        {
            m_pBase = Allocate<T>(m_pBase, dwSize);
            m_dwSize = dwSize;
        }
        else if (dwSize < m_dwSize)
        {
            while (dwSize < m_dwCount)
            {
                m_pBase[--m_dwCount].~T();
            }
            if (dwSize)
            {
                m_pBase = Allocate<T>(m_pBase, dwSize);
                m_dwSize = dwSize;
            }
            else
            {
                m_pBase = nullptr;
                m_dwSize = 0;
            }
        }
        return *this;
    }

    template<typename T>
    inline DWORD Array<T>::get_Count() const
    {
        return m_dwCount;
    }
}
