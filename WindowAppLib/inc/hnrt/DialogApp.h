#pragma once


#include "hnrt/DialogBox.h"
#include "hnrt/AnyApp.h"


namespace hnrt
{
	class DialogApp
		: public AnyApp
		, public DialogBox
	{
	public:

		DialogApp(UINT idTemplate);
		DialogApp(const DialogApp&) = delete;
		virtual ~DialogApp() = default;
		void operator =(const DialogApp&) = delete;
		virtual void Open(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow);
		void SetText(PCWSTR);

	protected:

		virtual void ProcessMessage(MSG* pMsg);
		virtual void OnClose();
	};
}
