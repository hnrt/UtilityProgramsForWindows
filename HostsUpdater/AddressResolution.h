#pragma once

#include "hnrt/String.h"
#include <vector>

namespace hnrt
{
	class AddressResolution
	{
	public:

		AddressResolution(const String& szAlias, const String& szHostName);
		AddressResolution(const AddressResolution&);
		~AddressResolution() = default;
		AddressResolution& operator =(const AddressResolution&);
		PCWSTR operator [](int index) const;
		void Clear();
		bool Resolve();

		PCWSTR get_Alias() const;
		PCWSTR get_HostName() const;
		DWORD get_Count() const;
		INT get_Error() const;

		__declspec(property(get = get_Alias)) PCWSTR Alias;
		__declspec(property(get = get_HostName)) PCWSTR HostName;
		__declspec(property(get = get_Count)) DWORD Count;
		__declspec(property(get = get_Error)) INT Error;

	private:

		String m_szAlias;
		String m_szHostName;
		std::vector<String> m_Addresses;
		INT m_Error;
	};

	inline PCWSTR AddressResolution::get_Alias() const
	{
		return m_szAlias;
	}

	inline PCWSTR AddressResolution::get_HostName() const
	{
		return m_szHostName;
	}

	inline DWORD AddressResolution::get_Count() const
	{
		return static_cast<DWORD>(m_Addresses.size());
	}

	inline INT AddressResolution::get_Error() const
	{
		return m_Error;
	}
}
