#pragma once
#ifndef GUID_E7FBC9207E264659B8BAC33A417692F2
#define GUID_E7FBC9207E264659B8BAC33A417692F2

#ifndef STDAFX_H
#include <wtypes.h>
#include <assert.h>
#include <memory>
#endif

namespace basis {

/*!	Critical section for class members.
	If this object was declared as a static class member,
	using enter and leave function makes codes
	accessing other static members to be thread-safe.

	If this object is a non-static member,
	other non-static members can be thread-safe using this.

	@code
	class foo {
	public:
		foo() : var(0) {}
		int get() {
			// Standard way to enter and leave.
			m_cs.enter();
			return m_var++;
			m_cs.leave();
		}
		int resetIfGreaterThan(int n) {
			// Simple way to enter and leave automatically.
			auto cs = m_cs.local();
			if (m_var <= n)
				return get();
			return m_var = 0;
		}
	private:
		CriticalSection m_cs;
		int m_var;
	};
	@endcode

	local() function create another CriticalSection object
	that shares core instance with origin.
	This scope-local CriticalSection created by local()
	function has independent counter for enter/leave.
	Thus local object leave critical section rightly
	as it scoped out.

	Making m_var chaged to static member needs that
	both of m_var and m_cs declared as static.
	So, all foo objects can share m_var resource
	thread-safely.

	@note
		When a class is copied or moved,
		CriticalSection objects of the class should be still unmoved.

*/
class CriticalSection {
public:
	//! Default constructor
	//! @exception std::bad_alloc
	CriticalSection() : nEnter(0), bAutoLeave(false),
		m_p(new Instance){}

	CriticalSection(CriticalSection&) = delete;
	CriticalSection&operator=(CriticalSection&) = delete;

	CriticalSection(CriticalSection&& rhs) noexcept = default;
	CriticalSection&operator=(CriticalSection&&rhs) noexcept = default;

	/*! Destructor.
		If object is created by local,
		it makes leave() automatically
		while counter is greater than 0.
		Despite above case, we'd get to be claimed by assert()
		if not made leave() manually.

		Assertion code will claim if all of these are true.
		@li Valid object, not to be moved.
		@li Not an object created by local().
		@li Counter is not 0.

	*/
	~CriticalSection() noexcept {
		if (!m_p) return;		// After moved.
		if (bAutoLeave) {		// Made by local()
			while (nEnter > 0)
				leave();
		}
		if (nEnter != 0)		// Check our fault.
			assert(nEnter == 0);// <Set break point to debug.
	}

	/*!	Enter critical section.
		There's a chance to be waited before entering.
		This function increments inner counter.
	*/
	CriticalSection& enter() noexcept
	{
		if (m_p) {
			EnterCriticalSection(&m_p->cs);
			++nEnter;
		}
		return *this;
	}

	/*! Provides critical section object that is local to the scope.
		Created object has a independet inner counter
		that is entered 1 time
		and will make its leave as it scoped out.

		We should catch a returned value not to be soonly destructed 
		because created object is temporal.
	*/
	CriticalSection local() const noexcept {
		CriticalSection cs(this->m_p);
		cs.enter();
		return cs;
	}

	/*! Leaves critical section.
		This function decrements inner counter.
	*/
	void leave() noexcept
	{
		if (m_p) {
			LeaveCriticalSection(&m_p->cs);
			--nEnter;
		}
	}

	/*! Enter critical section if possible.
		It works as enter() if it is immediately possible 
		and returns true.<br />
		If not, this function returns false without waiting.
	*/
	bool tryEnter() noexcept
	{
		if (!m_p || TryEnterCriticalSection(&m_p->cs) == 0)
			return false;
		++nEnter;
		return true;
	}

private:
	//! Critical section core instance.
	struct Instance {
		CRITICAL_SECTION cs;	
		Instance() noexcept {
			InitializeCriticalSection(&cs); }
		~Instance() {
			DeleteCriticalSection(&cs); }
	};

	// for local() function
	CriticalSection(std::shared_ptr<Instance> p) noexcept
		: m_p(p), nEnter(0), bAutoLeave(true) {}

	//! Refference to instance.
	std::shared_ptr<Instance> m_p;

	//! Enter/Leave counter
	int nEnter;

	/*! Whether it should leave automaticaly.
		Only instance created by local() function have it of true.
	*/
	bool bAutoLeave;
};

}  // namespace

#endif

