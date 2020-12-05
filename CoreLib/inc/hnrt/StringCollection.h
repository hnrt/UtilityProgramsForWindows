#pragma once


#include <Windows.h>


namespace hnrt
{
    class StringCollection
    {
    public:

        StringCollection();
        StringCollection(const StringCollection&);
        virtual ~StringCollection();
        StringCollection& operator =(const StringCollection&);
        size_t get_Capacity() const;
        void set_Capacity(size_t value);
        size_t get_Count() const;
        PCWSTR operator [](size_t index) const;
        StringCollection& Clear();
        StringCollection& Add(PCWSTR psz, size_t cch = static_cast<size_t>(-1));
        StringCollection& RemoveAt(size_t index);
        StringCollection& Split(PCWSTR psz, WCHAR sep, bool bAll = false);

        __declspec(property(get = get_Capacity, put = set_Capacity)) size_t Capacity;
        __declspec(property(get = get_Count)) size_t Count;

    protected:

        PCWSTR* m_pArray;
        size_t m_Capacity;
        size_t m_Count;
    };
}
