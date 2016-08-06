#pragma once
#ifndef GUID_05B31CB4D53047EDAFFA66A83A82742A
#define GUID_05B31CB4D53047EDAFFA66A83A82742A
#ifndef STDAFX_H
#include <memory>
#include <functional>
#endif
#include "types.h"
#include "ievent_handler.h"

namespace basis {

class StringBuffer;
enum class Message : int;

/*! Window class.
    @note All coordinates used as parameters and return values
    is expressed in virtual screen coordinates.
*/
class Window : public IEventHandler {
public:
    using Listener = std::function<int(Window*, Message, WPARAM, LPARAM)>;
    Window();
    virtual ~Window();

    Window(const Window&) = delete;
    Window&operator=(const Window&) = delete;
    Window(Window&&) = default;
    Window&operator=(Window&&) = default;

    operator HWND() const;

    const Window& hook(IEventHandler *p) const;

    const Window& hook(Listener f) const;

    const Window& unhook(IEventHandler *p) const;

    //! デフォルトのウィンドウ作成
    virtual Window& create();

    void waitToEnd() const;

    virtual int onEvent(Window*, Message, WPARAM, LPARAM) override { return 0; }

    //! Places a message in window's message queue and returns control immediatly.
    void post(Message msg, WPARAM wp, LPARAM lp);

    /*! Returns rectangle of the window.
        This function returns the rectangle of the window.
        The rectangle is including non-client area,
        title bar and border for example.
        Different from GetWindowRect API function,
        this function returns it excluding aero glass padding,
        that is used by Windows Aero on Windows Vista or later.
    */
    Rect getRect() const;

    /*! Sets rectangle of the window.
        This function retreives a rectangle consits of
        left-top and right bottom coordinates.
        This function will calculate aero glass position
        so that window will fit left-top edge
        when left-top coordinate is set to be (0, 0).
    */
    void setRect(Rect dest) const;

    Rect getWindowRect() const;    // API wrapper
    Size getWindowSize() const;

    /*! Returns client area of the window.
        Different from GetClientRect API function, returned
        rectangle is expressed in virtual-screen coordinates.
    */
    Rect getClientRectInScreen() const;

    Rect getClientRect() const; // API wrapper
    Size getClientSize() const;

    // Gets a coordinate where normalized window would be positioned.
    Rect place() const;

    //! Sets position for normalized window.
    bool place(const Rect& target) const;

    //! Moves the window relatively.
    bool move(Size s) const;

    //! Moves the window to specified virtual screen coordinates.
    bool moveTo(Point pt) const;

    // Gets window size.
    Size getSize() const { return getRect().size(); }

    // Gets window width.
    int getWidth() const { return getRect().width(); }

    // Gets window height.
    int getHeight() const { return getRect().height(); }

    //! Maximizes the window.
    void maximize() const;

    //! Minimizes the window.
    void minimize() const;

    //! Makes window back from being maximized or minimized.
    void normalize() const;

    //! Returns whether the window has been maximized or not.
    bool isMaximized() const;

    //! Returns whether the window has been minimized or not.
    bool isMinimized() const;

    //! Maximize the window to fit entire virtual screen.
    void maximize_multi() const;

    //! Gets window style.
    LONG getStyle() const;

    //! Sets window style.
    LONG setStyle(LONG windowStyle) const;

    //! Gets caption's height the window has.
    int getCaptionHeight() const;

    /*!    Makes the window popuped or un-popuped.
        If the window had been maximized,
        window size will not change before and after.
        If the window size was normal, window size will change,
        but window's client area will not change or move.
    */
    void popup(bool state = true) const;

    //! Returns whether the window is popuped or not.
    bool isPopup() const;

    /*! Shows the window.
        It can also retreive the parameter
        that was handed to entry point to indicate how to show it.
    */
    const Window& show(int nShow = SW_SHOW) const;

    //! Hides the window.
    const Window& hide() const;

    void invalidate() const;
    void invalidate(const Rect &rc) const;

    //! Updates the window by executing WM_PAINT message.
    void update() const;

    //! Activates the window; makes it to be focused.
    bool activate() const;

    //! Makes the window foreground.
    bool setForeground() const;

    //! Changes the text of the window title bar.
    bool setTitle(const TCHAR *p) const;

    //! Returns a copied string of the window title bar.
    StringBuffer getTitle() const;

    HWND addChild(const TCHAR *title, Rect pos, DWORD addStyle);

    HWND addButton(const TCHAR *title, const Rect& pos);

    HWND addRadioButton(const TCHAR *title, const Rect& pos);

protected:
    virtual int run() const;
    void destroy();
    bool applyFrame() const;
    bool setWindowRect(const Rect &rc, UINT flag) const;// API wrapper

    //! リスナにイベントを渡す
    int broadcast(Message, WPARAM, LPARAM);

private:
    class DWM;  // Desktop Window Manager, Utility
    class Impl;
    std::unique_ptr<Impl> impl;
};

}  // namespace

#endif
