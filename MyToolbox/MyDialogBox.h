#pragma once

#include "hnrt/DialogBox.h"
#include "hnrt/Menu.h"
#include "hnrt/LastModified.h"
#include "hnrt/String.h"

#define CP_AUTODETECT 0
#define CP_UTF16 1200

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
		void SetId(INT id);
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
		virtual void OnSettingChanged(UINT);
		virtual void OnFeederNotify(ULONGLONG);

		INT get_id() const;

		__declspec(property(get = get_id)) INT Id;

	protected:

		virtual void OnCreate();
		virtual void OnDestroy();
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

	private:

		INT m_id;
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
		LastModified m_LastModified;
	};

	inline void MyDialogBox::SetId(INT id)
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

	inline void MyDialogBox::OnCut()
	{
	}

	inline void MyDialogBox::OnCopy()
	{
	}

	inline void MyDialogBox::OnPaste()
	{
	}

	inline void MyDialogBox::OnDelete()
	{
	}

	inline void MyDialogBox::OnSelectAll()
	{
	}

	inline void MyDialogBox::OnCopyAll()
	{
	}

	inline void MyDialogBox::OnClear()
	{
	}
	
	inline void MyDialogBox::OnExecute()
	{
	}

	inline void MyDialogBox::OnSettingChanged(UINT uId)
	{
		UNREFERENCED_PARAMETER(uId);
	}

	inline void MyDialogBox::OnFeederNotify(ULONGLONG nbytes)
	{
		UNREFERENCED_PARAMETER(nbytes);
	}
}
