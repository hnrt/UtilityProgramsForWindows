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
		void MeasureFrame(HWND hwnd);
		void SetMinimumSize(LONG cx, LONG cy);
		void OnSize(HWND hwnd, WPARAM wParam, LPARAM lParam, WindowLayout& rLayout);
		LONG get_InitialWidth() const;
		LONG get_InitialHeight() const;
		LONG get_MinimumWidth() const;
		LONG get_MinimumHeight() const;
		LONG get_WindowWidth() const;
		LONG get_WindowHeight() const;
		LONG get_ClientWidth() const;
		LONG get_ClientHeight() const;
		LONG get_LeftFrameThickness() const;
		LONG get_RightFrameThickness() const;
		LONG get_TopFrameThickness() const;
		LONG get_BottomFrameThickness() const;

		__declspec(property(get = get_InitialWidth)) LONG InitialWidth;
		__declspec(property(get = get_InitialHeight)) LONG InitialHeight;
		__declspec(property(get = get_MinimumWidth)) LONG MinimumWidth;
		__declspec(property(get = get_MinimumHeight)) LONG MinimumHeight;
		__declspec(property(get = get_WindowWidth)) LONG WindowWidth;
		__declspec(property(get = get_WindowHeight)) LONG WindowHeight;
		__declspec(property(get = get_ClientWidth)) LONG ClientWidth;
		__declspec(property(get = get_ClientHeight)) LONG ClientHeight;
		__declspec(property(get = get_LeftFrameThickness)) LONG LeftFrameThickness;
		__declspec(property(get = get_RightFrameThickness)) LONG RightFrameThickness;
		__declspec(property(get = get_TopFrameThickness)) LONG TopFrameThickness;
		__declspec(property(get = get_BottomFrameThickness)) LONG BottomFrameThickness;

	private:

		LONG m_cxInitial;
		LONG m_cyInitial;
		LONG m_cxMinimum;
		LONG m_cyMinimum;
		LONG m_cxWindow;
		LONG m_cyWindow;
		LONG m_cxClientInitial;
		LONG m_cyClientInitial;
		LONG m_cxClient;
		LONG m_cyClient;
		LONG m_cxLeftFrame;
		LONG m_cxRightFrame;
		LONG m_cyTopFrame;
		LONG m_cyBottomFrame;
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

	inline LONG WindowSize::get_WindowWidth() const
	{
		return m_cxWindow;
	}

	inline LONG WindowSize::get_WindowHeight() const
	{
		return m_cyWindow;
	}

	inline LONG WindowSize::get_ClientWidth() const
	{
		return m_cxClient;
	}

	inline LONG WindowSize::get_ClientHeight() const
	{
		return m_cyClient;
	}

	inline LONG WindowSize::get_LeftFrameThickness() const
	{
		return m_cxLeftFrame;
	}

	inline LONG WindowSize::get_RightFrameThickness() const
	{
		return m_cxRightFrame;
	}

	inline LONG WindowSize::get_TopFrameThickness() const
	{
		return m_cyTopFrame;
	}

	inline LONG WindowSize::get_BottomFrameThickness() const
	{
		return m_cyBottomFrame;
	}
}
