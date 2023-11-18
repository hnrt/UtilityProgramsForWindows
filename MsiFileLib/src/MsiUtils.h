#pragma once


#include <Msi.h>
#include "hnrt/String.h"
#include "hnrt/ByteString.h"


namespace hnrt
{
    struct MsiUtils
    {
        static String GetString(MSIHANDLE hRecord, UINT uField);
        static void ReadStream(MSIHANDLE hRecord, UINT uField, ByteString& data);
    };
}
