#pragma once


#include "hnrt/Clipboard.h"


namespace hnrt
{
	class MyClipboardObserverClient
	{
	public:

		MyClipboardObserverClient();
		MyClipboardObserverClient(const MyClipboardObserverClient&) = delete;
		virtual ~MyClipboardObserverClient() = default;
		void operator =(const MyClipboardObserverClient&) = delete;
		virtual void ClipboardCopy(HWND hwnd, PCWSTR psz);
		virtual void ClipboardPaste(HWND hwnd, PCWSTR psz);
	};

	inline MyClipboardObserverClient::MyClipboardObserverClient()
	{
	}

	inline void MyClipboardObserverClient::ClipboardCopy(HWND hwnd, PCWSTR psz)
	{
		UNREFERENCED_PARAMETER(hwnd);
		UNREFERENCED_PARAMETER(psz);
	}

	inline void MyClipboardObserverClient::ClipboardPaste(HWND hwnd, PCWSTR psz)
	{
		UNREFERENCED_PARAMETER(hwnd);
		UNREFERENCED_PARAMETER(psz);
	}

	class MyClipboardObserver
		: public ClipboardObserver
	{
	public:

		MyClipboardObserver(MyClipboardObserverClient&);
		MyClipboardObserver(const MyClipboardObserver&) = delete;
		virtual ~MyClipboardObserver() = default;
		void operator =(const MyClipboardObserver&) = delete;
		virtual void Copy(HWND hwnd, PCWSTR psz);
		virtual void Paste(HWND hwnd, PCWSTR psz);

	private:

		MyClipboardObserverClient& m_client;
	};
}
