#include "profile.h"
#include "private_profile.h"
#include "cursor.h"
#include "stdfnc.h"

#include "ids.h"
#include "menu.h"
#include "draw_list.h"
#include "ui_listener.h"



namespace image_viewer {

CImageViewer::Control::Control(CImageViewer &parent_)
	: parent(parent_), m_bGripImage(0), m_bDragList(0)
{
	parent.hook(this);
	wheel.setUnit(WHEEL_DELTA);
	if (loadKeyCommands() == false)
		setToDefault();
}



int CImageViewer::Control::
onEvent(Window *win, Message msg, WPARAM wp, LPARAM lp)
{
	switch (msg) {

	case WM::KEYDOWN: {
 		const ID cmd = getCommand(wp);
		if (cmd == ID::UNDEFINED)
			return 0;
		const bool key_repeat = (lp & 0x40000000) != 0;
		if (!key_repeat
			|| cmd == ID::FILE_PREVIOUS || cmd == ID::FILE_NEXT)
			return parent.broadcast(win, WM::COMMAND, static_cast<UINT>(cmd), 0);
		return 1;
	}

	case WM::KEYUP:
		if (getCommand(wp) == ID::GRIP_IMAGE) {
			return parent.broadcast(win, WM::COMMAND,
				static_cast<int>(ID::UNGRIP_IMAGE), 0);
		}
		return 0;

	case WM::COMMAND: {
		switch (static_cast<ID>(LOWORD(wp))) {
		case ID::GRIP_IMAGE:
			m_bGripImage = true;
			return 1;
		case ID::UNGRIP_IMAGE:
			m_bGripImage = false;
			return 1;
		default:
			return 0;
		}
	}

	case WM::LBUTTONDBLCLK:
		mouse.proc(*win, msg, wp, lp);
		parent.onCommand(static_cast<WPARAM>(ID::SCREEN_TOGGLE));
		return 1;

	case WM::LBUTTONUP:
	case WM::RBUTTONUP:
		ReleaseCapture();
		// fall through
	case WM::RBUTTONDBLCLK:
	case WM::LBUTTONDOWN:
	case WM::RBUTTONDOWN:
		if (!mouse.proc(*win, msg, wp, lp))
			return 1;

		if (msg == WM::LBUTTONDOWN) {
			mouse.threshold(1);
			SetCapture(parent);
			m_bDragList = parent.list->rect().isInclusive(mouse.start());
			return 1;
		}

		if (mouse.vkey() == VK_RBUTTON) {
			if (mouse.state() & mouse.BUTTON_DOWN) {
				mouse.threshold(30, 30);
				hFlip.reset(0);
				hFlip.setUnit(30);
				vFlip.reset(0);
				vFlip.setUnit(30);
				return 1;
			}
			if (!mouse.isDragged()) {
				parent.menu->track(win->getClientRectInVirtualScreen().lefttop() + mouse.pos());
				return 1;
			}
		}
		return 1;

	case WM::MOUSEMOVE:
		return onMouseMove(win, msg, wp, lp);

	case WM::MOUSEWHEEL: {
		// 回数分だけカレントを移動する
		int n;
		wheel.add(-GET_WHEEL_DELTA_WPARAM(wp));
		while ((n = wheel.get()) != 0)
		{
			parent.post(WM::COMMAND, static_cast<int>((n < 0)
				? ID::FILE_PREVIOUS : ID::FILE_NEXT), 0);
		}
		return 1;

	} // case
	} // switch

	return 0;
} // onEvent



bool CImageViewer::Control::
loadKeyCommands()
{
	using namespace basis;
	auto& prof = parent.profile->control();
	if (prof.get(ID::CONTROL_USEDEFAULT, 0) != 0)
		return false;
	
	CKey key;
	StringBuffer s;
	for (int i = static_cast<int>(ID::COMMAND_BEGIN);
		i < static_cast<int>(ID::COMMAND_END);
		++i)
	{
		s.refer(prof.get(static_cast<ID>(i), nullptr));
		// カンマ区切りをすべて登録
		for (size_t n; !s.empty(); ) {
			n = s.find(_T(','));
			if (key.read(s.c_str())) {
				if (!keymap.append(key, i))
					MessageBox(0, s.c_str(), TEXT("Key setting has an error"), 0);
			}
			if (n == StringBuffer::npos)
				break;
			s.refer(s.c_str() + n + 1);
		}
	}
	return keymap.size() != 0;
}



void CImageViewer::Control::
setToDefault()
{
	static const std::pair<DWORD, ID> ls[] {
		{ VK_LEFT,   ID::FILE_PREVIOUS },
		{ VK_UP,     ID::FILE_PREVIOUS },
		{ VK_RIGHT,  ID::FILE_NEXT },
		{ VK_DOWN,   ID::FILE_NEXT },
		{ VK_HOME,   ID::FILE_FIRST },
		{ VK_END,    ID::FILE_LAST },
		{ VK_TAB,    ID::VIEW_LIST },
		{ VK_ESCAPE, ID::SCREEN_TOGGLE },
		{ VK_SPACE,  ID::GRIP_IMAGE },
		{ VK_DELETE, ID::FILE_DELETE },
	};

	for (auto i : ls) {
		keymap.append(i.first, static_cast<DWORD>(i.second));
	}
}



int CImageViewer::Control::
onMouseDrag(Window *win)
{
	if (mouse.vkey() == VK_LBUTTON)
		return onLButtonDrag(win);
	if (mouse.vkey() == VK_RBUTTON)
		return onRButtonDrag();
	return 1;
}



int CImageViewer::Control::
onLButtonDrag(Window *win)
{
	if (m_bDragList) {
		parent.list->move({ 0, mouse.dy() });
	}
	else if (m_bGripImage || win->isMaximized() || !win->isPopup()) {
		parent.move_image(mouse.getDifference());
	}
	else {
		win->move({ mouse.dx_abs(), mouse.dy_abs() });
		return 1;	// 再描画済み
	}

	win->update();
	return 1;
}



int CImageViewer::Control::
onMouseMove(Window *win, Message msg, WPARAM wp, LPARAM lp)
{
	if (mouse.proc(*win, msg, wp, lp) && (mouse.isDragged()))
		return onMouseDrag(win);

	// 一時的にタイトルバーを表示する
	int margin = GetSystemMetrics(SM_CYCAPTION);
	if (mouse.pos().y <= margin && mouse.pos().y >= -margin) {
		if (win->isPopup()) {
			parent.m_bTemporaryShowTitle = true;
			win->popup(false);
		}
		return 1;
	}

	// 解除
	else if (parent.m_bTemporaryShowTitle) {
		parent.m_bTemporaryShowTitle = false;
		if (win->isMaximized() || parent.menu->isSelected(ID::VIEW_POPUP))
			win->popup();
	}
	return 1;
}



int CImageViewer::Control::
onRButtonDrag()
{
	hFlip.add(mouse.dx());
	vFlip.add(mouse.dy());

	// 左右にドラッグで表示ファイル変更
	while (hFlip.over()) {
		ID cmd = (hFlip.get() < 0)	? ID::FILE_PREVIOUS : ID::FILE_NEXT;
		parent.post(WM::COMMAND, static_cast<WPARAM>(cmd), 0);
		vFlip.reset(0);
		return 1;
	}

	// 縦フリップでなんかやる
	while (vFlip.over()) {
		vFlip.get();
		// hFlip.reset(0);
	}
	return 0;
}

}  // namespace