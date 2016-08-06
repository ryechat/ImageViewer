#ifndef STDAFX_H
#include <assert.h>
#include <vector>
#endif

#include "key_combination.h"
#include "popup_menu.h"

#include "profile.h"
#include "filer.h"
#include "ids.h"
#include "menu.h"

namespace image_viewer {

class CImageViewer::ContextMenu::Impl {
public:
    using CPopupMenu = basis::CPopupMenu;
    Impl(ContextMenu& parent_) : parent(parent_) {}
    ~Impl() = default;

    void create();
    void access(bool bSave);
    std::basic_string<TCHAR> getAcceleratorString(ID id);

    ContextMenu &parent;
    basis::CPopupMenu m_menu;
    std::vector<std::pair<int, int>> m_radio;
};



void CImageViewer::ContextMenu::Impl::
create()
{
    // メニュー・サブメニューのスタック
    std::vector<CPopupMenu*> menus;
    std::unique_ptr<CPopupMenu> child;
    menus.push_back(new CPopupMenu);

    if (parent.parent.m_lastPath.exist()) {
        menus.back()->insert(0, static_cast<int>(ID::LAST_PATH),
            parent.parent.m_lastPath.getFileName().c_str());
    }

    for (ID id = ID::MENU_BEGIN; id != ID::MENU_END; id = next(id))
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
            parent.parent.profile->getTranslatedString(id);

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

    m_radio.push_back({ static_cast<int>(ID::SORT_BEGIN) + 1,
        static_cast<int>(ID::SORT_END) - 1 });
}



std::basic_string<TCHAR> CImageViewer::ContextMenu::Impl::
getAcceleratorString(ID id)
{
    std::basic_string<TCHAR> str;
    basis::CKey accelerator_key;
    for (int i = 0;; i++) {
        accelerator_key = parent.parent.getKey(id, i);
        if (!accelerator_key)
            break;
        str += (i == 0) ? TEXT("\t") : TEXT(", ");
        str += accelerator_key.toStr();
    }
    return str;
}



void CImageViewer::ContextMenu::Impl::
access(bool bSave)
{
    parent.select(ID::USE_PROFILE);
    auto &p = parent.parent.profile->menu();

    const ID ids[] = {
        ID::VIEW_POPUP,
        ID::VIEW_FILENAME,
        ID::VIEW_FILELIST,
        ID::VIEW_UPSCALE,
        ID::VIEW_DOWNSCALE,
        ID::VIEW_CENTER
    };

    for (auto i : ids) {
        if (bSave)
            p.saveBoolean(i, parent.isSelected(i));
        else if (p.loadBoolean(i, 0) != 0)
            parent.select(i);
        else
            parent.clear(i);
    }

    if (bSave) {
        int n = static_cast<int>(parent.getSortWay())
            - static_cast<int>(ID::SORT_BEGIN);
        p.save(ID::SORT_END, n);
        return;
    }
    else {
        int id = p.load(ID::SORT_END, 0) + static_cast<int>(ID::SORT_BEGIN);
        parent.changeStatus(static_cast<ID>(id));
    }
}



CImageViewer::ContextMenu::ContextMenu(CImageViewer & parent_)
    : parent(parent_), impl(new Impl(*this))
{}



CImageViewer::ContextMenu::~ContextMenu() = default;



void CImageViewer::ContextMenu::
initialize()
{
    impl->create();
    if (parent.profile->isEnable())
        impl->access(false);
    updateStatus();
}



void CImageViewer::ContextMenu::
saveSettings()
{
    impl->access(true);
}



bool CImageViewer::ContextMenu::
changeStatus(ID id)
{
    // 2State Button
    switch(id) {
    case ID::USE_PROFILE:
    case ID::VIEW_FILELIST:
    case ID::VIEW_FILENAME:
    case ID::VIEW_UPSCALE:
    case ID::VIEW_DOWNSCALE:
    case ID::VIEW_CENTER:
    case ID::VIEW_POPUP:
        impl->m_menu.invert(static_cast<int>(id));
        return true;
    }

    // Radio button
    for (auto &i : impl->m_radio) {
        if (i.first <= static_cast<int>(id) && static_cast<int>(id) <= i.second)
        {
            impl->m_menu.radio(i.first, i.second, static_cast<int>(id));
            return true;
        }
    }

    return false;
}



ID CImageViewer::ContextMenu::
getSortWay()
{
    for (ID id = next(ID::SORT_BEGIN); id != ID::SORT_END; id = next(id)) {
        if (isSelected(id))
            return id;
    }
    return next(ID::SORT_BEGIN);
}



void CImageViewer::ContextMenu::
disable(ID id)
{
    impl->m_menu.disable(static_cast<int>(id));
}



void CImageViewer::ContextMenu::
enable(ID id)
{
    impl->m_menu.enable(static_cast<int>(id));
}



void CImageViewer::ContextMenu::
select(ID id)
{
    impl->m_menu.select(static_cast<int>(id));
}



bool CImageViewer::ContextMenu::
isSelected(ID id)
{
    return impl->m_menu.isSelected(static_cast<int>(id));
}



void CImageViewer::ContextMenu::
clear(ID id)
{
    impl->m_menu.clear(static_cast<int>(id));
}



bool CImageViewer::ContextMenu::updateStatus()
{
    const auto &f = parent.filer;
    const auto iCurrent = f->current();
    const bool bInvalidFile = f->isEmpty() || iCurrent == f->cend();

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
    if (bInvalidFile || iCurrent == f->cbegin())
    {
        disable(ID::FILE_BACK);
        disable(ID::FILE_FIRST);
    }
    else {
        enable(ID::FILE_BACK);
        enable(ID::FILE_FIRST);
    }

    // Disable if it is showing a last item.
    if (bInvalidFile || iCurrent == f->clast())
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
    impl->m_menu.redraw(parent);
    return impl->m_menu.track(parent, pt);
}

}  // namespace