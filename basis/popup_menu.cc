#ifndef STDAFX_H
#endif

#include "popup_menu.h"
#include "exception.h"

namespace basis {

CPopupMenu::CPopupMenu(HMENU h)
    : m_h(h), mi(new MENUITEMINFO{ sizeof(MENUITEMINFO) })
{
    if (h == 0)
        throw std::runtime_error(LOCATION);
}



CPopupMenu::~CPopupMenu()
{
    if (m_h)
        DestroyMenu(m_h);
    delete mi;
}



CPopupMenu& CPopupMenu::operator=(CPopupMenu&&rhs) noexcept
{
    std::swap(m_h, rhs.m_h);
    return *this;
}



void CPopupMenu::insert(int id_pos, int id, const TCHAR *str)
{
    UINT const flag = (str ? MF_STRING : MF_SEPARATOR);

    // 0のままだと順番がおかしくなるので確実に末尾にする
    if (!id_pos)
        id_pos = -1;

    BOOL const success = InsertMenu(m_h, static_cast<UINT>(id_pos),
        flag, static_cast<UINT>(id), str);

    if (!success) {
        throw api_runtime_error();
    }
}



void CPopupMenu::insertSeparator(int id_pos)
{
    insert(id_pos, 0, 0);
}



void CPopupMenu::insert(int id_pos, CPopupMenu& submenu, const TCHAR *str)
{
    mi->fMask = MIIM_STRING | MIIM_SUBMENU;
    mi->dwTypeData = const_cast<TCHAR*>(str);
    mi->hSubMenu = submenu.m_h;

    // 0のままだと順番がおかしくなるので確実に末尾にする
    if (!id_pos)
        id_pos = -1;

    BOOL success = InsertMenuItem(m_h, static_cast<UINT>(id_pos), FALSE, mi);

    if (!success) {
        throw api_runtime_error();
    }

    submenu.m_h = nullptr;    // ハンドルを渡した
}



int CPopupMenu::track(HWND hWnd, basis::Point pos) const
{
    UINT flag = TPM_RIGHTALIGN | TPM_TOPALIGN | TPM_RETURNCMD;

    int id = TrackPopupMenu(m_h, flag, pos.x, pos.y, 0, hWnd, nullptr);
    if (id) {
        PostMessage(hWnd, WM_COMMAND, id, 0);
    }
    return id;
}



bool CPopupMenu::isRadioButton(int id) const
{
    mi->fMask = MIIM_FTYPE;
    mi->fType = MFT_RADIOCHECK;
    if (GetMenuItemInfo(m_h, static_cast<UINT>(id), FALSE, mi) == FALSE)
        throw 0;
    return (mi->fType & MFT_RADIOCHECK) != 0;
}



void CPopupMenu::select(int id) const
{
    stateHelper(id, MFS_CHECKED, 1);
}



void CPopupMenu::clear(int id) const
{
    stateHelper(id, MFS_CHECKED, 0);
}



bool CPopupMenu::isSelected(int id) const
{
    return (stateHelper(id, MFS_CHECKED, 2) == 1);
}



bool CPopupMenu::invert(int id) const
{
    return (stateHelper(id, MFS_CHECKED, -1) == 1);
}



void CPopupMenu::enable(int id) const
{
    stateHelper(id, MFS_DISABLED, 0);
}



void CPopupMenu::disable(int id) const
{
    stateHelper(id, MFS_DISABLED, 1);
}



bool CPopupMenu::isEnable(int id) const
{
    return (stateHelper(id, MFS_DISABLED, 2) == 0);
}



bool CPopupMenu::invertEnable(int id) const
{
    return (stateHelper(id, MFS_DISABLED, -1) == 0);

}



void CPopupMenu::radio(int first, int last, int select) const
{
    BOOL const success = CheckMenuRadioItem(m_h,
        static_cast<UINT>(first), static_cast<UINT>(last),
        static_cast<UINT>(select), MF_BYCOMMAND);

    if (!success)
        throw api_runtime_error();
}



void CPopupMenu::erace(int id) noexcept
{
    RemoveMenu(m_h, static_cast<UINT>(id), 0U);
}



void CPopupMenu::detach() noexcept
{
    if (m_h) {
        while (RemoveMenu(m_h, 0U, MF_BYPOSITION));
    }
}



HMENU CPopupMenu::release() noexcept
{
    HMENU const hMenu = m_h;
    m_h = nullptr;
    return hMenu;
}



void CPopupMenu::redraw(HWND hWnd) const
{
    if (DrawMenuBar(hWnd) == FALSE)
        throw api_runtime_error();
}



//--- private



// stateを設定/取得(-1)する
// 戻り値は新しいstate
int CPopupMenu::state(int id, int stat) const
{
    mi->fMask = MIIM_STATE;
    mi->fState = stat;

    BOOL success;
    if (stat == -1) {
        success = GetMenuItemInfo(m_h, static_cast<UINT>(id), FALSE, mi);
    }
    else {
        success = SetMenuItemInfo(m_h, static_cast<UINT>(id), FALSE, mi);
    }

    if (!success) {
        throw api_runtime_error();
    }

    return mi->fState;
}



// チェック状態のOFF/ONを設定( 0/1 )
// 反転( -1 ) または取得する( 2 ) 
// 戻り値：新しいステート
int CPopupMenu::stateHelper(int id, int flag, int modify) const
{
    // 実行時のステート
    UINT stat = state(id);

    // ステートを変更
    if (modify != 2) {
        UINT operation;
        if (modify == 0)
            operation = 0;        // flagをoff
        else if (modify == -1)
            operation = ~stat;    // flagを反転
        else
            operation = flag;    // flagをon

        stat = ((stat & ~flag) | (operation & flag));

        state(id, stat);
    }

    // ステートのフラグ状態をかえす
    return (int)((stat & flag) != 0);
}

}  // namespace