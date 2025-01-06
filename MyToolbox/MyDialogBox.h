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
		virtual void OnTabSelectionChanging();
		virtual void OnTabSelectionChanged();
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

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
		virtual INT_PTR OnTimer(WPARAM wParam, LPARAM lParam);
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
		bool ApplyToLettercase(UINT, UINT&, UINT);
		void LoadTextFromFile(int id) const;
		void LoadTextFromFile(int id, String& szPath) const;
		void SaveTextAsFile(int id) const;
		void SaveTextAsFile(int id, String& szPath) const;
		void InitializeCodePageComboBox(int id, int initialSelection = CP_UTF8) const;
		void InitializeLineBreakComboBox(int id, int initialSelection = 0x0d0a) const;
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

#define RGB_SUCCESSFUL RGB(51, 102, 0)
#define RGB_ERROR RGB(153, 0, 0)
#define RGB_CHANGING RGB(130, 130, 0)
#define RGB_GOOD RGB_SUCCESSFUL
#define RGB_TOO_MANY RGB(0, 0, 204) // BLUE
#define RGB_TOO_FEW RGB_ERROR

#define TIMERID(id,uElapse) ((id)*10000+(uElapse))
