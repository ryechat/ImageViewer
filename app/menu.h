/*!
    @file Class definition of CImageViewer::ContextMenu.
    That class provides the application the way to manage
    context menu item's status.
*/
#pragma once
#ifndef GUID_56ED3B352A5D42EBA6E9E85670749C89
#define GUID_56ED3B352A5D42EBA6E9E85670749C89
#include "image_viewer.h"

namespace image_viewer {

enum class ID : int;

//! Menu Controls
class CImageViewer::ContextMenu {
public:
    ContextMenu(CImageViewer &parent_);
    ~ContextMenu();

    /*! メニュー状態を復元し、アプリケーション設定に反映する.
        ウィンドウを生成してから呼び出すこと。
    */
    void initialize();

    //! メニュー項目の状態を設定ファイルに保存する
    void saveSettings();
 
    //! アプリケーションの状態に合わせてメニュー項目の有効/無効を切り替える
    bool updateStatus();

    /*! 項目のチェック状態を反転する.
        ラジオボタンの場合はONになり、グループ内の他のラジオボタンがOFFになる
    */
    bool changeStatus(ID id);

    //! メニューを表示し、ユーザの選択を返す。キャンセルは0。
    int track(basis::Point pt) const;

    //! 選択されているソート条件を返す
    ID getSortWay();

    //! 項目を有効化する
    void enable(ID id);

    //! 項目を無効化する
    void disable(ID id);

    //! 項目をチェック状態にする
    void select(ID id);

    //! 項目がチェック状態かどうか
    bool isSelected(ID id);

    //! 項目のチェック状態を解除
    void clear(ID id);

private:
    CImageViewer& parent;
    class Impl;
    std::unique_ptr<Impl> impl;
};

}  // namespace

#endif