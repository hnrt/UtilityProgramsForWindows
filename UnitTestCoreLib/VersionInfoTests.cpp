#include "pch.h"
#include "CppUnitTest.h"
#include "hnrt/VersionInfo.h"
#include "hnrt/Debug.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace hnrt;

namespace UnitTestCoreLib
{
    TEST_CLASS(VersionInfoTests)
    {
    public:

	TEST_METHOD(Test1)
	{
	    RefPtr<VersionInfo> v = VersionInfo::Create();
	    Assert::IsNotNull(v.Ptr);
	}

	TEST_METHOD(Test2)
	{
	    RefPtr<VersionInfo> v = VersionInfo::Create();
	    Debug::Put(L"FileVersion=%u.%u.%u.%u", v->FileVersionMajor, v->FileVersionMinor, v->FileVersionPatch, v->FileVersionBuild);
	    v->Select();
	    Debug::Put(L"ProductName=%s", v->ProductName);
	    Assert::AreEqual(L"testhost", v->ProductName);
	}

	TEST_METHOD(Test3)
	{
	    WCHAR szPath[MAX_PATH] = { 0 };
	    HMODULE hMod = GetModuleHandleW(L"UnitTestCoreLib.dll");
	    GetModuleFileNameW(hMod, szPath, MAX_PATH);
	    RefPtr<VersionInfo> v = VersionInfo::Create(szPath);
	    Debug::Put(L"FileVersion=%u.%u.%u.%u", v->FileVersionMajor, v->FileVersionMinor, v->FileVersionPatch, v->FileVersionBuild);
	    v->Select();
	    Debug::Put(L"ProductName=%s", v->ProductName);
	    Assert::AreEqual(L"Coreライブラリのユニットテスト", v->ProductName);
	}
    };
}
