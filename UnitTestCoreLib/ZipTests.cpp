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
	struct MyFolderItemForEachCallback : public IFolderItemForEachCallback
	{
		StringCollection entries;

		MyFolderItemForEachCallback()
			: entries()
		{
		}

		virtual BOOL ForEach(FolderItemPtr& pFolderItem)
		{
			DebugFunc __func(L"MyZipForEachCallback::ZipForEach");
			if (pFolderItem.IsFolder)
			{
				Debug::Put(L"Beginning of Folder %s", pFolderItem.Name);
				MyFolderItemForEachCallback sub;
				pFolderItem.ForEach(sub);
				for (DWORD dwIndex = 0; dwIndex < sub.entries.Length; dwIndex++)
				{
					entries.Add(String(CONCAT3, pFolderItem.Name, String(L"/"), sub.entries[dwIndex]));
				}
				Debug::Put(L"End of Folder %s", pFolderItem.Name);
			}
			else
			{
				Debug::Put(L"name=%s type=%s size=%ld", pFolderItem.Name, pFolderItem.Type, pFolderItem.Size);
				entries.Add(pFolderItem.Name);
			}
			return TRUE;
		}
	};

	struct MyZipExtractCallbacks : IFolderItemProcessCallbacks
	{
		virtual void OnStart(PCWSTR, LONG)
		{
		}
		virtual void OnSuccess(PCWSTR pszPath)
		{
		}
		virtual void OnFailure(PCWSTR pszPath, HRESULT hr)
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
			ZipFile zip = ZipFile::Open(path);
			MyFolderItemForEachCallback cbs;
			zip.ForEach(cbs);
			StringCollection& entries = cbs.entries;
			Assert::AreEqual(4UL, entries.Length);
			Assert::AreEqual(true, entries.Contains(L"xyzzy.txt"));
			Assert::AreEqual(true, entries.Contains(L"quux.txt"));
			Assert::AreEqual(true, entries.Contains(L"2001/fred.txt"));
			Assert::AreEqual(true, entries.Contains(L"2001/waldo.docx"));
			Assert::AreEqual(false, entries.Contains(L"foo.txt"));
			Assert::AreEqual(false, entries.Contains(L"2001/bar.txt"));
			Assert::AreEqual(false, entries.Contains(L"9999/baz.txt"));
		}

		TEST_METHOD(Test02)
		{
			String strOutputDirectory = Path::GetTemporaryDirectory(L"ZipTests", L"Test02");
			Path::RemoveDirectory(strOutputDirectory);
			Assert::AreEqual(false, Path::Exists(strOutputDirectory));
			String path = GetZipArchivePath(L"arch1.zip");
			Assert::AreEqual(true, Path::Exists(path));
			ZipFile zip = ZipFile::Open(path);
			MyZipExtractCallbacks cbs;
			zip.ExtractFiles(L"*", strOutputDirectory, cbs);
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
			ZipFile zip = ZipFile::Open(path);
			MyZipExtractCallbacks cbs;
			zip.ExtractFiles(L"*.txt", strOutputDirectory, cbs);
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
			ZipFile zip = ZipFile::Open(path);
			MyZipExtractCallbacks cbs;
			zip.ExtractFiles(L"2001", strOutputDirectory, cbs);
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
