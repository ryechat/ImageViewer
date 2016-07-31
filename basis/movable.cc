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



//! ��΍��W��Ԃ��B

Point CMovable::pt() {
	auto cs = m_cs.local();
	if (!m_parent) return m_pos;
	return m_parent->pt() + m_pos;
}

//! ���΍��W�Ɉړ�����B

bool CMovable::setPos(Point pos) {
	if (!isMovableTo(pos))
		return false;
	forceSetPos(pos);
	return true;
}

//! ���݈ʒu���瑊�Έړ�����B

bool CMovable::move(basis::Size amount) {
	return setPos(m_pos + amount);
}

//! ��΍��W�Ɉړ�����B
/*! @return �ʒu�ύX�ɐ����������ǂ����B
*/

bool CMovable::moveTo(Point pt) {
	return setPos(posFromPt(pt));
}

//! �w����W�Ɉړ��\���ǂ�����Ԃ��B
/*! ���̊֐����I�[�o�[���C�h���邱�Ƃňړ��͈͂𐧌�����B
*/

bool CMovable::isMovableTo(Point pos) {
	(void)pos;
	return true;
}

//! 4���_�̐�΍��W��Ԃ�

Rect CMovable::rect() {
	Point p(pt());
	return{ p.x, p.y, p.x + m_size.x, p.y + m_size.y };
}

//! ��΍��W�𑊑΍��W�ɕϊ�

Point CMovable::posFromPt(Point pt) {
	auto cs = m_cs.local();
	return (m_parent) ? pt - m_parent->pt() : pt;
}

//! ���΍��W���΍��W�ɕϊ�

Point CMovable::ptFromPos(Point pos) {
	auto cs = m_cs.local();
	return (m_parent) ? m_parent->pt() + m_pos : pos;
}

}  // namespace