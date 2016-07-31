#include "mouse_drag.h"
#include "window_message.h"

#ifndef GET_X_LPARAM	// windowsx.h
#define GET_X_LPARAM(lp)	((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp)    ((int)(short)HIWORD(lp))
#endif

namespace {

inline bool isStillPressing(WPARAM wp, UINT vkey) {
	if (vkey == VK_LBUTTON && (wp & MK_LBUTTON))
		return true;
	if (vkey == VK_RBUTTON && (wp & MK_RBUTTON))
		return true;
	return false;
}

inline basis::Point GetMousePosition(const LPARAM lp) {
	basis::Point p;
	p.x = GET_X_LPARAM(lp);
	p.y = GET_Y_LPARAM(lp);
	return p;
}
}  // namespace

namespace basis {

CMouseDrag::CMouseDrag()
	: m_keys(0), m_vkey(0), m_state(STATE::BUTTON_UP)
{}



// BUTTONDOWN時にstate = MD_BUTTON_DOWN
// MOUSEMOVE時にstate |= MD_MOUSE_MOVE
// スレッショルドを越えてたら |= MD_MOUSE_DRAG
// BUTTONUP時にstate |= MD_BUTTON_UP

bool CMouseDrag::proc(HWND hWnd, basis::Message msg, WPARAM wp, LPARAM lp)
{
	// ドラッグ検出
	using WM = basis::Message;
	if (msg == WM::MOUSEMOVE) {
		updatePos(hWnd, lp);
		if (m_state & STATE::BUTTON_DOWN) {
			// 離された瞬間を検出できていない場合に備える
			if (!isStillPressing(wp, m_vkey)) {
				m_state = (m_state & ~STATE::BUTTON_DOWN)
					| STATE::BUTTON_UP;
				return false;
			}
			// すでにドラッグ中
			if (isDragged())
				return true;

			// スレッショルドチェック
			m_state |= STATE::MOUSE_MOVE;
			if (isOveringThreshold(m_pos)) {
				m_state |= STATE::MOUSE_DRAG;
				return true;
			}
		}
		return false;
	}

	// モニタ開始
	if (msg == WM::LBUTTONDOWN || msg == WM::RBUTTONDOWN) {
		m_state = STATE::BUTTON_DOWN;
		m_start = m_prev = m_pos = GetMousePosition(lp);
		m_vkey = (msg == WM::LBUTTONDOWN) ? VK_LBUTTON : VK_RBUTTON;
		m_keys = static_cast<UINT>(wp);
		if (GetKeyState(VK_MENU)) {
			m_keys |= MK_MENU;
		}
		return true;
	}

	// モニタ終了
	if (msg == WM::LBUTTONUP || msg == WM::RBUTTONUP) {
		// 対象のキーでなければ通知しない
		if (m_vkey == VK_LBUTTON && msg != WM::LBUTTONUP)
			return false;
		if (m_vkey == VK_RBUTTON && msg != WM::RBUTTONUP)
			return false;
		m_state = (m_state & ~STATE::BUTTON_DOWN)
			| STATE::BUTTON_UP;
		return true;
	}
	return false;
}



// m_prev, m_posを更新する
// マルチモニタ環境での負数の座標値にLOWORDマクロは使えない

Point& CMouseDrag::updatePos(HWND hWnd, const LPARAM lp)
{
	m_prev = m_pos;
	m_pos = GetMousePosition(lp);

	// 絶対座標の保存
	m_prev_scr = m_scr;
	POINT pt{ m_pos.x, m_pos.y };
	ClientToScreen(hWnd, &pt);
	m_scr = static_cast<Point>(pt);
	return m_pos;
}



// スレッショルドを(すでに)越えたかどうか

bool CMouseDrag::isOveringThreshold(Point p)
{
	p.x -= m_start.x;
	p.y -= m_start.y;
	if (m_threshold.x > 0) {
		if (p.x >= m_threshold.x || p.x <= -m_threshold.x)
			return true;
	}
	if (m_threshold.y > 0)
		return (p.y >= m_threshold.y || p.y <= -m_threshold.y);
	return false;
}



int CMouseDrag::dx() {
	return isMoved() ? m_pos.x - m_prev.x : 0;
}



int CMouseDrag::dy() {
	return isMoved() ? m_pos.y - m_prev.y : 0;
}



basis::Size CMouseDrag::getDifference()
{
	if (isMoved())
		return static_cast<Size>(m_pos - m_prev);
	else
		return{};
}



basis::Size CMouseDrag::getTravel() {
	if (isMoved())
		return static_cast<Size>(m_pos - m_start);
	else
		return{};
}



int CMouseDrag::
dx_abs()
{
	return isMoved() ? m_scr.x - m_prev_scr.x : 0;
}



int CMouseDrag::
dy_abs()
{
	return isMoved() ? m_scr.y - m_prev_scr.y : 0;
}



Size CMouseDrag::threshold(int dx, int dy) {
	if (dx != -1) {
		if (dy == -1) dy = dx;
		m_threshold.x = dx;
		m_threshold.y = (dy != -1) ? dy : dx;
	}
	return m_threshold;
}

}  // namespace