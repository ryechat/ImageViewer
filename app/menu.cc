#ifndef STDAFX_H
#include <assert.h>
#include <vector>
#endif

#include "window_message.h"
#include "key_combination.h"

#include "image_viewer.h"
#include "profile.h"
#include "ui_listener.h"
#include "filer.h"
#include "menu.h"
#include "ids.h"

using namespace basis;

namespace image_viewer {



void CImageViewer::ContextMenu::
initialize()
{
	create();
	if (parent.profile->isEnable())
		access(false);
	updateStatus();
}



bool CImageViewer::ContextMenu::
changeStatus(ID id)
{
	auto p = isRadioButton(id);
	if (p.first) {
		m_menu.radio(p.first, p.second, static_cast<int>(id));
		return true;
	}

	// 2State Buttons
	switch(id) {
	case ID::USE_PROFILE:
	case ID::VIEW_LIST:
	case ID::VIEW_FILENAME:
	case ID::VIEW_UPSCALE:
	case ID::VIEW_DOWNSCALE:
	case ID::VIEW_CENTER:
	case ID::VIEW_POPUP:
		m_menu.invert(static_cast<int>(id));
		return true;
	}

	return false;
}


void CImageViewer::ContextMenu::create()
{
	// メニュー・サブメニューのスタック
	std::vector<CPopupMenu*> menus;
	std::unique_ptr<CPopupMenu> child;
	menus.push_back(new CPopupMenu);

	CFilePath last_opened_directory =
		parent.profile->general().get(ID::LAST_PATH, nullptr);
	if (last_opened_directory.exist()) {
		menus.back()->insert(0, static_cast<int>(ID::LAST_PATH),
			last_opened_directory.getFileName().c_str());
	}

	for (ID id = ID::MENU_BEGIN; id != ID::MENU_END;
		id = static_cast<ID>(static_cast<int>(id)+1))
	{
		switch (id) {
		case ID::SORT_BEGIN:
		case ID::VIEW_BEGIN:
			menus.push_back(new CPopupMenu);
			break;

		case ID::SORT_END:
		case ID::VIEW_END:
			// Finalize creating child
			child.reset(menus.back());
			menus.pop_back();
			break;

		case ID::USE_PROFILE:
		case ID::VIEW_CENTER:
		case ID::WINDOW_CLOSE:
			menus.back()->insertSeparator(0);
			break;

		}// switch(id)

		std::basic_string<TCHAR> menu_rabel =
			parent.profile->getTranslatedString(id);

		if (!menu_rabel.empty()) {
			menu_rabel += getAcceleratorString(id);
			if (child) {
				menus.back()->insert(0, *child, menu_rabel.c_str());
				child.reset();
			}
			else {
				menus.back()->insert(0, static_cast<int>(id), menu_rabel.c_str());
			}
		}
		assert(child == nullptr);
	}

	assert(menus.size() == 1);

	m_menu = menus[0]->release();
	delete menus[0];
}



std::basic_string<TCHAR> CImageViewer::ContextMenu::
getAcceleratorString(ID id)
{
	std::basic_string<TCHAR> str;
	CKey accelerator_key;
	for (int i = 0;; i++) {
		accelerator_key = parent.input->getKey(id, i);
		if (!accelerator_key)
			break;
		str += (i == 0) ? TEXT("\t") : TEXT(", ");
		str += accelerator_key.toStr();
	}
	return str;
}



void CImageViewer::ContextMenu::access(bool bSave)
{
	select(ID::USE_PROFILE);
	auto &p = parent.profile->menu();

	const ID ids[] = {
		ID::VIEW_POPUP,
		ID::VIEW_FILENAME,
		ID::VIEW_LIST,
		ID::VIEW_UPSCALE,
		ID::VIEW_DOWNSCALE,
		ID::VIEW_CENTER
	};

	for (auto i : ids) {
		if (bSave)
			p.write(i, isSelected(i) ? 1 : 0);
		else if (p.get(i, 0) != 0)
			select(i);
		else
			clear(i);
	}

	if (bSave) {
		p.write(ID::SORT_END, getSortWay() - static_cast<int>(ID::SORT_BEGIN));
		return;
	}
	else {
		changeStatus(static_cast<ID>(p.get(ID::SORT_END, 0) +
			static_cast<int>(ID::SORT_BEGIN)));
	}
}



int CImageViewer::ContextMenu::
getSortWay()
{
	const int id_first = static_cast<int>(ID::SORT_BEGIN) + 1;
	for (int i = id_first; i != static_cast<int>(ID::SORT_END); ++i) {
		if (isSelected(static_cast<ID>(i)))
			return i;
	}
	return id_first;
}



bool CImageViewer::ContextMenu::updateStatus()
{
	const auto *pFiler = parent.filer.get();
	const auto iCurrent = pFiler->current();
	const bool bInvalidFile = pFiler->isEmpty() || iCurrent == pFiler->cend();

	ID ids[] = {
		ID::SHOW_PROPERTY,
		ID::FILE_RELOAD,
		ID::FILE_DELETE
	};
	for (auto i : ids) {
		if (bInvalidFile)
			disable(i);
		else
			enable(i);
	}

	// Disable if it is showing a first item.
	if (bInvalidFile || iCurrent == pFiler->cbegin())
	{
		disable(ID::FILE_PREVIOUS);
		disable(ID::FILE_FIRST);
	}
	else {
		enable(ID::FILE_PREVIOUS);
		enable(ID::FILE_FIRST);
	}

	// Disable if it is showing a last item.
	if (bInvalidFile || iCurrent == pFiler->clast())
	{
		disable(ID::FILE_NEXT);
		disable(ID::FILE_LAST);
	}
	else {
		enable(ID::FILE_NEXT);
		enable(ID::FILE_LAST);
	}
	return true;
}



int CImageViewer::ContextMenu::
track(basis::Point pt) const
{
	m_menu.redraw(parent);
	return m_menu.track(parent, pt);
}



std::pair<int, int> CImageViewer::ContextMenu::
isRadioButton(ID id)
{
	for (auto &i : m_radio) {
		if (static_cast<int>(i.first) <= static_cast<int>(id)
			&& static_cast<int>(id) <= static_cast<int>(i.second)) {
			return{ static_cast<int>(i.first), static_cast<int>(i.second) };
		}
	}
	return{ 0, 0 };
}

}  // namespace