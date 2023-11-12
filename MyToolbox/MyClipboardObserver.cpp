#include "pch.h"
#include "MyClipboardObserver.h"


using namespace hnrt;


MyClipboardObserver::MyClipboardObserver(MyClipboardObserverClient& client)
	: ClipboardObserver()
	, m_client(client)
{
}


void MyClipboardObserver::Copy(HWND hwnd, PCWSTR psz)
{
	m_client.ClipboardCopy(hwnd, psz);
}


void MyClipboardObserver::Paste(HWND hwnd, PCWSTR psz)
{
	m_client.ClipboardPaste(hwnd, psz);
}
