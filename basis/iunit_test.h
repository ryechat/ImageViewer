#pragma once
#ifndef GUID_6982DB1EB5D04F939E5EFCBA3B0D17EA
#define GUID_6982DB1EB5D04F939E5EFCBA3B0D17EA
/*! @file
	iunit_test.h contains definition of
	IUnitTest, unit test class interface.

	This codes shows how to make unit tests.
	@par
	@code
	// --- Class declaration part
	that can be divided to header file --- //
	#include "CBase.h"

	#include "iunit_test.h"
	UNIT_TEST(CDerived)

	class CDerived : public CBase
	{
	public:
		bool derived();
	};

	// -------------- Class definition -------------- //
	bool derived();

	// -------------- Unit test definition -------------- //
	UNIT_TEST_FUNC(CDerived)
	{
		CDerived obj;
		if (obj.derived() != true)
			throw 0;

		return true;
	}
	@endcode
	
	Let's see in detail.

	@code
	#include "CBase.h"
	@endcode
	If CBase also has a unit test defined in "CBase.h",
	CBase will be certainly tested before CDerived tested.

	@code
	UNIT_TEST(CDerived)
	@endcode
	CDerived is not even declared yet,
	another class, named TEST_CDerived was
	defined and created here.<br \>
	In the constructor of TEST_CDerived,
	it goes to register pointer
	to the static list IUnitTest has.<br \>
	It will succeed unless a pointer of the same class that
	exists within other translation unit had already registered.

	@code
	UNIT_TEST_FUNC(CDerived) { ... }
	@endcode
	This is a declaration of TEST_CDerived::test().
	One can give it definition in the next.

	These created tests are available from anywhere
	after reached a entry point of a program.

	@code
	IUnitTest::PerformUnitTest();
	@endcode
	This function executes respective test functions regisered.

	To be said generally, these gimmicks are...
	@li Runtime test
	@li Available in anywhere
	@li Available with single call
	@li Ignored if release build.
	@li Not rebuked by CrtDbg, macros to select memory leaks.
	@li Capable to be in cpp files so not increase class headers
	despite MAKE_PATH macro.
	@li Capable to sort tests by relationships of classes.
*/
#ifndef STDAFX_H
#include <vector>
#include <typeinfo>
#endif

#include "ienumlator.h"

/*!	Unit test class interface.
	See also iunit_test.h to know how to make unit tests.
*/
class IUnitTest {
private:
	class Enumlator;
public:
	virtual ~IUnitTest() {}

	//! Executes the unit test.
	virtual bool test() = 0;

	//! Executes all registered unit tests.
	static void PerformUnitTest() {
#ifdef _DEBUG
		int nUnitTest = ncodes(0);
		IUnitTest* p = 0;
		for (int i = 0; i < nUnitTest; i++) {
			p = GetUnitTest(i);
			if (p->test() == false)
				throw 0;
		}
#endif
	}

	//! Adds a pointer of unit test to the list.
	template<class T>
	static bool PushUnitTest() {
		static T t;
		for (int i = ncodes(0); i; i--) {
			if (GetUnitTest(i) == &t)
				return false;
		}
		GetUnitTest(ncodes(0)) = &t;
		ncodes(1);
		return true;
	}

	static int Size() { return ncodes(0); }

	static Enumlator GetEnumlator() {
		return Enumlator(&GetUnitTest(0));
	}

private:
	static IUnitTest*& GetUnitTest(int n) {
		static IUnitTest* ar[kUnitTestMax];
		return ar[n];
	}
	static int ncodes(int add) {
		static int n = 0;
		int ret = n;
		n += add;
		return ret;
	}
	static constexpr int kUnitTestMax = 255;


private:class Enumlator	: public ::basis::IEnumlator<IUnitTest*>
{
public:
	Enumlator(IUnitTest** p) : m_p(p), m_end(0) {}

	bool next() override {
		if (!m_end)
			m_end = m_p + IUnitTest::Size();
		if (m_p == m_end)
			return false;
		++m_p;
		return (m_p != m_end);
	}

	bool empty() const override {
		return m_p == m_end;
	}

	IUnitTest*& get() override {
		return *m_p;
	}

private:
	IUnitTest **m_p;
	IUnitTest **m_end;
};  // Enumlator

}; // IUnitTest



/*! Gives a test class definition and its substance.
	The object add itself to a list of test codes as it created.
*/
#ifdef _DEBUG
#define UNIT_TEST(hoge) struct TEST_##hoge : public IUnitTest {\
	bool test() override; };\
	static bool TEST_##hoge##_TRIGGER = IUnitTest::PushUnitTest<TEST_##hoge>();
#else
#define UNIT_TEST(hoge) namespace {\
	struct TEST_##hoge { bool test(); }; }
#endif



/*! Defines a test function of hoge class.
	See a sample below.
	@code
	UNIT_TEST_FUNC(hoge) { return true; }
	@endcode
	This will be expanded to be something like below.
	@code
	bool TEST_hoge() { return true; }
	@endcode
	UNIT_TEST and UNIT_TEST_FUNC macros allows one to be
	ignorant which classes provide test functions.
*/
#define UNIT_TEST_FUNC(hoge) bool TEST_##hoge##::test()

/*! @class hoge
	Dummy name for certain macros.
*/

#endif