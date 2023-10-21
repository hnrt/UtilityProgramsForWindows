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
		LONG HorizontalGap(const RECT&) const;
		LONG VerticalGap(const RECT&) const;
		LONG FromLeftToRight(const RECT&) const;
		LONG FromTopToBottom(const RECT&) const;
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

	inline LONG RectangleMetrics::HorizontalGap(const RECT& other) const
	{
		return other.left - right;
	}

	inline LONG RectangleMetrics::VerticalGap(const RECT& other) const
	{
		return other.top - bottom;
	}

	inline LONG RectangleMetrics::FromLeftToRight(const RECT& other) const
	{
		return other.right - left;
	}

	inline LONG RectangleMetrics::FromTopToBottom(const RECT& other) const
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

	inline void MoveHorizontally(RectangleMetrics& rect, LONG xDelta)
	{
		rect.left += xDelta;
		rect.right += xDelta;
	}

	inline void MoveVertically(RectangleMetrics& rect, LONG yDelta)
	{
		rect.top += yDelta;
		rect.bottom += yDelta;
	}

	inline void MoveHorizontallyAndExtend(RectangleMetrics& rect, LONG xDelta, LONG cxDelta)
	{
		rect.left += xDelta;
		rect.right += xDelta;
		rect.cx += cxDelta;
	}

	inline void MoveVerticallyAndExtend(RectangleMetrics& rect, LONG yDelta, LONG cyDelta)
	{
		rect.top += yDelta;
		rect.bottom += yDelta;
		rect.cy += cyDelta;
	}

	inline void RepositionLeft(RectangleMetrics& rect, LONG left)
	{
		LONG xDelta = left - rect.left;
		rect.left += xDelta;
		rect.right += xDelta;
	}

	inline void RepositionRight(RectangleMetrics& rect, LONG right)
	{
		LONG xDelta = right - rect.right;
		rect.left += xDelta;
		rect.right += xDelta;
	}

	inline void RepositionTop(RectangleMetrics& rect, LONG top)
	{
		LONG yDelta = top - rect.top;
		rect.top += yDelta;
		rect.bottom += yDelta;
	}

	inline void RepositionBottom(RectangleMetrics& rect, LONG bottom)
	{
		LONG yDelta = bottom - rect.bottom;
		rect.top += yDelta;
		rect.bottom += yDelta;
	}

	inline void ExtendHorizontally(RectangleMetrics& rect1, RectangleMetrics& rect2, RectangleMetrics& rect3, LONG cxDelta)
	{
		LONG cxGap12 = rect1.HorizontalGap(rect2);
		LONG cxGap23 = rect2.HorizontalGap(rect3);
		rect1.cx = (rect1.FromLeftToRight(rect3) - cxGap12 - cxGap23 + cxDelta) / 3;
		rect2.left = rect1.right + cxGap12;
		rect2.cx = rect1.cx;
		rect3.left = rect2.right + cxGap23;
		rect3.right += cxDelta;
	}

	inline void CenterHorizontally(const RectangleMetrics& rect0, RectangleMetrics& rect1, RectangleMetrics& rect2, RectangleMetrics& rect3)
	{
		LONG cxGap12 = rect1.HorizontalGap(rect2);
		LONG cxGap23 = rect2.HorizontalGap(rect3);
		RepositionLeft(rect1, rect0.left + (rect0.cx - rect1.FromLeftToRight(rect3)) / 2);
		RepositionLeft(rect2, rect1.right + cxGap12);
		RepositionLeft(rect3, rect2.right + cxGap23);
	}

	inline void ExtendVertically(RectangleMetrics& rect1, RectangleMetrics& rect2, LONG cyDelta)
	{
		LONG cyGap = rect1.VerticalGap(rect2);
		rect1.cy = (rect1.FromTopToBottom(rect2) - cyGap + cyDelta) / 2;
		rect2.top = rect1.bottom + cyGap;
		rect2.bottom += cyDelta;
	}

	inline void ExtendVertically(RectangleMetrics& rect1, RectangleMetrics& rect2, RectangleMetrics& rect3, LONG cyDelta)
	{
		LONG cyGap12 = rect1.VerticalGap(rect2);
		LONG cyGap23 = rect2.VerticalGap(rect3);
		rect1.cy = (rect1.FromTopToBottom(rect3) - cyGap12 - cyGap23 + cyDelta) / 3;
		rect2.top = rect1.bottom + cyGap12;
		rect2.cy = rect1.cy;
		rect3.top = rect2.bottom + cyGap23;
		rect3.bottom += cyDelta;
	}

	inline void BindRightToLeft(RectangleMetrics& rect, const RectangleMetrics& before, const RectangleMetrics& after)
	{
		rect.right = after.left - rect.HorizontalGap(before);
	}

	inline void BindLeftToRight(RectangleMetrics& rect, const RectangleMetrics& before, const RectangleMetrics& after)
	{
		rect.left = after.right + before.HorizontalGap(rect);
	}
}
