#ifndef STDAFX_H
#include <algorithm>
#endif
#include "memory_status.h"
#include "loader.h"
#include "profile.h"
#include "filer.h"
#include "list_item.h"
#include "ids.h"

namespace image_viewer {

CImageViewer::Loader::Loader(CImageViewer& parent_)
: parent(parent_),
m_preloadRange(parent.profile->general().get(ID::LOADER_RANGE, 20)),
m_memoryCapMegaBytes(parent.profile->get(ID::LOADER_MEMORY_CAP, 0))
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	if (0 < si.dwNumberOfProcessors) {
		m_threads.setThreadCount((std::min<int>)(si.dwNumberOfProcessors, 32));
	}
}



// bWait = true : 画像を読み込む。ロードエラーはfalse
// bWait = false : 事前読み込み開始。ロードエラーはfalse
CImageViewer::Loader::Status CImageViewer::Loader::
loadImage(iterator itr, bool bWait)
{
	if (bWait) {
		waitIfLoading(itr);
	}
	else {
		if (isLoading(itr))
			return Status::Loading;
	}

	Contents *p = itr->get();

	if (p->isLoaded())
		return Status::Finished;

	if (p->isLoadingFailed())
		return Status::Failed;

	if (beginLoad(itr) == false)
		return Status::Failed;

	if (!bWait)
		return Status::Loading;

	waitIfLoading(itr);

	if (p->isLoaded())
		return Status::Finished;

	if (p->isLoadingFailed())
		return Status::Failed;

	return Status::CannotOpen;
}



// 別スレッドでのロードタスクを追加する
bool CImageViewer::Loader::
beginLoad(iterator itr) try
{
	HANDLE h = CreateEvent(0, 1, 0, 0);

	m_cs.enter();
	m_loading.emplace_back(itr, h);
	m_cs.leave();

	m_threads.addTask([=]() {
		auto path = parent.m_dir + itr->get()->fileName();
		itr->get()->loadImage(path.path());
		auto cs = m_cs.local();
		for (auto &&i : m_loading) {
			if (i.first == itr) {
				SetEvent(i.second);
				break;
			}
		}
		parent.post(WM::COMMAND, static_cast<int>(ID::LOADER_IMAGE_LOADED), 0);
	});
	return true;
}
catch (std::exception &e) {
	MessageBoxA(0, e.what(), 0, 0);
	return false;
}



bool CImageViewer::Loader::
waitIfLoading(iterator itr, DWORD time)
{
	// m_loadingの要素を削除する責任がある
	// 待機完了したら削除すること
	auto cs = m_cs.local();
	for (auto &&i = m_loading.begin(); i != m_loading.end(); ++i) {
		if (i->first != itr)
			continue;

		cs.leave();
		if (WaitForSingleObject(i->second, time) == WAIT_TIMEOUT)
			return false;
		cs.enter();

		CloseHandle(i->second);
		m_loading.erase(i);
		break;
	}
	return true;
}



bool CImageViewer::Loader::
waitIfAnyImageIsLoading()
{
	//	waitIfLoadingを呼び出さないとロードタスクは更新されない。
	auto cs = m_cs.local();
	while (m_loading.size() > 0) {
		iterator itr = m_loading.front().first;
		cs.leave();
		if (waitIfLoading(itr, 3000) == false)
			return false;
		cs.enter();
	}
	return true;
}



void CImageViewer::Loader::
preloadAround(iterator iter)
{
	int nThread = m_threads.threadCount();
	int avail = (std::numeric_limits<int>::max)();
	if (m_memoryCapMegaBytes) {
		basis::ProcessMemoryStatus pms(1024 * 1024);
		pms.update();
		avail = static_cast<int>((m_memoryCapMegaBytes - pms.usage()) / 20);
	}

	CFiler *pFiler = parent.filer.get();
	iterator iForward = iter;
	iterator iBackward = iter;
	iterator *ref = nullptr;
	Status status;
	bool bEnd = false;
	iterator last = parent.filer->end();

	// 優先度順にロード
	for (int i = m_preloadRange * 2; i > 0; i--) {
		ref = (i % 2 == 0) ? &iForward : &iBackward;
		iter = pFiler->move(*ref, (i % 2 == 0) ? 1 : -1);
		if (iter == *ref) {
			if (bEnd)
				return;
			bEnd = true;
			continue;
		}
		bEnd = false;
		*ref = iter;
		if (avail <= 1 && i <= m_preloadRange * 2 - 2) {
			if (iter->get()->isLoaded())
				last = iter;
			continue;
		}
		if (nThread-- < 0)
			continue;
		status = loadImage(iter, false);
		if (status == Status::Failed) {
			if (iter->get()->isLoadingFailed())
				pFiler->erase(iter);
		}
		else if (status == Status::Loading) {
			--avail;
		}
	}
	if (last != parent.filer->end())
		releaseIfOk(*ref);
}



void CImageViewer::Loader::
helper_release(iterator itr, bool bPerform, bool bMark)
{
	CFiler *pFiler = parent.filer.get();

	assert(itr != pFiler->end());

	iterator iEnd = ++pFiler->move(itr, m_preloadRange);
	for (iterator i = pFiler->move(itr, -m_preloadRange); i != iEnd; ++i)
	{
		bool *bUnload = &i->get()->bUnload;
		if (bPerform) {
			if (*bUnload)
				releaseIfOk(i);
		}
		else {
			*bUnload = bMark;
		}
	}
}



void CImageViewer::Loader::
releaseIfOk(iterator itr)
{
	waitIfLoading(itr);
	itr->get()->unload();
}

}  // namespace