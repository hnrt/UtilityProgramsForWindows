#pragma once


#include "hnrt/DialogBox.h"


namespace hnrt
{
	class MyDialogBox
		: public DialogBox
	{
	public:

		MyDialogBox(UINT idTemplate);
		MyDialogBox(const MyDialogBox&) = delete;
		virtual ~MyDialogBox() = default;
		void operator =(const MyDialogBox&) = delete;
		void SetId(INT id);
		bool IsActive() const;
		virtual void OnTabSelectionChanging();
		virtual void OnTabSelectionChanged();
		virtual void OnCopy();
		virtual void OnFeederNotify(ULONGLONG);

		INT get_id() const;

		__declspec(property(get = get_id)) INT Id;

	private:

		INT m_id;
		bool m_bActive;
	};

	inline MyDialogBox::MyDialogBox(UINT idTemplate)
		: DialogBox(idTemplate)
		, m_id(-1)
		, m_bActive(false)
	{
	}

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

	inline void MyDialogBox::OnTabSelectionChanging()
	{
		m_bActive = false;
	}

	inline void MyDialogBox::OnTabSelectionChanged()
	{
		m_bActive = true;
	}

	inline void MyDialogBox::OnCopy()
	{
	}

	inline void MyDialogBox::OnFeederNotify(ULONGLONG unused)
	{
	}
}
