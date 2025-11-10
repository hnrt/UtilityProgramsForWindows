#pragma once

#include "hnrt/String.h"

namespace hnrt
{
	class SemanticVersion
	{
	public:

		SemanticVersion();
		SemanticVersion(PCWSTR);
		SemanticVersion(const SemanticVersion&);
		~SemanticVersion() = default;
		SemanticVersion& operator = (const SemanticVersion&);
		String ToString() const;
		int CompareTo(const SemanticVersion&) const;

		bool operator == (const SemanticVersion& other) const { return CompareTo(other) == 0; }
		bool operator != (const SemanticVersion& other) const { return CompareTo(other) != 0; }
		bool operator <= (const SemanticVersion & other) const { return CompareTo(other) <= 0; }
		bool operator < (const SemanticVersion& other) const { return CompareTo(other) < 0; }
		bool operator >= (const SemanticVersion& other) const { return CompareTo(other) >= 0; }
		bool operator > (const SemanticVersion& other) const { return CompareTo(other) > 0; }

		int get_Level() const { return m_Level; }
		int get_Major() const { return m_Major; }
		int get_Minor() const { return m_Minor; }
		int get_Patch() const { return m_Patch; }
		PCWSTR get_PreRelease() const { return m_PreRelease; }
		PCWSTR get_Build() const { return m_Build; }

		__declspec(property(get = get_Level)) int Level;
		__declspec(property(get = get_Major)) int Major;
		__declspec(property(get = get_Minor)) int Minor;
		__declspec(property(get = get_Patch)) int Patch;
		__declspec(property(get = get_PreRelease)) PCWSTR PreRelease;
		__declspec(property(get = get_Build)) PCWSTR Build;

	private:

		bool Parse(PCWSTR);
		static bool ParseNumericIdentifier(PCWSTR&, int&);
		static bool ParseSuffix(PCWSTR&, String&);
		static bool ParseAlphaNumericIdentifier(PCWSTR&);
		static int Compare(PCWSTR, PCWSTR);

		int m_Level;
		int m_Major;
		int m_Minor;
		int m_Patch;
		String m_PreRelease;
		String m_Build;
	};
}
