#pragma once


#include "hnrt/WindowLayout.h"


namespace hnrt
{
	class WindowSize
	{
	public:

		WindowSize();
		WindowSize(const WindowSize&) = delete;
		~WindowSize() = default;
		void operator =(const WindowSize&) = delete;
		void InitializeSize(HWND hwnd);
		void SetMinimumSize(LONG cx, LONG cy);
		void OnSize(HWND hwnd, WPARAM wParam, LPARAM lParam, WindowLayout& rLayout);
		LONG get_InitialWidth() const;
		LONG get_InitialHeight() const;
		LONG get_MinimumWidth() const;
		LONG get_MinimumHeight() const;
		LONG get_BorderThickness() const;
		LONG get_TitleHeight() const;

		__declspec(property(get = get_InitialWidth)) LONG InitialWidth;
		__declspec(property(get = get_InitialHeight)) LONG InitialHeight;
		__declspec(property(get = get_MinimumWidth)) LONG MinimumWidth;
		__declspec(property(get = get_MinimumHeight)) LONG MinimumHeight;
		__declspec(property(get = get_BorderThickness)) LONG BorderThickness;
		__declspec(property(get = get_TitleHeight)) LONG TitleHeight;

	private:

		LONG m_cxInitial;
		LONG m_cyInitial;
		LONG m_cxMinimum;
		LONG m_cyMinimum;
		LONG m_cxClientInitial;
		LONG m_cyClientInitial;
		LONG m_cxClient;
		LONG m_cyClient;
		LONG m_cxBorder;
		LONG m_cyTitle;
	};

	inline LONG WindowSize::get_InitialWidth() const
	{
		return m_cxInitial;
	}

	inline LONG WindowSize::get_InitialHeight() const
	{
		return m_cyInitial;
	}

	inline LONG WindowSize::get_MinimumWidth() const
	{
		return m_cxMinimum;
	}

	inline LONG WindowSize::get_MinimumHeight() const
	{
		return m_cyMinimum;
	}

	inline LONG WindowSize::get_BorderThickness() const
	{
		return m_cxBorder;
	}

	inline LONG WindowSize::get_TitleHeight() const
	{
		return m_cyTitle;
	}
}
