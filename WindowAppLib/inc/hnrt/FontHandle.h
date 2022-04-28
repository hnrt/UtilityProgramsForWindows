#pragma once


#include <Windows.h>


namespace hnrt
{
	class FontHandle
	{
	public:

		FontHandle(HFONT h = nullptr);
		FontHandle(const FontHandle&) = delete;
		~FontHandle();
		void operator = (const FontHandle&) = delete;
		FontHandle& operator =(HFONT h);
		operator HFONT() const;

	private:

		HFONT m_h;
	};

	inline FontHandle::operator HFONT() const
	{
		return m_h;
	}
}
