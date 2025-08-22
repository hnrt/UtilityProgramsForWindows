#include "pch.h"
#include "CppUnitTest.h"
#include "resource.h"
#include "hnrt/ResourceString.h"
#include "hnrt/Debug.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace hnrt;

namespace UnitTestCoreLib
{
	TEST_CLASS(ResourceStringTests)
	{
		TEST_CLASS_INITIALIZE(Initialize)
		{
			ResourceString::m_hInstance = GetModuleHandleW(L"UnitTestCoreLib.dll");
			Logger::WriteMessage(String::Format(L"ResourceString::m_hInstance=%p", ResourceString::m_hInstance));
		}

		TEST_METHOD(Test01)
		{
			ResourceString s1(IDS_ENGLISH);
			ResourceString s2(IDS_JAPANESE);
			Debug::Put(L"ResourceStringTest1: s1=\"%s\"", s1);
			Debug::Put(L"ResourceStringTest1: s2=\"%s\"", s2);
			Assert::AreEqual(0, wcscmp(s1, L"‰pŒê"));
			Assert::AreEqual(0, wcscmp(s2, L"“ú–{Œê"));
		}

		TEST_METHOD(Test02)
		{
			ResourceString s1(IDS_ENGLISH);
			ResourceString s2(IDS_JAPANESE);
			String s3 = String::Format(L"%s|%s", s1, s2);
			Debug::Put(L"ResourceStringTest2: s3=\"%s\"", s3);
			Assert::AreEqual(0, wcscmp(s3, L"‰pŒê|“ú–{Œê"));
		}
	};
}
