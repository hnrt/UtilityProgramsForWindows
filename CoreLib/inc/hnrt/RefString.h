#pragma once

#include "hnrt/RefObj.h"
#include "hnrt/StringCommons.h"
#include "hnrt/StringOptions.h"
#include "hnrt/Heap.h"

namespace hnrt
{
	template<typename T>
	class RefString
		: public RefObj
	{
	public:


        void* operator new(::size_t, ::size_t);
        void operator delete(void*);
        void operator delete(void*, ::size_t);
        RefString(const RefString&) = delete;
        virtual ~RefString();
        void operator =(const RefString&) = delete;
        void SetLength(SSIZE_T = -1);
        void Fill(T);
        SIZE_T get_len() const;

        __declspec(property(get = get_len)) SIZE_T Len;

    private:

        RefString(SIZE_T);
        RefString(SIZE_T, T);
        RefString(const T*, SIZE_T);
        RefString(const T*, ::va_list, SIZE_T);
        RefString(const T*, SIZE_T, const T*, SIZE_T);
        RefString(const T*, SIZE_T, const T*, ::va_list, SIZE_T);

        SIZE_T m_len;
        T m_buf[1];

    public:

        static RefString<T>* GetThis(T*);
        static T* GetString(RefString<T>*);
        static T* Create(SIZE_T);
        static T* Create(SIZE_T, T);
        static T* Create(const T*, SSIZE_T = -1);
        static T* Create(const T*, ::va_list);
        static T* Create(const T*, const T*, SSIZE_T = -1);
        static T* Create(const T*, const T*, ::va_list);
        static T* Create(StringOptions, const T*);
        static T* Create(StringOptions, const T*, ::va_list);
    };

