#pragma once


#include "hnrt/IMsiRowCollection.h"


namespace hnrt
{
    class MsiRowCollection
        : public IMsiRowCollection
    {
    public:

        virtual ~MsiRowCollection();
        MsiRowCollection();
        MsiRowCollection(const MsiRowCollection&) = delete;
        virtual long get_Count() const { return m_count; }
        virtual const IMsiRow& operator[](long rowNo) const;
        virtual IMsiRow& operator[](long rowNo);
        virtual void Add(IMsiRow* pRow);

    private:

        long m_count;
        IMsiRow** m_ppRows;
        long m_capacity;
    };
}
