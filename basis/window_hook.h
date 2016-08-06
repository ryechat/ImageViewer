#pragma once
#ifndef GUID_65F96D9BE4D14C599276F442B3038637
#define GUID_65F96D9BE4D14C599276F442B3038637

#ifndef STDAFX_H
#include <wtypes.h>
#include <list>
#include <functional>
#endif

namespace basis {

class IEventHandler;
class Window;
enum class Message : int;

//! ウィンドウフック
/*!    <p>メッセ―ジを受け取るイベントリスナをフックできるクラス。
    </p>
    <p>ウィンドウクラスで使われ、ウィンドウ生成直後は
    ウィンドウ自身のリスナのみを格納することになる。
*/
class WindowHook {
public:
    using Function = std::function<int(Window*, Message, WPARAM, LPARAM)>;
    using Element = std::pair<IEventHandler*, Function>;
    using Container = std::list<Element>;
    WindowHook() = default;
    virtual ~WindowHook() = default;

    WindowHook(const WindowHook &) = default;
    WindowHook&operator=(const WindowHook &) = default;
    WindowHook(WindowHook &&) = default;
    WindowHook&operator=(WindowHook &&) = default;

    //! ウィンドウのリスナを登録
    void push(IEventHandler *p);

    //! ウィンドウのリスナを登録
    void push(Function f);

    //! リスナを除去する
    void unhook(IEventHandler *p);

    /*! メッセージを各リスナに(新しいほうから)配信する.
        @return
        非0を返すものに出会ったらその値を返す。
        すべてのリスナが0を返したとき、戻り値は0。
    */
    int dispatch(Window *win, Message msg, WPARAM wp, LPARAM lp) const;

protected:

    //! 全消去
    void clear();

    //! 指定したリスナが登録されているか？
    bool exist(const IEventHandler *p) const;

private:
    // IEventHandlerインタフェースを継承するクラスへのポインタ、
    // もしくは関数オブジェクトとして、リスナをリスト化して管理
    Container listeners;
};

}  // namespace

#endif
