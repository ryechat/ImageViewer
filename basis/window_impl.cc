#ifndef STDAFX_H
#include <algorithm>
#endif
#include "stdfnc.h"
#include "window_impl.h"

namespace basis {

Window * Window::Impl::ConstructingInstance = nullptr;

CriticalSection Window::Impl::m_cs;

bool Window::Impl::
create()
{
    if (m_thread.isBusy())
        return false;

    m_cs.enter();
    ConstructingInstance = parent;
    m_thread.addTask([this] {
        std::basic_string<TCHAR> className = TEXT("Address");
        className.append(ToStr(reinterpret_cast<size_t>(this)));

        ATOM atom{ createWindowAtom(className.c_str(), &Dispatch) };

        CreateWindowEx(0, reinterpret_cast<TCHAR*>(atom), 0,
            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
            0, 0, 0, 0);
    });
    m_thread.wait(-1);
    m_cs.leave();

    if (!m_h)
        return false;
    m_thread.addTask([this] { parent->run(); });
    return true;
}



bool Window::Impl::
destroy()
{
    if (m_h && !DestroyWindow(m_h))
        return false;
    m_h = nullptr;
    return true;
}



void Window::Impl::
waitToEnd()
{
    m_thread.wait(-1);
}



void Window::Impl::
join()
{
    m_thread.join();
}



void Window::Impl::
hook(IEventHandler * p)
{
    if (p && !exist(p))
        m_hook.push_front({ p,{} });
}



void Window::Impl::
hook(Listener f)
{
    m_hook.push_front({ nullptr, std::move(f) });
}



void Window::Impl::
unhook(IEventHandler *p)
{
    if (!p)
        return;
    for (auto i = m_hook.begin(); i != m_hook.end(); ++i) {
        if (i->first == p) {
            i = m_hook.erase(i);
            break;
        }
    }
}



int Window::Impl::
broadcast(Message msg, WPARAM wp, LPARAM lp)
{
    int ret;
    for (auto &i : m_hook) {
        if (i.first)
            ret = i.first->onEvent(parent, msg, wp, lp);
        else
            ret = i.second(parent, msg, wp, lp);
        if (ret)
            return ret;
    }
    return 0;
}



int Window::Impl::
dispatch(Window *win, Message msg, WPARAM wp, LPARAM lp) const
{
    return static_cast<int>(Dispatch(*win, static_cast<UINT>(msg), wp, lp));
}



bool Window::Impl::
exist(const IEventHandler *p)
{
    return p && std::any_of(m_hook.cbegin(), m_hook.cend(),
        [p](const Element& e) { return (e.first == p); }
    );
}



ATOM Window::Impl::
createWindowAtom(const TCHAR *identifier, WNDPROC procedure)
{
    WNDCLASSEX wc{ sizeof(WNDCLASSEX) };
    // wc.cbClsExtra =
    // wc.hInstance =
    // wc.lpszMenuName =
    // wc.hIconSm =
    wc.lpfnWndProc = procedure;
    wc.cbWndExtra = sizeof(INT_PTR); // Window*を格納する

    wc.hIcon = static_cast<HICON>(LoadImage(0,
        IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
    wc.hCursor = static_cast<HCURSOR>(LoadImage(0,
        IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;

    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    wc.lpszClassName = identifier;

    ATOM atom{ RegisterClassEx(&wc) };
    if (atom == 0) {
        DWORD const error_class_atom_conflicted = 0x582;
        assert(GetLastError() != error_class_atom_conflicted);
    }
    return atom;
}



// システムコールバック。インスタンスにフックされたリスナに配信する
LRESULT Window::Impl::
Dispatch(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    Window *p{ GetInstance(hWnd) };

    int ret = p ? p->impl->broadcast(static_cast<Message>(msg), wp, lp) : 0;

    return ret ? ret : DefWindowProc(hWnd, msg, wp, lp);
}



Window * Window::Impl::
GetInstance(HWND hWnd)
{
    // ハンドルからインスタンスを取得
    Window *pInst = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    // 生成時の最初の配信で初期設定
    if (!pInst && ConstructingInstance) {
        pInst = ConstructingInstance;
        ConstructingInstance = nullptr;
        pInst->impl->m_h = hWnd;

        // インスタンスのアドレスを仕込む
        SetWindowLongPtr(hWnd, GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(pInst));
    }
    return pInst;
}

}  // namespace
