/* API Compatibility
LoadImage, RegisterClassEx
Windows NT / 2000：Windows NT 4.0 以降
Windows 95 / 98：Windows 95 以降
ヘッダー：Winuser.h で宣言、Windows.h をインクルード
インポートライブラリ：User32.lib を使用
Unicode：Windows NT / 2000 は Unicode 版と ANSI 版を実装

GetWindowLongPtr, SetWindowLongPtr
Windows NT / 2000：Windows NT 3.1 以降
Windows 95 / 98：Windows 95 以降
Windows CE：Windows CE 1.0 以降
ヘッダー：winuser.h 内で宣言
インポートライブラリ：user32.lib を使用
Unicode：Windows NT / 2000 は Unicode 版と ANSI 版を実装

CreateWindowEx, DefWindowProc, PeekMessage, GetMessage, DispatchMessage
Windows NT / 2000：Windows NT 3.1 以降
Windows 95 / 98：Windows 95 以降
ヘッダー：Winuser.h 内で宣言、Windows.h をインクルード
インポートライブラリ：User32.lib を使用
Unicode：Windows NT / 2000 は Unicode 版と ANSI 版を実装
*/
#ifndef STDAFX_H
#include <dwmapi.h>
#endif
#include "monitor.h"
#include "string_buffer.h"
#include "window.h"
#include "exception.h"
#include "stdfnc.h"

namespace basis {

// DWM provides the way to get actual window rect.
// Because of that GetWindowRect API returns the rectangle
// including skelton area on Windows Vista or later system,
// the rectangle returned by GetWindowRect API would be different
// from what we could see on the screen.
// In the case that actual window rect is necessary,
// IsCompositionEnabled() function tells us whether it should call
// GetExtendedRect() function instead of GetWindowRect API.
class Window::DWM
{
	typedef HRESULT(WINAPI *TyIsEnabled)(BOOL*);
	typedef HRESULT(WINAPI *TyAttribute)(HWND, DWORD, LPCVOID, DWORD);

public:
	/* Checks whether Windows Aero is enabled or not.
		This helps us know what GetWindowRect() API returns.
		If IsCompositionEnabled() function returned true,
		Windows Aero has been enabled.
		GetWindowRect() API then returns Aero glass area.
		This means what the API returned is inconsitent
		with what we could see on the screen.
		To workaround this, GetExtendedRect() function can be used.
		When this function returned false,
		GetWindowRect() API has no matters about that.
	*/
	static bool IsCompositionEnabled()
	{
		static auto fp = reinterpret_cast<TyIsEnabled>
			(getProc("DwmIsCompositionEnabled"));

		if (!fp)
			return false;

		BOOL b;
		return (fp(&b) == S_OK && b != FALSE);
	}

	/*! Gets the practical rectangle of the window.
		This function works only on Windows Vista or later
		and when IsCompositionEnabled() function returns true.
	*/
	static bool GetExtendedRect(const Window *h, Rect* p)
	{
		const HRESULT hr = getWindowAttribute(h,
			DWMWA_EXTENDED_FRAME_BOUNDS, p, sizeof(RECT));
		return hr == S_OK;
	}

protected:

	static HRESULT getWindowAttribute(
		const Window *h, DWORD dwAttr, LPCVOID pAttr, DWORD cbAttr)
	{
		if (!GetFunction())
			return -1;
		return GetFunction()(*h, dwAttr, pAttr, cbAttr);
	}

	static inline FARPROC getProc(LPCSTR name)
	{
		if (!module()) return nullptr;
		return GetProcAddress(module(), name);
	}

private:
	static HMODULE module()
	{
		static HMODULE hDwm = load();
		return hDwm;
	}

	static HMODULE load()
	{
		StringBuffer buf(GetSystemDirectory(0, 0));
		GetSystemDirectory(buf.data(),
			static_cast<UINT>(buf.capacity()));

		return LoadLibrary(buf.append(_T("\\dwmapi.dll")).c_str());
	}

	static TyAttribute GetFunction()
	{
		static auto fp = reinterpret_cast<TyAttribute>
			(getProc("DwmGetWindowAttribute"));
		return fp;
	}
};


bool inline isValid(MSG *msg)
{
	return msg->message != WM_QUIT;
}

bool inline getMessageIfExist(MSG * msg, UINT filter)
{
	return isValid(msg)
		&& PeekMessage(msg, NULL, filter, filter, PM_REMOVE) != FALSE
		&& isValid(msg);
}

bool inline waitForMessagePosted(MSG * msg, UINT filter)
{
	return isValid(msg)
		&& GetMessage(msg, NULL, filter, filter) != FALSE;
}

} // namespace


