#ifndef STDAFX_H
#include <assert.h>
#include <algorithm>
#endif

#include "window_message.h"
#include "stdfnc.h"
#include "monitor.h"
#include "exception.h"

#include "image_viewer.h"
#include "ids.h"
#include "list_item.h"
#include "profile.h"
#include "ui_listener.h"
#include "draw_list.h"
#include "menu.h"
#include "filer.h"
#include "loader.h"

namespace image_viewer {

CImageViewer::CImageViewer(char * /*cmd*/, int nShow)
	: m_exitCode(0), isImageInvalidated(true)
{
	profile.reset(new Profile);
	menu.reset(new ContextMenu(*this));
	input.reset(new Control(*this));
	filer.reset(new CFiler(*this));
	list.reset(new CDrawList(*this));
	loader.reset(new Loader(*this));

	createMainWindow(this);
	saveload(false);

	menu->initialize();
	if (menu->isSelected(ID::VIEW_LIST))
		list->enable();

	m_captionConfirmDelete = 
		profile->getTranslatedString(ID::FILE_DELETE);
	m_textConfirmDelete =
		profile->getTranslatedString(ID::CONFIRM_DELETE);
	updateTitleBar();
	show(nShow);
}



CImageViewer::~CImageViewer()
{}



int CImageViewer::
onEvent(Window *win, WM msg, WPARAM wp, LPARAM lp) try
{
	UNREFERENCED_PARAMETER(lp);
	assert(win == this);

	switch (msg) {
	case WM::CREATE:
		DragAcceptFiles(*win, true);
		return 0;

	case WM::PAINT:
		return onPaint();

	case WM::COMMAND:
		return onCommand(wp);

	case WM::DROPFILES:
		activate();
		setForeground();
		setPath(::basis::GetDropFile(wp, 0));
		return 1;

	case WM::SIZE:
	case WM::SIZING:
		isImageInvalidated = true;
		win->update();
		return 0;

	case WM::ERASEBKGND:
		invalidate();
		
		return 1;

	case WM::CONTEXTMENU: // Shift + F10
		menu->track({});
		return 1;

	case WM::CLOSE:
		if (profile->isEnable()) {
			menu->saveSettings();
			saveload(true);
		}
		DestroyWindow(*win);
		m_exitCode = 0;
		return 0;

	case WM::DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return 0;
	}
}
catch (std::exception &e)
{
	MessageBoxA(0, e.what(), "Exception", 0);
	throw e;
}



void CImageViewer::saveload(bool bSave)
{
	if (bSave && m_dir.exist())
		profile->general().write(ID::LAST_PATH, m_dir.path().c_str());

	profile->window();
	if (profile->get(ID::WINDOW_REMINDER, true) == false)
		return;

	RECT rc = place();
	if (profile->loadBoolean(ID::WINDOW_POSITION, false)) {
		profile->access(ID::WINDOW_LEFT, &rc.left, bSave);
		profile->access(ID::WINDOW_TOP, &rc.top, bSave);
		profile->access(ID::WINDOW_RIGHT, &rc.right, bSave);
		profile->access(ID::WINDOW_BOTTOM, &rc.bottom, bSave);
		!bSave && place(rc);
	}

	bool bZoomed = bSave && isMaximized();
	if (profile->loadBoolean(ID::WINDOW_ZOOMING, false)) {
		profile->booleanReadWrite(ID::WINDOW_MAXIMIZE, &bZoomed, bSave);
		if (!bSave && bZoomed)
			maximize();
	}

	bool bPopup = bSave && menu->isSelected(ID::VIEW_POPUP);
	if (profile->loadBoolean(ID::WINDOW_STYLE, false)) {
		profile->booleanReadWrite(ID::VIEW_POPUP, &bPopup, bSave);
		if (!bSave && bPopup)
			popup();
	}
}



// 次候補(iNext)をカレント設定するタイプのループに使う。
// 設定に成功、またはすでに探索末尾(iLimit)である場合はfalseを返す。
// 失敗すると要素を削除したうえでtrueを返すので、
// 再度引数を渡してループさせること。
bool CImageViewer::
helper_show_must_loop(iterator iNext, const_iterator iLimit)
{
	// もう探索できないので終了。
	if (iNext == filer->end()) {
		invalidate();
		return false;
	}

	// ロード成功したら終了
	if (setCurrent(iNext) == true) {
		return false;
	}

	// 失敗したやつ消して、ループ継続
	filer->erase(iNext);
	return true;
}



void CImageViewer::
showPrev()
{
	if (filer->current() == filer->begin())
		return;
	while (helper_show_must_loop(--filer->current(), filer->begin()))
		; // noop
}



void CImageViewer::
showNext()
{
	if (filer->current() == filer->end())
		return;

	while (helper_show_must_loop(++filer->current(), filer->end()))
		; // noop
}



void CImageViewer::
showFirst()
{
	while (helper_show_must_loop(filer->begin(), filer->end()))
		; // noop
}



void CImageViewer::
showLast()
{
	while (helper_show_must_loop(filer->last(),
		filer->end()))
		; // nop
}



bool CImageViewer::
setPath(basis::CFilePath path)
{
	if (loader->waitIfAnyImageIsLoading() == false) {
		MessageBox(0, TEXT("Loading thread wouldn't respond."
			"Operations are annulled."), 0, 0);
		return false;
	}

	if (!path.exist())
		return false;

	m_dir = (path.isDirectory()) ? path : path.getDir();

	filer->generate(m_dir.path().c_str());
	filer->sort();
	filer->setCurrent(filer->begin());

	if (path.isDirectory()) {
		showFirst();
		return true;
	}

	auto filename = path.getFileName();
	iterator itr = filer->search([=](Element &p)->bool {
		return (filename == p->fileName());
	});
	setCurrent(itr);
	return true;
}



bool CImageViewer::
setCurrent(const iterator &itr)
{
	if (itr == filer->end())
		return false;

	// 範囲外のキャッシュ削除
	loader->markToReleaseAround(filer->current());
	loader->unmarkAround(itr);
	loader->performReleaseAround(filer->current());

	if (loader->loadImage(itr, true) != Loader::Status::Finished)
		return false;

	// 移動
	filer->setCurrent(itr);
	loader->preloadAround(itr);

	// 更新
	list->invalidate();
	updateTitleBar();

	m_offset.reset();
	invalidate_image();
	update();
	return true;
}



void CImageViewer::
reloadCurrent()
{
	if (filer->current() == filer->end())
		return;
	filer->current()->get()->unload();
	if (loader->loadImage(filer->current(), false) == Loader::Status::Finished)
		setCurrent(filer->current());
}



int CImageViewer::
onCommand(WPARAM wp)
{
	ID const id = static_cast<ID>(LOWORD(wp));
	menu->changeStatus(id);

	switch (id) {
	case ID::USE_PROFILE:
		if (menu->isSelected(id))
			profile->enable();
		else
			profile->disable();
		return 1;

	case ID::LAST_PATH:
		setPath(basis::CFilePath(profile->general().get(id, nullptr)));
		break;

	case ID::FILE_PREVIOUS:
		showPrev();
		break;

	case ID::FILE_NEXT:
		showNext();
		break;

	case ID::FILE_FIRST:
		showFirst();
		break;

	case ID::FILE_LAST:
		showLast();
		break;

	case ID::FILE_RELOAD:
		reloadCurrent();
		break;

	case ID::FILE_DELETE:
	case ID::FILE_QUICK_DELETE:
	case ID::LIST_REMOVE:
		if (filer->isEmpty())
			break;
		if (id == ID::FILE_DELETE) {
			if (IDOK != MessageBox(*this, m_textConfirmDelete.c_str(),
				m_captionConfirmDelete.c_str(), MB_OKCANCEL))
				break;
		}
		loader->waitIfLoading(filer->current());
		if (id != ID::LIST_REMOVE &&
			(m_dir + filer->current()->get()->fileName()).trash() == false)
				break;
		filer->erase(filer->current());
		setCurrent(filer->current());
		break;

	case ID::VIEW_POPUP:
		popup(menu->isSelected(id));
		break;

	case ID::VIEW_FILENAME:
		updateTitleBar();
		break;

	case ID::VIEW_LIST:
		if (menu->isSelected(id))
			list->enable();
		else
			list->disable();
		break;

	case ID::VIEW_UPSCALE:
	case ID::VIEW_DOWNSCALE:
	case ID::VIEW_CENTER:
		m_offset.reset();
		invalidate_image();
		break;

	case ID::LOADER_IMAGE_LOADED:
		list->invalidate();
		break;

	case ID::SCREEN_TOGGLE:
		toggleScreen();
		break;

	case ID::SORT_LESSER_WRITE:
	case ID::SORT_GREATER_WRITE:
	case ID::SORT_LESSER_ACCESS:
	case ID::SORT_GREATER_ACCESS:
	case ID::SORT_LESSER_CREATION:
	case ID::SORT_GREATER_CREATION:
		filer->sort();
		list->invalidate();
		update();
		break;

	case ID::WINDOW_CLOSE:
		post(basis::Message::CLOSE, 0, 0);
		break;

	case ID::SHOW_PROPERTY:
		if (!filer->isEmpty()) {
			Element p = *filer->current();
			auto path = m_dir + p->fileName();
			ShowProperty(*this, path.path().c_str());
			break;
		}

	default:
		return 0;
	} // switch

	return 1;
}



void CImageViewer::
update() const
{
	menu->updateStatus();
	UpdateWindow(*this);
	updateTitleBar();
}



void CImageViewer::
invalidate() const
{
	isImageInvalidated = true;
	Window::invalidate(nullptr);
}



void CImageViewer::
invalidate(const Rect &rc) const
{
	if (rc) {
		RECT rect = rc;
		Window::invalidate(&rect);
	}
}



bool CImageViewer::
updateTitleBar() const
{
	tstr title;
	auto current = filer->isEmpty() ? nullptr : filer->current()->get();
	if (current && menu->isSelected(ID::VIEW_FILENAME))
		title = current->fileName();
	else
		title = NAME_VERSION;

	int index = static_cast<int>(current
		? filer->indexof(filer->current()) + 1
		: 0);
	int size = static_cast<int>(filer->size());

	title += TEXT(" [") + basis::ToStr(index)
		+ TEXT("/") + basis::ToStr(size)
		+ TEXT("]");

	return setTitle(title.c_str());
}



bool CImageViewer::
toggleScreen()
{
	if (!isMaximized()) {
		popup(true);
		maximize();
	}
	else if (!isMultiMaximized()) {
		maximize_multi();
	}
	else {
		normalize();
		popup(menu->isSelected(ID::VIEW_POPUP));
	}
	m_offset.reset();
	invalidate();
	return true;
}



bool CImageViewer::isMultiMaximized() const
{
	if (!isMaximized())
		return false;

	Rect vs = basis::Monitor::GetVirtualScreen();
	Rect rc = getRect();
	return (rc.width() >= vs.width() && rc.height() >= vs.height());
}



int CImageViewer::
onPaint()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(*this, &ps);

