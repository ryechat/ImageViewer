#pragma once
#ifndef GUID_970E28484B3F45E18EDCA1E4A996BF26
#define GUID_970E28484B3F45E18EDCA1E4A996BF26

#include "image_viewer.h"
#include "key_map.h"
#include "mouse_drag.h"
#include "unit_dispenser.h"
#include "ids.h"

namespace image_viewer {

class CImageViewer::Control final : public basis::IEventHandler {
public:
	Control(CImageViewer &parent_);
	~Control() = default;

	virtual int onEvent(Window*, Message, WPARAM, LPARAM) final;

	ID getCommand(WPARAM wp) {
		return static_cast<ID>(keymap.getCommand(static_cast<DWORD>(wp), true));
	}

    basis::CKey getKey(ID id, int n) {
		return keymap.getKey(static_cast<int>(id), n);
	}

private:
	bool loadKeyCommands();
	int onMouseMove(Window*, Message, WPARAM, LPARAM);
	int onMouseDrag(Window*);
    int onLButtonClick();
	int onRButtonDrag();
	int onLButtonDrag(Window*);

	CImageViewer &parent;
	bool m_bGripImage;
	basis::CKeyMap keymap;
	basis::CMouseDrag mouse;
	CUnitDispenser<short> wheel;	// マウスホイールの回転検出
	CUnitDispenser<int> hFlip;		// 左右ドラッグ
	CUnitDispenser<int> vFlip;		// 上下ドラッグ
	bool m_bDragList;				// Dragging item-list? or image.
};

}  // namespace

#endif