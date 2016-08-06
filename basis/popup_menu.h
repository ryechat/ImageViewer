#pragma once
#ifndef GUID_E47B5B4E32ED48FF9396F82D9615F6B1
#define GUID_E47B5B4E32ED48FF9396F82D9615F6B1

#ifndef STDAFX_H
#include <wtypes.h>
#include <vector>
#endif
#include "types.h"

namespace basis {

// メニュー。
// 解体時に所有権のあるメニュー・サブメニューは解体される。
class CPopupMenu {
public:
    CPopupMenu() : CPopupMenu(CreatePopupMenu()) {}
    virtual ~CPopupMenu();

    CPopupMenu(const CPopupMenu&) = default;
    CPopupMenu& operator=(const CPopupMenu&) = default;
    CPopupMenu(CPopupMenu&&rhs) noexcept { *this = std::move(rhs); }
    CPopupMenu& operator=(CPopupMenu&&rhs) noexcept;

    CPopupMenu(HMENU h);

    // 項目をpositionの前に挿入
    // 末尾なら0または(UINT)-1にすること
    void insert(int position, int id, const TCHAR *title);

    // セパレータを挿入
    void insertSeparator(int position);

    // 子メニューをpositionの前に挿入
    // メニューハンドルは親メニューに委譲される
    void insert(int position, CPopupMenu& submenu, const TCHAR *title);

    // 指定されたidを持つ項目を取り除く
    void erace(int id) noexcept;

    // すべての項目、サブメニューを切り離す（破壊されない）
    void detach() noexcept;

    // メニューの所有権を放棄し、ハンドルをかえす
    HMENU release() noexcept;

    // 変更を反映する。track前に必ず実行すること
    void redraw(HWND) const;

    /*!    ショートカットメニューを表示する.
        表示中は処理がブロックされる。
        @param position 表示するスクリーン座標
        @return item ID, if user selected. Otherwise, 0.
    */
    int track(HWND hWnd, basis::Point pos) const;

    // first~lastのうちidだけを選択状態にする
    void radio(int first, int last, int id) const;

    // チェックマーク
    void select(int id) const;
    void clear(int id) const;
    bool isSelected(int id) const;

    bool isRadioButton(int id) const;
    bool invert(int id) const;        // 戻り値はisSelected

    // 有効化/無効化
    void enable(int id) const;
    void disable(int id) const;
    bool isEnable(int id) const;
    bool invertEnable(int id) const;    // 戻り値はisEnable

protected:
    int state(int id, int stat = -1) const;
    int  stateHelper(int id, int flag, int modify) const;

    HMENU m_h;
    MENUITEMINFO *mi;
};

}  // namespace

#endif

