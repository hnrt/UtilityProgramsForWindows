#pragma once

#include <Windows.h>
#include <bcrypt.h>
#include <vector>
#include "hnrt/String.h"

namespace hnrt
{
	class BCryptHandle
	{
	public:

		BCryptHandle(BCRYPT_HANDLE = nullptr);
		BCryptHandle(const BCryptHandle&) = delete;
		virtual ~BCryptHandle() = default;
		void operator =(const BCryptHandle&) = delete;
		operator BCRYPT_HANDLE() const;

	protected:

		String GetPropertyString(PCWSTR) const;
		DWORD GetPropertyDWORD(PCWSTR) const;
		std::vector<DWORD> GetPropertyArrayDWORD(PCWSTR) const;
		std::vector<DWORD> GetPropertyKeyLengths() const;
		void SetProperty(PCWSTR, PCWSTR) const;

		BCRYPT_HANDLE m_h;
	};

	inline BCryptHandle::BCryptHandle(BCRYPT_HANDLE h)
		: m_h(h)
	{
	}

	inline BCryptHandle::operator BCRYPT_HANDLE() const
	{
		return m_h;
	}

	String BCryptErrorLabel(NTSTATUS);
}
