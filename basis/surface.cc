#include "surface.h"
#include "window.h"
#include "stdfnc.h"

#ifndef ARG_POS_SIZE
#define ARG_POS_SIZE(rc) rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top
#endif

namespace basis {



void Surface::reset(HDC hdc) noexcept
{
	resetFont();
	if (m_default && m_default != reinterpret_cast<HBITMAP>(-1)) {
		DeleteObject(swapBitmap(m_default));
		m_default = 0;
	}
	if (m_h && m_default != reinterpret_cast<HBITMAP>(-1)) {
		DeleteDC(m_h);
	}
	m_h = hdc;
}



void Surface::
create(HDC src, Size s)
{
	HDC hdc = src ? src : GetDC(0);
	reset(CreateCompatibleDC(hdc));
	setBitmap(CreateCompatibleBitmap(hdc, s.x, s.y));
	if (!src)
		ReleaseDC(0, hdc);
}



bool Surface::
compatible(HDC hdc, Size s)
{
	Size size = getSize();
	if (!m_h || size.x < s.x || size.y < s.y ||
		GetDeviceCaps(hdc, BITSPIXEL) != GetDeviceCaps(m_h, BITSPIXEL)) {
		create(hdc, s);
		return true;
	}
	return false;
}



HPEN Surface::
pen(HPEN h) const noexcept
{	// Null if error
	return static_cast<HPEN>(SelectObject(m_h, h));
}



HPEN Surface::
pen(HGDIOBJ h) const noexcept
{
	return pen(static_cast<HPEN>(h));
}



HBRUSH Surface::
brush(HBRUSH h) const noexcept
{
	return static_cast<HBRUSH>(SelectObject(m_h, h));
}



HBRUSH Surface::
brush(HGDIOBJ h) const noexcept
{
	return brush(static_cast<HBRUSH>(h));
}



bool Surface::
rectangle(const Rect & rc) const noexcept
{
	return 0 != Rectangle(m_h, rc.left, rc.top, rc.right, rc.bottom);
}



HFONT Surface::
setFont(HFONT f)
{
	f = static_cast<HFONT>(SelectObject(m_h, f));
	if (!m_font) {
		m_font = f;
		return 0;
	}
	return f;
}



HFONT Surface::
resetFont()
{
	HFONT h;
	if (m_font) {
		h = setFont(m_font);
		m_font = 0;
		return h;
	}
	return 0;
}



int Surface::
getFontHeight()
{
	return GetFontHeight(m_h, 0, 0);
}



Size Surface::
getSize() const noexcept
{	// Null if error
	return getSize(static_cast<HBITMAP>(
		GetCurrentObject(m_h, OBJ_BITMAP)));
}



size_t Surface::
usage() const
{
    Size size = getSize();
    return GetDeviceCaps(m_h, BITSPIXEL) * GetDeviceCaps(m_h, PLANES) / 8 *
        size.x * size.y;
}



Size Surface::
getSize(HBITMAP h) noexcept
{
	BITMAP bmp;
	if (GetObject(h, sizeof bmp, &bmp))
		return{ bmp.bmWidth, bmp.bmHeight };
	else
		return{ 0, 0 };
}



bool Surface::
setBitmap(HBITMAP hBmp)
{
	if (!hBmp)
		throw 0;

	if (!m_h)
		create(0, getSize(hBmp));

	HBITMAP ret = swapBitmap(hBmp);

	if (m_default)
		DeleteObject(ret);
	else
		m_default = ret;

	return ret != hBmp;
}



HBITMAP Surface::
swapBitmap(HBITMAP hBmp)
{
	return static_cast<HBITMAP>(SelectObject(m_h, hBmp));
}



bool Surface::
transfer(HDC hdc, const Rect &dest,	const Rect& src) const
{
	if (!m_h)
		return false;

	if (dest.size() == src.size()) {
		return (0 != BitBlt(hdc, ARG_POS_SIZE(dest),
			m_h, src.left, src.top, SRCCOPY));
	}

	int prevMode = SetStretchBltMode(hdc, HALFTONE);
	if (prevMode == 0 || prevMode == ERROR_INVALID_PARAMETER)
		return false;

	// HALFTONEÇ…ê›íËÇµÇΩèÍçáÇÕïKÇ∏çƒê›íË
	SetBrushOrgEx(hdc, 0, 0, nullptr);

	return (0 != StretchBlt(hdc, ARG_POS_SIZE(dest),
		m_h, ARG_POS_SIZE(src), SRCCOPY));
}

}  // namespace