#include "pch.h"
#include "CppUnitTest.h"
#include "hnrt/Hook.h"
#include "hnrt/Debug.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace hnrt;

namespace UnitTestHookLib
{
	TEST_CLASS(UnitTestHookLib)
	{
	public:
		
		TEST_METHOD(Test0101)
		{
			CHAR szPath[MAX_PATH] = { 0 };
			GetModuleFileNameA(GetModuleHandleA("UnitTestHookLib.dll"), szPath, MAX_PATH);
			char* p = strstr(szPath, "UnitTestHookLib");
			memcpy_s(p + 8, 4, "Core", 4);
			p = strstr(p + 16, "UnitTestHookLib");
			memcpy_s(p + 8, 4, "Core", 4);
			Debug::Put(L"Test0101: Path=%hs", szPath);
			Hook& hook = Hook::Instance();
			hook.SetTimeDisplacement(0);
			HMODULE hModule = LoadLibraryA(szPath);
			Assert::IsNotNull(hModule);
			FreeLibrary(hModule);
		}

		TEST_METHOD(Test0102)
		{
			Hook& hook = Hook::Instance();
			hook.SetTimeDisplacement(0);
			HMODULE hModule = LoadLibraryA("BogusBogus123.dll");
			Assert::IsNull(hModule);
		}

		TEST_METHOD(Test0201)
		{
			WCHAR szPath[MAX_PATH] = { 0 };
			GetModuleFileNameW(GetModuleHandleW(L"UnitTestHookLib.dll"), szPath, MAX_PATH);
			wchar_t* p = wcsstr(szPath, L"UnitTestHookLib");
			wmemcpy_s(p + 8, 4, L"Core", 4);
			p = wcsstr(p + 16, L"UnitTestHookLib");
			wmemcpy_s(p + 8, 4, L"Core", 4);
			Debug::Put(L"Test0201: Path=%s", szPath);
			Hook& hook = Hook::Instance();
			hook.SetTimeDisplacement(0);
			HMODULE hModule = LoadLibraryW(szPath);
			Assert::IsNotNull(hModule);
			FreeLibrary(hModule);
		}

		TEST_METHOD(Test0202)
		{
			Hook& hook = Hook::Instance();
			hook.SetTimeDisplacement(0);
			HMODULE hModule = LoadLibraryW(L"BogusBogus789.dll");
			Assert::IsNull(hModule);
		}

		TEST_METHOD(Test0301)
		{
			CHAR szPath[MAX_PATH] = { 0 };
			GetModuleFileNameA(GetModuleHandleA("UnitTestHookLib.dll"), szPath, MAX_PATH);
			char* p = strstr(szPath, "UnitTestHookLib");
			memcpy_s(p + 8, 4, "Core", 4);
			p = strstr(p + 16, "UnitTestHookLib");
			memcpy_s(p + 8, 4, "Core", 4);
			Debug::Put(L"Test0301: Path=%hs", szPath);
			Hook& hook = Hook::Instance();
			hook.SetTimeDisplacement(0);
			HMODULE hModule = LoadLibraryExA(szPath, NULL, 0);
			Assert::IsNotNull(hModule);
			FreeLibrary(hModule);
		}

		TEST_METHOD(Test0302)
		{
			CHAR szPath[MAX_PATH] = { 0 };
			GetModuleFileNameA(GetModuleHandleA("UnitTestHookLib.dll"), szPath, MAX_PATH);
			char* p = strstr(szPath, "UnitTestHookLib");
			memcpy_s(p + 8, 4, "Core", 4);
			p = strstr(p + 16, "UnitTestHookLib");
			memcpy_s(p + 8, 4, "Core", 4);
			Debug::Put(L"Test0302: Path=%hs", szPath);
			Hook& hook = Hook::Instance();
			hook.SetTimeDisplacement(0);
			HMODULE hModule = LoadLibraryExA(szPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
			Assert::IsNotNull(hModule);
			FreeLibrary(hModule);
		}

		TEST_METHOD(Test0303)
		{
			Hook& hook = Hook::Instance();
			hook.SetTimeDisplacement(0);
			HMODULE hModule = LoadLibraryExA("BogusBogus123.dll", NULL, 0);
			Assert::IsNull(hModule);
		}

		TEST_METHOD(Test0304)
		{
			Hook& hook = Hook::Instance();
			hook.SetTimeDisplacement(0);
			HMODULE hModule = LoadLibraryExA("BogusBogus123.dll", NULL, LOAD_LIBRARY_AS_DATAFILE);
			Assert::IsNull(hModule);
		}

		TEST_METHOD(Test0401)
		{
			WCHAR szPath[MAX_PATH] = { 0 };
			GetModuleFileNameW(GetModuleHandleW(L"UnitTestHookLib.dll"), szPath, MAX_PATH);
			wchar_t* p = wcsstr(szPath, L"UnitTestHookLib");
			wmemcpy_s(p + 8, 4, L"Core", 4);
			p = wcsstr(p + 16, L"UnitTestHookLib");
			wmemcpy_s(p + 8, 4, L"Core", 4);
			Debug::Put(L"Test0401: Path=%s", szPath);
			Hook& hook = Hook::Instance();
			hook.SetTimeDisplacement(0);
			HMODULE hModule = LoadLibraryExW(szPath, NULL, 0);
			Assert::IsNotNull(hModule);
			FreeLibrary(hModule);
		}

		TEST_METHOD(Test0402)
		{
			WCHAR szPath[MAX_PATH] = { 0 };
			GetModuleFileNameW(GetModuleHandleW(L"UnitTestHookLib.dll"), szPath, MAX_PATH);
			wchar_t* p = wcsstr(szPath, L"UnitTestHookLib");
			wmemcpy_s(p + 8, 4, L"Core", 4);
			p = wcsstr(p + 16, L"UnitTestHookLib");
			wmemcpy_s(p + 8, 4, L"Core", 4);
			Debug::Put(L"Test0402: Path=%s", szPath);
			Hook& hook = Hook::Instance();
			hook.SetTimeDisplacement(0);
			HMODULE hModule = LoadLibraryExW(szPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
			Assert::IsNotNull(hModule);
			FreeLibrary(hModule);
		}

		TEST_METHOD(Test0403)
		{
			Hook& hook = Hook::Instance();
			hook.SetTimeDisplacement(0);
			HMODULE hModule = LoadLibraryExW(L"BogusBogus789.dll", NULL, 0);
			Assert::IsNull(hModule);
		}

		TEST_METHOD(Test0404)
		{
			Hook& hook = Hook::Instance();
			hook.SetTimeDisplacement(0);
			HMODULE hModule = LoadLibraryExW(L"BogusBogus789.dll", NULL, LOAD_LIBRARY_AS_DATAFILE);
			Assert::IsNull(hModule);
		}

		TEST_METHOD(Test0501)
		{
			SYSTEMTIME st0 = { 0 };
			GetLocalTime(&st0);
			Assert::IsFalse(st0.wYear == 2020);
			Hook& hook = Hook::Instance();
			hook.SetDate(2020, 1, 1);
			SYSTEMTIME st = { 0 };
			GetLocalTime(&st);
			Assert::IsTrue(st.wYear == 2020);
			Assert::IsTrue(st.wMonth == 1);
			Assert::IsTrue(st.wDay == 1);
			hook.SetTimeDisplacement(0);
			GetLocalTime(&st);
			Assert::IsTrue(st.wYear == st0.wYear);
			Assert::IsTrue(st.wMonth == st0.wMonth);
			Assert::IsTrue(st.wDay == st0.wDay);
		}
	};
}
