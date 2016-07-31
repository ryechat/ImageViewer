/*!
@file mouse_drag.h	2015/12/12

マウスドラッグのリスナクラス
ウィンドウメッセージからドラッグに関連する操作を抽出し
ドラッグの開始・実行・終了のタイミングを通知、
ドラッグ量などの関数を提供する
*/
#pragma once
#ifndef CMouseDrag_h
#define CMouseDrag_h

#ifndef STDAFX_H
#include <wtypes.h>
#endif
#include "types.h"

// マウス関連のWM_でわたされるボタン状態(WPARAM)にALTキーをつけくわえて、
// .m_keysで開始時のボタン押下状態を管理する
// 環境によってはMK_ALTが存在するらしいのでこの名前は避ける
#ifndef MK_MENU
#define MK_MENU 0x80
#endif
// 他はWM_LBUTTONDOWNでわたされるWPARAMと同じ

namespace basis {

enum class Message : int;

class CMouseDrag {
public:
	using Point = basis::Point;
	using Size = basis::Size;
	enum STATE : int {
		UNDEFINED = 0,
		BUTTON_UP = 1,
		BUTTON_DOWN = 2,
		MOUSE_MOVE = 4,
		SLIGHT_MOVE = BUTTON_DOWN + MOUSE_MOVE,
		MOUSE_DRAG = 8,
		DRAGGING = MOUSE_MOVE + MOUSE_DRAG,
	};

	CMouseDrag();

	// メッセージを解析しドラッグの開始・実行・終了時にtrueをかえす
	bool proc(HWND hWnd, basis::Message msg, WPARAM wp, LPARAM lp);

	// ドラッグの開始地点をかえす
	Point start() { return m_start; }

	// 
	Point pos() { return m_pos; }

	// 前回WM_MOUSEMOVEからの横方向移動差分を返す
	int dx();

	// 前回WM_MOUSEMOVEからの縦方向移動差分を返す
	int dy();

	Size getDifference();

	// スクリーン座標上のマウス移動差分のみを返す
	// ウィンドウの移動による相対的移動は無視される
	int dx_abs();
	int dy_abs();
	Size getTravel();

	// ドラッグ開始時の複合マウスキーステート
	UINT keys() { return m_keys; }

	// ドラッグ開始のトリガーとなったボタンのvirtual key
	UINT vkey() { return m_vkey; }

	int state() { return m_state; }

	bool isMoved()
	{
		return (static_cast<int>(m_state)
			& static_cast<int>(STATE::MOUSE_MOVE)) != 0;
	}

	bool isDragged()
	{
		return (static_cast<int>(m_state)
			& static_cast<int>(STATE::MOUSE_DRAG)) != 0;
	}

	// ドラッグ開始とみなす距離を設定する(初期値1)
	// クリック地点からこの分だけ離れるまで差分をかえさない
	// pixelを省略した場合は現在値をかえす
	Size threshold(int dx = -1, int dy = -1);

protected:
	// スレッショルドを(すでに)越えたかどうか
	bool isOveringThreshold(Point p);
	// m_prev, m_posを更新
	Point& updatePos(HWND hWnd, const LPARAM lp);

private:
	Point m_pos;		// クライアント座標
	Point m_scr;		// スクリーン座標
	Point m_start;
	Point m_prev;
	Point m_prev_scr;
	UINT m_keys;
	UINT m_vkey;

	// これがBUTTON_UPのときはドラッグ中ではない
	int m_state;

	// この値以上になるまでドラッグとみなさない(procでtrueをかえさない)
	Size m_threshold;
};

}  // namespace

#endif