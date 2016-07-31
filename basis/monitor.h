#pragma once
#ifndef GUID_A03BAB06D25C40F99E26A5AE91EC30D8
#define GUID_A03BAB06D25C40F99E26A5AE91EC30D8
#include "types.h"
#include "iunit_test.h"

namespace basis {

UNIT_TEST(Monitor)

/*! Monitor class.
	@par About virtual screen coordinates.
	The origin point (0, 0) of virtual screen coordinates
	is left-top of a primary window.
	All windows placed left or above primary window should
	have negative coordinates.
	Note that if a window is size of 1024x768,
	the coordinate (1024, 768) is NOT available
	because horizontal coordinates is from 0 to 1023.
	GDI functions, though, accepts like rectangle(0, 0, 1024, 768)
	and it will paint at rectangle(0, 0, 1023, 767)
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
		If monitor is not selected; handle is NULL,
		returns rectangle of privary monitor instead.
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
	MONITORINFO getMonitorInfo();
	HMONITOR m_h;
};

}  // namespace

#endif
