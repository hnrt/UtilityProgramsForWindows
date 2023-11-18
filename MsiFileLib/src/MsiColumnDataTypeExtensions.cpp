#include "pch.h"
#include <map>
#include "hnrt/StringCaseInsensitive.h"
#include "hnrt/SpinLock.h"
#include "hnrt/MsiColumnDataTypeExtensions.h"
#include "hnrt/Exception.h"


using namespace hnrt;


typedef std::map<StringCase, MsiColumnDataType> MsiColumnDataTypeMap;
typedef std::pair<StringCase, MsiColumnDataType> MsiColumnDataTypeMapEntry;


static SpinLock::Type s_TypeMapLock = SPIN_LOCK_INITIALIZER;
static MsiColumnDataTypeMap s_TypeMap;


static void InitTypeMap()
{
#define ADDTYPE(x) s_TypeMap.insert(MsiColumnDataTypeMapEntry(StringCase(L#x),MsiColumnDataType::x))
    ADDTYPE(Text);
    ADDTYPE(UpperCase);
    ADDTYPE(LowerCase);
    ADDTYPE(Integer);
    ADDTYPE(DoubleInteger);
    ADDTYPE(Time);
    ADDTYPE(Date);
    ADDTYPE(Identifier);
    ADDTYPE(Property);
    ADDTYPE(Filename);
    ADDTYPE(WildCardFilename);
    ADDTYPE(Path);
    ADDTYPE(Paths);
    ADDTYPE(AnyPath);
    ADDTYPE(DefaultDir);
    ADDTYPE(RegPath);
    ADDTYPE(Formatted);
    ADDTYPE(FormattedSDDLText);
    ADDTYPE(Template);
    ADDTYPE(Condition);
    ADDTYPE(GUID);
    ADDTYPE(Version);
    ADDTYPE(Language);
    ADDTYPE(Binary);
    ADDTYPE(CustomSource);
    ADDTYPE(Cabinet);
    ADDTYPE(Shortcut);
#undef ADDTYPE
}


MsiColumnDataType MsiColumnDataTypeExtensions::ToType(PCWSTR psz)
{
    SpinLock lock(s_TypeMapLock);
    if (!s_TypeMap.size())
    {
        InitTypeMap();
    }
    MsiColumnDataTypeMap::const_iterator iter = s_TypeMap.find(psz);
    if (iter != s_TypeMap.end())
    {
        return iter->second;
    }
    else
    {
        throw Exception(L"MsiColumnDataTypeBase::ToType failed.");
    }
}


PCWSTR MsiColumnDataTypeExtensions::ToString(MsiColumnDataType type)
{
    SpinLock lock(s_TypeMapLock);
    if (!s_TypeMap.size())
    {
        InitTypeMap();
    }
    for (MsiColumnDataTypeMap::const_iterator iter = s_TypeMap.begin(); iter != s_TypeMap.end(); iter++)
    {
        if (iter->second == type)
        {
            return iter->first;
        }
    }
    return L"?";
}