namespace basis {

Window * Window::ConstructingInstance = nullptr;

int Window::run()
{
	MSG msg{};

	// WM_TIMERを優先して処理する
	while (getMessageIfExist(&msg, WM_TIMER)
		|| waitForMessagePosted(&msg, WM_NULL))
	{
		DispatchMessage(&msg);
	}

	int const exit_code = static_cast<int>(msg.wParam);

	return exit_code;
}



ATOM inline Window::
createWindowAtom(const TCHAR *identifier, WNDPROC procedure)
{
	// 別のアプリケーションと競合することはないので何でもいい

	WNDCLASSEX wc{ sizeof(WNDCLASSEX) };
	// wc.cbClsExtra = 0;
	// wc.hInstance = nullptr;
	// wc.lpszMenuName = nullptr;
	// wc.hIconSm = NULL;
	wc.lpfnWndProc = procedure;
	wc.cbWndExtra = sizeof(INT_PTR); // Window*を格納する

									 // LoadIcon, LoadCursorは使わない
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



HWND Window::addChild(const TCHAR *title, Rect pos, DWORD addStyle)
{
	if (pos.empty()) {
		pos.left = pos.top = CW_USEDEFAULT;
	}

	return CreateWindow(TEXT("BUTTON"), title,
		WS_CHILD | WS_VISIBLE | addStyle,
		pos.left, pos.top, pos.width(), pos.height(), m_h, 0,
		reinterpret_cast<HINSTANCE>(GetWindowLongPtr(m_h, GWLP_HINSTANCE)),
		NULL);
}



inline Window * Window::
GetInstance(HWND hWnd)
{
	// ハンドルからインスタンスを取得する
	Window *pInst = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	// 生成時の最初の配信で初期設定をする
	if (!pInst && ConstructingInstance) {
		pInst = ConstructingInstance;
		pInst->m_h = hWnd;

		// ハンドルから取得できるようにアドレスを仕込む
		SetWindowLongPtr(hWnd, GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(pInst));

		ConstructingInstance = nullptr;
	}

	return pInst;
}



// システムコールバック。インスタンスにフックされたリスナに配信する
LRESULT Window::
Dispatch(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	Window *p{ GetInstance(hWnd) };

	int result = p ? p->m_hook.dispatch(p,
		static_cast<Message>(msg), wp, lp) : 0;

	return result ? result : DefWindowProc(hWnd, msg, wp, lp);
}



WNDPROC Window::
GetWndProc(Window *win)
{
	// CreateWindowするとWM_CREATE等のメッセージが
	// staticメンバ関数dispatch()に飛んでくるので、
	// そこでこの値を参照してHWNDに埋め込む。
	ConstructingInstance = win;
	return &Dispatch;
}



int Window::
broadcast(Window *win, Message msg, WPARAM wp, LPARAM lp)
{
	return static_cast<int>(Dispatch(*win, static_cast<UINT>(msg), wp, lp));
}



HWND Window::
createMainWindow(Window *win)
{
	static const TCHAR *className{ TEXT("Default") };

	ATOM atom{ createWindowAtom(className, GetWndProc(win)) };

	return CreateWindowEx(0, reinterpret_cast<TCHAR*>(atom), 0,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
		0, 0, 0, 0);
}



Rect inline Window::
getWindowRect() const
{
	RECT rc;
	if (!GetWindowRect(*this, &rc))
		throw api_runtime_error();
	return rc;
}



bool inline Window::
setWindowRect(const Rect& rc, Set flag) const
{
	return SetWindowPos(*this, 0, rc.left, rc.top,
		rc.width(), rc.height(), static_cast<UINT>(flag))
		!= FALSE;
}



//! Returns the practical rectanle of the window.
Rect Window::
getRect() const
{
	Rect rc;
	if (DWM::IsCompositionEnabled() &&
		DWM::GetExtendedRect(this, &rc))
		return rc;
	return getWindowRect();
}



bool Window::
setRect(Rect dest) const
{
	RECT rc;
	if (!GetWindowRect(*this, &rc))
		return false;

	// エアログラスの領域分ずらした位置を計算
	dest = dest + rc - getRect();

	return setWindowRect(dest, Set::RESIZE);
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
getClientRectInVirtualScreen() const
{
	auto rc = getClientRect();
	LONG right = rc.right, bottom = rc.bottom;
	ClientToScreen(*this, reinterpret_cast<POINT*>(&rc.right));
	rc.left += rc.right - right;
	rc.top += rc.bottom - bottom;
	return rc;
}



Size Window::
clientSize() const
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
	return setWindowRect(rc, Set::MOVE);
}



bool Window::
moveTo(Point pt) const
{
	auto size = getSize();
	return MoveWindow(*this, pt.x, pt.y, size.x, size.y, TRUE) != FALSE;
}



Size Window::
getSize() const
{
	return getRect().size();
}



int Window::
getWidth() const
{
	return getRect().width();
}



int Window::
getHeight() const
{
	return getRect().height();
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

	Rect rc = (maximized) ? getRect() : getClientRectInVirtualScreen();

	if (bPopup) {
		setStyle((getStyle() &~WS_OVERLAPPEDWINDOW) | WS_POPUP);
		applyFrame();
		setRect(rc);
		return;
	}

	setStyle((getStyle() &~WS_POPUP) | WS_OVERLAPPEDWINDOW);
	applyFrame();
	setRect(maximized ? rc : rc += getRect() -= getClientRectInVirtualScreen());
}



bool Window::
isPopup() const
{
	return (getStyle() & WS_POPUP) != 0;
}



void Window::
show(int nShow) const
{
	ShowWindow(*this, nShow);
}



void Window::
hide() const
{
	ShowWindow(*this, SW_HIDE);
}



void Window::
invalidate(const RECT *rc) const
{
	// rcがnullptr、 空、有効の3パターンあるので、
	// basis::Rect版をつくるならオーバーロード定義が必要
	if (InvalidateRect(*this, rc, 0) == FALSE)
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
	return setWindowRect({}, Set::ACTIVATE);
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
	auto size = static_cast<int>
		(DefWindowProc(*this, WM_GETTEXTLENGTH, 0, 0));



	StringBuffer buf(size + 1);
	DefWindowProc(*this, WM_GETTEXT,
		static_cast<WPARAM>(buf.capacity()),
		reinterpret_cast<LPARAM>(buf.data()));
	return buf;
}

} // namespace