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


//! 移動可能なグラフィックオブジェクトの座標。
/*!		親を持つとき親の動きに連動する。
		スクリーン座標はpt, 親に対する相対座標はposで表す。
*/
class CMovable {
public:
	struct Test;

	CMovable(CMovable *parent = nullptr);

	CMovable(CMovable *parent, Point pos, Size size = {});
	CMovable(const Rect& rc);
	virtual ~CMovable();

	CMovable& operator=(const Rect &rc);

	//! 絶対座標を返す。
	Point pt();

	//! 相対座標を返す。
	Point pos();

	void forceSetPos(Point pos);

	//! 相対座標に移動する。
	bool setPos(Point pos);

	//! 現在位置から相対移動する。
	bool move(Size amount);

	//! 絶対座標に移動する。
	/*! @return 位置変更に成功したかどうか。
	*/
	bool moveTo(Point pt);

	//! 指定座標に移動可能かどうかを返す。
	/*! この関数をオーバーライドすることで移動範囲を制限する。
	*/
	virtual bool isMovableTo(Point pos);

	int width();
	void width(int w);
	int height();
	void height(int h);

	Size size();
	void size(Size s);

	//! 4頂点の絶対座標を返す
	Rect rect();

	//! 絶対座標を相対座標に変換
	Point posFromPt(Point pt);

	//! 相対座標を絶対座標に変換
	Point ptFromPos(Point pos);

	//! オフセットを返すオブジェクト
	CMovable *parent();
	void setParent(CMovable *parent);

	//! デフォルトの相対位置を指定する
	void setBase(Point pos);

	//! デフォルトの相対位置に移動する
	void resetPos();
	
protected:
	CMovable *m_parent; //!< 親
	Point     m_pos;	//!< 親に対する相対座標
	Point	  m_base;	//!< 相対座標の基本値。resetのパラメータ。
	Size      m_size;	//!< ピクセル単位のサイズ
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