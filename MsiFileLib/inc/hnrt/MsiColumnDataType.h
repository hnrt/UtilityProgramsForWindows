#pragma once


namespace hnrt
{
    enum class MsiColumnDataType
    {
        Text,
        UpperCase,
        LowerCase,
        Integer,
        DoubleInteger,
        Time,
        Date,
        Identifier,
        Property,
        Filename,
        WildCardFilename,
        Path,
        Paths,
        AnyPath,
        DefaultDir,
        RegPath,
        Formatted,
        FormattedSDDLText,
        Template,
        Condition,
        GUID,
        Version,
        Language,
        Binary,
        CustomSource,
        Cabinet,
        Shortcut
    };
}
