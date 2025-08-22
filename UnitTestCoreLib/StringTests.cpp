#include "pch.h"
#include "CppUnitTest.h"
#include "hnrt/String.h"
#include "hnrt/Debug.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace hnrt;

namespace UnitTestCoreLib
{
	TEST_CLASS(StringTests)
	{
	public:

		TEST_METHOD(Test01)
		{
			String s;
			Assert::AreEqual(0, wcscmp(s, L""));
		}

		TEST_METHOD(Test02)
		{
			String s(L"XYZ");
			Debug::Put(L"StringTest2: %s", s);
			Assert::AreEqual(0, wcscmp(s, L"XYZ"));
		}

		TEST_METHOD(Test03)
		{
			String s1(L"ABC");
			String s2(s1);
			String s3(s2);
			Debug::Put(L"StringTest3: s1@%p=%s", s1.Ptr, s1);
			Debug::Put(L"StringTest3: s2@%p=%s", s2.Ptr, s2);
			Debug::Put(L"StringTest3: s3@%p=%s", s3.Ptr, s3);
			const void* p1 = s1.Ptr;
			const void* p2 = s2.Ptr;
			const void* p3 = s3.Ptr;
			Assert::AreEqual(0, wcscmp(s1, L"ABC"));
			Assert::AreEqual(0, wcscmp(s2, L"ABC"));
			Assert::AreEqual(0, wcscmp(s3, L"ABC"));
			Assert::AreEqual(true, p1 == p2);
			Assert::AreEqual(true, p2 == p3);
			Assert::AreEqual(true, p3 == p1);
		}

		TEST_METHOD(Test04)
		{
			String s1(L"DEF");
			String s2(L"DEF");
			String s3(L"DEF");
			Debug::Put(L"StringTest4: s1@%p=%s", s1.Ptr, s1);
			Debug::Put(L"StringTest4: s2@%p=%s", s2.Ptr, s2);
			Debug::Put(L"StringTest4: s3@%p=%s", s3.Ptr, s3);
			const void* p1 = s1.Ptr;
			const void* p2 = s2.Ptr;
			const void* p3 = s3.Ptr;
			Assert::AreEqual(0, wcscmp(s1, L"DEF"));
			Assert::AreEqual(0, wcscmp(s2, L"DEF"));
			Assert::AreEqual(0, wcscmp(s3, L"DEF"));
			Assert::AreEqual(false, p1 == p2);
			Assert::AreEqual(false, p2 == p3);
			Assert::AreEqual(false, p3 == p1);
		}

		TEST_METHOD(Test05)
		{
			String s1(L"GHIJKL", 1);
			String s2(L"GHIJKL", 2);
			String s3(L"GHIJKL", 3);
			String s4(L"GHIJKL", 4);
			String s5(L"GHIJKL", 5);
			String s6(L"GHIJKL", 6);
			String s7(L"GHIJKL", 7);
			String s8(L"GHIJKL", 8);
			Debug::Put(L"StringTest5: s1@%p=%s", s1, s1);
			Debug::Put(L"StringTest5: s2@%p=%s", s2, s2);
			Debug::Put(L"StringTest5: s3@%p=%s", s3, s3);
			Debug::Put(L"StringTest5: s4@%p=%s", s4, s4);
			Debug::Put(L"StringTest5: s5@%p=%s", s5, s5);
			Debug::Put(L"StringTest5: s6@%p=%s", s6, s6);
			Debug::Put(L"StringTest5: s7@%p=%s", s7, s7);
			Debug::Put(L"StringTest5: s8@%p=%s", s8, s8);
			Assert::AreEqual(0, wcscmp(s1, L"G"));
			Assert::AreEqual(0, wcscmp(s2, L"GH"));
			Assert::AreEqual(0, wcscmp(s3, L"GHI"));
			Assert::AreEqual(0, wcscmp(s4, L"GHIJ"));
			Assert::AreEqual(0, wcscmp(s5, L"GHIJK"));
			Assert::AreEqual(0, wcscmp(s6, L"GHIJKL"));
			Assert::AreEqual(0, wcscmp(s7, L"GHIJKL"));
			Assert::AreEqual(0, wcscmp(s8, L"GHIJKL"));
			Assert::AreEqual(1ULL, s1.Len);
			Assert::AreEqual(2ULL, s2.Len);
			Assert::AreEqual(3ULL, s3.Len);
			Assert::AreEqual(4ULL, s4.Len);
			Assert::AreEqual(5ULL, s5.Len);
			Assert::AreEqual(6ULL, s6.Len);
			Assert::AreEqual(6ULL, s7.Len);
			Assert::AreEqual(6ULL, s8.Len);
		}

		TEST_METHOD(Test06)
		{
			String* ps = Sprintf(L"%s_%d_%g", L"xyzzy", 123, 987.654);
			Debug::Put(L"StringTest6: [1] *ps=%s", *ps);
			Assert::AreEqual(0, wcscmp(*ps, L"xyzzy_123_987.654"));
			*ps = String::Empty;
			Debug::Put(L"StringTest6: [2] *ps=%s ps->Ptr=\"%s\"", *ps, ps->Ptr);
			Assert::AreEqual(0, wcscmp(*ps, L""));
			delete ps;
		}

		TEST_METHOD(Test07)
		{
			String s1(CONCAT, L"555", L"MNO", L"PQR", L"STU", NULL);
			String s2(CONCAT3, L"666", L"MNO", L"PQR", L"STU", NULL);
			String s3(UPPERCASE, L"vwxyz123");
			String s4(LOWERCASE, L"VWXYZ456");
			String s5(TRIM, L"  789 0  00  000  ");
			String s6(TRIM_HEAD, L"  789 0  00  000  ");
			String s7(TRIM_TAIL, L"  789 0  00  000  ");
			Debug::Put(L"StringTest7: [1] s1@%p=\"%s\"", s1, s1);
			Debug::Put(L"StringTest7: [1] s2@%p=\"%s\"", s2, s2);
			Debug::Put(L"StringTest7: [1] s3@%p=\"%s\"", s3, s3);
			Debug::Put(L"StringTest7: [1] s4@%p=\"%s\"", s4, s4);
			Debug::Put(L"StringTest7: [1] s5@%p=\"%s\"", s5, s5);
			Debug::Put(L"StringTest7: [1] s6@%p=\"%s\"", s6, s6);
			Debug::Put(L"StringTest7: [1] s7@%p=\"%s\"", s7, s7);
			Assert::AreEqual(0, wcscmp(s1, L"555MNOPQRSTU"));
			Assert::AreEqual(0, wcscmp(s2, L"666MNOPQR"));
			Assert::AreEqual(0, wcscmp(s3, L"VWXYZ123"));
			Assert::AreEqual(0, wcscmp(s4, L"vwxyz456"));
			Assert::AreEqual(0, wcscmp(s5, L"789 0  00  000"));
			Assert::AreEqual(0, wcscmp(s6, L"789 0  00  000  "));
			Assert::AreEqual(0, wcscmp(s7, L"  789 0  00  000"));
			s3 = s3.Lowercase();
			s4 = s4.Uppercase();
			s6 = s6.Trim(TRIM_TAIL);
			s7 = s7.Trim(TRIM_HEAD);
			Debug::Put(L"StringTest7: [2] s3@%p=\"%s\"", s3, s3);
			Debug::Put(L"StringTest7: [2] s4@%p=\"%s\"", s4, s4);
			Debug::Put(L"StringTest7: [2] s6@%p=\"%s\"", s6, s6);
			Debug::Put(L"StringTest7: [2] s7@%p=\"%s\"", s7, s7);
			Assert::AreEqual(0, wcscmp(s3, L"vwxyz123"));
			Assert::AreEqual(0, wcscmp(s4, L"VWXYZ456"));
			Assert::AreEqual(0, wcscmp(s6, L"789 0  00  000"));
			Assert::AreEqual(0, wcscmp(s7, L"789 0  00  000"));
		}

		TEST_METHOD(Test08)
		{
			String s1(L"123", L"456");
			const void* p1A = s1.Ptr;
			Debug::Put(L"StringTest8: [1] s1@%p=\"%s\"", s1, s1);
			Assert::AreEqual(0, wcscmp(s1, L"123456"));
			s1.ZeroFill();
			const void* p1B = s1.Ptr;
			Debug::Put(L"StringTest8: [2] s1@%p=\"%s\"", s1, s1);
			Assert::AreEqual(0, wcscmp(s1, L""));
			Assert::AreEqual(true, p1A == p1B);
		}

		TEST_METHOD(Test09)
		{
			static CHAR d3[] = { '\xE6', '\x97', '\xA5', '\xE6', '\x9C', '\xAC', '\xE8', '\xAA', '\x9E', '\x00' };
			static CHAR d4[] = { '\x93', '\xFA', '\x96', '\x7B', '\x8C', '\xEA', '\x00' };
			String s1(CP_ACP, "888");
			String s2(CP_ACP, "999999", 3);
			String s3(CP_UTF8, d3);
			String s4(932, d4);
			Debug::Put(L"StringTest9: s1@%p=\"%s\"", s1, s1);
			Debug::Put(L"StringTest9: s2@%p=\"%s\"", s2, s2);
			Debug::Put(L"StringTest9: s3@%p=\"%s\"", s3, s3);
			Debug::Put(L"StringTest9: s4@%p=\"%s\"", s4, s4);
			Assert::AreEqual(0, wcscmp(s1, L"888"));
			Assert::AreEqual(0, wcscmp(s2, L"999"));
			Assert::AreEqual(0, wcscmp(s3, L"“ú–{Œê"));
			Assert::AreEqual(0, wcscmp(s4, L"“ú–{Œê"));
		}

		TEST_METHOD(Test10)
		{
			String s1 = L"123";
			Debug::Put(L"StringTest10: [1] s1@%p=\"%s\"", s1, s1);
			Assert::AreEqual(0, wcscmp(s1, L"123"));
			s1 = L"ABC";
			Debug::Put(L"StringTest10: [2] s1@%p=\"%s\"", s1, s1);
			Assert::AreEqual(0, wcscmp(s1, L"ABC"));
			s1 += L"456";
			Debug::Put(L"StringTest10: [3] s1@%p=\"%s\"", s1, s1);
			Assert::AreEqual(0, wcscmp(s1, L"ABC456"));
			s1 += L"XYZ";
			Debug::Put(L"StringTest10: [4] s1@%p=\"%s\"", s1, s1);
			Assert::AreEqual(0, wcscmp(s1, L"ABC456XYZ"));
			String s2 = L"ABC456XYZ";
			Debug::Put(L"StringTest10: [4] s2@%p=\"%s\"", s2, s2);
			Assert::AreEqual(true, s1 == s2);
			Assert::AreEqual(false, s1 != s2);
			Assert::AreEqual(false, s1 < s2);
			Assert::AreEqual(true, s1 <= s2);
			Assert::AreEqual(false, s1 > s2);
			Assert::AreEqual(true, s1 >= s2);
			String s3 = L"ABC456";
			Debug::Put(L"StringTest10: [4] s3@%p=\"%s\"", s3, s3);
			Assert::AreEqual(false, s1 == s3);
			Assert::AreEqual(true, s1 != s3);
			Assert::AreEqual(false, s1 < s3);
			Assert::AreEqual(false, s1 <= s3);
			Assert::AreEqual(true, s1 > s3);
			Assert::AreEqual(true, s1 >= s3);
			String s4 = L"ABC456ZZZ";
			Debug::Put(L"StringTest10: [4] s4@%p=\"%s\"", s4, s4);
			Assert::AreEqual(false, s1 == s4);
			Assert::AreEqual(true, s1 != s4);
			Assert::AreEqual(true, s1 < s4);
			Assert::AreEqual(true, s1 <= s4);
			Assert::AreEqual(false, s1 > s4);
			Assert::AreEqual(false, s1 >= s4);
			String s5 = s1 + s4;
			Debug::Put(L"StringTest10: [5] s1@%p=\"%s\"", s1, s1);
			Debug::Put(L"StringTest10: [5] s4@%p=\"%s\"", s4, s4);
			Debug::Put(L"StringTest10: [5] s5@%p=\"%s\"", s5, s5);
			Assert::AreEqual(0, wcscmp(s1, L"ABC456XYZ"));
			Assert::AreEqual(0, wcscmp(s4, L"ABC456ZZZ"));
			Assert::AreEqual(0, wcscmp(s5, L"ABC456XYZABC456ZZZ"));
		}

		TEST_METHOD(Test11)
		{
			String s1 = L"123";
			Debug::Put(L"StringTest11: [1] s1@%p=\"%s\"", s1, s1);
			Assert::AreEqual(0, wcscmp(s1, L"123"));
			s1 = String::Format(L"%d%s", 444, L"quux");
			Debug::Put(L"StringTest11: [2] s1@%p=\"%s\"", s1, s1);
			Assert::AreEqual(0, wcscmp(s1, L"444quux"));
			s1.AppendFormat(L"-%s-%d", L"fred", 555);
			Debug::Put(L"StringTest11: [3] s1@%p=\"%s\"", s1, s1);
			Assert::AreEqual(0, wcscmp(s1, L"444quux-fred-555"));
			s1.AppendFormat(L"@%d", 777);
			Debug::Put(L"StringTest11: [4] s1@%p=\"%s\"", s1, s1);
			Assert::AreEqual(0, wcscmp(s1, L"444quux-fred-555@777"));
		}

		TEST_METHOD(Test12)
		{
			String s1 = L"789";
			Debug::Put(L"StringTest12: [1] s1@%p=\"%s\"", s1, s1);
			Assert::AreEqual(0, wcscmp(s1, L"789"));
			s1 = VaFormatTest(s1, L"%d%s", 111, L"QUUX");
			Debug::Put(L"StringTest12: [2] s1@%p=\"%s\"", s1, s1);
			Assert::AreEqual(0, wcscmp(s1, L"111QUUX"));
			s1 = VaAppendFormatTest(s1, L"-%s-%d", L"FRED", 222);
			Debug::Put(L"StringTest12: [3] s1@%p=\"%s\"", s1, s1);
			Assert::AreEqual(0, wcscmp(s1, L"111QUUX-FRED-222"));
			s1 = VaAppendFormatTest(s1, L"@%d", 333);
			Debug::Put(L"StringTest12: [4] s1@%p=\"%s\"", s1, s1);
			Assert::AreEqual(0, wcscmp(s1, L"111QUUX-FRED-222@333"));
		}

		TEST_METHOD(Test13)
		{
			String s1 = L"A quick brown fox jumps over the lazy dog.";
			int o0 = s1.IndexOf('@');
			int o1 = s1.IndexOf('A');
			int o2 = s1.IndexOf('b');
			int o3 = s1.IndexOf('A', 3);
			int o4 = s1.IndexOf('b', 4);
			int o5 = s1.IndexOf(' ');
			int o6 = s1.IndexOf(' ', 2);
			int o7 = s1.IndexOf(' ', 8);
			Debug::Put(L"StringTest13: IndexOf('a')=%d", o0);
			Debug::Put(L"StringTest13: IndexOf('A')=%d", o1);
			Debug::Put(L"StringTest13: IndexOf('b')=%d", o2);
			Debug::Put(L"StringTest13: IndexOf('A',3)=%d", o3);
			Debug::Put(L"StringTest13: IndexOf('b',4)=%d", o4);
			Debug::Put(L"StringTest13: IndexOf(' ')=%d", o5);
			Debug::Put(L"StringTest13: IndexOf(' ',2)=%d", o6);
			Debug::Put(L"StringTest13: IndexOf(' ',8)=%d", o7);
			Assert::AreEqual(-1, o0);
			Assert::AreEqual(0, o1);
			Assert::AreEqual(8, o2);
			Assert::AreEqual(-1, o3);
			Assert::AreEqual(8, o4);
			Assert::AreEqual(1, o5);
			Assert::AreEqual(7, o6);
			Assert::AreEqual(13, o7);
		}

		TEST_METHOD(Test14)
		{
			String s1 = L"AAA BXX BBC AAA BBB CCC";
			int o0 = s1.IndexOf(L"123");
			int o1 = s1.IndexOf(L"AA");
			int o2 = s1.IndexOf(L"AA", 1);
			int o3 = s1.IndexOf(L"AA", 2);
			int o4 = s1.IndexOf(L"AA", 13);
			int o5 = s1.IndexOf(L"AA", 14);
			int o6 = s1.IndexOf(L"BBB", 0);
			int o9 = s1.IndexOf(L"BBBB", 0);
			Debug::Put(L"StringTest14: IndexOf(\"123\")=%d", o0);
			Debug::Put(L"StringTest14: IndexOf(\"AA\")=%d", o1);
			Debug::Put(L"StringTest14: IndexOf(\"AA\",1)=%d", o2);
			Debug::Put(L"StringTest14: IndexOf(\"AA\",2)=%d", o3);
			Debug::Put(L"StringTest14: IndexOf(\"AA\",13)=%d", o4);
			Debug::Put(L"StringTest14: IndexOf(\"AA\",14)=%d", o5);
			Debug::Put(L"StringTest14: IndexOf(\"BBB\",0)=%d", o6);
			Debug::Put(L"StringTest14: IndexOf(\"BBBB\",0)=%d", o9);
			Assert::AreEqual(-1, o0);
			Assert::AreEqual(0, o1);
			Assert::AreEqual(1, o2);
			Assert::AreEqual(12, o3);
			Assert::AreEqual(13, o4);
			Assert::AreEqual(-1, o5);
			Assert::AreEqual(16, o6);
			Assert::AreEqual(-1, o9);
		}

		TEST_METHOD(Test15)
		{
			String s1 = L"A quick brown fox jumps over the lazy dog.";
			int o0 = s1.LastIndexOf('@');
			int o1 = s1.LastIndexOf('A');
			int o2 = s1.LastIndexOf('b');
			int o3 = s1.LastIndexOf('A', 3);
			int o4 = s1.LastIndexOf('b', 4);
			int o5 = s1.LastIndexOf(' ');
			int o6 = s1.LastIndexOf(' ', 2);
			int o7 = s1.LastIndexOf(' ', 8);
			Debug::Put(L"StringTest15: LastIndexOf('a')=%d", o0);
			Debug::Put(L"StringTest15: LastIndexOf('A')=%d", o1);
			Debug::Put(L"StringTest15: LastIndexOf('b')=%d", o2);
			Debug::Put(L"StringTest15: LastIndexOf('A',3)=%d", o3);
			Debug::Put(L"StringTest15: LastIndexOf('b',4)=%d", o4);
			Debug::Put(L"StringTest15: LastIndexOf(' ')=%d", o5);
			Debug::Put(L"StringTest15: LastIndexOf(' ',2)=%d", o6);
			Debug::Put(L"StringTest15: LastIndexOf(' ',8)=%d", o7);
			Assert::AreEqual(-1, o0);
			Assert::AreEqual(0, o1);
			Assert::AreEqual(8, o2);
			Assert::AreEqual(0, o3);
			Assert::AreEqual(-1, o4);
			Assert::AreEqual(37, o5);
			Assert::AreEqual(1, o6);
			Assert::AreEqual(7, o7);
		}

		TEST_METHOD(Test16)
		{
			String s1 = L"AAA BXX BBC AAA BBB CCC";
			int o0 = s1.LastIndexOf(L"123");
			int o1 = s1.LastIndexOf(L"AA");
			int o2 = s1.LastIndexOf(L"AA", 1);
			int o3 = s1.LastIndexOf(L"AA", 2);
			int o4 = s1.LastIndexOf(L"AA", 13);
			int o5 = s1.LastIndexOf(L"AA", 14);
			int o6 = s1.LastIndexOf(L"BBB", 0);
			int o9 = s1.LastIndexOf(L"BBBB", 0);
			Debug::Put(L"StringTest16: LastIndexOf(\"123\")=%d", o0);
			Debug::Put(L"StringTest16: LastIndexOf(\"AA\")=%d", o1);
			Debug::Put(L"StringTest16: LastIndexOf(\"AA\",1)=%d", o2);
			Debug::Put(L"StringTest16: LastIndexOf(\"AA\",2)=%d", o3);
			Debug::Put(L"StringTest16: LastIndexOf(\"AA\",13)=%d", o4);
			Debug::Put(L"StringTest16: LastIndexOf(\"AA\",14)=%d", o5);
			Debug::Put(L"StringTest16: LastIndexOf(\"BBB\",0)=%d", o6);
			Debug::Put(L"StringTest16: LastIndexOf(\"BBBB\",0)=%d", o9);
			Assert::AreEqual(-1, o0);
			Assert::AreEqual(13, o1);
			Assert::AreEqual(0, o2);
			Assert::AreEqual(1, o3);
			Assert::AreEqual(12, o4);
			Assert::AreEqual(13, o5);
			Assert::AreEqual(-1, o6);
			Assert::AreEqual(-1, o9);
		}

	private:

		String* Sprintf(PCWSTR pszFormat, ...)
		{
			va_list argList;
			va_start(argList, pszFormat);
			String* ps = new String(pszFormat, argList);
			va_end(argList);
			return ps;
		}

		String VaFormatTest(String s, PCWSTR pszFormat, ...)
		{
			va_list argList;
			va_start(argList, pszFormat);
			s.VaFormat(pszFormat, argList);
			va_end(argList);
			return s;
		}

		String VaAppendFormatTest(String s, PCWSTR pszFormat, ...)
		{
			va_list argList;
			va_start(argList, pszFormat);
			s.VaAppendFormat(pszFormat, argList);
			va_end(argList);
			return s;
		}
	};
}
