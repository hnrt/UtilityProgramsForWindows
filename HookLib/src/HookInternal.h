#pragma once

#include <Windows.h>
#include <tlhelp32.h>
#include <dbghelp.h>
#include <map>
#include "hnrt/Hook.h"
#include "hnrt/SpinLock.h"
#include "hnrt/String.h"

namespace hnrt
{
	class HookInternal
		: public Hook
	{
	public:

		HookInternal();
		HookInternal(const HookInternal&) = delete;
		virtual ~HookInternal();
		void operator =(const HookInternal&) = delete;
		virtual void SetTimeDisplacement(LONGLONG);
		virtual void SetDate(int, int, int);

		void* getOriginal(const char*) const;
		void* getReplacement(const char*) const;
		void ApplyTimeDisplacementTo(FILETIME&) const;
		void ApplyTimeZoneTo(SYSTEMTIME&) const;

	private:

		typedef std::map<String, void*> Map;
		typedef std::pair<String, void*> Entry;
		typedef std::map<ULONGLONG*, ULONGLONG> OverrideMap;
		typedef std::pair<ULONGLONG*, ULONGLONG> OverrideEntry;
		typedef std::map<PBYTE, OverrideMap*> ModuleOverrideMap;
		typedef std::pair<PBYTE, OverrideMap*> ModuleOverrideEntry;

		void InitializeMaps();
		void Install();
		void Install(PBYTE, PCWSTR);
		void Install(PBYTE, PCWSTR, PIMAGE_IMPORT_DESCRIPTOR, OverrideMap&);
		void Install(PBYTE, PCWSTR, PIMAGE_IMPORT_DESCRIPTOR, PIMAGE_THUNK_DATA, PIMAGE_THUNK_DATA, OverrideMap&);
		void Uninstall();
		void Uninstall(PBYTE, PCWSTR);
		void Uninstall(PBYTE, PCWSTR, const OverrideMap&) const;

		static HookInternal& Instance();
		static HMODULE WINAPI hookLoadLibraryA(LPCSTR lpFileName);
		static HMODULE WINAPI hookLoadLibraryW(LPCWSTR lpFileName);
		static HMODULE WINAPI hookLoadLibraryExA(LPCSTR lpFileName, HANDLE hFile, DWORD dwFlags);
		static HMODULE WINAPI hookLoadLibraryExW(LPCWSTR lpFileName, HANDLE hFile, DWORD dwFlags);
		static void WINAPI hookGetSystemTimeAsFileTime(LPFILETIME pFT);
		static void WINAPI hookGetSystemTime(LPSYSTEMTIME pST);
		static void WINAPI hookGetLocalTime(LPSYSTEMTIME pST);

		mutable SpinLock::Type m_Lock;
		ModuleOverrideMap m_ModuleOverrideMap;
		Map m_Originals;
		Map m_Replacements;
		TIME_ZONE_INFORMATION m_TimeZone;
		LONGLONG m_TimeDisplacement;
	};
}
