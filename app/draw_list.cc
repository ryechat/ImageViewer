#ifndef _STDAFX
#include <algorithm>
#endif

#include "stdfnc.h"
#include "draw_list.h"
#include "profile.h"
#include "window.h"
#include "menu.h"
#include "filer.h"
#include "loader.h"
#include "ids.h"

namespace {
/*! Draws a string onto the screen.
    @return Output rectangle.
    @param pt Position of left-top corner of the text.
    @param bOut Whether put out the text or not.
*/
basis::Rect TextOutRect(HDC h, basis::Point pt, const TCHAR *str, bool bOut)
{
	// It seems that TextOut function couldn't recognize SJIS strings,
	// so SJIS strings drawn by TextOut function corrupted.
	// To deal with this, we should call UNICODE version of it.
	
	auto wstr = basis::StringBuffer(0, str).toUTF16();
	SIZE s{};
	GetTextExtentPoint32W(h, wstr.c_str(), static_cast<int>(wstr.size()), &s);

	basis::Rect rc = { pt.x, pt.y, pt.x + s.cx, pt.y + s.cy };
	if (bOut)
		TextOutW(h, pt.x, pt.y, wstr.c_str(), static_cast<int>(wstr.size()));
	return rc;
}
} // namespace



namespace image_viewer {

CImageViewer::CDrawList::
CDrawList(CImageViewer &parent_) : parent(parent_), m_enable(false)
{
	sNoFileInfo = parent.profile->getTranslatedString(ID::LIST_EMPTY);
	m_offset.setBase({ 5, 5 });
	m_offset.resetPos();

	LOGFONT f;
	f.lfHeight = 0;
	f.lfWidth = 0;
	f.lfEscapement = 0;
	f.lfOrientation = 0;
	f.lfWeight = FW_BOLD;
	f.lfItalic = 0;
	f.lfUnderline = 0;
	f.lfStrikeOut = 0;
	f.lfCharSet = ANSI_CHARSET;
	f.lfOutPrecision = OUT_DEFAULT_PRECIS | OUT_TT_PRECIS;
	f.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	f.lfQuality = DRAFT_QUALITY;
	f.lfPitchAndFamily = VARIABLE_PITCH | FF_DONTCARE;
	f.lfFaceName[0] = '\0';
	boldFont = CreateFontIndirect(&f);
}



CImageViewer::CDrawList::~CDrawList()
{
    DeleteObject(boldFont);
}



void CImageViewer::CDrawList::
invalidate()
{
	parent.invalidate(m_offset.rect());
	parent.invalidate(drawList(&parent.m_backbuffer, false));
}



//! リストの出力範囲を返す
/*!	bDraw = true : リストを出力し、描画範囲を更新する
	bDraw = false: 描画範囲をかえす（更新はしない）
*/
basis::Rect CImageViewer::CDrawList::
drawList(Surface *surface, bool bDraw)
{
	Rect rc;
	if (parent.filer->isEmpty()) {
		surface->setFont(boldFont);
		if (bDraw)
			SetTextColor(*surface, Colors[not_read]);
		rc = TextOutRect(*surface, m_offset.pos(), sNoFileInfo.c_str(), bDraw);
	}
	else if (m_enable) {
		rc = do_drawList(surface, bDraw);
	}

	if (bDraw) {
		m_offset.width((std::max)(rc.right - m_offset.pt().x, 0));
		m_offset.height((std::max)(rc.bottom - m_offset.pt().y, 0));
	}

	return rc;
}



basis::Rect CImageViewer::CDrawList::
do_drawList(Surface *surface, bool b)
{
    Rect rc, total;
	int pitch = surface->getFontHeight();
	int client_height = parent.getClientRect().height();
	iterator iEnd = parent.filer->end();
    if (b)
        m_pos.clear();

	basis::Point pt = m_offset.pos();
	ColorType color;
	for (iterator item = parent.filer->begin(); item != iEnd; ++item) {
		if (pt.y >= client_height)
			break;
		if (pt.y <= -pitch) {
			pt.y += pitch;
			continue;
		}

		if (item == parent.filer->current()) {
			surface->setFont(boldFont);
			color = current_file;
		}
		else {
			surface->resetFont();
			if (!b || item->get()->isLoaded())
				color = loaded_image;
			else if (parent.loader->isLoading(item))
				color = now_loading;
			else
				color = not_read;
		}
		if (b)
			SetTextColor(*surface, Colors[color]);

        rc = TextOutRect(*surface, pt, item->get()->fileName(), b);
        total.unite(rc);
        if (b)
            m_pos.push_back({ item, rc });
		pt.y += (color == current_file) ? surface->getFontHeight() : pitch;
	}
    surface->resetFont();
	return total;
}



CImageViewer::iterator CImageViewer::CDrawList::
itemFromPt(basis::Point pt)
{
    for (auto &i : m_pos) {
        if (i.second.isInclusive(pt))
            return i.first;
    }
    return parent.filer->end();
}



bool CImageViewer::CDrawList::isInclusive(basis::Point pt) {
    return itemFromPt(pt) != parent.filer->end();
}

}  // namespace