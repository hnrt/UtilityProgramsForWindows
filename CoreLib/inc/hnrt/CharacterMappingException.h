#pragma once

#include <Windows.h>
#include "hnrt/Exception.h"

namespace hnrt
{
    class CharacterMappingException
        : public Exception
    {
    public:

        CharacterMappingException();
        CharacterMappingException(DWORD dwError);
        CharacterMappingException(PCWSTR pszMessage);
        CharacterMappingException(const CharacterMappingException& src);
        virtual ~CharacterMappingException() = default;
        CharacterMappingException& operator =(const CharacterMappingException& src);
    };
}
