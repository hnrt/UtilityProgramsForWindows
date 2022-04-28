#pragma once


#include <Windows.h>


namespace hnrt
{
	struct RectangleMetrics
	{
		LONG Left;
		LONG Top;
		LONG Right;
		LONG Bottom;

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
		bool Parse(PCWSTR psz);

		LONG get_Width() const;
		LONG get_Height() const;

		__declspec(property(get = get_Width)) LONG Width;
		__declspec(property(get = get_Height)) LONG Height;
	};

	inline RectangleMetrics::RectangleMetrics()
		: Left(0)
		, Top(0)
		, Right(0)
		, Bottom(0)
	{
	}

	inline RectangleMetrics::RectangleMetrics(const RectangleMetrics& src)
		: Left(src.Left)
		, Top(src.Top)
		, Right(src.Right)
		, Bottom(src.Bottom)
	{
	}

	inline RectangleMetrics::RectangleMetrics(const RECT& src)
		: Left(src.left)
		, Top(src.top)
		, Right(src.right)
		, Bottom(src.bottom)
	{
	}

	inline RectangleMetrics& RectangleMetrics::operator =(const RectangleMetrics& src)
	{
		Left = src.Left;
		Top = src.Top;
		Right = src.Right;
		Bottom = src.Bottom;
		return *this;
	}

	inline RectangleMetrics& RectangleMetrics::operator =(const RECT& src)
	{
		Left = src.left;
		Top = src.top;
		Right = src.right;
		Bottom = src.bottom;
		return *this;
	}

	inline RectangleMetrics& RectangleMetrics::FromWindow(HWND hwnd)
	{
		GetWindowRect(hwnd, reinterpret_cast<RECT*>(this));
		return *this;
	}

	inline RectangleMetrics& RectangleMetrics::FromClient(HWND hwnd)
	{
		GetClientRect(hwnd, reinterpret_cast<RECT*>(this));
		return *this;
	}

	inline RectangleMetrics& RectangleMetrics::ToScreen(HWND hwnd)
	{
		ClientToScreen(hwnd, reinterpret_cast<POINT*>(&Left));
		ClientToScreen(hwnd, reinterpret_cast<POINT*>(&Right));
		return *this;
	}

	inline RectangleMetrics& RectangleMetrics::ToClient(HWND hwnd)
	{
		ScreenToClient(hwnd, reinterpret_cast<POINT*>(&Left));
		ScreenToClient(hwnd, reinterpret_cast<POINT*>(&Right));
		return *this;
	}

	inline bool RectangleMetrics::Parse(PCWSTR psz)
	{
		PWSTR pEnd;
		long n1 = wcstol(psz, &pEnd, 10);
		if (*pEnd == L'\0')
		{
			Left = n1;
			Top = n1;
			Right = n1;
			Bottom = n1;
			return true;
		}
		else if (*pEnd == L',')
		{
			long n2 = wcstol(++pEnd, &pEnd, 10);
			if (*pEnd == L'\0')
			{
				Left = n1;
				Top = n2;
				Right = n1;
				Bottom = n2;
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
						Left = n1;
						Top = n2;
						Right = n3;
						Bottom = n4;
						return true;
					}
				}
			}
		}
		return false;
	}

	inline LONG RectangleMetrics::get_Width() const
	{
		return Right - Left;
	}

	inline LONG RectangleMetrics::get_Height() const
	{
		return Bottom - Top;
	}
}
