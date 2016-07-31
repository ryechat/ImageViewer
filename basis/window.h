#pragma once
#ifndef GUID_05B31CB4D53047EDAFFA66A83A82742A
#define GUID_05B31CB4D53047EDAFFA66A83A82742A

#include "types.h"
#include "ievent_handler.h"
#include "window_hook.h"

namespace basis {

class StringBuffer;

/*! Window class.
	@note All coordinates used as parameters and return values
	is expressed in virtual screen coordinates.
*/
class Window : public IEventHandler {
public:
	Window() { hook(this); }
	virtual ~Window() = default;

	Window(const Window&) = delete;
	Window&operator=(const Window&) = delete;
	Window(Window&&) = delete;
	Window&operator=(Window&&) = delete;

	operator HWND() const { return m_h; }

	virtual int onEvent(Window*, Message, WPARAM, LPARAM) override = 0;

	int post(Message msg, WPARAM wp, LPARAM lp) {
		return PostMessage(*this, static_cast<UINT>(msg), wp, lp);
	}

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
	bool setRect(Rect dest) const;

	Rect getWindowRect() const;	// API wrapper
	Rect getClientRect() const; // API wrapper

	/*! Returns client area of the window.
		Different from GetClientRect API function, returned
		rectangle is expressed in virtual-screen coordinates.
	*/
	Rect getClientRectInVirtualScreen() const;

	Size clientSize() const;

	// Gets a coordinate where normalized window would be positioned.
	Rect place() const;

	//! Sets position for normalized window.
	bool place(const Rect& target) const;

	//! Moves the window relatively.
	bool move(Size s) const;

	//! Moves the window to specified virtual screen coordinates.
	bool moveTo(Point pt) const;

	// Gets window size.
	Size getSize() const;

	// Gets window width.
	int getWidth() const;

	// Gets window height.
	int getHeight() const;

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

	/*!	Makes the window popuped or un-popuped.
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
	void show(int nShow = SW_SHOW) const;

	//! Hides the window.
	void hide() const;

	void invalidate(const RECT *rc) const;

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
	HWND addButton(const TCHAR *title, const Rect& pos) {
		return addChild(title, pos, BS_DEFPUSHBUTTON);
	}
	HWND addRadioButton(const TCHAR *title, const Rect& pos) {
		return addChild(title, pos, BS_AUTORADIOBUTTON);
	}

	int run();
	void hook(IEventHandler *p) { m_hook.push_front(p); }
	void unhook(IEventHandler *p) { m_hook.unhook(p); }


protected:

	//! デフォルトのウィンドウ作成関数
	virtual HWND createMainWindow(Window *win);

	//! フックされたリスナにイベントを渡す
	int broadcast(Window*, Message, WPARAM, LPARAM);

private:
	class DWM;  // Desktop Window Manager

	enum class Set : unsigned {
		ACTIVATE = SWP_NOSIZE | SWP_NOMOVE,
		MOVE   = SWP_NOSIZE   | SWP_NOSENDCHANGING,
		RESIZE = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER,
		FRAME  = SWP_NOSIZE   | SWP_NOCOPYBITS | SWP_NOMOVE
			| SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOOWNERZORDER
			| SWP_NOZORDER,
	};
	bool setWindowRect(const Rect &rc, Set flag) const;// API wrapper

	bool applyFrame() const { return setWindowRect({}, Set::FRAME); }

	//! ウィンドウ生成時に使うプロシージャを返す
	WNDPROC GetWndProc(Window *win);

	//! ウィンドウハンドルに関連付けられたイベントハンドラを取得し委譲する。
	static LRESULT CALLBACK Dispatch(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

	//! CreateWindow中のインスタンス
	static Window *ConstructingInstance;

	//! HWNDに仕込んだポインタを取得する
	static Window * GetInstance(HWND hWnd);

	HWND m_h;
	WindowHook m_hook;
	ATOM inline createWindowAtom(const TCHAR *identifier, WNDPROC proc);
};

}  // namespace

#endif