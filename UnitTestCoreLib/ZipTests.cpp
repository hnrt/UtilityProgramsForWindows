#include "pch.h"
#include "CppUnitTest.h"
#include "hnrt/ZipFile.h"
#include "hnrt/Debug.h"
#include "hnrt/Path.h"
#include "hnrt/StringCollection.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace hnrt;

namespace UnitTestCoreLib
{
	struct MyZipForEachCallback : IZipForEachCallback
	{
		StringCollection entries;

		MyZipForEachCallback()
			: entries()
		{
		}

		virtual void ZipForEach(ZipArchiveEntry& entry)
		{
			DebugFunc __func(L"MyZipForEachCallback::ZipForEach");
			Debug::Put(L"name=%s type=%s size=%ld", entry.Name, entry.Type, entry.Size);
			if (entry.IsFolder)
			{
				Debug::Put(L"Beginning of Folder %s", entry.Name);
				MyZipForEachCallback sub;
				entry.ForEach(sub);
				for (DWORD dwIndex = 0; dwIndex < sub.entries.Count; dwIndex++)
				{
					entries.Add(String(CONCAT3, entry.Name, String(L"/"), sub.entries[dwIndex]));
				}
				Debug::Put(L"End of Folder %s", entry.Name);
			}
			else
			{
				entries.Add(entry.Name);
			}
		}
	};

	struct MyZipExtractCallbacks : IZipExtractCallbacks
	{
		virtual void ZipExtractOnStart(PCWSTR, LONG)
		{
		}
		virtual void ZipExtractOnSuccess(PCWSTR pszPath)
		{
		}
		virtual void ZipExtractOnFailure(PCWSTR pszPath, HRESULT hr)
		{
		}
	};

