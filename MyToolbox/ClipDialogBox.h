#pragma once


#include "MyDialogBox.h"
#include "MyClipboardObserver.h"
#include "hnrt/String.h"
#include <map>


namespace hnrt
{
	typedef std::map<PCWSTR, PCWSTR, StringLessThan> ClipMap;
	typedef std::pair<PCWSTR, PCWSTR> ClipEntry;

	class ClipDialogBox
		: public MyDialogBox
		, public MyClipboardObserverClient
	{
	public:

		ClipDialogBox();
		ClipDialogBox(const ClipDialogBox&) = delete;
		virtual ~ClipDialogBox();
		void operator =(const ClipDialogBox&) = delete;
		virtual void OnTabSelectionChanging();
		virtual void OnTabSelectionChanged();
		virtual void OnCopy();
		virtual void OnDelete();
		virtual void ClipboardCopy(HWND hwnd, PCWSTR psz);

	private:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		void OnSelectionChange();

		RefPtr<ClipboardObserver> m_pCO;
		PCWSTR m_pszDirectoryPath;
		ClipMap m_mapHash;
		PCWSTR m_pszFilePath;
		bool m_bProcessing;
	};
}
