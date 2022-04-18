#pragma once

#include "resource.h"
#include "hnrt/DialogApp.h"

namespace hnrt
{
	class GuidGenerator2
		: public DialogApp
	{
	public:

		GuidGenerator2();
		GuidGenerator2(const GuidGenerator2&) = delete;
		virtual ~GuidGenerator2() = default;
		virtual void Open(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow);

	private:

		virtual void OnCreate(HWND hDlg);
		virtual void OnDestory(HWND hDlg);
		virtual void OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam);

		void ChangeFormat(HWND hDlg, UINT uSelected = 0);
		void ChangeGuid(HWND hDlg);
		void CopyToClipboard(HWND hDlg);

		GUID m_guid;
		WCHAR m_szFormatted[260];
		UINT m_uCurrentlySelected = IDC_RADIO_UPPERCASE;
	};
}