	TEST_CLASS(ZipTests)
	{
	public:

		TEST_CLASS_INITIALIZE(Initialize)
		{
			DebugFunc func(L"ZipTests::Initialize");
			String strTemporaryDirectory = Path::GetTemporaryDirectory(L"ZipTests");
			Path::CreateDirectory(strTemporaryDirectory);
			Debug::Put(L"TemporaryDirectory=%s", strTemporaryDirectory);
		}

		TEST_CLASS_CLEANUP(CleanUp)
		{
			DebugFunc func(L"ZipTests::CleanUp");
		}

		TEST_METHOD_INITIALIZE(BeforeTest)
		{
			DebugFunc func(L"ZipTests::BeforeTest");
		}

		TEST_METHOD_CLEANUP(AfterTest)
		{
			DebugFunc func(L"ZipTests::AfterTest");
		}

		TEST_METHOD(Test01)
		{
			String path = GetZipArchivePath(L"arch1.zip");
			Assert::AreEqual(true, Path::Exists(path));
			ZipArchive* p = ZipFile::OpenRead(path);
			MyZipForEachCallback cbs;
			p->ForEach(cbs);
			delete p;
			Assert::AreEqual(4UL, cbs.entries.Count);
			Assert::AreEqual(true, cbs.entries.Contains(L"xyzzy.txt"));
			Assert::AreEqual(true, cbs.entries.Contains(L"quux.txt"));
			Assert::AreEqual(true, cbs.entries.Contains(L"2001/fred.txt"));
			Assert::AreEqual(true, cbs.entries.Contains(L"2001/waldo.docx"));
			Assert::AreEqual(false, cbs.entries.Contains(L"foo.txt"));
			Assert::AreEqual(false, cbs.entries.Contains(L"2001/bar.txt"));
			Assert::AreEqual(false, cbs.entries.Contains(L"9999/baz.txt"));
		}

		TEST_METHOD(Test02)
		{
			String strOutputDirectory = Path::GetTemporaryDirectory(L"ZipTests", L"Test02");
			Path::RemoveDirectory(strOutputDirectory);
			Assert::AreEqual(false, Path::Exists(strOutputDirectory));
			String path = GetZipArchivePath(L"arch1.zip");
			Assert::AreEqual(true, Path::Exists(path));
			MyZipExtractCallbacks cbs;
			ZipFile::ExtractFiles(path, L"*", strOutputDirectory, cbs);
			Assert::AreEqual(true, Path::Exists(Path::Combine(strOutputDirectory, L"xyzzy.txt")));
			Assert::AreEqual(true, Path::Exists(Path::Combine(strOutputDirectory, L"quux.txt")));
			Assert::AreEqual(true, Path::Exists(Path::Combine(strOutputDirectory, L"2001", L"fred.txt")));
			Assert::AreEqual(true, Path::Exists(Path::Combine(strOutputDirectory, L"2001", L"waldo.docx")));
			Assert::AreEqual(false, Path::Exists(Path::Combine(strOutputDirectory, L"foo.txt")));
			Assert::AreEqual(false, Path::Exists(Path::Combine(strOutputDirectory, L"2001", L"bar.txt")));
			Assert::AreEqual(false, Path::Exists(Path::Combine(strOutputDirectory, L"9999", L"baz.txt")));
		}

		TEST_METHOD(Test03)
		{
			String strOutputDirectory = Path::GetTemporaryDirectory(L"ZipTests", L"Test03");
			Path::RemoveDirectory(strOutputDirectory);
			Assert::AreEqual(false, Path::Exists(strOutputDirectory));
			String path = GetZipArchivePath(L"arch1.zip");
			Assert::AreEqual(true, Path::Exists(path));
			MyZipExtractCallbacks cbs;
			ZipFile::ExtractFiles(path, L"*.txt", strOutputDirectory, cbs);
			Assert::AreEqual(true, Path::Exists(Path::Combine(strOutputDirectory, L"xyzzy.txt")));
			Assert::AreEqual(true, Path::Exists(Path::Combine(strOutputDirectory, L"quux.txt")));
			Assert::AreEqual(true, Path::Exists(Path::Combine(strOutputDirectory, L"2001", L"fred.txt")));
			Assert::AreEqual(false, Path::Exists(Path::Combine(strOutputDirectory, L"2001", L"waldo.docx")));
			Assert::AreEqual(false, Path::Exists(Path::Combine(strOutputDirectory, L"foo.txt")));
			Assert::AreEqual(false, Path::Exists(Path::Combine(strOutputDirectory, L"2001", L"bar.txt")));
			Assert::AreEqual(false, Path::Exists(Path::Combine(strOutputDirectory, L"9999", L"baz.txt")));
		}

		TEST_METHOD(Test04)
		{
			String strOutputDirectory = Path::GetTemporaryDirectory(L"ZipTests", L"Test04");
			Path::RemoveDirectory(strOutputDirectory);
			Assert::AreEqual(false, Path::Exists(strOutputDirectory));
			String path = GetZipArchivePath(L"arch1.zip");
			Assert::AreEqual(true, Path::Exists(path));
			MyZipExtractCallbacks cbs;
			ZipFile::ExtractFiles(path, L"2001", strOutputDirectory, cbs);
			Assert::AreEqual(false, Path::Exists(Path::Combine(strOutputDirectory, L"xyzzy.txt")));
			Assert::AreEqual(false, Path::Exists(Path::Combine(strOutputDirectory, L"quux.txt")));
			Assert::AreEqual(true, Path::Exists(Path::Combine(strOutputDirectory, L"2001", L"fred.txt")));
			Assert::AreEqual(true, Path::Exists(Path::Combine(strOutputDirectory, L"2001", L"waldo.docx")));
			Assert::AreEqual(false, Path::Exists(Path::Combine(strOutputDirectory, L"foo.txt")));
			Assert::AreEqual(false, Path::Exists(Path::Combine(strOutputDirectory, L"2001", L"bar.txt")));
			Assert::AreEqual(false, Path::Exists(Path::Combine(strOutputDirectory, L"9999", L"baz.txt")));
		}

		String GetZipArchivePath(PCWSTR pszFileName)
		{
			String curDir = Path::GetCurrentDirectory();
			Debug::Put(L"CurrentDirectory=%s", curDir);
			String platformDir = Path::GetDirectoryName(curDir);
			String binDir = Path::GetDirectoryName(platformDir);
			String projectDir = Path::GetDirectoryName(binDir);
			String path = Path::Combine(projectDir, L"data", pszFileName);
			Debug::Put(L"ZipArchive=%s", path);
			return path;
		}
	};
}
