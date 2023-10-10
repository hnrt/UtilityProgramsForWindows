#pragma once


#include "hnrt/WindowDesign.h"
#include <map>


namespace hnrt
{
	class WindowLayoutSnapshot
	{
	public:

		WindowLayoutSnapshot();
		WindowLayoutSnapshot(const WindowLayoutSnapshot&) = delete;
		virtual ~WindowLayoutSnapshot() = default;
		void operator =(const WindowLayoutSnapshot&) = delete;
		const RectangleMetrics& operator [](int id) const;
		RectangleMetrics& operator [](int id);
		WindowLayoutSnapshot& Clear();
		WindowLayoutSnapshot& Add(int id, const RectangleMetrics& rect);
		WindowLayoutSnapshot& Add(HWND hwnd, int id = 0);
		WindowLayoutSnapshot& Clone(WindowLayoutSnapshot& target);
		void ApplyTo(HWND hwnd);

	protected:

		std::map<int, RectangleMetrics> m_rects;
	};
}