	if (m_backbuffer.compatible(hdc, clientSize())) {
		SetBkMode(m_backbuffer, TRANSPARENT);
		m_backbuffer.pen(GetStockObject(WHITE_PEN));
		m_backbuffer.brush(GetStockObject(WHITE_BRUSH));
	}

	if (filer->isEmpty()) {
		m_backbuffer.rectangle(ps.rcPaint);
	}
	else {
		Rect image_rect;
		if (!filer->isEmpty())
			image_rect = filer->current()->get()->rect();
		Size drawing_size = getDrawSize(image_rect.size());

		Rect src = { 0, 0, drawing_size.x, drawing_size.y };
		if (m_offscreen.compatible(hdc, drawing_size) ||
		isImageInvalidated) {
			isImageInvalidated = false;
			filer->current()->get()->draw(m_offscreen, src, image_rect);
		}

		m_drawingRect = getDrawRect(drawing_size);
		m_offscreen.transfer(m_backbuffer, m_drawingRect, src);
		ClearBackground(m_backbuffer, ps.rcPaint, m_drawingRect);
	}

	list->draw(&m_backbuffer);
	m_backbuffer.transfer(hdc, ps.rcPaint, ps.rcPaint);
	EndPaint(*this, &ps);
	return 1;
}



basis::Rect CImageViewer::
getDrawRect()
{
	if (filer->isEmpty())
		return{};
	return getDrawRect(getDrawSize(filer->current()->get()->getSize()));
}



basis::Rect CImageViewer::
getDrawRect(const Size &size)
{
	Rect rc{ 0, 0, size.x, size.y };
	rc.slide(m_offset);
	if (menu->isSelected(ID::VIEW_CENTER))
		rc.slide((clientSize() - size) / 2);
	return rc;
}



basis::Size CImageViewer::
getDrawSize(const Size &image_size)
{
	Size size = image_size;
	const auto client = clientSize();

	if (!client.x || !client.y || !size.x || !size.y)
		return{};

	if (menu->isSelected((client.x > size.x && client.y > size.y)
		? ID::VIEW_UPSCALE : ID::VIEW_DOWNSCALE))
	{
		if (static_cast<double>(size.x) / client.x
			> static_cast<double>(size.y) / client.y)
		{
			size.y = static_cast<int>(0.5 + size.y
				* static_cast<double>(client.x) / size.x);
			size.x = client.x;
		}
		else {
			size.x = static_cast<int>(0.5 + size.x
				* static_cast<double>(client.y) / size.y);
			size.y = client.y;
		}
	}
	return size;
}



void CImageViewer::
invalidate_image()
{
	isImageInvalidated = true;
	invalidate(m_drawingRect);
	invalidate(getDrawRect());
}

}  // namespace
