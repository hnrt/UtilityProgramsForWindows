#pragma once


#include <Windows.h>


namespace hnrt
{
	class LastModified
	{
	public:

		static void SetDefaultGracePeriod(LONG nGracePeriodInMilliseconds);

		LastModified(LONG nGracePeriodInMilliseconds = -1);
		LastModified(const LastModified& src) = default;
		~LastModified() = default;
		void Clear();
		operator bool() const;
		LONGLONG get_At() const;
		int get_By() const;
		void put_By(int);
		bool get_IsUpdateRequired() const;

		__declspec(property(get = get_At)) LONGLONG At;
		__declspec(property(get = get_By, put = put_By)) int By;
		__declspec(property(get = get_IsUpdateRequired)) bool IsUpdateRequired;

	private:

		static LONGLONG m_DefaultGracePeriod;

		LONGLONG m_GracePeriod;
		LONGLONG m_At;
		int m_By;
	};

	inline LONGLONG LastModified::get_At() const
	{
		return m_At;
	}

	inline int LastModified::get_By() const
	{
		return m_By;
	}

	inline LastModified::operator bool() const
	{
		return m_At > 0;
	}
}
