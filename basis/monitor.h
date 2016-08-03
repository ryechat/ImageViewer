#pragma once
#ifndef GUID_A03BAB06D25C40F99E26A5AE91EC30D8
#define GUID_A03BAB06D25C40F99E26A5AE91EC30D8
#include "types.h"
#include "iunit_test.h"

namespace basis {

UNIT_TEST(Monitor)

/*! Monitor class.
	@par In virtual screen coordinates,
	its origin point (0, 0) is left-top of a primary window.
	All windows placed left or above of primary window should
	have negative coordinates.
	Note that if a window is of 1024x768 size,
	the coordinate (1024, 768) is not included,
	because horizontal coordinates is from 0 to 1023.
	GDI functions, though, accepts rectangle(0, 0, 1024, 768)
	and it will paint rectangle at (0, 0, 1023, 767)
	ignoring right and bottom end of the rectangle.
*/
class Monitor {
public:
	Monitor(HMONITOR h = 0);
	Monitor(const Point &pos, bool nearest = false);
	Monitor(const RECT &rc);

	Monitor(const Monitor&) = default;
	Monitor&operator=(const Monitor&) = default;

	operator HMONITOR() { return m_h; }

	//! Returns number of monitors alive.
	static int Count();

	//! Returns primary monitor.
	static Monitor GetPrimary();

	//! Returns rectangle of virtual screen.
	static Rect GetVirtualScreen();

	bool isPrimary();

	/*! Return rectangle of display monitor.
		If monitor is not selected, rectangle of privary monitor will be returned.
	*/
	Rect getRect();

	/*! Returns rectangle of work area.
		Work area, despite task bar and side bar area,
		is used to maximize application window by Windows.
	*/
	Rect getWorkArea();

	//! Returns device name.
	std::basic_string<TCHAR> getName();

private:
    static BOOL CALLBACK proc(
        HMONITOR hMonitor,  // ディスプレイモニタのハンドル
        HDC hdcMonitor,     // モニタに適したデバイスコンテキストのハンドル
        LPRECT lprcMonitor, // モニタ上の交差部分を表す長方形領域へのポインタ
        LPARAM dwData       // EnumDisplayMonitors から渡されたデータ
        );
	MONITORINFO getMonitorInfo();
	HMONITOR m_h;
};

}  // namespace

#endif
