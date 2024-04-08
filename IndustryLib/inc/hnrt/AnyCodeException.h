#pragma once

#include "hnrt/Exception.h"

namespace hnrt
{
	class AnyCodeException
		: public Exception
	{
	public:

		AnyCodeException(int nReason, int nOffset);
		AnyCodeException(const AnyCodeException& other);
		virtual ~AnyCodeException() = default;
		AnyCodeException& operator =(const AnyCodeException& other);
		int get_Reason() const;
		int get_Offset() const;

		__declspec(property(get = get_Reason)) int Reason;
		__declspec(property(get = get_Offset)) int Offset;

	protected:

		int m_nReason;
		int m_nOffset;
	};

	inline int AnyCodeException::get_Reason() const
	{
		return m_nReason;
	}

	inline int AnyCodeException::get_Offset() const
	{
		return m_nOffset;
	}
}
