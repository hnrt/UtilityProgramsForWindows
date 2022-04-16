#pragma once

#include "resource.h"
#include "hnrt/Hash.h"
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

	class Checksum
	{
	public:

		Checksum();
		Checksum(const Checksum&) = delete;
		~Checksum();
		void operator =(const Checksum&) = delete;
		void Open(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow);
		void Run();
		bool ProcessMessages();
		void Close();

		inline int get_ExitCode();

		__declspec(property(get = get_ExitCode)) int ExitCode;

	private:

		friend FileDataFeederEx;

		static INT_PTR CALLBACK ProcessMessage(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
		static Checksum* GetInstance(HWND hDlg);
		static void UpdateLayout(HWND hDlg, UINT id, LONG dx, LONG dy, LONG dcx, LONG dcy, BOOL bInvalidate = FALSE);

		void OnCreate(HWND hDlg);
		void OnDestory(HWND hDlg);
		void OnSize(HWND hDlg, WPARAM wParam, LPARAM lParam);
		void OnExit(HWND hDlg);
		void OnBrowse(HWND hDlg);
		void OnCalculate(HWND hDlg);
		void OnCopy(HWND hDlg);
		void OnSelectInput(HWND hDlg, UINT uInput);
		void OnSelectMethod(HWND hDlg, UINT uMethod);
		void Calculate(HWND hDlg, DataFeeder& rDataFeeder);
		bool isFile(HWND hDlg) const;
		bool isMD5(HWND hDlg) const;
		bool isSHA1(HWND hDlg) const;
		bool isSHA256(HWND hDlg) const;
		bool isSHA384(HWND hDlg) const;
		bool isSHA512(HWND hDlg) const;
		UINT GetCodePage(HWND hDlg);
		UINT GetLineBreak(HWND hDlg);
		UINT ConvertToLF(PWCHAR pStart, UINT uLength);
		void SetResultHeader(HWND hDlg);
		void SetResultHeader(HWND hDlg, ULONGLONG nBytesIn);
		void SetResultHeader(HWND hDlg, ULONGLONG nBytesIn, ULONG nBytesOut);
		void SetResult(HWND hDlg);
		void SetResult(HWND hDlg, Hash& rHash);

		int m_iExitCode;
		HACCEL m_hAccelTable;
		HWND m_hwnd;
		Hash m_hash;
		UINT m_uMethod;
		LONG m_Width;
		LONG m_Height;
		LONG m_PaddingRight;
		LONG m_PaddingBottom;
		LONG m_LastClientWidth;
		LONG m_LastClientHeight;
	};

	inline int Checksum::get_ExitCode()
	{
		return m_iExitCode;
	}
}