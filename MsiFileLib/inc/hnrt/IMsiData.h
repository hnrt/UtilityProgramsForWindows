#pragma once


#include <Windows.h>
#include <stddef.h>


namespace hnrt
{
    struct IMsiText;
    typedef IMsiText IMsiUpperCase;
    typedef IMsiText IMsiLowerCase;
    struct IMsiInteger;
    struct IMsiDoubleInteger;
    struct IMsiTime;
    struct IMsiDate;
    typedef IMsiText IMsiIdentifier;
    typedef IMsiIdentifier IMsiProperty;
    struct IMsiFilename;
    typedef IMsiFilename IMsiWildCardFilename;
    typedef IMsiText IMsiPath;
    struct IMsiPaths;
    typedef IMsiFilename IMsiAnyPath;
    typedef IMsiFilename IMsiDefaultDir;
    typedef IMsiText IMsiFormatted;
    typedef IMsiFormatted IMsiRegPath;
    typedef IMsiText IMsiFormattedSDDLText;
    typedef IMsiFormatted IMsiTemplate;
    typedef IMsiText IMsiCondition;
    typedef IMsiText IMsiGUID;
    struct IMsiVersion;
    typedef IMsiText IMsiLanguage;
    struct IMsiBinary;
    typedef IMsiIdentifier IMsiCustomSource;
    typedef IMsiIdentifier IMsiCabinet;
    typedef IMsiIdentifier IMsiShortcut;

    struct IMsiData
    {
        virtual ~IMsiData() = default;
        virtual bool get_Null() const = 0;
        const IMsiText* ToText() const;
        const IMsiUpperCase* ToUpperCase() const;
        const IMsiLowerCase* ToLowerCase() const;
        const IMsiInteger* ToInteger() const;
        const IMsiDoubleInteger* ToDoubleInteger() const;
        const IMsiTime* ToTime() const;
        const IMsiDate* ToDate() const;
        const IMsiIdentifier* ToIdentifier() const;
        const IMsiProperty* ToProperty() const;
        const IMsiFilename* ToFilename() const;
        const IMsiWildCardFilename* ToWildCardFilename() const;
        const IMsiPath* ToPath() const;
        const IMsiPaths* ToPaths() const;
        const IMsiAnyPath* ToAnyPath() const;
        const IMsiDefaultDir* ToDefaultDir() const;
        const IMsiRegPath* ToRegPath() const;
        const IMsiFormatted* ToFormatted() const;
        const IMsiFormattedSDDLText* ToFormattedSDDLText() const;
        const IMsiTemplate* ToTemplate() const;
        const IMsiCondition* ToCondition() const;
        const IMsiGUID* ToGUID() const;
        const IMsiVersion* ToVersion() const;
        const IMsiLanguage* ToLanguage() const;
        const IMsiBinary* ToBinary() const;
        const IMsiCustomSource* ToCustomSource() const;
        const IMsiCabinet* ToCabinet() const;
        const IMsiShortcut* ToShortcut() const;

        __declspec(property(get = get_Null)) bool IsNull;
    };

    struct IMsiText
        : public IMsiData
    {
        virtual PCWSTR get_Text() const = 0;

        __declspec(property(get = get_Text)) PCWSTR Text;
    };

    struct IMsiInteger
        : public IMsiText
    {
        virtual long get_Value() const = 0;

        __declspec(property(get = get_Value)) long Value;
    };

    struct IMsiDoubleInteger
        : public IMsiText
    {
        virtual LONGLONG get_Value() const = 0;

        __declspec(property(get = get_Value)) LONGLONG Value;
    };

    struct IMsiTime
        : public IMsiText
    {
        virtual USHORT get_Hours() const = 0;
        virtual USHORT get_Minutes() const = 0;
        virtual USHORT get_Seconds() const = 0;

        __declspec(property(get = get_Hours)) USHORT Hours;
        __declspec(property(get = get_Minutes)) USHORT Minutes;
        __declspec(property(get = get_Seconds)) USHORT Seconds;
    };

    struct IMsiDate
        : public IMsiText
    {
        virtual USHORT get_Year() const = 0;
        virtual USHORT get_Month() const = 0;
        virtual USHORT get_Day() const = 0;

        __declspec(property(get = get_Year)) USHORT Year;
        __declspec(property(get = get_Month)) USHORT Month;
        __declspec(property(get = get_Day)) USHORT Day;
    };