    template<typename T>
    void* RefString<T>::operator new(::size_t size, ::size_t cch)
    {
        static const ::ptrdiff_t offset = (reinterpret_cast<char*>(&reinterpret_cast<RefString<T>*>(0)->m_buf[0]) - reinterpret_cast<char*>(reinterpret_cast<RefString<T>*>(0)));
        ::size_t required = offset + (cch + 1) * sizeof(T);
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
    RefString<T>::RefString(SIZE_T cch)
        : RefObj()
        , m_len(cch)
        , m_buf()
    {
    }

    template<typename T>
    RefString<T>::RefString(SIZE_T cch, T fill)
        : RefObj()
        , m_len(cch)
        , m_buf()
    {
        m_buf[StrFill(&m_buf[0], fill, m_len)] = (T)0;
    }

    template<typename T>
    RefString<T>::RefString(const T* psz, SIZE_T cch)
        : RefObj()
        , m_len(cch)
        , m_buf()
    {
        (void)StrCopy(&m_buf[0], psz, m_len);
    }

    template<typename T>
    RefString<T>::RefString(const T* pszFormat, ::va_list argList, SIZE_T cch)
        : RefObj()
        , m_len(cch)
        , m_buf()
    {
        (void)VaFormat(&m_buf[0], m_len + 1, pszFormat, argList);
    }

    template<typename T>
    RefString<T>::RefString(const T* psz1, SIZE_T cch1, const T* psz2, SIZE_T cch2)
        : RefObj()
        , m_len(cch1 + cch2)
        , m_buf()
    {
        (void)StrCopy(&m_buf[0], psz1, cch1);
        (void)StrCopy(&m_buf[cch1], psz2, cch2);
    }

    template<typename T>
    RefString<T>::RefString(const T* psz1, SIZE_T cch1, const T* pszFormat, ::va_list argList, SIZE_T cch2)
        : RefObj()
        , m_len(cch1 + cch2)
        , m_buf()
    {
        (void)StrCopy(&m_buf[0], psz1, cch1);
        (void)VaFormat(&m_buf[cch1], cch2 + 1, pszFormat, argList);
    }

    template<typename T>
    RefString<T>::~RefString()
    {
        (void)StrFill(&m_buf[0], (T)0, m_len);
    }

    template<typename T>
    void RefString<T>::SetLength(SSIZE_T cch)
    {
        m_len = StrLen(&m_buf[0], cch < 0 ? m_len : (cch < static_cast<SSIZE_T>(m_len) ? cch : m_len));
        m_buf[m_len] = (T)0;
    }

    template<typename T>
    void RefString<T>::Fill(T c)
    {
        StrFill(&m_buf[0], c, m_len);
        if (c)
        {
            m_buf[m_len] = (T)0;
        }
        else
        {
            m_len = 0;
        }
    }

    template<typename T>
    ::SIZE_T RefString<T>::get_len() const
    {
        return m_len;
    }

    template<typename T>
    RefString<T>* RefString<T>::GetThis(T* psz)
    {
        static const ::ptrdiff_t offset = (reinterpret_cast<char*>(&reinterpret_cast<RefString<T>*>(0)->m_buf[0]) - reinterpret_cast<char*>(reinterpret_cast<RefString<T>*>(0)));
        return reinterpret_cast<RefString<T>*>(reinterpret_cast<char*>(psz) - offset);
    }

    template<typename T>
    T* RefString<T>::GetString(RefString<T>* ptr)
    {
        return &ptr->m_buf[0];
    }

    template<typename T>
    T* RefString<T>::Create(SIZE_T cch)
    {
        return GetString(new(cch) RefString<T>(cch));
    }

    template<typename T>
    T* RefString<T>::Create(SIZE_T cch, T c)
    {
        return GetString(new(cch) RefString<T>(cch, c));
    }

    template<typename T>
    T* RefString<T>::Create(const T* psz, SSIZE_T cch)
    {
        if (psz && cch)
        {
            cch = StrLen(psz, cch);
            return GetString(new(cch) RefString<T>(psz, cch));
        }
        else
        {
            T sz[1] = { (T)0 };
            return GetString(new(0) RefString<T>(sz, 0));
        }
    }

    template<typename T>
    T* RefString<T>::Create(const T* psz, ::va_list argList)
    {
        SIZE_T cch = VaFormatLength(psz, argList);
        return GetString(new(cch) RefString<T>(psz, argList, cch));
    }

    template<typename T>
    T* RefString<T>::Create(const T* psz1, const T* psz2, SSIZE_T cch)
    {
        SIZE_T cch1 = StrLen(psz1);
        SIZE_T cch2 = StrLen(psz2, cch);
        return GetString(new(cch1 + cch2) RefString<T>(psz1, cch1, psz2, cch2));
    }

    template<typename T>
    T* RefString<T>::Create(const T* psz1, const T* psz2, ::va_list argList)
    {
        SIZE_T cch1 = StrLen(psz1);
        SIZE_T cch2 = VaFormatLength(psz2, argList);
        return GetString(new(cch1 + cch2) RefString<T>(psz1, cch1, psz2, argList, cch2));
    }

    template<typename T>
    T* RefString<T>::Create(StringOptions option, const T* psz)
    {
        switch (option)
        {
        case UPPERCASE:
        case LOWERCASE:
        {
            SIZE_T cch = StrLen(psz);
            T* pszReturn = GetString(new(cch) RefString<T>(psz, cch));
            StrCase(option, pszReturn, cch);
            return pszReturn;
        }
        case TRIM:
        case TRIM_HEAD:
        case TRIM_TAIL:
        {
            int start = 0;
            int end = 0;
            StrTrimScan(psz, start, end, option);
            int cch = end - start;
            return GetString(new(cch) RefString<T>(psz + start, cch));
        }
        default:
            return nullptr;
        }
    }

    template<typename T>
    T* RefString<T>::Create(StringOptions option, const T* psz, ::va_list argList)
    {
        switch (option)
        {
        case CONCAT:
        case CONCAT2:
        case CONCAT3:
        case CONCAT4:
        case CONCAT5:
        case CONCAT6:
        case CONCAT7:
        case CONCAT8:
        case CONCAT9:
        {
            SIZE_T cch = VaConcatLength(option, psz, argList);
            T* pszReturn = GetString(new(cch) RefString<T>(cch));
            (void)VaConcat(option, psz, argList, pszReturn);
            return pszReturn;
        }
        default:
            return nullptr;
        }
    }
}
