#ifndef STDAFX_H
#include <memory>
#include <vector>
#include <list>
#include <assert.h>
#endif
#include "critical_section.h"
#include "thread_unity.h"

namespace basis {

class CThreadUnity::Impl {
public:
	Impl() { setThreadCount(1); }
	~Impl() {}
	using TaskTy = CThread::TaskTy;
	void setThreadCount(int nThreads);
	void addTask(CThread *pThread, TaskTy task);
	void reloadTask(CThread *pThread);
	void run(CThread *pThread, TaskTy task);
	bool wait(bool bWaitAll, DWORD waitMilliSeconds);
	CThread *getSuspendedThread();

	CriticalSection m_cs;
	std::list<TaskTy> m_tasks;
	std::vector<HANDLE> m_signals;
	std::vector<std::unique_ptr<CThread>> m_threads;
};



void CThreadUnity::Impl::
setThreadCount(int nThreads)
{
    assert(nThreads >= 0);
	CriticalSection cs = m_cs.local();
	while (m_threads.size() > static_cast<size_t>(nThreads)) {
		m_threads.back()->wait(INFINITE);
		m_threads.pop_back();
		m_signals.pop_back();
	}
	while (m_threads.size() < static_cast<size_t>(nThreads)) {
		m_threads.emplace_back(new CThread);
		m_signals.push_back(m_threads.back()->getWaitHandle());
	}
}



void CThreadUnity::Impl::
addTask(CThread *thread, TaskTy func)
{
	auto cs = m_cs.local();
	if (!thread)
		thread = getSuspendedThread();

	if (thread)
		run(thread, std::move(func));
	else
		m_tasks.push_back(std::move(func));
}



bool CThreadUnity::Impl::
wait(bool bWaitAll, DWORD dwMilliseconds)
{
	DWORD size = static_cast<DWORD>(m_signals.size());
	DWORD result = WaitForMultipleObjects(size,
		&m_signals[0], bWaitAll, dwMilliseconds);

	return (result != WAIT_TIMEOUT);
}



void CThreadUnity::Impl::
run(CThread *pThread, TaskTy task)
{
	TaskTy fReload{ [this, pThread] {
		reloadTask(pThread);
	} };

	pThread->addTask(task);
	pThread->addTask(fReload);
}



CThread* CThreadUnity::Impl::
getSuspendedThread()
{
	CriticalSection cs = m_cs.local();
	for (auto &p : m_threads) {
		if (!p->isBusy())
			return p.get();
	}
	return nullptr;
}



void CThreadUnity::Impl::
reloadTask(CThread *pThread)
{
	CriticalSection cs = m_cs.local();
	if (m_tasks.empty())
		return;

	run(pThread, m_tasks.front());
	m_tasks.pop_front();
}



// CThreadUnity


CThreadUnity::CThreadUnity() : impl(new Impl)
{
	impl->setThreadCount(1);
}



CThreadUnity::~CThreadUnity()
{
	delete impl;
}



int CThreadUnity::
threadCount()
{
	return static_cast<int>(impl->m_threads.size());
}



bool CThreadUnity::
setThreadCount(int nThreads)
{
	if (nThreads > 0 && nThreads <= MAX_THREADS) {
		impl->setThreadCount(nThreads);
		return true;
	}
	return false;
}



bool CThreadUnity::
wait(bool b, int time)
{
	DWORD t = (time < 0) ? INFINITE : time;
	return impl->wait(b, t);
}



void CThreadUnity::
addTask(CThread::TaskTy task)
{
	impl->addTask(nullptr, std::move(task));
}

}  // namespace