#pragma once


namespace hnrt
{
    template<typename T>
    class Buffer
    {
    public:

        Buffer(size_t len = 0);
        Buffer(const Buffer&) = delete;
        ~Buffer();
        Buffer& operator =(const Buffer&) = delete;
        operator const T* () const;
        operator T* ();
        T* operator &();
        void Resize(size_t len);
        T* get_Ptr() const;
        T* get_Ptr();
        size_t get_Len() const;

        __declspec(property(get = get_Ptr)) T* Ptr;
        __declspec(property(get = get_Len)) size_t Len;

    protected:

        T* m_ptr;
        size_t m_len;
    };

    template<typename T>
    inline Buffer<T>::Buffer(size_t len)
        : m_ptr(len ? new T[len] : nullptr)
        , m_len(len)
    {
    }

    template<typename T>
    inline Buffer<T>::~Buffer()
    {
        delete[] m_ptr;
    }

    template<typename T>
    inline Buffer<T>::operator const T* () const
    {
        return m_ptr;
    }

    template<typename T>
    inline Buffer<T>::operator T* ()
    {
        return m_ptr;
    }

    template<typename T>
    inline T* Buffer<T>::operator &()
    {
        return m_ptr;
    }

    template<typename T>
    inline void Buffer<T>::Resize(size_t len)
    {
        delete[] m_ptr;
        if (len)
        {
            m_ptr = new T[len];
            m_len = len;
        }
        else
        {
            m_ptr = nullptr;
            m_len = 0;
        }
    }

    template<typename T>
    inline T* Buffer<T>::get_Ptr() const
    {
        return m_ptr;
    }

    template<typename T>
    inline T* Buffer<T>::get_Ptr()
    {
        return m_ptr;
    }

    template<typename T>
    inline size_t Buffer<T>::get_Len() const
    {
        return m_len;
    }
}
