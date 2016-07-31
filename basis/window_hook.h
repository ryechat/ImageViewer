#pragma once
#ifndef GUID_65F96D9BE4D14C599276F442B3038637
#define GUID_65F96D9BE4D14C599276F442B3038637

#ifndef STDAFX_H
#include <wtypes.h>
#include <list>
#endif

namespace basis {

class IEventHandler;
class Window;
enum class Message : int;

//! ウィンドウフック
/*!	<p>メッセ―ジを受け取るイベントリスナをフックできるクラス。
	</p>
	<p>ウィンドウクラスで使われ、ウィンドウ生成直後は
	ウィンドウ自身のリスナのみを格納することになる。
*/
class WindowHook {
public:
	WindowHook() = default;
	virtual ~WindowHook() = default;

	WindowHook(const WindowHook &) = default;
	WindowHook&operator=(const WindowHook &) = default;
	WindowHook(WindowHook &&) = default;
	WindowHook&operator=(WindowHook &&) = default;

	//! イベントリスナクラスを先頭にフックする
	//! 子クラスのリスナはこれを呼ぶ
	void push_front(IEventHandler *p);

	//! イベントリスナクラスを末尾にフックする
	//! 親クラスのリスナはこれを呼ぶ
	void push_back(IEventHandler *p);

	//! リスナを除去する
	void unhook(IEventHandler *p);

	//! 全消去
	void clear();

	//! 先頭から順にメッセージを配信する
	int dispatch(Window *win, Message msg,
		WPARAM wp, LPARAM lp) const;

	//! 指定したリスナが登録されているか？
	bool exist(const IEventHandler *p) const;

private:
	using Container = std::list<IEventHandler*>;
	Container eventHandlers;
};

}  // namespace

#endif
