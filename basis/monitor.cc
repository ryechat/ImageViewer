/*!
    @file
        Monitor definition.
*/
#include "singleton.h"
#include "exception.h"
#include "monitor.h"
#pragma comment(lib, "user32.lib")

namespace basis {

Monitor::Monitor(HMONITOR h)
    : m_h(h)
{}



Monitor::Monitor(const Point &pos, bool nearest)
    : Monitor(MonitorFromPoint({ pos.x, pos.y }, nearest
        ? MONITOR_DEFAULTTONEAREST : MONITOR_DEFAULTTONULL))
{}



Monitor::Monitor(const RECT &rc)
    : Monitor(MonitorFromRect(&rc, MONITOR_DEFAULTTONULL))
{}



int    Monitor::Count()    // static
{
    return GetSystemMetrics(SM_CMONITORS);
}



Monitor Monitor::GetPrimary()
{
    return MonitorFromPoint({ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
}



Rect Monitor::GetVirtualScreen()    // static
{
    // Virtual screen is not available on Win95
    RECT rc{ 0, 0, GetSystemMetrics(SM_CXVIRTUALSCREEN) };
    if (rc.right) {
        rc.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
        rc.right += rc.left;
        rc.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
        rc.bottom = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        rc.bottom += rc.top;
    }
    else {
        rc.right = GetSystemMetrics(SM_CXSCREEN);
        rc.bottom = GetSystemMetrics(SM_CYSCREEN);
    }
    return rc;
}



bool Monitor::isPrimary()
{
    return (getMonitorInfo().dwFlags & MONITORINFOF_PRIMARY) != 0;
}



Rect Monitor::getRect()
{
    return getMonitorInfo().rcMonitor;
}



Rect Monitor::getWorkArea()
{
    return getMonitorInfo().rcWork;
}



MONITORINFO Monitor::getMonitorInfo()
{
    MONITORINFO mi{ sizeof(mi) };
    GetMonitorInfo(m_h ? m_h : GetPrimary(), &mi);
    return mi;
}



std::basic_string<TCHAR> Monitor::getName()
{
    MONITORINFOEX mi;
    mi.cbSize = sizeof(mi);
    GetMonitorInfo(m_h ? m_h : GetPrimary(), &mi);
    return mi.szDevice;
}

}  // namespace