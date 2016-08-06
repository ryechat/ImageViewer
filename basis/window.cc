#ifndef STDAFX_H
#include <dwmapi.h>
#endif
#include "monitor.h"
#include "string_buffer.h"
#include "exception.h"
#include "stdfnc.h"

#include "window_dwm.h"
#include "window_impl.h"

namespace {

bool inline isValid(MSG *msg)
{
    return msg->message != WM_QUIT;
}

bool inline getMessageIfExist(MSG * msg, UINT filter)
{
    return isValid(msg)
        && PeekMessage(msg, nullptr, filter, filter, PM_REMOVE) != FALSE
        && isValid(msg);
}

bool inline waitForMessagePosted(MSG * msg, UINT filter)
{
    return isValid(msg)
        && GetMessage(msg, nullptr, filter, filter) != FALSE;
}

} // namespace


namespace basis {

Window::Window() : impl(new Impl(this))
{}

Window::~Window()
{
    PostMessage(*this, WM_CLOSE, 1, 0);
    impl->join();
}



Window::operator HWND() const { return impl->handle(); }



int Window::run() const
{
    MSG msg{};

    // WM_TIMERÇóDêÊÇµÇƒèàóùÇ∑ÇÈ
    while (getMessageIfExist(&msg, WM_TIMER)
        || waitForMessagePosted(&msg, WM_NULL))
    {
        DispatchMessage(&msg);
    }

    int const exit_code = static_cast<int>(msg.wParam);

    return exit_code;
}



const Window & Window::hook(IEventHandler * p) const
{
    impl->hook(p);
    return *this;
}



const Window & Window::hook(Listener f) const
{
    impl->hook(std::move(f));
    return *this;
}



const Window & Window::unhook(IEventHandler * p) const
{
    impl->unhook(p);
    return *this;
}



Window& Window::create()
{
    if (!impl->create())
        throw std::runtime_error(LOCATION);
    return *this;
}



void Window::
destroy()
{
    if (!impl->destroy())
        throw api_runtime_error();
}



void Window::post(Message msg, WPARAM wp, LPARAM lp)
{
    if (PostMessage(*this, static_cast<UINT>(msg), wp, lp) == FALSE)
        throw api_runtime_error();
}



HWND Window::addChild(const TCHAR *title, Rect pos, DWORD addStyle)
{
    return CreateWindow(TEXT("BUTTON"), title,
        WS_CHILD | WS_VISIBLE | addStyle,
        pos.left, pos.top, pos.width(), pos.height(), *this, 0,
        reinterpret_cast<HINSTANCE>(GetWindowLongPtr(*this, GWLP_HINSTANCE)),
        nullptr);
}



HWND Window::addButton(const TCHAR * title, const Rect & pos)
{
    return addChild(title, pos, BS_DEFPUSHBUTTON);
}



HWND Window::addRadioButton(const TCHAR * title, const Rect & pos)
{
    return addChild(title, pos, BS_AUTORADIOBUTTON);
}



void Window::waitToEnd() const
{
    impl->waitToEnd();
}



int Window::broadcast(Message msg, WPARAM wp, LPARAM lp)
{
    return impl->broadcast(msg, wp, lp);
}



Size Window::
getWindowSize() const
{
    return getWindowRect().size();
}



Rect inline Window::
getWindowRect() const
{
    RECT rc;
    if (!GetWindowRect(*this, &rc))
        throw api_runtime_error();
    return rc;
}



//! Returns the practical rectanle of the window.
Rect Window::
getRect() const
{
    RECT rc;
    if (DWM::IsCompositionEnabled() && DWM::GetExtendedRect(this, &rc))
        return rc;
    return getWindowRect();
}



void Window::
setRect(Rect dest) const
{
    if (DWM::IsCompositionEnabled())
        dest = dest + getWindowRect() - getRect();

    if (!setWindowRect(dest, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER))
        throw api_runtime_error();
}



Rect Window::
getClientRect() const
{
    RECT rc{};
    if (GetClientRect(*this, &rc) == FALSE)
        throw api_runtime_error();
    return rc;
}



Rect Window::
getClientRectInScreen() const
{
    auto rc = getClientRect();
    LONG right = rc.right, bottom = rc.bottom;
    ClientToScreen(*this, reinterpret_cast<POINT*>(&rc.right));
    rc.left += rc.right - right;
    rc.top += rc.bottom - bottom;
    return rc;
}



Size Window::
getClientSize() const
{
    return getClientRect().size();
}



Rect Window::
place() const
{
    WINDOWPLACEMENT wp{ sizeof wp };
    if (GetWindowPlacement(*this, &wp) == FALSE)
        throw api_runtime_error();
    return wp.rcNormalPosition;
}



bool Window::
place(const Rect& rc) const
{
    WINDOWPLACEMENT wp{ sizeof wp };
    if (!GetWindowPlacement(*this, &wp))
        return false;

    wp.rcNormalPosition = static_cast<RECT>(rc);
    return 0 != SetWindowPlacement(*this, &wp);
}



bool Window::
move(Size s) const
{
    Rect rc = getWindowRect();
    rc.left += s.x;
    rc.top += s.y;
    return setWindowRect(rc, SWP_NOSIZE | SWP_NOSENDCHANGING);
}



bool Window::
moveTo(Point pt) const
{
    auto size = getSize();
    return MoveWindow(*this, pt.x, pt.y, size.x, size.y, TRUE) != FALSE;
}



void Window::
maximize() const
{
    ShowWindow(*this, SW_MAXIMIZE);
}



void Window::
minimize() const
{
    ShowWindow(*this, SW_MINIMIZE);
}



void Window::
normalize() const
{
    ShowWindow(*this, SW_RESTORE);
}



bool Window::
isMaximized() const
{
    return IsZoomed(*this) != FALSE;
}



bool Window::
isMinimized() const
{ 
    return IsIconic(*this) != FALSE;
}



void Window::
maximize_multi() const
{
    maximize();
    setRect(Monitor::GetVirtualScreen());
    return;
}



LONG Window::
getStyle() const
{
    return GetWindowLong(*this, GWL_STYLE);
}



LONG Window::
setStyle(LONG newStyle) const
{
    return SetWindowLong(*this, GWL_STYLE, newStyle);
}



int Window::
getCaptionHeight() const
{
    if ((getStyle() & WS_CAPTION) == 0L)
        return 0L;

    return static_cast<int>(GetSystemMetrics(SM_CYCAPTION));
}



void Window::
popup(bool bPopup) const
{
    if (isPopup() == bPopup)
        return;

    const bool maximized = isMaximized();

    Rect rc = (maximized) ? getRect() : getClientRectInScreen();

    if (bPopup) {
        setStyle((getStyle() &~WS_OVERLAPPEDWINDOW) | WS_POPUP);
        applyFrame();
        setRect(rc);
        return;
    }

    setStyle((getStyle() &~WS_POPUP) | WS_OVERLAPPEDWINDOW);
    applyFrame();
    setRect(maximized ? rc : rc + getRect() - getClientRectInScreen());
}



bool Window::
isPopup() const
{
    return (getStyle() & WS_POPUP) != 0;
}



const basis::Window& Window::
show(int nShow) const
{
    ShowWindow(*this, nShow);
    return *this;
}



const basis::Window& Window::
hide() const
{
    ShowWindow(*this, SW_HIDE);
    return *this;
}



void Window::
invalidate() const
{
    if (InvalidateRect(*this, nullptr, 0) == FALSE)
        throw api_runtime_error();
}



void Window::
invalidate(const Rect &rc) const
{
    RECT rect = rc;
    if (InvalidateRect(*this, &rect, 0) == FALSE)
        throw api_runtime_error();
}



void Window::
update() const
{
    UpdateWindow(*this);
}



bool Window::
activate() const
{
    return setWindowRect({}, SWP_NOSIZE | SWP_NOMOVE);
}



bool Window::
setForeground() const
{
    return SetForegroundWindow(*this) != FALSE;
}



bool Window::
setTitle(const TCHAR *p) const
{
    return SetWindowText(*this, p) != FALSE;
}



StringBuffer Window::
getTitle() const
{
    auto size = static_cast<int>(DefWindowProc(*this, WM_GETTEXTLENGTH, 0, 0));

    StringBuffer buf(size + 1);
    DefWindowProc(*this, WM_GETTEXT, static_cast<WPARAM>(buf.capacity()),
        reinterpret_cast<LPARAM>(buf.data()));
    return buf;
}



bool Window::
setWindowRect(const Rect& rc, UINT flag) const
{
    return SetWindowPos(*this, 0, rc.left, rc.top,
        rc.width(), rc.height(), flag) != FALSE;
}



bool Window::
applyFrame() const
{
    return setWindowRect({}, SWP_NOSIZE | SWP_NOCOPYBITS | SWP_NOMOVE
        | SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOOWNERZORDER
        | SWP_NOZORDER);
}

} // namespace
