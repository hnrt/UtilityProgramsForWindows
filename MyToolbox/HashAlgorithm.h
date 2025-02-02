#pragma once

#include <Windows.h>

namespace hnrt
{
	enum HashAlgorithm
	{
		MD5,
		SHA1,
		SHA256,
		SHA384,
		SHA512
	};

	class HashAlgorithmHelpers
	{
	public:

		static HashAlgorithm FromString(PCWSTR);
		static PCWSTR ToString(HashAlgorithm);
	};
}
