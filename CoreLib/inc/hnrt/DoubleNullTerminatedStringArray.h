#pragma once


#include <Windows.h>


namespace hnrt
{
    class DoubleNullTerminatedStringArray
    {
    public:

        DoubleNullTerminatedStringArray();
        DoubleNullTerminatedStringArray(const DoubleNullTerminatedStringArray&);
        virtual ~DoubleNullTerminatedStringArray();
        DoubleNullTerminatedStringArray& operator =(const DoubleNullTerminatedStringArray&);
        operator PCWSTR () const;
        PCWSTR get_Ptr() const;
        ULONG get_Count() const;
        PCWSTR operator [](ULONG index);
        DoubleNullTerminatedStringArray& Clear();
        DoubleNullTerminatedStringArray& Add(PCWSTR psz);

        __declspec(property(get = get_Ptr)) PCWSTR Ptr;
        __declspec(property(get = get_Count)) ULONG Count;

    protected:

        wchar_t* m_pszz;
        size_t m_count;
    };

    inline DoubleNullTerminatedStringArray::operator PCWSTR () const
    {
        return m_pszz;
    }

    inline PCWSTR DoubleNullTerminatedStringArray::get_Ptr() const
    {
        return m_pszz;
    }

    inline ULONG DoubleNullTerminatedStringArray::get_Count() const
    {
        return static_cast<ULONG>(m_count);
    }
}
