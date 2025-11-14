#pragma once

#include "hnrt/Heap.h"
#include <stdexcept>

namespace hnrt
{
    template<typename T>
    class Array
    {
    public:

        Array(SIZE_T capacity = 0);
        Array(const Array<T>& src);
        ~Array();
        Array<T>& SetCapacity(SIZE_T capacity);
        Array<T>& SetLength(SIZE_T length);
        Array<T>& Assign(const Array<T>& src);
        Array<T>& PushBack(const T& value);
        const T& At(SSIZE_T index) const;
        T& At(SSIZE_T index);
        Array<T>& operator =(const Array<T>& src);
        Array<T>& operator +=(const T& value);
        const T& operator [](SSIZE_T index) const;
        T& operator [](SSIZE_T index);

        DWORD get_Length() const;
        void set_Length(DWORD);

        __declspec(property(get = get_Length, put = set_Length)) DWORD Length;

    protected:

        T* m_pBase;
        DWORD m_dwCapacity;
        DWORD m_dwLength;
    };

    template<typename T>
    inline Array<T>::Array(SIZE_T capacity)
        : m_pBase(nullptr)
        , m_dwCapacity(0)
        , m_dwLength(0)
    {
        SetCapacity(capacity);
    }

    template<typename T>
    inline Array<T>::Array(const Array<T>& src)
        : m_pBase(nullptr)
        , m_dwCapacity(0)
        , m_dwLength(0)
    {
        Assign(src);
    }

    template<typename T>
    inline Array<T>::~Array()
    {
        SetCapacity(0);
    }

    template<typename T>
    inline Array<T>& Array<T>::SetCapacity(SIZE_T capacity)
    {
        DWORD dwCapacity = static_cast<DWORD>(capacity);
        if (m_dwCapacity < dwCapacity)
        {
            m_pBase = Allocate<T>(m_pBase, dwCapacity);
            m_dwCapacity = dwCapacity;
        }
        else if (dwCapacity < m_dwCapacity)
        {
            while (dwCapacity < m_dwLength)
            {
                m_pBase[--m_dwLength].~T();
            }
            if (dwCapacity)
            {
                m_pBase = Allocate<T>(m_pBase, dwCapacity);
                m_dwCapacity = dwCapacity;
            }
            else
            {
                free(m_pBase);
                m_pBase = nullptr;
                m_dwCapacity = 0;
            }
        }
        return *this;
    }

    template<typename T>
    inline Array<T>& Array<T>::SetLength(SIZE_T length)
    {
        DWORD dwLength = static_cast<DWORD>(length);
        if (m_dwCapacity < dwLength)
        {
            SetCapacity(dwLength);
        }
        if (m_dwLength < dwLength)
        {
            do
            {
                new (&m_pBase[m_dwLength++]) T();
            }
            while (m_dwLength < dwLength);
        }
        else
        {
            while (dwLength < m_dwLength)
            {
                m_pBase[--m_dwLength].~T();
            }
        }
        return *this;
    }

    template<typename T>
    inline Array<T>& Array<T>::Assign(const Array<T>& src)
    {
        SetCapacity(0);
        if (src.m_dwCapacity)
        {
            m_pBase = Allocate<T>(src.m_dwCapacity);
            m_dwCapacity = src.m_dwCapacity;
            for (DWORD dwIndex = 0; dwIndex < src.m_dwLength; dwIndex++)
            {
                new (&m_pBase[dwIndex]) T(src.m_pBase[dwIndex]);
            }
            m_dwLength = src.m_dwLength;
        }
        return *this;
    }

    template<typename T>
    inline Array<T>& Array<T>::PushBack(const T& value)
    {
        if (m_dwCapacity < m_dwLength + 1)
        {
            DWORD dwCapacity = m_dwCapacity < 16 ? 32 : m_dwCapacity < 65536 ? (m_dwCapacity * 2) : (m_dwCapacity + 65536);
            SetCapacity(dwCapacity);
        }
        new (&m_pBase[m_dwLength++]) T(value);
        return *this;
    }

    template<typename T>
    inline const T& Array<T>::At(SSIZE_T index) const
    {
        if (0 <= index && index < static_cast<SSIZE_T>(m_dwLength))
        {
            return m_pBase[index];
        }
        else if (0 <= static_cast<SSIZE_T>(m_dwLength) + index && index < 0)
        {
            return m_pBase[m_dwLength + index];
        }
        else
        {
            throw std::runtime_error("Array<T>::At: Index out of range.");
        }
    }

    template<typename T>
    inline T& Array<T>::At(SSIZE_T index)
    {
        if (0 <= index && index < static_cast<SSIZE_T>(m_dwLength))
        {
            return m_pBase[index];
        }
        else if (0 <= static_cast<SSIZE_T>(m_dwLength) + index && index < 0)
        {
            return m_pBase[m_dwLength + index];
        }
        else
        {
            throw std::runtime_error("Array<T>::At: Index out of range.");
        }
    }

    template<typename T>
    inline Array<T>& Array<T>::operator =(const Array<T>& src)
    {
        return Assign(src);
    }

    template<typename T>
    inline Array<T>& Array<T>::operator +=(const T& value)
    {
        return PushBack(value);
    }

    template<typename T>
    inline const T& Array<T>::operator [](SSIZE_T index) const
    {
        return At(index);
    }

    template<typename T>
    inline T& Array<T>::operator [](SSIZE_T index)
    {
        return At(index);
    }

    template<typename T>
    inline DWORD Array<T>::get_Length() const
    {
        return m_dwLength;
    }

    template<typename T>
    inline void Array<T>::set_Length(DWORD dwLength)
    {
        SetLength(dwLength);
    }
}
