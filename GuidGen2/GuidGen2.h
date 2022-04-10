#pragma once

#include "resource.h"

#ifdef CreateDialog
#undef CreateDialog
#endif

namespace hnrt
{
	class GuidGenerator2
	{
	public:

		GuidGenerator2();
		GuidGenerator2(const GuidGenerator2&) = delete;
		~GuidGenerator2() = default;
		HWND CreateDialog(HINSTANCE hInstance);

	private:

		static INT_PTR CALLBACK ProcessMessage(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
		static GuidGenerator2* GetInstance(HWND hDlg);

		void OnCreate(HWND hDlg);
		void OnDestory(HWND hDlg);
		void OnExit(HWND hDlg);
		void ChangeFormat(HWND hDlg, UINT uSelected = 0);
		void ChangeGuid(HWND hDlg);
		void CopyToClipboard(HWND hDlg);

		GUID m_guid;
		WCHAR m_szFormatted[260];
		UINT m_uCurrentlySelected = IDC_RADIO_UPPERCASE;
	};
}
