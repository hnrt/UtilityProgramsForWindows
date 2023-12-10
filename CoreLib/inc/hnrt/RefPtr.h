#pragma once


namespace hnrt
{
    template<typename T>
    class RefPtr
    {
    public:

        RefPtr(T* = nullptr);
        RefPtr(const RefPtr<T>&);
        ~RefPtr();
        RefPtr<T>& operator =(T*);
        RefPtr<T>& operator =(const RefPtr<T>&);
        T** operator &();
        const T* get_Ptr() const;
        T* get_Ptr();
        const T* operator ->() const;
        T* operator ->();
        operator bool() const;

        __declspec(property(get = get_Ptr)) T* Ptr;

    private:

        T* m_ptr;

    public:

        static RefPtr<T> Cast(T* ptr);
    };

    template<typename T>
    inline RefPtr<T>::RefPtr(T* ptr)
        : m_ptr(ptr)
    {
    }

    template<typename T>
    inline RefPtr<T>::RefPtr(const RefPtr<T>& src)
        : m_ptr(src.m_ptr)
    {
        if (m_ptr)
        {
            m_ptr->AddRef();
        }
    }

    template<typename T>
    inline RefPtr<T>::~RefPtr()
    {
        if (m_ptr)
        {
            m_ptr->Release();
        }
    }

    template<typename T>
    inline RefPtr<T>& RefPtr<T>::operator =(T* ptr)
    {
        if (m_ptr)
        {
            m_ptr->Release();
        }
        m_ptr = ptr;
        return *this;
    }

    template<typename T>
    inline RefPtr<T>& RefPtr<T>::operator =(const RefPtr<T>& src)
    {
        if (m_ptr)
        {
            m_ptr->Release();
        }
        m_ptr = src.m_ptr;
        if (m_ptr)
        {
            m_ptr->AddRef();
        }
        return *this;
    }

    template<typename T>
    inline T** RefPtr<T>::operator &()
    {
        if (m_ptr)
        {
            m_ptr->Release();
            m_ptr = nullptr;
        }
        return &m_ptr;
    }

    template<typename T>
    inline const T* RefPtr<T>::get_Ptr() const
    {
        return m_ptr;
    }

    template<typename T>
    inline T* RefPtr<T>::get_Ptr()
    {
        return m_ptr;
    }

    template<typename T>
    inline const T* RefPtr<T>::operator ->() const
    {
        return m_ptr;
    }

    template<typename T>
    inline T* RefPtr<T>::operator ->()
    {
        return m_ptr;
    }

    template<typename T>
    inline RefPtr<T>::operator bool() const
    {
        return m_ptr != nullptr;
    }

    template<typename T>
    inline bool operator == (const RefPtr<T>& a, const RefPtr<T>& b)
    {
        return a.Ptr == b.Ptr;
    }

    template<typename T>
    inline bool operator < (const RefPtr<T>& a, const RefPtr<T>& b)
    {
        return a.Ptr < b.Ptr;
    }

    template<typename T>
    inline bool operator > (const RefPtr<T>& a, const RefPtr<T>& b)
    {
        return a.Ptr > b.Ptr;
    }

    template<typename T>
    inline RefPtr<T> RefPtr<T>::Cast(T* ptr)
    {
        if (ptr)
        {
            ptr->AddRef();
            return RefPtr<T>(ptr);
        }
        else
        {
            return RefPtr<T>();
        }
    }
}
