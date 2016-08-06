#ifndef STDAFX_H
#include <algorithm>
#endif
#include "memory_status.h"
#include "ids.h"
#include "loader.h"
#include "profile.h"
#include "filer.h"
#include "list_item.h"

namespace image_viewer {

CImageViewer::Loader::Loader(CImageViewer& parent_)
: parent(parent_),
m_maxPreload(parent.profile->general().load(ID::LOADER_RANGE_MAX, 20)),
m_minPreload(parent.profile->load(ID::LOADER_RANGE_MIN, 4)),
m_memoryCapMegaBytes(parent.profile->load(ID::LOADER_MEMORY_CAP, 0))
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
    assert(itr != parent.filer->end());

    if (bWait) {
        waitIfLoading(itr);
    }
    else {
        if (isLoading(itr))
            return Status::Loading;
    }

    if (itr->get()->isLoaded())
        return Status::Finished;

    if (itr->get()->isLoadingFailed())
        return Status::Failed;

    if (beginLoad(itr) == false)
        return Status::Failed;

    if (!bWait)
        return Status::Loading;

    waitIfLoading(itr);

    if (itr->get()->isLoaded())
        return Status::Finished;

    if (itr->get()->isLoadingFailed())
        return Status::Failed;

    return Status::CannotOpen;
}



// 別スレッドでのロードタスクを追加する
bool CImageViewer::Loader::
beginLoad(iterator itr) try
{
    m_cs.enter();
    m_loading.emplace_back(itr, CreateEvent(0, 1, 0, 0));
    m_cs.leave();

    m_threads.addTask([itr, this]() {
        itr->get()->loadImage(parent.m_dir + itr->get()->fileName());
        m_cs.enter();
        for (auto &&i : m_loading) {
            if (i.first == itr) {
                SetEvent(i.second);
                break;
            }
        }
        m_cs.leave();
        parent.post(WM::COMMAND, static_cast<int>(ID::LOADER_IMAGE_LOADED), 0);
    });
    return true;
}
catch (std::exception &e) {
    MessageBoxA(0, e.what(), 0, 0);
    return false;
}



bool CImageViewer::Loader::
waitIfLoading(iterator itr, int time)
{
    // m_loadingの要素を削除する責任がある
    // 待機完了したら削除すること
    DWORD t = time < 0 ? INFINITE : time;
    auto cs = m_cs.local();
    for (auto &&i = m_loading.begin(); i != m_loading.end(); ++i) {
        if (i->first != itr)
            continue;

        cs.leave();
        if (WaitForSingleObject(i->second, t) == WAIT_TIMEOUT)
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
    //    waitIfLoadingを呼び出さないとロードタスクは更新されない。
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
    constexpr int megabytes = 1024 * 1024;
    int avail;
    if (m_memoryCapMegaBytes) {
        basis::ProcessMemoryStatus pms(megabytes);
        pms.update();
        avail = m_memoryCapMegaBytes - static_cast<int>(pms.usage());
    }
    else {
        basis::SystemMemoryStatus sms(megabytes);
        sms.update();
        avail = static_cast<int>(sms.avail()) - 200; // 200MBは残す
    }

    auto &f = parent.filer;
    iterator iForward = iter;
    iterator iBackward = iter;
    iterator *ref = nullptr;

    // 優先度順にロード
    for (int i = 0; i < m_maxPreload * 2; ++i) {
        ref = (i % 2 == 0) ? &iForward : &iBackward;
        iter = f->move(*ref, (i % 2 == 0) ? 1 : -1);
        if (iter == *ref)
            continue;

        if (iter->get()->isLoadingFailed()) {
            f->erase(iter);
            continue;
        }
        if (avail < 10 && i >= m_minPreload * 2)
            break;

        *ref = iter; // インクリメント
        if (!iter->get()->isLoaded()) {
            loadImage(iter, false);
            avail -= (std::max)(iter->get()->weight / megabytes, 20);
        }
    }
}



void CImageViewer::Loader::
helper_release(iterator itr, bool bPerform, bool bMark)
{
    assert(itr != parent.filer->end());

    iterator iEnd = std::next(parent.filer->move(itr, m_maxPreload));
    for (iterator i = parent.filer->move(itr, -m_maxPreload); i != iEnd; ++i)
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
    assert(itr != parent.filer->end());
    waitIfLoading(itr);
    itr->get()->unload();
}

}  // namespace