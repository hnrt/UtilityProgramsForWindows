#pragma once


#include <Windows.h>


namespace hnrt
{
	struct RectangleMetrics
		: public RECT
	{
		RectangleMetrics();
		RectangleMetrics(const RectangleMetrics&);
		RectangleMetrics(const RECT&);
		~RectangleMetrics() = default;
		RectangleMetrics& operator =(const RectangleMetrics&);
		RectangleMetrics& operator =(const RECT&);
		RectangleMetrics& FromWindow(HWND hwnd);
		RectangleMetrics& FromClient(HWND hwnd);
		RectangleMetrics& ToScreen(HWND hwnd);
		RectangleMetrics& ToClient(HWND hwnd);
		LONG HorizontalGap(const RECT&);
		LONG VerticalGap(const RECT&);
		LONG FromLeftToRight(const RECT&);
		LONG FromTopToBottom(const RECT&);
		bool Parse(PCWSTR psz);
		LONG GetX() const;
		LONG GetY() const;
		LONG GetWidth() const;
		LONG GetHeight() const;
		void SetX(LONG);
		void SetY(LONG);
		void SetWidth(LONG);
		void SetHeight(LONG);

		__declspec(property(get = GetX, put = SetX)) LONG x;
		__declspec(property(get = GetY, put = SetY)) LONG y;
		__declspec(property(get = GetWidth, put = SetWidth)) LONG cx;
		__declspec(property(get = GetHeight, put = SetHeight)) LONG cy;
		__declspec(property(get = GetWidth, put = SetWidth)) LONG Width;
		__declspec(property(get = GetHeight, put = SetHeight)) LONG Height;
	};

	inline RectangleMetrics::RectangleMetrics()
		: RECT()
	{
	}

	inline RectangleMetrics::RectangleMetrics(const RectangleMetrics& src)
		: RECT()
	{
		*this = src;
	}

	inline RectangleMetrics::RectangleMetrics(const RECT& src)
	{
		*this = src;
	}

	inline RectangleMetrics& RectangleMetrics::operator =(const RectangleMetrics& src)
	{
		left = src.left;
		top = src.top;
		right = src.right;
		bottom = src.bottom;
		return *this;
	}

	inline RectangleMetrics& RectangleMetrics::operator =(const RECT& src)
	{
		left = src.left;
		top = src.top;
		right = src.right;
		bottom = src.bottom;
		return *this;
	}

	inline RectangleMetrics& RectangleMetrics::FromWindow(HWND hwnd)
	{
		GetWindowRect(hwnd, this);
		return *this;
	}

	inline RectangleMetrics& RectangleMetrics::FromClient(HWND hwnd)
	{
		GetClientRect(hwnd, this);
		return *this;
	}

	inline RectangleMetrics& RectangleMetrics::ToScreen(HWND hwnd)
	{
		ClientToScreen(hwnd, reinterpret_cast<POINT*>(&left));
		ClientToScreen(hwnd, reinterpret_cast<POINT*>(&right));
		return *this;
	}

	inline RectangleMetrics& RectangleMetrics::ToClient(HWND hwnd)
	{
		ScreenToClient(hwnd, reinterpret_cast<POINT*>(&left));
		ScreenToClient(hwnd, reinterpret_cast<POINT*>(&right));
		return *this;
	}

	inline LONG RectangleMetrics::HorizontalGap(const RECT& other)
	{
		return other.left - right;
	}

	inline LONG RectangleMetrics::VerticalGap(const RECT& other)
	{
		return other.top - bottom;
	}

	inline LONG RectangleMetrics::FromLeftToRight(const RECT& other)
	{
		return other.right - left;
	}

	inline LONG RectangleMetrics::FromTopToBottom(const RECT& other)
	{
		return other.bottom - top;
	}

	inline bool RectangleMetrics::Parse(PCWSTR psz)
	{
		PWSTR pEnd;
		long n1 = wcstol(psz, &pEnd, 10);
		if (*pEnd == L'\0')
		{
			left = n1;
			top = n1;
			right = n1;
			bottom = n1;
			return true;
		}
		else if (*pEnd == L',')
		{
			long n2 = wcstol(++pEnd, &pEnd, 10);
			if (*pEnd == L'\0')
			{
				left = n1;
				top = n2;
				right = n1;
				bottom = n2;
				return true;
			}
			else if (*pEnd == L',')
			{
				long n3 = wcstol(++pEnd, &pEnd, 10);
				if (*pEnd == L',')
				{
					long n4 = wcstol(++pEnd, &pEnd, 10);
					if (*pEnd == L'\0')
					{
						left = n1;
						top = n2;
						right = n3;
						bottom = n4;
						return true;
					}
				}
			}
		}
		return false;
	}

	inline LONG RectangleMetrics::GetX() const
	{
		return left;
	}

	inline LONG RectangleMetrics::GetY() const
	{
		return top;
	}

	inline LONG RectangleMetrics::GetWidth() const
	{
		return right - left;
	}

	inline LONG RectangleMetrics::GetHeight() const
	{
		return bottom - top;
	}

	inline void RectangleMetrics::SetX(LONG value)
	{
		left = value;
	}

	inline void RectangleMetrics::SetY(LONG value)
	{
		top = value;
	}

	inline void RectangleMetrics::SetWidth(LONG value)
	{
		right = left + value;
	}

	inline void RectangleMetrics::SetHeight(LONG value)
	{
		bottom = top + value;
	}
}
