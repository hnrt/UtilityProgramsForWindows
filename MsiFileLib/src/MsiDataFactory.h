#pragma once


#include <Msi.h>
#include "hnrt/ByteString.h"
#include "hnrt/StringCollection.h"
#include "hnrt/MsiColumnDataType.h"
#include "hnrt/IMsiData.h"


namespace hnrt
{
    struct MsiDataFactory
    {
        static IMsiData* CreateData(MSIHANDLE hRecord, UINT uField, MsiColumnDataType type);
    };

    class MsiText
        : public IMsiText
    {
    public:

        MsiText(MSIHANDLE hRecord, UINT uField);
        MsiText(const MsiText&) = delete;
        virtual bool get_Null() const { return m_bNull; }
        virtual PCWSTR get_Text() const { return m_szText; }

    protected:

        bool m_bNull;
        String m_szText;
    };

    class MsiInteger
        : public IMsiInteger
    {
    public:

        MsiInteger(MSIHANDLE hRecord, UINT uField);
        MsiInteger(const MsiInteger&) = delete;
        virtual bool get_Null() const { return m_bNull; }
        virtual PCWSTR get_Text() const { return m_szText; }
        virtual long get_Value() const { return m_value; }

    protected:

        bool m_bNull;
        String m_szText;
        long m_value;
    };

    class MsiDoubleInteger
        : public IMsiDoubleInteger
    {
    public:

        MsiDoubleInteger(MSIHANDLE hRecord, UINT uField);
        MsiDoubleInteger(const MsiDoubleInteger&) = delete;
        virtual bool get_Null() const { return m_bNull; }
        virtual PCWSTR get_Text() const { return m_szText; }
        virtual LONGLONG get_Value() const { return m_value; }

    protected:

        bool m_bNull;
        String m_szText;
        LONGLONG m_value;
    };

    class MsiTime
        : public IMsiTime
    {
    public:

        MsiTime(MSIHANDLE hRecord, UINT uField);
        MsiTime(const MsiTime&) = delete;
        virtual bool get_Null() const { return m_bNull; }
        virtual PCWSTR get_Text() const { return m_szText; }
        virtual USHORT get_Hours() const { return (static_cast<USHORT>(m_value) >> 0) & 0x1F; }
        virtual USHORT get_Minutes() const { return (static_cast<USHORT>(m_value) >> 5) & 0x3F; }
        virtual USHORT get_Seconds() const { return ((static_cast<USHORT>(m_value) >> 11) & 0x1F) << 1; }

    protected:

        bool m_bNull;
        String m_szText;
        ULONG m_value;
    };

    class MsiDate
        : public IMsiDate
    {
    public:

        MsiDate(MSIHANDLE hRecord, UINT uField);
        MsiDate(const MsiDate&) = delete;
        virtual bool get_Null() const { return m_bNull; }
        virtual PCWSTR get_Text() const { return m_szText; }
        virtual USHORT get_Year() const { return ((static_cast<USHORT>(m_value) >> 0) & 0x7F) + 1980; }
        virtual USHORT get_Month() const { return (static_cast<USHORT>(m_value) >> 7) & 0xF; }
        virtual USHORT get_Day() const { return (static_cast<USHORT>(m_value) >> 11) & 0x1F; }

    protected:

        bool m_bNull;
        String m_szText;
        ULONG m_value;
    };

    class MsiFilename
        : public IMsiFilename
    {
    public:

        MsiFilename(MSIHANDLE hRecord, UINT uField);
        MsiFilename(const MsiFilename&) = delete;
        virtual bool get_Null() const { return m_bNull; }
        virtual PCWSTR get_Text() const { return m_szText; }
        virtual PCWSTR get_ShortName() const { return m_szShortName; }
        virtual PCWSTR get_LongName() const { return m_szLongName; }

    protected:

        bool m_bNull;
        String m_szText;
        String m_szShortName;
        String m_szLongName;
    };

    class MsiPaths
        : public IMsiPaths
    {
    public:

        virtual ~MsiPaths() = default;
        MsiPaths(MSIHANDLE hRecord, UINT uField);
        MsiPaths(const MsiPaths&) = delete;
        virtual bool get_Null() const { return m_bNull; }
        virtual PCWSTR get_Text() const { return m_szText; }
        virtual long get_Count() const { return static_cast<long>(m_Paths.Length); }
        virtual PCWSTR operator [](long index) const { return m_Paths[index]; }

    protected:

        bool m_bNull;
        String m_szText;
        StringCollection m_Paths;
    };

    class MsiVersion
        : public IMsiVersion
    {
    public:

        MsiVersion(MSIHANDLE hRecord, UINT uField);
        MsiVersion(const MsiVersion&) = delete;
        virtual bool get_Null() const { return m_bNull; }
        virtual PCWSTR get_Text() const { return m_szText; }
        virtual long operator [](long index) const;

    protected:

        bool m_bNull;
        String m_szText;
        long m_values[4];
    };

    class MsiBinary
        : public IMsiBinary
    {
    public:

        MsiBinary(MSIHANDLE hRecord, UINT uField);
        MsiBinary(const MsiBinary&) = delete;
        virtual bool get_Null() const { return m_bNull; }
        virtual const BYTE* get_Ptr() const { return m_data; }
        virtual size_t get_Len() const { return m_data.Len; }

    protected:

        bool m_bNull;
        ByteString m_data;
    };
}