    struct IMsiFilename
        : public IMsiText
    {
        virtual PCWSTR get_ShortName() const = 0;
        virtual PCWSTR get_LongName() const = 0;

        __declspec(property(get = get_ShortName)) PCWSTR ShortName;
        __declspec(property(get = get_LongName)) PCWSTR LongName;
    };

    struct IMsiPaths
        : public IMsiText
    {
        virtual long get_Count() const = 0;
        virtual PCWSTR operator [](long index) const = 0;

        __declspec(property(get = get_Count)) long Count;
    };

    struct IMsiVersion
        : public IMsiText
    {
        virtual long operator [](long index) const = 0;
    };

    struct IMsiBinary
        : public IMsiData
    {
        virtual const BYTE* get_Ptr() const = 0;
        virtual size_t get_Len() const = 0;

        __declspec(property(get = get_Ptr)) const BYTE* Ptr;
        __declspec(property(get = get_Len)) size_t Len;
    };

    inline const IMsiText* IMsiData::ToText() const { return dynamic_cast<const IMsiText*>(this); }
    inline const IMsiUpperCase* IMsiData::ToUpperCase() const { return dynamic_cast<const IMsiUpperCase*>(this); }
    inline const IMsiLowerCase* IMsiData::ToLowerCase() const { return dynamic_cast<const IMsiLowerCase*>(this); }
    inline const IMsiInteger* IMsiData::ToInteger() const { return dynamic_cast<const IMsiInteger*>(this); }
    inline const IMsiDoubleInteger* IMsiData::ToDoubleInteger() const { return dynamic_cast<const IMsiDoubleInteger*>(this); }
    inline const IMsiTime* IMsiData::ToTime() const { return dynamic_cast<const IMsiTime*>(this); }
    inline const IMsiDate* IMsiData::ToDate() const { return dynamic_cast<const IMsiDate*>(this); }
    inline const IMsiIdentifier* IMsiData::ToIdentifier() const { return dynamic_cast<const IMsiIdentifier*>(this); }
    inline const IMsiProperty* IMsiData::ToProperty() const { return dynamic_cast<const IMsiProperty*>(this); }
    inline const IMsiFilename* IMsiData::ToFilename() const { return dynamic_cast<const IMsiFilename*>(this); }
    inline const IMsiWildCardFilename* IMsiData::ToWildCardFilename() const { return dynamic_cast<const IMsiWildCardFilename*>(this); }
    inline const IMsiPath* IMsiData::ToPath() const { return dynamic_cast<const IMsiPath*>(this); }
    inline const IMsiPaths* IMsiData::ToPaths() const { return dynamic_cast<const IMsiPaths*>(this); }
    inline const IMsiAnyPath* IMsiData::ToAnyPath() const { return dynamic_cast<const IMsiAnyPath*>(this); }
    inline const IMsiDefaultDir* IMsiData::ToDefaultDir() const { return dynamic_cast<const IMsiDefaultDir*>(this); }
    inline const IMsiRegPath* IMsiData::ToRegPath() const { return dynamic_cast<const IMsiRegPath*>(this); }
    inline const IMsiFormatted* IMsiData::ToFormatted() const { return dynamic_cast<const IMsiFormatted*>(this); }
    inline const IMsiFormattedSDDLText* IMsiData::ToFormattedSDDLText() const { return dynamic_cast<const IMsiFormattedSDDLText*>(this); }
    inline const IMsiTemplate* IMsiData::ToTemplate() const { return dynamic_cast<const IMsiTemplate*>(this); }
    inline const IMsiCondition* IMsiData::ToCondition() const { return dynamic_cast<const IMsiCondition*>(this); }
    inline const IMsiGUID* IMsiData::ToGUID() const { return dynamic_cast<const IMsiGUID*>(this); }
    inline const IMsiVersion* IMsiData::ToVersion() const { return dynamic_cast<const IMsiVersion*>(this); }
    inline const IMsiLanguage* IMsiData::ToLanguage() const { return dynamic_cast<const IMsiLanguage*>(this); }
    inline const IMsiBinary* IMsiData::ToBinary() const { return dynamic_cast<const IMsiBinary*>(this); }
    inline const IMsiCustomSource* IMsiData::ToCustomSource() const { return dynamic_cast<const IMsiCustomSource*>(this); }
    inline const IMsiCabinet* IMsiData::ToCabinet() const { return dynamic_cast<const IMsiCabinet*>(this); }
    inline const IMsiShortcut* IMsiData::ToShortcut() const { return dynamic_cast<const IMsiShortcut*>(this); }
}
