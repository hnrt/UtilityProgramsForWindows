#pragma once

#include "hnrt/String.h"

#define GS1PREFIX_LENGTH 3
#define GS1COMPANYPREFIX_LENGTH7 7
#define GS1COMPANYPREFIX_LENGTH9 9
#define GS1COMPANYPREFIX_LENGTH10 10
#define GTIN13_LENGTH_EXCLUDING_CD 12
#define GTIN13_LENGTH 13

namespace hnrt
{
	class GTIN13
	{
	public:

		static GTIN13 Parse(PCWSTR psz, int nGS1CompanyPrefixLength = GS1COMPANYPREFIX_LENGTH7);

		/// <summary>
		/// This static method computes the GTIN-13 check digit.
		/// </summary>
		/// <param name="s">String of 12 digits.</param>
		/// <returns>The resulting check digit.</returns>
		static WCHAR ComputeCheckDigit(const WCHAR s[]);

		GTIN13(int nGS1CompanyPrefixLength = GS1COMPANYPREFIX_LENGTH7);
		GTIN13(const GTIN13&) = default;
		~GTIN13() = default;
		GTIN13& operator =(PCWSTR psz);
		GTIN13& operator +=(int delta);
		GTIN13& operator -=(int delta);
		operator PCWSTR() const;
		WCHAR get_CheckDigit() const;
		int get_GS1CompanyPrefixLength() const;
		int get_ItemReferenceLenth() const;
		String get_GS1CompanyPrefix() const;
		String get_ItemReference() const;

		__declspec(property(get = get_CheckDigit)) WCHAR CheckDigit;
		__declspec(property(get = get_GS1CompanyPrefixLength)) int GS1CompanyPrefixLength;
		__declspec(property(get = get_ItemReferenceLenth)) int ItemReferenceLenth;
		__declspec(property(get = get_GS1CompanyPrefix)) String GS1CompanyPrefix;
		__declspec(property(get = get_ItemReference)) String ItemReference;

	protected:

		WCHAR m_d[GTIN13_LENGTH + 1];
		int m_GS1CompanyPrefixLength;
	};

	inline GTIN13::operator PCWSTR() const
	{
		return m_d;
	}

	inline WCHAR GTIN13::get_CheckDigit() const
	{
		return m_d[GTIN13_LENGTH_EXCLUDING_CD];
	}

	inline int GTIN13::get_GS1CompanyPrefixLength() const
	{
		return m_GS1CompanyPrefixLength;
	}

	inline int GTIN13::get_ItemReferenceLenth() const
	{
		return GTIN13_LENGTH_EXCLUDING_CD - m_GS1CompanyPrefixLength;
	}

	inline String GTIN13::get_GS1CompanyPrefix() const
	{
		return String(&m_d[0], m_GS1CompanyPrefixLength);
	}

	inline String GTIN13::get_ItemReference() const
	{
		return String(&m_d[m_GS1CompanyPrefixLength], ItemReferenceLenth);
	}

	typedef GTIN13 JAN;
}
