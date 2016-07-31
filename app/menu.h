#pragma once
#ifndef GUID_56ED3B352A5D42EBA6E9E85670749C89
#define GUID_56ED3B352A5D42EBA6E9E85670749C89
#ifndef STDAFX
#include <vector>
#endif
#include "image_viewer.h"
#include "popup_menu.h"

namespace image_viewer {

enum class ID : int;

//! Menu Controls
class CImageViewer::ContextMenu {
public:
	ContextMenu(CImageViewer &parent_) : parent(parent_) {}
	~ContextMenu() = default;

	void initialize();
	void saveSettings() { access(true); }
 
	/*! Synchronize its status with a selected item.
		It disables buttons such as "next file" that would be unsuitable
		if the item was a last file in the directory.
	*/
	bool updateStatus();

	/*! Changes status of a selected item.
		If the menu item specified by id was a button,
		this function will invert selected and unselected.
		If the menu item was radio button that was tied up with other items
		by calling makeRadioButton() function, the item specified as a
		parameter will be selected, and other tied-up item will be cleared.
	*/
	bool changeStatus(ID id);

	/*! Shows the context menu.
		This function will block the flow of the thread while menu is shown.
		If an item was selected by user, this function will post message
		to the parent window before returning selected ID.
		If menu was closed without any item selected, return value is 0.
	*/
	int track(basis::Point pt) const;

	int getSortWay();

	void disable(ID id) { m_menu.disable(static_cast<int>(id)); }
	void enable(ID id) { m_menu.enable(static_cast<int>(id)); }

	void select(ID id) { m_menu.select(static_cast<int>(id)); }
	bool isSelected(ID id) { return m_menu.isSelected(static_cast<int>(id)); }
	void clear(ID id) { m_menu.clear(static_cast<int>(id)); }

private:
	void create();
	void makeRadioButton(ID first, ID last) { m_radio.emplace_back(first, last); }
	std::pair<int, int> isRadioButton(ID id);

	CImageViewer &parent;
	basis::CPopupMenu m_menu;
	void access(bool bSave);
	std::basic_string<TCHAR> getAcceleratorString(ID id);

	std::vector<std::pair<ID, ID>> m_radio;
};

}  // namespace

#endif