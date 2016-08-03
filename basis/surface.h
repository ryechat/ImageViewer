#pragma once
#ifndef GUID_34D3C18605834843B468B02CA31C7E47
#define GUID_34D3C18605834843B468B02CA31C7E47

#ifndef STDAFX_H
#include <memory>
#endif

#include "types.h"

namespace basis {

/*
	Lapper-class for Device Contexts and those APIs associated with.
*/
class Surface
{
public:
	Surface() : Surface(0) {}
	Surface(HDC hdc) : m_h(hdc),
		m_default(hdc ? reinterpret_cast<HBITMAP>(-1) : 0),
		m_font(0) {}
	~Surface() { reset(); }

	Surface(const Surface &s) = delete;
	Surface&operator=(const Surface &s) = delete;
	Surface(Surface &&s) { *this = std::move(s); }
	Surface&operator=(Surface &&s) {
		reset(s.m_h);
		m_default = s.m_default;
		m_font = s.m_font;
		s.m_h = 0;
		s.m_default = 0;
		s.m_font = 0;
		return *this;
	}

	explicit operator bool() const { return m_h != 0; }
	operator HDC() const { return m_h; }

	void reset() noexcept { reset(0); }

	/*! Creates itself as memory device context.
	*/
	void create(HDC src, Size s);

	/*! Ensures compatibility and minimum size.
		Unless this object has the same src hdc
		AND adequate size, this function will call create().
		@return If surface recreated, return value is true.
		@note
			If src was not the same hdc that has been specified
			when current bitmap had been created,
			this function creates new one even if
			two HDCs were actually compatible with each other.
			My personal test showed that the hdc
			returned by GetDC or BeginPaint was not changed
			while owner display was still the same one.
	*/
	bool compatible(HDC hdc, Size s);

	//! Selects GDI Pen object as current.
	//! @return Previous HPEN.
	HPEN pen(HPEN h) const noexcept;

	//! Selects GDI Pen object as current.
	//! @return Previous HPEN.
	HPEN pen(HGDIOBJ h) const noexcept;

	//! Selects GDI Brush object as current.
	//! @return Previous HBRUSH.
	HBRUSH brush(HBRUSH h) const noexcept;

	//! Selects GDI Brush object as current.
	//! @return Previous HBRUSH.
	HBRUSH brush(HGDIOBJ h) const noexcept;

	bool rectangle(const Rect& rc) const noexcept;

	HFONT setFont(HFONT);
	HFONT resetFont();
	int getFontHeight();

	//! Returns width and height of the Device Context Bitmap. 
	Size getSize() const noexcept;

    //! Returns estimated memory usage in bytes.
    size_t usage() const;

	/*! Sets new bitmap as current.
		Previous bitmap would be released.
	*/
	bool setBitmap(HBITMAP hBmp);

	bool transfer(HDC hdc, const Rect&dest, const Rect& src) const;

	static Size getSize(HBITMAP h) noexcept;

protected:
	HBITMAP swapBitmap(HBITMAP hBmp);
	void reset(HDC hdc) noexcept;
private:
	HDC m_h;

	// Default bitmap. It must be set back before m_h deleted not to be leaked.
	// Also, If it is -1, m_h is not necessary to be deleted.
	HBITMAP m_default;
	HFONT m_font;
};

}  // namespace

#endif