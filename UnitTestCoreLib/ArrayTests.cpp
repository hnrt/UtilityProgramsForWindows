#include "pch.h"
#include "CppUnitTest.h"
#include "hnrt/Array.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace hnrt;

namespace UnitTestCoreLib
{
	class Quux
	{
	public:
		Quux() : m_id(++c), m_a(100), m_b(true) {}
		Quux(int a, bool b) : m_id(++c), m_a(a), m_b(b) {}
		Quux(const Quux& src) : m_id(++c), m_a(src.m_a), m_b(src.m_b) {}
		~Quux() { f[d++] = m_id; }
	public:
		size_t m_id;
		int m_a;
		bool m_b;
		static size_t c;
		static size_t d;
		static size_t f[100];
		static void Init();
	};

	size_t Quux::c = 0;
	size_t Quux::d = 0;
	size_t Quux::f[100] = { 0 };

	void Quux::Init()
	{
		c = 0;
		d = 0;
		memset(f, 0, sizeof(f));
	}

	TEST_CLASS(ArrayTests)
	{
	public:

		TEST_METHOD(Test0000)
		{
			Quux::Init();
			Array<Quux> x;
			Assert::AreEqual(0UL, x.Capacity);
			Assert::AreEqual(0UL, x.Length);
		}

		TEST_METHOD(Test0001)
		{
			Quux::Init();
			Array<Quux> x(13);
			Assert::AreEqual(13UL, x.Capacity);
			Assert::AreEqual(0UL, x.Length);
		}

		TEST_METHOD(Test0002)
		{
			Quux::Init();
			Array<Quux> x(17);
			Array<Quux> y(x);
			Assert::AreEqual(17UL, y.Capacity);
			Assert::AreEqual(0UL, y.Length);
		}

		TEST_METHOD(Test0003)
		{
			Quux::Init();
			Array<Quux> x(17);
			Array<Quux> y;
			x = y;
			Assert::AreEqual(0UL, x.Capacity);
			Assert::AreEqual(0UL, x.Length);
		}

		TEST_METHOD(Test0004)
		{
			Quux::Init();
			Array<Quux> x;
			x += Quux(23, true);
			Assert::AreEqual(32UL, x.Capacity);
			Assert::AreEqual(1UL, x.Length);
			Assert::AreEqual(23, x[0].m_a);
			Assert::IsTrue(x[0].m_b);
		}

		TEST_METHOD(Test0005)
		{
			Quux::Init();
			Array<Quux> x;
			x += Quux(23, true);
			x += Quux(29, false);
			Assert::AreEqual(32UL, x.Capacity);
			Assert::AreEqual(2UL, x.Length);
			Assert::AreEqual(23, x[0].m_a);
			Assert::IsTrue(x[0].m_b);
			Assert::AreEqual(29, x[1].m_a);
			Assert::IsFalse(x[1].m_b);
		}

		TEST_METHOD(Test0006)
		{
			Quux::Init();
			Array<Quux> x;
			x.PushBack(Quux(23, true))
				.PushBack(Quux(29, false))
				.PushBack(Quux(13, false))
				.PushBack(Quux(19, true));
			Assert::AreEqual(32UL, x.Capacity);
			Assert::AreEqual(4UL, x.Length);
			Assert::AreEqual(23, x[0].m_a);
			Assert::IsTrue(x[0].m_b);
			Assert::AreEqual(29, x[1].m_a);
			Assert::IsFalse(x[1].m_b);
			Assert::AreEqual(13, x[2].m_a);
			Assert::IsFalse(x[2].m_b);
			Assert::AreEqual(19, x[3].m_a);
			Assert::IsTrue(x[3].m_b);
		}

		TEST_METHOD(Test0007)
		{
			Quux::Init();
			Array<Quux> x;
			x.PushBack(Quux(23, true));
			x.PushBack(Quux(29, false));
			x.PushBack(Quux(13, false));
			x.PushBack(Quux(19, true));
			x.SetCapacity(2);
			Assert::AreEqual(2UL, x.Capacity);
			Assert::AreEqual(2UL, x.Length);
			Assert::AreEqual(23, x[0].m_a);
			Assert::IsTrue(x[0].m_b);
			Assert::AreEqual(29, x[1].m_a);
			Assert::IsFalse(x[1].m_b);
			Assert::AreEqual(8ULL, Quux::c);
			Assert::AreEqual(6ULL, Quux::d);
			Assert::AreEqual(1ULL, Quux::f[0]);
			Assert::AreEqual(3ULL, Quux::f[1]);
			Assert::AreEqual(5ULL, Quux::f[2]);
			Assert::AreEqual(7ULL, Quux::f[3]);
			Assert::AreEqual(8ULL, Quux::f[4]);
			Assert::AreEqual(6ULL, Quux::f[5]);
		}

		TEST_METHOD(Test0008)
		{
			Quux::Init();
			Array<Quux> x;
			x.SetLength(2);
			Assert::AreEqual(32UL, x.Capacity);
			Assert::AreEqual(2UL, x.Length);
			Assert::AreEqual(100, x[0].m_a);
			Assert::IsTrue(x[0].m_b);
			Assert::AreEqual(100, x[1].m_a);
			Assert::IsTrue(x[1].m_b);
			Assert::AreEqual(2ULL, Quux::c);
			Assert::AreEqual(0ULL, Quux::d);
		}

		TEST_METHOD(Test0009)
		{
			Quux::Init();
			Array<Quux> x;
			x.SetLength(2);
			x.SetLength(0);
			Assert::AreEqual(32UL, x.Capacity);
			Assert::AreEqual(0UL, x.Length);
			Assert::AreEqual(2ULL, Quux::c);
			Assert::AreEqual(2ULL, Quux::d);
			Assert::AreEqual(2ULL, Quux::f[0]);
			Assert::AreEqual(1ULL, Quux::f[1]);
		}

		TEST_METHOD(Test0010)
		{
			Quux::Init();
			Array<Quux> x;
			x.SetLength(2);
			x.SetCapacity(0);
			Assert::AreEqual(0UL, x.Capacity);
			Assert::AreEqual(0UL, x.Length);
			Assert::AreEqual(2ULL, Quux::c);
			Assert::AreEqual(2ULL, Quux::d);
			Assert::AreEqual(2ULL, Quux::f[0]);
			Assert::AreEqual(1ULL, Quux::f[1]);
		}

		TEST_METHOD(Test0011)
		{
			Quux::Init();
			{
				Array<Quux> x;
				x += Quux(200, false);
				x += Quux(250, false);
				x.Length = 32;
				x += Quux(300, false);
				x[-2] = Quux(400, true);
				Assert::AreEqual(64UL, x.Capacity);
				Assert::AreEqual(33UL, x.Length);
				Assert::AreEqual(200, x[0].m_a);
				Assert::IsFalse(x[0].m_b);
				Assert::AreEqual(250, x[1].m_a);
				Assert::IsFalse(x[1].m_b);
				Assert::AreEqual(100, x[2].m_a);
				Assert::IsTrue(x[2].m_b);
				Assert::AreEqual(400, x[31].m_a);
				Assert::IsTrue(x[31].m_b);
				Assert::AreEqual(300, x[32].m_a);
				Assert::IsFalse(x[32].m_b);
				Assert::AreEqual(300, x[-1].m_a);
				Assert::IsFalse(x[-1].m_b);
				Assert::AreEqual(400, x[-2].m_a);
				Assert::IsTrue(x[-2].m_b);
				Assert::AreEqual(37ULL, Quux::c);
				Assert::AreEqual(4ULL, Quux::d);
			}
			Assert::AreEqual(37ULL, Quux::c);
			Assert::AreEqual(37ULL, Quux::d);
		}

		TEST_METHOD(Test0012)
		{
			Quux::Init();
			Array<Quux> x;
			Assert::AreEqual(0UL, x.Capacity);
			Assert::AreEqual(0UL, x.Length);
			x += Quux(200, false);
			x += Quux(250, false);
			Array<Quux> y;
			x += y;
			Assert::AreEqual(32UL, x.Capacity);
			Assert::AreEqual(2UL, x.Length);
			y += Quux(300, true);
			y += Quux(350, true);
			x += y;
			Assert::AreEqual(32UL, x.Capacity);
			Assert::AreEqual(4UL, x.Length);
			Assert::AreEqual(200, x[0].m_a);
			Assert::IsFalse(x[0].m_b);
			Assert::AreEqual(250, x[1].m_a);
			Assert::IsFalse(x[1].m_b);
			Assert::AreEqual(300, x[2].m_a);
			Assert::IsTrue(x[2].m_b);
			Assert::AreEqual(350, x[3].m_a);
			Assert::IsTrue(x[3].m_b);
		}
	};
}
