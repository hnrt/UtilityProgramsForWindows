#pragma once

#include "hnrt/FileDataFeeder.h"

namespace hnrt
{
	class FileDataFeederEx
		: public FileDataFeeder
	{
	public:

		FileDataFeederEx(PCWSTR pszFileName, HWND hDlg);
		FileDataFeederEx(const FileDataFeederEx&) = delete;
		virtual ~FileDataFeederEx() = default;
		void operator =(const FileDataFeederEx&) = delete;
		virtual bool HasNext();

	protected:

		HWND m_hDlg;
		ULONGLONG m_LastTick;
	};
}
