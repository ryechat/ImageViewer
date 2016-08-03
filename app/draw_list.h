#pragma once
#ifndef GUID_2BBA94E8F65D481D904502F7C1B79E19
#define GUID_2BBA94E8F65D481D904502F7C1B79E19

#include "movable.h"
#include "list_item.h"
#include "image_viewer.h"
#include "surface.h"

namespace image_viewer {

class CImageViewer::CDrawList {
public:
	using Surface = basis::Surface;
	CDrawList(CImageViewer &parent_);
    ~CDrawList();

    // Shows the list.
    void show() {
        m_enable = true;
        invalidate();
    }

    // Hides the list.
    void hide() {
        m_enable = false;
        invalidate();
    }

    // Invalidate the rectangle to where it draws the list.
	void invalidate();

	//! Returns rectangle that the list was drawn expressed by client coordinate.
	Rect rect() { return m_offset.rect(); }

	//! Draws the list.
	Rect draw(Surface *s) { return drawList(s, true); }

	//! Moves the list. This will cause WM_PAINT message posted.
	void move(basis::Size diff) {
		parent.invalidate(rect());
		m_offset.move(diff);
		invalidate();
	}

    iterator itemFromPt(basis::Point pt);
    bool isInclusive(basis::Point pt);

private:
	/*! Draws the list and returns the rectangle.
		If bDraw was set to be false then not drawn,
		this allows getting next area to draw.
	*/
	Rect drawList(Surface *s, bool bDraw);
	Rect do_drawList(Surface *s, bool bDraw);

    using Position = std::pair<iterator, Rect>;
    std::vector<Position> m_pos;

	CImageViewer &parent;
	bool m_enable;
	HFONT boldFont;
	basis::CMovable m_offset;
	std::basic_string<TCHAR> sNoFileInfo; // Drawn if there was no file to show.

	enum ColorType { not_read, now_loading, loaded_image, current_file };
	static constexpr COLORREF Colors[sizeof(ColorType)] =
		{ 0, RGB(0, 0, 255), RGB(0, 255, 0), RGB(255, 0, 0) };
};

}  // namespace

#endif