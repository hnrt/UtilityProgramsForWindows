#pragma once

#include "hnrt/DialogBox.h"
#include "hnrt/Menu.h"
#include "hnrt/LastModified.h"
#include "hnrt/String.h"
#include "ControlIdCollection.h"


namespace hnrt
{
	class MyDialogBox
		: public DialogBox
	{
	public:

		MyDialogBox(UINT, PCWSTR);
		MyDialogBox(const MyDialogBox&) = delete;
		virtual ~MyDialogBox() = default;
		void operator =(const MyDialogBox&) = delete;
		void SetId(UINT id);
		bool IsActive() const;
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		virtual void OnTabSelectionChanging();
		virtual void OnTabSelectionChanged();
		virtual void OnNew();
		virtual void OnLoadFrom();
		virtual void OnSaveAs();
		virtual void OnLoad1From();
		virtual void OnSave1As();
		virtual void OnLoad2From();
		virtual void OnSave2As();
		virtual void OnCut();
		virtual void OnCopy();
		virtual void OnPaste();
		virtual void OnDelete();
		virtual void OnSelectAll();
		virtual void OnCopyAll();
		virtual void OnClear();
		virtual void OnExecute();
		virtual void OnCopyResult();
		virtual void OnSettingChanged(UINT id);
		virtual void OnFeederNotify(ULONGLONG);

		INT get_id() const;

		__declspec(property(get = get_id)) INT Id;

	protected:

		static void ProcessMessages();
		virtual void OnCreate();
		virtual void OnDestroy();
		virtual INT_PTR OnTimer(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnControlColorStatic(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnControlColorEdit(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnEditCommand(WPARAM wParam, LPARAM lParam);
		virtual void OnEditSetFocus(int id);
		virtual void OnEditKillFocus(int id);
		virtual void OnEditChanged(int id);
		virtual void UpdateControlsState(int id);
		void SetTimer(UINT uElapse);
		void AddEditControlMenus(int id = 0);
		void UpdateEditControlMenus(int id = 0);
		void AddInputCodePageSettingMenus();
		void AddOutputCodePageSettingMenus();
		bool ApplyToInputCodePage(UINT);
		bool ApplyToOutputCodePage(UINT);
		void AddHashAlgorithmSettingMenus(UINT);
		bool ApplyToHashAlgorithm(UINT, UINT&, UINT);
		void AddLettercaseSettingMenus(UINT);
		bool ApplyToLettercase(UINT, StringOptions&);
		bool LoadTextFromFile(int id);
		bool LoadTextFromFile(int id, String& szPath);
		bool SaveTextAsFile(int id) const;
		bool SaveTextAsFile(int id, String& szPath) const;
		void InitializeCodePageComboBox(int id, int initialSelection = CP_UTF8) const;
		void InitializeLineBreakComboBox(int id, LineBreak initialSelection = LineBreak::CRLF) const;
		void InitializeLetterCaseComboBox(int id, StringOptions initialSelection = StringOptions::UPPERCASE) const;
		void InitializeOffsetComboBox(int id, int initialSelection = 0) const;
		void SetLengthText(int id, int expected, int actual) const;
		void FilterText(int id, BOOL (*pfnIsValid)(WCHAR));

	private:

		void KillTimers();

		UINT m_id;
		bool m_bActive;

	protected:

		String m_szRegistryKeyName;
		int m_cProcessing;
		Menu m_menuFile;
		Menu m_menuEdit;
		Menu m_menuView;
		Menu m_menuSettings;
		Menu m_menuHelp;
		UINT m_uInputCodePage;
		UINT m_uOutputCodePage;
		bool m_bOutputBOM;
		UINT m_CurrentEdit;
		LastModified m_LastModified;
		UINT m_timers[4];
		DWORD m_dwFlags;
	};

	inline void MyDialogBox::SetId(UINT id)
	{
		m_id = id;
	}

	inline bool MyDialogBox::IsActive() const
	{
		return m_bActive;
	}

	inline INT MyDialogBox::get_id() const
	{
		return m_id;
	}

	inline INT_PTR MyDialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
	{
		UNREFERENCED_PARAMETER(lParam);
		UNREFERENCED_PARAMETER(wParam);
		return FALSE;
	}

	inline void MyDialogBox::OnNew()
	{
	}

	inline void MyDialogBox::OnLoadFrom()
	{
	}

	inline void MyDialogBox::OnSaveAs()
	{
	}

	inline void MyDialogBox::OnLoad1From()
	{
	}

	inline void MyDialogBox::OnSave1As()
	{
	}

	inline void MyDialogBox::OnLoad2From()
	{
	}

	inline void MyDialogBox::OnSave2As()
	{
	}

	inline void MyDialogBox::OnExecute()
	{
	}

	inline void MyDialogBox::OnCopyResult()
	{
	}

	inline void MyDialogBox::OnSettingChanged(UINT id)
	{
		UNREFERENCED_PARAMETER(id);
	}

	inline void MyDialogBox::OnFeederNotify(ULONGLONG nbytes)
	{
		UNREFERENCED_PARAMETER(nbytes);
	}
}

constexpr auto RGB_SUCCESSFUL = RGB(51, 102, 0);
constexpr auto RGB_ERROR = RGB(153, 0, 0);
constexpr auto RGB_CHANGING = RGB(130, 130, 0);
constexpr auto RGB_GOOD = RGB_SUCCESSFUL;
constexpr auto RGB_TOO_MANY = RGB(0, 0, 204); // BLUE
constexpr auto RGB_TOO_FEW = RGB_ERROR;

#define TIMERID(id,uElapse) ((id)*10000+(uElapse))

constexpr auto FLAG_BUSY = 1 << 0;
constexpr auto FLAG_PANE1_SUCCESSFUL = 1 << 1;
constexpr auto FLAG_PANE1_ERROR = 1 << 2;
constexpr auto FLAG_PANE2_SUCCESSFUL = 1 << 3;
constexpr auto FLAG_PANE2_ERROR = 1 << 4;
constexpr auto FLAG_STATUS_SUCCESSFUL = 1 << 5;
constexpr auto FLAG_STATUS_ERROR = 1 << 6;
constexpr auto MASK_ERROR = FLAG_PANE1_ERROR | FLAG_PANE2_ERROR | FLAG_STATUS_ERROR;
constexpr auto MASK_PANE1 = FLAG_PANE1_SUCCESSFUL | FLAG_PANE1_ERROR;
constexpr auto MASK_PANE2 = FLAG_PANE2_SUCCESSFUL | FLAG_PANE2_ERROR;
constexpr auto MASK_STATUS = FLAG_STATUS_SUCCESSFUL | FLAG_STATUS_ERROR;
