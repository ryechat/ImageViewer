#include "movable.h"

namespace basis {

CMovable::CMovable(CMovable *parent)
: CMovable(parent, {}, {}) {}

CMovable::CMovable(CMovable *parent, Point pos, Size size)
: m_parent(parent), m_pos(pos), m_size(size) {}

CMovable::~CMovable() {}

CMovable::CMovable(const Rect& rc)
	: CMovable(nullptr, rc.lefttop(), rc.size())
{}

CMovable& CMovable::operator=(const Rect &rc)
{
	moveTo(rc.lefttop());
	width(rc.width());
	height(rc.height());
	return *this;
}



//! 絶対座標を返す。

Point CMovable::pt() {
	auto cs = m_cs.local();
	if (!m_parent) return m_pos;
	return m_parent->pt() + m_pos;
}

//! 相対座標に移動する。

bool CMovable::setPos(Point pos) {
	if (!isMovableTo(pos))
		return false;
	forceSetPos(pos);
	return true;
}

//! 現在位置から相対移動する。

bool CMovable::move(basis::Size amount) {
	return setPos(m_pos + amount);
}

//! 絶対座標に移動する。
/*! @return 位置変更に成功したかどうか。
*/

bool CMovable::moveTo(Point pt) {
	return setPos(posFromPt(pt));
}

//! 指定座標に移動可能かどうかを返す。
/*! この関数をオーバーライドすることで移動範囲を制限する。
*/

bool CMovable::isMovableTo(Point pos) {
	(void)pos;
	return true;
}

//! 4頂点の絶対座標を返す

Rect CMovable::rect() {
	Point p(pt());
	return{ p.x, p.y, p.x + m_size.x, p.y + m_size.y };
}

//! 絶対座標を相対座標に変換

Point CMovable::posFromPt(Point pt) {
	auto cs = m_cs.local();
	return (m_parent) ? pt - m_parent->pt() : pt;
}

//! 相対座標を絶対座標に変換

Point CMovable::ptFromPos(Point pos) {
	auto cs = m_cs.local();
	return (m_parent) ? m_parent->pt() + m_pos : pos;
}

}  // namespace