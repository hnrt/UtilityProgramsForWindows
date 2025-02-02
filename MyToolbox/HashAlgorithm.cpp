#include "pch.h"
#include "HashAlgorithm.h"
#include "hnrt/String.h"
#include "hnrt/Exception.h"
#include <map>


using namespace hnrt;


static const std::map<String, HashAlgorithm> s_Mappings = {
	{ L"MD5", HashAlgorithm::MD5 },
	{ L"SHA-1", HashAlgorithm::SHA1 },
	{ L"SHA-256", HashAlgorithm::SHA256 },
	{ L"SHA-384", HashAlgorithm::SHA384 },
	{ L"SHA-512", HashAlgorithm::SHA512 },
	{ L"SHA1", HashAlgorithm::SHA1 },
	{ L"SHA256", HashAlgorithm::SHA256 },
	{ L"SHA384", HashAlgorithm::SHA384 },
	{ L"SHA512", HashAlgorithm::SHA512 },
	{ L"md5", HashAlgorithm::MD5 },
	{ L"sha-1", HashAlgorithm::SHA1 },
	{ L"sha-256", HashAlgorithm::SHA256 },
	{ L"sha-384", HashAlgorithm::SHA384 },
	{ L"sha-512", HashAlgorithm::SHA512 },
	{ L"sha1", HashAlgorithm::SHA1 },
	{ L"sha256", HashAlgorithm::SHA256 },
	{ L"sha384", HashAlgorithm::SHA384 },
	{ L"sha512", HashAlgorithm::SHA512 }
};


HashAlgorithm HashAlgorithmHelpers::FromString(PCWSTR psz)
{
	std::map<String, HashAlgorithm>::const_iterator iter = s_Mappings.find(psz);
	if (iter == s_Mappings.end())
	{
		throw Exception(L"HashAlgorithmHelpers::FromString: Bad value(%s)", psz);
	}
	return iter->second;
}


PCWSTR HashAlgorithmHelpers::ToString(HashAlgorithm ha)
{
	switch (ha)
	{
	case HashAlgorithm::MD5: return L"MD5";
	case HashAlgorithm::SHA1: return L"SHA-1";
	case HashAlgorithm::SHA256: return L"SHA-256";
	case HashAlgorithm::SHA384: return L"SHA-384";
	case HashAlgorithm::SHA512: return L"SHA-512";
	default: throw Exception(L"HashAlgorithm::ToString: Bad value(%d)", ha);
	}
}
