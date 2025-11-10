#include "pch.h"
#include "CppUnitTest.h"
#include "hnrt/SemanticVersion.h"
#include "hnrt/Debug.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace hnrt;

namespace UnitTestCoreLib
{
	TEST_CLASS(SemanticVersionTests)
	{
	public:

		TEST_METHOD(Test00101)
		{
			SemanticVersion v(L"@");
			Assert::AreEqual(-1, v.Level);
		}

		TEST_METHOD(Test00102)
		{
			SemanticVersion v(L"00");
			Assert::AreEqual(-1, v.Level);
		}

		TEST_METHOD(Test00103)
		{
			SemanticVersion v(L"0/0");
			Assert::AreEqual(-1, v.Level);
		}

		TEST_METHOD(Test00104)
		{
			SemanticVersion v(L"0/0/0");
			Assert::AreEqual(-1, v.Level);
		}

		TEST_METHOD(Test00201)
		{
			SemanticVersion v1(L"");
			Assert::AreEqual(0, v1.Level);
		}

		TEST_METHOD(Test00301)
		{
			SemanticVersion v(L"0");
			Assert::AreEqual(1, v.Level);
		}

		TEST_METHOD(Test00302)
		{
			SemanticVersion v(L"1");
			Assert::AreEqual(1, v.Level);
		}

		TEST_METHOD(Test00303)
		{
			SemanticVersion v(L"10");
			Assert::AreEqual(1, v.Level);
		}

		TEST_METHOD(Test00304)
		{
			SemanticVersion v(L"100");
			Assert::AreEqual(1, v.Level);
		}

		TEST_METHOD(Test00305)
		{
			SemanticVersion v(L"3-SNAPSHOT");
			Assert::AreEqual(1, v.Level);
		}

		TEST_METHOD(Test00306)
		{
			SemanticVersion v(L"4+EXPERIMENTAL");
			Assert::AreEqual(1, v.Level);
		}

		TEST_METHOD(Test00401)
		{
			SemanticVersion v(L"0.0");
			Assert::AreEqual(2, v.Level);
		}

		TEST_METHOD(Test00402)
		{
			SemanticVersion v(L"10.20");
			Assert::AreEqual(2, v.Level);
		}

		TEST_METHOD(Test00403)
		{
			SemanticVersion v(L"100.200");
			Assert::AreEqual(2, v.Level);
		}

		TEST_METHOD(Test00404)
		{
			SemanticVersion v(L"0.0-alpha-1");
			Assert::AreEqual(2, v.Level);
			Assert::AreEqual(L"alpha-1", v.PreRelease);
			Assert::AreEqual(L"", v.Build);
		}

		TEST_METHOD(Test00405)
		{
			SemanticVersion v(L"1.0-beta-3+sha256");
			Assert::AreEqual(2, v.Level);
			Assert::AreEqual(L"beta-3", v.PreRelease);
			Assert::AreEqual(L"sha256", v.Build);
		}

		TEST_METHOD(Test00406)
		{
			SemanticVersion v(L"2.113+md5.789");
			Assert::AreEqual(2, v.Level);
			Assert::AreEqual(L"", v.PreRelease);
			Assert::AreEqual(L"md5.789", v.Build);
		}

		TEST_METHOD(Test00501)
		{
			SemanticVersion v(L"0.0.0");
			Assert::AreEqual(3, v.Level);
		}

		TEST_METHOD(Test00502)
		{
			SemanticVersion v(L"10.20.30");
			Assert::AreEqual(3, v.Level);
		}

		TEST_METHOD(Test00503)
		{
			SemanticVersion v(L"100.200.334");
			Assert::AreEqual(3, v.Level);
		}

		TEST_METHOD(Test00504)
		{
			SemanticVersion v(L"0.0.0-alpha.1");
			Assert::AreEqual(3, v.Level);
			Assert::AreEqual(L"alpha.1", v.PreRelease);
			Assert::AreEqual(L"", v.Build);
		}

		TEST_METHOD(Test00505)
		{
			SemanticVersion v(L"1.0.1-beta.73+xyz256.3-9");
			Assert::AreEqual(3, v.Level);
			Assert::AreEqual(L"beta.73", v.PreRelease);
			Assert::AreEqual(L"xyz256.3-9", v.Build);
		}

		TEST_METHOD(Test00506)
		{
			SemanticVersion v(L"2.113.0+md5.789");
			Assert::AreEqual(3, v.Level);
			Assert::AreEqual(L"", v.PreRelease);
			Assert::AreEqual(L"md5.789", v.Build);
		}

		TEST_METHOD(Test00601)
		{
			SemanticVersion v1(L"@");
			SemanticVersion v2(L"$");
			Assert::AreEqual(true, v1 == v2);

		}

		TEST_METHOD(Test00602)
		{
			SemanticVersion v1(L"");
			SemanticVersion v2(L"$");
			Assert::AreEqual(true, v1 > v2);

		}

		TEST_METHOD(Test00603)
		{
			SemanticVersion v1(L"");
			SemanticVersion v2(L"0");
			Assert::AreEqual(true, v1 > v2);

		}

		TEST_METHOD(Test00604)
		{
			SemanticVersion v1(L"");
			SemanticVersion v2(L"1.0");
			Assert::AreEqual(true, v1 > v2);

		}

		TEST_METHOD(Test00605)
		{
			SemanticVersion v1(L"");
			SemanticVersion v2(L"1.0.0");
			Assert::AreEqual(true, v1 > v2);

		}

		TEST_METHOD(Test00606)
		{
			SemanticVersion v1(L"4");
			SemanticVersion v2(L"4");
			Assert::AreEqual(true, v1 == v2);

		}

		TEST_METHOD(Test00607)
		{
			SemanticVersion v1(L"5");
			SemanticVersion v2(L"4");
			Assert::AreEqual(true, v1 > v2);

		}

		TEST_METHOD(Test00608)
		{
			SemanticVersion v1(L"4");
			SemanticVersion v2(L"4.0");
			Assert::AreEqual(true, v1 < v2);

		}

		TEST_METHOD(Test00609)
		{
			SemanticVersion v1(L"5");
			SemanticVersion v2(L"4.0");
			Assert::AreEqual(true, v1 > v2);

		}

		TEST_METHOD(Test00610)
		{
			SemanticVersion v1(L"5.0");
			SemanticVersion v2(L"5.0.0");
			Assert::AreEqual(true, v1 < v2);

		}

		TEST_METHOD(Test00611)
		{
			SemanticVersion v1(L"5.1");
			SemanticVersion v2(L"5.0.0");
			Assert::AreEqual(true, v1 > v2);

		}

		TEST_METHOD(Test00612)
		{
			SemanticVersion v1(L"5.1.0");
			SemanticVersion v2(L"5.1.0");
			Assert::AreEqual(true, v1 == v2);

		}

		TEST_METHOD(Test00613)
		{
			SemanticVersion v1(L"5.1.0");
			SemanticVersion v2(L"5.1.1");
			Assert::AreEqual(true, v1 < v2);

		}

		TEST_METHOD(Test00614)
		{
			SemanticVersion v1(L"5.3.0");
			SemanticVersion v2(L"5.1.4");
			Assert::AreEqual(true, v1 > v2);

		}

		TEST_METHOD(Test00615)
		{
			SemanticVersion v1(L"5.3.0");
			SemanticVersion v2(L"6.1.4");
			Assert::AreEqual(true, v1 < v2);

		}

		TEST_METHOD(Test00616)
		{
			SemanticVersion v1(L"5.4.0-rc.78");
			SemanticVersion v2(L"5.4.0");
			Assert::AreEqual(true, v1 < v2);

		}

		TEST_METHOD(Test00617)
		{
			SemanticVersion v1(L"5.4.0-rc.78");
			SemanticVersion v2(L"5.4.0-rc.222");
			Assert::AreEqual(true, v1 < v2);

		}

		TEST_METHOD(Test00618)
		{
			SemanticVersion v1(L"5.4.0-alpha-789");
			SemanticVersion v2(L"5.4.0-beta-333");
			Assert::AreEqual(true, v1 < v2);

		}

		TEST_METHOD(Test00619)
		{
			SemanticVersion v1(L"6.0.0-alpha-789");
			SemanticVersion v2(L"5.4.0-beta-333");
			Assert::AreEqual(true, v1 > v2);

		}
	};
}
