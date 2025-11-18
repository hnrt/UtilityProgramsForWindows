#pragma once

#include "hnrt/RefObj.h"
#include "hnrt/StringCommons.h"
#include "hnrt/Heap.h"

namespace hnrt
{
	template<typename T>
	class RefString
		: public RefObj
	{
    private:

        ULONG m_Length;

        RefString(SIZE_T);
        RefString(const T*, SIZE_T);

    public:

        RefString(const RefString&) = delete;
        virtual ~RefString();
        void Truncate(SIZE_T);

        void* operator new(::size_t, ::size_t);
        void operator delete(void*);
        void operator delete(void*, ::size_t);
        void operator =(const RefString&) = delete;

    public:

        SIZE_T get_Length() const;

        __declspec(property(get = get_Length)) SIZE_T Length;

    public:

        static RefString<T>& Get(T*);
        static T* Get(RefString<T>&);
        static T* Create(SIZE_T);
        static T* Create(const T*, SSIZE_T = -1);
    };

    template<typename T>
    RefString<T>::RefString(SIZE_T cch)
        : RefObj()
        , m_Length(static_cast<ULONG>(cch))
    {
    }

    template<typename T>
    RefString<T>::RefString(const T* psz, SIZE_T cch)
        : RefObj()
        , m_Length(static_cast<ULONG>(cch))
    {
        T* ptr = Get(*this);
        MemCpy(ptr, psz, cch);
        ptr[cch] = 0;
    }

    template<typename T>
    RefString<T>::~RefString()
    {
        MemSet(Get(*this), 0, m_Length);
    }

    template<typename T>
    void RefString<T>::Truncate(SIZE_T cch)
    {
        if (cch < static_cast<SIZE_T>(m_Length))
        {
            T* ptr = Get(*this);
            MemSet(ptr + cch, 0, static_cast<SIZE_T>(m_Length) - cch);
            m_Length = static_cast<ULONG>(cch);
        }
    }

    template<typename T>
    void* RefString<T>::operator new(::size_t size, ::size_t cch)
    {
        ::size_t required = sizeof(RefString<T>) + (cch + 1) * sizeof(T);
        return Malloc(required > size ? required : size);
    }

    template<typename T>
    void RefString<T>::operator delete(void* ptr)
    {
        free(ptr);
    }

    template<typename T>
    void RefString<T>::operator delete(void* ptr, ::size_t)
    {
        free(ptr);
    }

    template<typename T>
    ::SIZE_T RefString<T>::get_Length() const
    {
        return static_cast<::SIZE_T>(m_Length);
    }

    template<typename T>
    RefString<T>& RefString<T>::Get(T* psz)
    {
        return *(reinterpret_cast<RefString<T>*>(psz) - 1);
    }

    template<typename T>
    T* RefString<T>::Get(RefString<T>& str)
    {
        return reinterpret_cast<T*>(&str + 1);
    }

    template<typename T>
    T* RefString<T>::Create(SIZE_T cch)
    {
        return Get(*(new(cch) RefString<T>(cch)));
    }

    template<typename T>
    T* RefString<T>::Create(const T* psz, SSIZE_T cch)
    {
        if (cch)
        {
            cch = StrLen(psz, cch);
            return Get(*(new(cch) RefString<T>(psz, cch)));
        }
        else
        {
            return Get(*(new(0) RefString<T>(psz, 0)));
        }
    }

    template<typename T>
    T* StringAddRef(T* psz)
    {
        if (psz)
        {
            RefString<T>::Get(psz).AddRef();
        }
        return psz;
    }

    template<typename T>
    void StringRelease(T* psz)
    {
        if (psz)
        {
            RefString<T>::Get(psz).Release();
        }
    }
}
