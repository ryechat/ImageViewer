#pragma once
#ifndef GUID_A3A269A23F564BB2B49C231E7FD120CA
#define GUID_A3A269A23F564BB2B49C231E7FD120CA

#ifndef STDAFX_H
#include <wtypes.h>
#endif

#include "types.h"
#include "critical_section.h"
#include "iunit_test.h"

namespace basis {

UNIT_TEST(CMovable)


//! �ړ��\�ȃO���t�B�b�N�I�u�W�F�N�g�̍��W�B
/*!		�e�����Ƃ��e�̓����ɘA������B
		�X�N���[�����W��pt, �e�ɑ΂��鑊�΍��W��pos�ŕ\���B
*/
class CMovable {
public:
	struct Test;

	CMovable(CMovable *parent = nullptr);

	CMovable(CMovable *parent, Point pos, Size size = {});
	CMovable(const Rect& rc);
	virtual ~CMovable();

	CMovable& operator=(const Rect &rc);

	//! ��΍��W��Ԃ��B
	Point pt();

	//! ���΍��W��Ԃ��B
	Point pos();

	void forceSetPos(Point pos);

	//! ���΍��W�Ɉړ�����B
	bool setPos(Point pos);

	//! ���݈ʒu���瑊�Έړ�����B
	bool move(Size amount);

	//! ��΍��W�Ɉړ�����B
	/*! @return �ʒu�ύX�ɐ����������ǂ����B
	*/
	bool moveTo(Point pt);

	//! �w����W�Ɉړ��\���ǂ�����Ԃ��B
	/*! ���̊֐����I�[�o�[���C�h���邱�Ƃňړ��͈͂𐧌�����B
	*/
	virtual bool isMovableTo(Point pos);

	int width();
	void width(int w);
	int height();
	void height(int h);

	Size size();
	void size(Size s);

	//! 4���_�̐�΍��W��Ԃ�
	Rect rect();

	//! ��΍��W�𑊑΍��W�ɕϊ�
	Point posFromPt(Point pt);

	//! ���΍��W���΍��W�ɕϊ�
	Point ptFromPos(Point pos);

	//! �I�t�Z�b�g��Ԃ��I�u�W�F�N�g
	CMovable *parent();
	void setParent(CMovable *parent);

	//! �f�t�H���g�̑��Έʒu���w�肷��
	void setBase(Point pos);

	//! �f�t�H���g�̑��Έʒu�Ɉړ�����
	void resetPos();
	
protected:
	CMovable *m_parent; //!< �e
	Point     m_pos;	//!< �e�ɑ΂��鑊�΍��W
	Point	  m_base;	//!< ���΍��W�̊�{�l�Breset�̃p�����[�^�B
	Size      m_size;	//!< �s�N�Z���P�ʂ̃T�C�Y
	CriticalSection m_cs;
};

inline Point CMovable::
pos() { return m_pos; }

inline void CMovable::
forceSetPos(Point pos) { m_pos = pos; }

inline int CMovable::
width() { return m_size.x; }

inline void CMovable::
width(int w) { m_size.x = w; }

inline int CMovable::
height() { return m_size.y; }

inline void CMovable::
height(int h) { m_size.y = h; }

inline Size CMovable::
size() { return m_size; }

inline void CMovable::
size(Size s) { m_size = s; }

inline CMovable * CMovable::
parent() { return m_parent; }

inline void CMovable::
setParent(CMovable * parent) { m_parent = parent; }

inline void CMovable::
setBase(Point pos) {
	m_base = pos;
}

inline void CMovable::
resetPos() {
	m_pos = m_base;
}

}  // namespace

#endif