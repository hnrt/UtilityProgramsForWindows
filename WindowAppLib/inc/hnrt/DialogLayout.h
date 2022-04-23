#pragma once
#pragma once


#include <Windows.h>


namespace hnrt
{
	class DialogLayout
	{
	public:

		DialogLayout() = default;
		DialogLayout(const DialogLayout&) = delete;
		virtual ~DialogLayout() = default;
		void operator =(const DialogLayout&) = delete;
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);

		static void UpdateLayout(HWND hDlg, UINT id, LONG dx, LONG dy, LONG dcx, LONG dcy, BOOL bInvalidate = FALSE);
	};
}
