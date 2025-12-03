#pragma once

#include <stdexcept>
#include <Windows.h>
#include "hnrt/RefArray.h"
#include "hnrt/Interlocked.h"

namespace hnrt
{
    template<typename T>
    class Array
    {
    protected:

        T* m_pBase;

    public:

        Array(SIZE_T initialCapacity = 0);
        Array(const Array<T>& src);
        ~Array();
        ULONG GetCapacity() const;
        void SetCapacity(ULONG uCapacity);
        ULONG GetLength() const;
        void SetLength(ULONG uLength);
        Array<T>& Assign(const Array<T>& src);
        Array<T>& PushBack(const T& value);
        Array<T>& PushBack(const Array<T>& src);
        const T& At(SSIZE_T index) const;
        T& At(SSIZE_T index);

        Array<T>& operator =(const Array<T>& src);
        Array<T>& operator +=(const T& value);
        Array<T>& operator +=(const Array<T>& src);
        const T& operator [](SSIZE_T index) const;
        T& operator [](SSIZE_T index);

        __declspec(property(get = GetCapacity, put = SetCapacity)) ULONG Capacity;
        __declspec(property(get = GetLength, put = SetLength)) ULONG Length;

    private:

        static ULONG newCapacity(ULONG x);
    };

    template<typename T>
    Array<T>::Array(SIZE_T initialCapacity)
        : m_pBase(RefArray<T>::Create(initialCapacity))
    {
    }

    template<typename T>
    Array<T>::Array(const Array<T>& src)
        : m_pBase(ArrayAddRef<T>(src.m_pBase))
    {
    }

    template<typename T>
    Array<T>::~Array()
    {
        ArrayRelease<T>(Interlocked<T*>::ExchangePointer(&m_pBase, nullptr));
    }

    template<typename T>
    ULONG Array<T>::GetCapacity() const
    {
        return m_pBase ? static_cast<ULONG>(RefArray<T>::Get(m_pBase).Capacity) : 0;
    }

    template<typename T>
    void Array<T>::SetCapacity(ULONG uCapacity)
    {
        Interlocked<T*>::ExchangePointer(&m_pBase, RefArray<T>::Resize(m_pBase, uCapacity));
    }

    template<typename T>
    ULONG Array<T>::GetLength() const
    {
        return m_pBase ? static_cast<ULONG>(RefArray<T>::Get(m_pBase).Length) : 0;
    }

    template<typename T>
    void Array<T>::SetLength(ULONG uLength)
    {
        if (Capacity < uLength)
        {
            SetCapacity(newCapacity(uLength));
        }
        if (m_pBase)
        {
            RefArray<T>::Get(m_pBase).Length = uLength;
        }
    }

    template<typename T>
    Array<T>& Array<T>::Assign(const Array<T>& src)
    {
        ArrayRelease<T>(Interlocked<T*>::ExchangePointer(&m_pBase, ArrayAddRef<T>(src.m_pBase)));
        return *this;
    }

    template<typename T>
    Array<T>& Array<T>::PushBack(const T& value)
    {
        ULONG length = Length;
        ULONG capacity = Capacity;
        if (length == capacity)
        {
            SetCapacity(newCapacity(capacity));
        }
        RefArray<T>::Get(m_pBase).PushBack(value);
        return *this;
    }

    template<typename T>
    Array<T>& Array<T>::PushBack(const Array<T>& src)
    {
        ULONG delta = src.Length;
        if (delta)
        {
            ULONG required = Length + delta;
            if (Capacity < required)
            {
                SetCapacity(newCapacity(required));
            }
            RefArray<T>::Get(m_pBase).PushBack(RefArray<T>::Get(src.m_pBase));
        }
        return *this;
    }

    template<typename T>
    const T& Array<T>::At(SSIZE_T index) const
    {
        SSIZE_T length = static_cast<SSIZE_T>(Length);
        if (0 <= index && index < length)
        {
            return m_pBase[index];
        }
        else if (0 <= length + index && index < 0)
        {
            return m_pBase[length + index];
        }
        else
        {
            throw std::runtime_error("Array<T>::At: Index out of range.");
        }
    }

    template<typename T>
    T& Array<T>::At(SSIZE_T index)
    {
        SSIZE_T length = static_cast<SSIZE_T>(Length);
        if (0 <= index && index < length)
        {
            return m_pBase[index];
        }
        else if (0 <= length + index && index < 0)
        {
            return m_pBase[length + index];
        }
        else
        {
            throw std::runtime_error("Array<T>::At: Index out of range.");
        }
    }

    template<typename T>
    Array<T>& Array<T>::operator =(const Array<T>& src)
    {
        return Assign(src);
    }

    template<typename T>
    Array<T>& Array<T>::operator +=(const T& value)
    {
        return PushBack(value);
    }

    template<typename T>
    Array<T>& Array<T>::operator +=(const Array<T>& src)
    {
        return PushBack(src);
    }

    template<typename T>
    const T& Array<T>::operator [](SSIZE_T index) const
    {
        return At(index);
    }

    template<typename T>
    T& Array<T>::operator [](SSIZE_T index)
    {
        return At(index);
    }

    template<typename T>
    ULONG Array<T>::newCapacity(ULONG x)
    {
        // next power of 2
        if ((x & (x - 1)) != 0)
        {
            x--;
            x |= x >> 1;
            x |= x >> 2;
            x |= x >> 4;
            x |= x >> 8;
            x |= x >> 16;
            x++;
        }
        x = x < 16 ? 32 : x < 65536 ? x * 2 : x + 65536;
        return x;
    }
}
