#include "pch.h"
#include "CppUnitTest.h"
#include "hnrt/Path.h"
#include "hnrt/Exception.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace hnrt;

namespace UnitTestCoreLib
{
    TEST_CLASS(PathTests)
    {
    public:

	TEST_METHOD(TestGetFileName)
	{
	    PCWSTR psz;

	    psz = Path::GetFileName(L"abc.xyz");
	    Assert::AreEqual(L"abc.xyz", psz);
	    psz = Path::GetFileName(L"def\\abc.xyz");
	    Assert::AreEqual(L"abc.xyz", psz);
	    psz = Path::GetFileName(L".\\abc.xyz");
	    Assert::AreEqual(L"abc.xyz", psz);
	    psz = Path::GetFileName(L"..\\abc.xyz");
	    Assert::AreEqual(L"abc.xyz", psz);
	    psz = Path::GetFileName(L"ghi\\def\\abc.xyz");
	    Assert::AreEqual(L"abc.xyz", psz);
	    psz = Path::GetFileName(L".\\def\\abc.xyz");
	    Assert::AreEqual(L"abc.xyz", psz);
	    psz = Path::GetFileName(L"..\\def\\abc.xyz");
	    Assert::AreEqual(L"abc.xyz", psz);
	    psz = Path::GetFileName(L"\\ghi\\def\\abc.xyz");
	    Assert::AreEqual(L"abc.xyz", psz);
	    psz = Path::GetFileName(L"C:\\ghi\\def\\abc.xyz");
	    Assert::AreEqual(L"abc.xyz", psz);
	    psz = Path::GetFileName(L"\\\\remote\\share\\ghi\\def\\abc.xyz");
	    Assert::AreEqual(L"abc.xyz", psz);
	    psz = Path::GetFileName(L"");
	    Assert::AreEqual(L"", psz);
	}

	TEST_METHOD(TestGetFileNameWithoutExtension)
	{
	    PCWSTR psz;

	    psz = Path::GetFileNameWithoutExtension(L"abc.xyz");
	    Assert::AreEqual(L"abc", psz);
	    psz = Path::GetFileNameWithoutExtension(L"def\\abc.xyz");
	    Assert::AreEqual(L"abc", psz);
	    psz = Path::GetFileNameWithoutExtension(L".\\abc.xyz");
	    Assert::AreEqual(L"abc", psz);
	    psz = Path::GetFileNameWithoutExtension(L"..\\abc.xyz");
	    Assert::AreEqual(L"abc", psz);
	    psz = Path::GetFileNameWithoutExtension(L"ghi\\def\\abc.xyz");
	    Assert::AreEqual(L"abc", psz);
	    psz = Path::GetFileNameWithoutExtension(L".\\def\\abc.xyz");
	    Assert::AreEqual(L"abc", psz);
	    psz = Path::GetFileNameWithoutExtension(L"..\\def\\abc.xyz");
	    Assert::AreEqual(L"abc", psz);
	    psz = Path::GetFileNameWithoutExtension(L"\\ghi\\def\\abc.xyz");
	    Assert::AreEqual(L"abc", psz);
	    psz = Path::GetFileNameWithoutExtension(L"C:\\ghi\\def\\abc.xyz");
	    Assert::AreEqual(L"abc", psz);
	    psz = Path::GetFileNameWithoutExtension(L"\\\\remote\\share\\ghi\\def\\abc.xyz");
	    Assert::AreEqual(L"abc", psz);
	    psz = Path::GetFileNameWithoutExtension(L"abc");
	    Assert::AreEqual(L"abc", psz);
	    psz = Path::GetFileNameWithoutExtension(L"def\\abc");
	    Assert::AreEqual(L"abc", psz);
	    psz = Path::GetFileNameWithoutExtension(L"abc.uvw.xyz");
	    Assert::AreEqual(L"abc.uvw", psz);
	    psz = Path::GetFileNameWithoutExtension(L"def.uvw.xyz\\abc.uvw.xyz");
	    Assert::AreEqual(L"abc.uvw", psz);
	    psz = Path::GetFileNameWithoutExtension(L"abc.");
	    Assert::AreEqual(L"abc", psz);
	    psz = Path::GetFileNameWithoutExtension(L"");
	    Assert::AreEqual(L"", psz);
	}

	TEST_METHOD(TestGetExtension)
	{
	    PCWSTR psz;

	    psz = Path::GetExtension(L"abc.xyz");
	    Assert::AreEqual(L".xyz", psz);
	    psz = Path::GetExtension(L"abc.");
	    Assert::AreEqual(L".", psz);
	    psz = Path::GetExtension(L"abc");
	    Assert::AreEqual(L"", psz);
	    psz = Path::GetExtension(L"ghi.rst\\def.uvw\\abc.xyz");
	    Assert::AreEqual(L".xyz", psz);
	    psz = Path::GetExtension(L"");
	    Assert::AreEqual(L"", psz);
	}

	TEST_METHOD(TestGetDirectoryName)
	{
	    PCWSTR psz;

	    // DOS
	    psz = Path::GetDirectoryName(L"");
	    Assert::AreEqual(L".", psz);
	    psz = Path::GetDirectoryName(L".");
	    Assert::AreEqual(L"..", psz);
	    psz = Path::GetDirectoryName(L"abc.xyz");
	    Assert::AreEqual(L".", psz);
	    psz = Path::GetDirectoryName(L"\\abc.xyz");
	    Assert::AreEqual(L"\\", psz);
	    psz = Path::GetDirectoryName(L"C:\\abc.xyz");
	    Assert::AreEqual(L"C:\\", psz);
	    psz = Path::GetDirectoryName(L"def\\abc.xyz");
	    Assert::AreEqual(L"def", psz);
	    psz = Path::GetDirectoryName(L"\\def\\abc.xyz");
	    Assert::AreEqual(L"\\def", psz);
	    psz = Path::GetDirectoryName(L"C:\\def\\abc.xyz");
	    Assert::AreEqual(L"C:\\def", psz);
	    psz = Path::GetDirectoryName(L"ghi\\def\\abc.xyz");
	    Assert::AreEqual(L"ghi\\def", psz);
	    psz = Path::GetDirectoryName(L"\\ghi\\def\\abc.xyz");
	    Assert::AreEqual(L"\\ghi\\def", psz);
	    psz = Path::GetDirectoryName(L"C:\\ghi\\def\\abc.xyz");
	    Assert::AreEqual(L"C:\\ghi\\def", psz);
	    psz = Path::GetDirectoryName(L"abc.xyz\\");
	    Assert::AreEqual(L".", psz);
	    psz = Path::GetDirectoryName(L"\\abc.xyz\\");
	    Assert::AreEqual(L"\\", psz);
	    psz = Path::GetDirectoryName(L"C:\\abc.xyz\\");
	    Assert::AreEqual(L"C:\\", psz);
	    psz = Path::GetDirectoryName(L"def\\abc.xyz\\");
	    Assert::AreEqual(L"def", psz);
	    psz = Path::GetDirectoryName(L"\\def\\abc.xyz\\");
	    Assert::AreEqual(L"\\def", psz);
	    psz = Path::GetDirectoryName(L"C:\\def\\abc.xyz\\");
	    Assert::AreEqual(L"C:\\def", psz);
	    psz = Path::GetDirectoryName(L"ghi\\def\\abc.xyz\\");
	    Assert::AreEqual(L"ghi\\def", psz);
	    psz = Path::GetDirectoryName(L"\\ghi\\def\\abc.xyz\\");
	    Assert::AreEqual(L"\\ghi\\def", psz);
	    psz = Path::GetDirectoryName(L"C:\\ghi\\def\\abc.xyz\\");
	    Assert::AreEqual(L"C:\\ghi\\def", psz);
	    // UNC
	    psz = Path::GetDirectoryName(L"\\\\remote\\share");
	    Assert::AreEqual(L"\\\\remote\\share", psz);
	    psz = Path::GetDirectoryName(L"\\\\remote\\share\\");
	    Assert::AreEqual(L"\\\\remote\\share", psz);
	    psz = Path::GetDirectoryName(L"\\\\remote\\share\\abc.xyz");
	    Assert::AreEqual(L"\\\\remote\\share", psz);
	    psz = Path::GetDirectoryName(L"\\\\remote\\share\\def\\abc.xyz");
	    Assert::AreEqual(L"\\\\remote\\share\\def", psz);
	    psz = Path::GetDirectoryName(L"\\\\remote\\share\\ghi\\def\\abc.xyz");
	    Assert::AreEqual(L"\\\\remote\\share\\ghi\\def", psz);
	    // DOS with bEndsWithSeparator = true
	    psz = Path::GetDirectoryName(L"", true);
	    Assert::AreEqual(L".\\", psz);
	    psz = Path::GetDirectoryName(L".", true);
	    Assert::AreEqual(L"..\\", psz);
	    psz = Path::GetDirectoryName(L"abc.xyz", true);
	    Assert::AreEqual(L".\\", psz);
	    psz = Path::GetDirectoryName(L"\\abc.xyz", true);
	    Assert::AreEqual(L"\\", psz);
	    psz = Path::GetDirectoryName(L"C:\\abc.xyz", true);
	    Assert::AreEqual(L"C:\\", psz);
	    psz = Path::GetDirectoryName(L"def\\abc.xyz", true);
	    Assert::AreEqual(L"def\\", psz);
	    psz = Path::GetDirectoryName(L"\\def\\abc.xyz", true);
	    Assert::AreEqual(L"\\def\\", psz);
	    psz = Path::GetDirectoryName(L"C:\\def\\abc.xyz", true);
	    Assert::AreEqual(L"C:\\def\\", psz);
	    psz = Path::GetDirectoryName(L"ghi\\def\\abc.xyz", true);
	    Assert::AreEqual(L"ghi\\def\\", psz);
	    psz = Path::GetDirectoryName(L"\\ghi\\def\\abc.xyz", true);
	    Assert::AreEqual(L"\\ghi\\def\\", psz);
	    psz = Path::GetDirectoryName(L"C:\\ghi\\def\\abc.xyz", true);
	    Assert::AreEqual(L"C:\\ghi\\def\\", psz);
	    psz = Path::GetDirectoryName(L"abc.xyz\\", true);
	    Assert::AreEqual(L".\\", psz);
	    psz = Path::GetDirectoryName(L"\\abc.xyz\\", true);
	    Assert::AreEqual(L"\\", psz);
	    psz = Path::GetDirectoryName(L"C:\\abc.xyz\\", true);
	    Assert::AreEqual(L"C:\\", psz);
	    psz = Path::GetDirectoryName(L"def\\abc.xyz\\", true);
	    Assert::AreEqual(L"def\\", psz);
	    psz = Path::GetDirectoryName(L"\\def\\abc.xyz\\", true);
	    Assert::AreEqual(L"\\def\\", psz);
	    psz = Path::GetDirectoryName(L"C:\\def\\abc.xyz\\", true);
	    Assert::AreEqual(L"C:\\def\\", psz);
	    psz = Path::GetDirectoryName(L"ghi\\def\\abc.xyz\\", true);
	    Assert::AreEqual(L"ghi\\def\\", psz);
	    psz = Path::GetDirectoryName(L"\\ghi\\def\\abc.xyz\\", true);
	    Assert::AreEqual(L"\\ghi\\def\\", psz);
	    psz = Path::GetDirectoryName(L"C:\\ghi\\def\\abc.xyz\\", true);
	    Assert::AreEqual(L"C:\\ghi\\def\\", psz);
	    // UNC with bEndsWithSeparator = true
	    psz = Path::GetDirectoryName(L"\\\\remote\\share", true);
	    Assert::AreEqual(L"\\\\remote\\share\\", psz);
	    psz = Path::GetDirectoryName(L"\\\\remote\\share\\", true);
	    Assert::AreEqual(L"\\\\remote\\share\\", psz);
	    psz = Path::GetDirectoryName(L"\\\\remote\\share\\abc.xyz", true);
	    Assert::AreEqual(L"\\\\remote\\share\\", psz);
	    psz = Path::GetDirectoryName(L"\\\\remote\\share\\def\\abc.xyz", true);
	    Assert::AreEqual(L"\\\\remote\\share\\def\\", psz);
	    psz = Path::GetDirectoryName(L"\\\\remote\\share\\ghi\\def\\abc.xyz", true);
	    Assert::AreEqual(L"\\\\remote\\share\\ghi\\def\\", psz);
	}

	TEST_METHOD(TestCombine)
	{
	    PCWSTR psz;

	    psz = Path::Combine(L"a", L"bcd");
	    Assert::AreEqual(L"a\\bcd", psz);
	    psz = Path::Combine(L"a", L"bcd", 1);
	    Assert::AreEqual(L"a\\b", psz);
	    psz = Path::Combine(L"a", L"b", L"cde");
	    Assert::AreEqual(L"a\\b\\cde", psz);
	    psz = Path::Combine(L"a", L"b", L"cde", 1);
	    Assert::AreEqual(L"a\\b\\c", psz);
	    psz = Path::Combine(L"a", L"b", L"c", L"def");
	    Assert::AreEqual(L"a\\b\\c\\def", psz);
	    psz = Path::Combine(L"a", L"b", L"c", L"def", 1);
	    Assert::AreEqual(L"a\\b\\c\\d", psz);
	    psz = Path::Combine(L"a", L"b", L"c", L"d", L"efg");
	    Assert::AreEqual(L"a\\b\\c\\d\\efg", psz);
	    psz = Path::Combine(L"a", L"b", L"c", L"d", L"efg", 1);
	    Assert::AreEqual(L"a\\b\\c\\d\\e", psz);
	}

	TEST_METHOD(TestGetKnownFolder)
	{
	    PCWSTR psz;
	    WCHAR buf[260];

	    psz = Path::GetKnownFolder(FOLDERID_Windows);
	    Assert::AreEqual(L"C:\\WINDOWS", ToUpper(psz, buf));
	    psz = Path::GetKnownFolder(FOLDERID_ProgramFilesX64);
	    Assert::AreEqual(L"C:\\PROGRAM FILES", ToUpper(psz, buf));
	    psz = Path::GetKnownFolder(FOLDERID_ProgramFilesX86);
	    Assert::AreEqual(L"C:\\PROGRAM FILES (X86)", ToUpper(psz, buf));
	}

	TEST_METHOD(TestCompare)
	{
	    int rc;

	    rc = Path::Compare(L"abc", L"xyz");
	    Assert::IsTrue(rc < 0);
	    rc = Path::Compare(L"abc", L"XYZ");
	    Assert::IsTrue(rc < 0);
	    rc = Path::Compare(L"ABC", L"xyz");
	    Assert::IsTrue(rc < 0);
	    rc = Path::Compare(L"ABC", L"XYZ");
	    Assert::IsTrue(rc < 0);
	    rc = Path::Compare(L"abc", L"abc");
	    Assert::IsTrue(rc == 0);
	    rc = Path::Compare(L"abc", L"ABC");
	    Assert::IsTrue(rc == 0);
	    rc = Path::Compare(L"ABC", L"abc");
	    Assert::IsTrue(rc == 0);
	    rc = Path::Compare(L"ABC", L"ABC");
	    Assert::IsTrue(rc == 0);
	    rc = Path::Compare(L"xyz", L"abc");
	    Assert::IsTrue(rc > 0);
	    rc = Path::Compare(L"xyz", L"ABC");
	    Assert::IsTrue(rc > 0);
	    rc = Path::Compare(L"XYZ", L"abc");
	    Assert::IsTrue(rc > 0);
	    rc = Path::Compare(L"XYZ", L"ABC");
	    Assert::IsTrue(rc > 0);
	    rc = Path::Compare(L"abc", L"abcd");
	    Assert::IsTrue(rc < 0);
	    rc = Path::Compare(L"abc", L"ABCD");
	    Assert::IsTrue(rc < 0);
	    rc = Path::Compare(L"ABC", L"abcd");
	    Assert::IsTrue(rc < 0);
	    rc = Path::Compare(L"ABC", L"ABCD");
	    Assert::IsTrue(rc < 0);
	    rc = Path::Compare(L"abcd", L"abc");
	    Assert::IsTrue(rc > 0);
	    rc = Path::Compare(L"abcd", L"ABC");
	    Assert::IsTrue(rc > 0);
	    rc = Path::Compare(L"ABCD", L"abc");
	    Assert::IsTrue(rc > 0);
	    rc = Path::Compare(L"ABCD", L"ABC");
	    Assert::IsTrue(rc > 0);
	}

	TEST_METHOD(TestWildcardMatch)
	{
	    bool result;

	    result = Path::WildcardMatch(L"", L"");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"abc", L"abc");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"abc", L"abcxyz");
	    Assert::IsFalse(result);
	    result = Path::WildcardMatch(L"abcxyz", L"abc");
	    Assert::IsFalse(result);
	    result = Path::WildcardMatch(L"*", L"");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"*", L"a");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"*", L"abc");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"?", L"");
	    Assert::IsFalse(result);
	    result = Path::WildcardMatch(L"?", L"a");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"?", L"abc");
	    Assert::IsFalse(result);
	    result = Path::WildcardMatch(L"abc*", L"abc");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"abc*", L"abca");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"abc*", L"abcabc");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"abc?", L"abc");
	    Assert::IsFalse(result);
	    result = Path::WildcardMatch(L"abc?", L"abca");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"abc?", L"abcabc");
	    Assert::IsFalse(result);
	    result = Path::WildcardMatch(L"*abc", L"abc");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"*abc", L"aabc");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"*abc", L"abcabc");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"?abc", L"abc");
	    Assert::IsFalse(result);
	    result = Path::WildcardMatch(L"?abc", L"aabc");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"?abc", L"abcabc");
	    Assert::IsFalse(result);
	    result = Path::WildcardMatch(L"abc*xyz", L"abcxyz");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"abc*xyz", L"abcoxyz");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"abc*xyz", L"abcopqxyz");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"abc?xyz", L"abcxyz");
	    Assert::IsFalse(result);
	    result = Path::WildcardMatch(L"abc?xyz", L"abcoxyz");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"abc?xyz", L"abcopqxyz");
	    Assert::IsFalse(result);
	    result = Path::WildcardMatch(L"abc*xyz*", L"abcxyz");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"abc*xyz*", L"abcxyzr");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"abc*xyz*", L"abcxyzrst");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"abc*xyz*", L"abcoxyz");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"abc*xyz*", L"abcopqxyz");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"abc*xyz*", L"abcoxyzr");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"abc*xyz*", L"abcopqxyzrst");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"*abc*xyz", L"abcxyz");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"*abc*xyz", L"aabcxyz");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"*abc*xyz", L"abcabcxyz");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"*abc*xyz", L"abcxxyz");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"*abc*xyz", L"abcxyzxyz");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"*abc*xyz", L"aabcxxyz");
	    Assert::IsTrue(result);
	    result = Path::WildcardMatch(L"*abc*xyz", L"abcabcxyzxyz");
	    Assert::IsTrue(result);
	}

    private:

	template<size_t N>
	PWSTR ToUpper(PCWSTR psz, WCHAR (&szBuf)[N])
	{
	    wcscpy_s(szBuf, psz);
	    errno_t err = _wcsupr_s(szBuf);
	    if (err)
	    {
		throw Exception(L"ToUpper: _wcsupr_s failed.");
	    }
	    return szBuf;
	}
    };
}
