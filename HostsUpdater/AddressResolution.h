#pragma once

#include <Windows.h>

namespace hnrt
{
	class AddressResolution
	{
	public:

		AddressResolution(PCWSTR pszAlias, PCWSTR pszHostName);
		AddressResolution(const AddressResolution&);
		~AddressResolution();
		AddressResolution& operator =(const AddressResolution&);
		PCWSTR operator[](int index) const;
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

		void Copy(const AddressResolution&);
		void Add(PCWSTR);

		PCWSTR m_pszAlias;
		PCWSTR m_pszHostName;
		PCWSTR* m_pAddresses;
		DWORD m_Size;
		DWORD m_Count;
		INT m_Error;
	};

	inline PCWSTR AddressResolution::get_Alias() const
	{
		return m_pszAlias;
	}

	inline PCWSTR AddressResolution::get_HostName() const
	{
		return m_pszHostName;
	}

	inline DWORD AddressResolution::get_Count() const
	{
		return m_Count;
	}

	inline INT AddressResolution::get_Error() const
	{
		return m_Error;
	}
}
