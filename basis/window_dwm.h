#pragma once
#ifndef GUID_DAA7C9A6D5F64054916D44959063EFA8
#define GUID_DAA7C9A6D5F64054916D44959063EFA8
#include "window.h"
#include "string_buffer.h"

namespace basis {

/*! DWM provides the way to get actual window rect.
    Because of that GetWindowRect API returns the rectangle
    including skelton area on Windows Vista or later system,
    the rectangle returned by GetWindowRect API would be different
    from what we could see on the screen.
    In the case that actual window rect is necessary,
    IsCompositionEnabled() function tells us whether it should call
    GetExtendedRect() function instead of GetWindowRect API.
*/
class Window::DWM {
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
        BOOL b;
        return (fp && fp(&b) == S_OK && b != FALSE);
    }

    /*! Gets the practical rectangle of the window.
    This function works only on Windows Vista or later
    and when IsCompositionEnabled() function returns true.
    */
    static bool GetExtendedRect(const Window *h, RECT* p)
    {
        return S_OK != getWindowAttribute(h,
            DWMWA_EXTENDED_FRAME_BOUNDS, p, sizeof(RECT));
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

}  // namespace

#endif