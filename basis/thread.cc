#ifndef STDAFX_H
#include <process.h>
#include <algorithm>
#include <list>
#endif

#include "exception.h"
#include "critical_section.h"
#include "thread.h"

namespace basis {

//! Private Implimentation of CThread.
class CThread::Impl {
public:
    Impl();
	~Impl() { release(); }

	//! Indices of array of handle.
	enum stat : BYTE {
		added,		//!< Event; Tasks were added.
		tarminate,	//!< Event; Destructor was called.
		ended,		//!< Event; Thread finished tasks on the list.
		thread		//!< Thread handle.
	};

	//! Handles that is explained by stat.
	HANDLE h[4];

	//! Adds a task into the list.
	void addTask(TaskTy f)
	{
		auto cs = m_cs.local();
		m_tasks.push_back(std::move(f));
		res(ended);
		set(added);
	}

	//! Waits specified event to be signaled.
	bool wait(stat s, DWORD t = INFINITE) const noexcept {
		return WaitForSingleObject(h[s], t)	!= WAIT_TIMEOUT;
	}

private:
	static unsigned __stdcall ThreadEntryPoint(void *p) {
		while (reinterpret_cast<Impl*>(p)->threadProcedure())
			;
		return EXITCODE;
	}

	bool threadProcedure();

	//! Set event to be signaled.
	void set(stat s) { SetEvent(h[s]); }

	//! Reset event.
	void res(stat s) { ResetEvent(h[s]); }

	void release() {
		for (;;) {	// Waits all tasks finished.
			m_cs.enter();
			if (!m_tasks.size())
				break;
			m_cs.leave();
			wait(ended);
		}
		set(tarminate);
		m_cs.leave();
		h[thread] && wait(thread);
		for (auto i : h) {
			i && CloseHandle(i);
		}
	}

	//! for re/setting events and tasks.
	CriticalSection  m_cs;

	//! FIFO list of tasks.
	std::list<TaskTy> m_tasks;
};



CThread::Impl::
Impl()
{
	for (int i = 0; i <= ended; i++)
		h[i] = CreateEvent(0, 1, (i <= tarminate) ? 0 : 1, 0);

	h[thread] = reinterpret_cast<HANDLE>(_beginthreadex(0,
		0, ThreadEntryPoint, this, 0, 0));
	
	// Check validity.
	if (std::any_of(h, &h[_countof(h)], [](HANDLE i) {
		return i == 0;
	})) {
		release();
		throw std::runtime_error(LOCATION);
	}
}



//! TaskPtr will be called back in ThreadEntryPoint()
bool CThread::Impl::
threadProcedure()
{
	//! Wait for added and tarminate
	WaitForMultipleObjects(2, &h[added], false, INFINITE);
	if (wait(tarminate, 0))
		return false;	// Break to tarminate.

	m_tasks.front().operator()();

	// This thread will still be running if there's a next task.
	CriticalSection cs = m_cs.local();
	m_tasks.pop_front();
	if (m_tasks.empty()) {
		set(ended);
		res(added);
	}
	return true;	// Continue looping.
}





// -------------------- CThread -------------------- //





CThread::CThread() : impl(new Impl) {}

CThread::~CThread() { delete impl; }



void CThread::join()
{
	delete impl;
	impl = nullptr;
}



void CThread::addTask(TaskTy f)
{
	if (!f)
		throw std::invalid_argument(LOCATION);
	if (impl)
		impl->addTask(std::move(f));
}



bool CThread::wait(int time) const noexcept
{
	DWORD t = time < 0 ? INFINITE : static_cast<DWORD>(time);
	return impl ? impl->wait(impl->ended, t) : true;
}





HANDLE CThread::getWaitHandle()
{
	return impl ? impl->h[impl->ended] : nullptr;
}



HANDLE CThread::getThreadHandle()
{
	return impl ? impl->h[impl->thread] : nullptr;
}

}  // namespace