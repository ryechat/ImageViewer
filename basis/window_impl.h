#pragma once
#ifndef GUID_B58C4C5E7A9343598E1D86A2FE6F8F3A
#define GUID_B58C4C5E7A9343598E1D86A2FE6F8F3A
#ifndef STDAFX_H
#include <list>
#endif
#include "window.h"
#include "thread.h"
#include "critical_section.h"

namespace basis {

class Window::Impl {
public:
    Impl(Window* win) : parent(win) {}
    ~Impl() = default;

    HWND handle() { return m_h; }
    bool create();
    bool destroy();
    void waitToEnd();
    void join();

    //! ウィンドウのリスナを登録
    void hook(IEventHandler *p);

    //! ウィンドウのリスナを登録
    void hook(Listener f);

    //! リスナを除去する
    void unhook(IEventHandler *p);

    //! フックされたリスナにイベントを渡す
    int broadcast(Message, WPARAM, LPARAM);

    //! メッセージを配信する
    int dispatch(Window*, Message, WPARAM, LPARAM) const;

private:
    bool exist(const IEventHandler *p);

    //! ウィンドウハンドルに関連付けられたイベントハンドラを取得し委譲する。
    static LRESULT CALLBACK Dispatch(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

    //! HWNDに仕込んだポインタを取得する
    static Window * GetInstance(HWND hWnd);

    ATOM createWindowAtom(const TCHAR *identifier, WNDPROC proc);

    //! CreateWindow中のインスタンス
    static Window *ConstructingInstance;
    static CriticalSection m_cs;

    Window* parent;
    HWND m_h;
    basis::CThread m_thread;

    using Element = std::pair<IEventHandler*, Listener>;
    std::list<Element> m_hook;
};

}  // namespace
#endif

