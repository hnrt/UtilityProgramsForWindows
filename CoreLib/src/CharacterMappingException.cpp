#include "pch.h"
#include "hnrt/CharacterMappingException.h"
#include "hnrt/ErrorMessage.h"


using namespace hnrt;


CharacterMappingException::CharacterMappingException()
	: Exception()
{
}


CharacterMappingException::CharacterMappingException(DWORD dwError)
	: Exception(L"%s", ErrorMessage::Get(dwError))
{
}


CharacterMappingException::CharacterMappingException(PCWSTR pszMessage)
	: Exception(L"%s", pszMessage)
{
}


CharacterMappingException::CharacterMappingException(const CharacterMappingException& src)
	: Exception(src)
{
}


CharacterMappingException& CharacterMappingException::operator =(const CharacterMappingException& src)
{
	Exception::operator =(src);
	return *this;
}
