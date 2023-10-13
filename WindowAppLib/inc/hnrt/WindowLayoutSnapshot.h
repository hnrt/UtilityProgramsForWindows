#pragma once


#include "hnrt/WindowDesign.h"
#include <map>


namespace hnrt
{
	class WindowLayoutSnapshot
	{
	public:

		WindowLayoutSnapshot(HWND hwndDialog = nullptr);
		WindowLayoutSnapshot(const WindowLayoutSnapshot&) = delete;
		virtual ~WindowLayoutSnapshot() = default;
		void operator =(const WindowLayoutSnapshot&) = delete;
		const RectangleMetrics& operator [](int id) const;
		RectangleMetrics& operator [](int id);
		HWND GetWindow() const;
		WindowLayoutSnapshot& SetWindow(HWND hwndDialog);
		WindowLayoutSnapshot& Clear();
		WindowLayoutSnapshot& Add(int id);
		WindowLayoutSnapshot& Add(int id, const RectangleMetrics& rect);
		WindowLayoutSnapshot& AddAllChildren(HWND hwndDialog = nullptr);
		WindowLayoutSnapshot& Clone(WindowLayoutSnapshot& target);
		void Apply();

	protected:

		HWND m_hwndDialog;
		std::map<int, RectangleMetrics> m_rects;
	};
}
