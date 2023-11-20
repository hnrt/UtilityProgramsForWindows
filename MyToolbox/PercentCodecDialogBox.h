#pragma once


#include "MyDialogBox.h"


namespace hnrt
{
	class PercentCodecDialogBox
		: public MyDialogBox
	{
	public:

		PercentCodecDialogBox();
		PercentCodecDialogBox(const PercentCodecDialogBox&) = delete;
		virtual ~PercentCodecDialogBox() = default;
		void operator =(const PercentCodecDialogBox&) = delete;
		virtual void OnTabSelectionChanging();
		virtual void OnTabSelectionChanged();
		virtual void OnLoadFrom();
		virtual void OnSaveAs();
		virtual void OnCut();
		virtual void OnCopy();
		virtual void OnPaste();
		virtual void OnDelete();
		virtual void OnSelectAll();
		virtual void OnCopyAll();
		virtual void OnClear();
		virtual void OnExecute();
		virtual void OnSettingChanged(UINT);

	private:

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta);
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnControlColorStatic(WPARAM wParam, LPARAM lParam);
		void OnSelectSource(int);
		bool OnEncode();
		bool OnDecode();
		UINT GetCodePage() const;
		void Encode(PCWSTR pszIn, UINT cchIn, UINT uCodePage, PWCHAR pOut, UINT cchOut, bool bUsePlus);
		void Encode(WCHAR c, UINT uCodePage, PWCHAR& pOut, PWCHAR pOutBound, UINT offset);
		void Decode(PCWSTR pszIn, UINT cchIn, UINT uCodePage, PWCHAR pOut, UINT cchOut);
		UINT GetDecodedOffset(PCWSTR pszIn, UINT cbOut);

		int get_CurrentEdit() const;
		String& get_CurrentPath() const;
		void set_CurrentPath(const String&);

		__declspec(property(get = get_CurrentEdit)) int CurrentEdit;
		__declspec(property(get = get_CurrentPath, put = set_CurrentPath)) String& CurrentPath;

		bool m_bEncodingError;
		bool m_bDecodingError;
		mutable String m_szOriginalPath;
		mutable String m_szEncodedPath;
	};
}
