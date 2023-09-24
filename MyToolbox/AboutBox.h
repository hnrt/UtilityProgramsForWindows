#pragma once


#include "hnrt/DialogBox.h"


namespace hnrt
{
	class AboutBox
		: public DialogBox
	{
	public:

		AboutBox();
		AboutBox(const AboutBox&) = delete;
		virtual ~AboutBox() = default;
		void operator =(const AboutBox&) = delete;
		void Show();
		void Hide();

	protected:

		virtual void OnClose();
		virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
	};
}
